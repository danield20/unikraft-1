/* SPDX-License-Identifier: ISC */
/*
 * Authors: Dan Williams
 *          Martin Lucina
 *          Ricardo Koller
 *          Felipe Huici <felipe.huici@neclab.eu>
 *          Florian Schmidt <florian.schmidt@neclab.eu>
 *          Simon Kuenzer <simon.kuenzer@neclab.eu>
 *
 * Copyright (c) 2015-2017 IBM
 * Copyright (c) 2016-2017 Docker, Inc.
 * Copyright (c) 2017 NEC Europe Ltd., NEC Corporation
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <string.h>
#include <uk/plat/common/sections.h>
#include <x86/cpu.h>
#include <x86/traps.h>
#include <kvm/config.h>
#include <kvm/console.h>
#include <kvm/intctrl.h>
#include <kvm-x86/multiboot.h>
#include <kvm-x86/multiboot_defs.h>
#include <uk/arch/limits.h>
#include <uk/arch/types.h>
#include <uk/plat/console.h>
#include <uk/assert.h>
#include <uk/essentials.h>

#include <errno.h>
#include <uk/plat/common/cpu.h>
#include <uk/plat/common/irq.h>
#include <uk/print.h>
#include <uk/plat/bootstrap.h>

#include <libelf.h>
#include <binfmt_elf.h>
#include <uk/essentials.h>
#include <uk/plat/memory.h>
#include <uk/allocbbuddy.h>
#include <sys/random.h>
#include <uk/swrand2.h>
#include <stdio.h>

#define PLATFORM_MEM_START 0x100000
#define PLATFORM_MAX_MEM_ADDR 0x40000000

#define MAX_CMDLINE_SIZE 8192
static char cmdline[MAX_CMDLINE_SIZE];
static int max_address;

struct kvmplat_config _libkvmplat_cfg = { 0 };

extern void _libkvmplat_newstack(uintptr_t stack_start, void (*tramp)(void *),
				 void *arg);

static inline void _mb_get_cmdline(struct multiboot_info *mi)
{
	char *mi_cmdline;

	if (mi->flags & MULTIBOOT_INFO_CMDLINE) {
		mi_cmdline = (char *)(__u64)mi->cmdline;

		if (strlen(mi_cmdline) > sizeof(cmdline) - 1)
			uk_pr_err("Command line too long, truncated\n");
		strncpy(cmdline, mi_cmdline,
			sizeof(cmdline));
	} else {
		/* Use image name as cmdline to provide argv[0] */
		uk_pr_debug("No command line present\n");
		strncpy(cmdline, CONFIG_UK_NAME, sizeof(cmdline));
	}

	/* ensure null termination */
	cmdline[(sizeof(cmdline) - 1)] = '\0';
}

static inline void _mb_init_mem(struct multiboot_info *mi)
{
	multiboot_memory_map_t *m;
	size_t offset, max_addr;

	/*
	 * Look for the first chunk of memory at PLATFORM_MEM_START.
	 */
	for (offset = 0; offset < mi->mmap_length;
	     offset += m->size + sizeof(m->size)) {
		m = (void *)(__uptr)(mi->mmap_addr + offset);
		if (m->addr == PLATFORM_MEM_START
		    && m->type == MULTIBOOT_MEMORY_AVAILABLE) {
			break;
		}
	}
	UK_ASSERT(offset < mi->mmap_length);

	/*
	 * Cap our memory size to PLATFORM_MAX_MEM_SIZE which boot.S defines
	 * page tables for.
	 */
	max_addr = m->addr + m->len;
	if (max_addr > PLATFORM_MAX_MEM_ADDR)
		max_addr = PLATFORM_MAX_MEM_ADDR;
	UK_ASSERT((size_t) __END <= max_addr);

	max_address = max_addr;

	/*
	 * Reserve space for boot stack at the end of found memory
	 */
	if ((max_addr - m->addr) < __STACK_SIZE)
		UK_CRASH("Not enough memory to allocate boot stack\n");

	_libkvmplat_cfg.heap.start = ALIGN_UP((uintptr_t) __END, __PAGE_SIZE);
	_libkvmplat_cfg.heap.end   = (uintptr_t) max_addr - __STACK_SIZE;
	_libkvmplat_cfg.heap.len   = _libkvmplat_cfg.heap.end
				     - _libkvmplat_cfg.heap.start;
	_libkvmplat_cfg.bstack.start = _libkvmplat_cfg.heap.end;
	_libkvmplat_cfg.bstack.end   = max_addr;
	_libkvmplat_cfg.bstack.len   = __STACK_SIZE;
}

static inline void _mb_init_initrd(struct multiboot_info *mi)
{
	multiboot_module_t *mod1;
	uintptr_t heap0_start, heap0_end;
	uintptr_t heap1_start, heap1_end;
	size_t    heap0_len,   heap1_len;

	/*
	 * Search for initrd (called boot module according multiboot)
	 */
	if (mi->mods_count == 0) {
		uk_pr_debug("No initrd present\n");
		goto no_initrd;
	}

	/*
	 * NOTE: We are only taking the first boot module as initrd.
	 *       Initrd arguments and further modules are ignored.
	 */
	UK_ASSERT(mi->mods_addr);

	mod1 = (multiboot_module_t *)((uintptr_t) mi->mods_addr);
	UK_ASSERT(mod1->mod_end >= mod1->mod_start);

	if (mod1->mod_end == mod1->mod_start) {
		uk_pr_debug("Ignoring empty initrd\n");
		goto no_initrd;
	}

	_libkvmplat_cfg.initrd.start = (uintptr_t) mod1->mod_start;
	_libkvmplat_cfg.initrd.end = (uintptr_t) mod1->mod_end;
	_libkvmplat_cfg.initrd.len = (size_t) (mod1->mod_end - mod1->mod_start);

	/*
	 * Check if initrd is part of heap
	 * In such a case, we figure out the remaining pieces as heap
	 */
	if (_libkvmplat_cfg.heap.len == 0) {
		/* We do not have a heap */
		goto out;
	}
	heap0_start = 0;
	heap0_end   = 0;
	heap1_start = 0;
	heap1_end   = 0;
	if (RANGE_OVERLAP(_libkvmplat_cfg.heap.start,
			  _libkvmplat_cfg.heap.len,
			  _libkvmplat_cfg.initrd.start,
			  _libkvmplat_cfg.initrd.len)) {
		if (IN_RANGE(_libkvmplat_cfg.initrd.start,
			     _libkvmplat_cfg.heap.start,
			     _libkvmplat_cfg.heap.len)) {
			/* Start of initrd within heap range;
			 * Use the prepending left piece as heap */
			heap0_start = _libkvmplat_cfg.heap.start;
			heap0_end   = ALIGN_DOWN(_libkvmplat_cfg.initrd.start,
						 __PAGE_SIZE);
		}
		if (IN_RANGE(_libkvmplat_cfg.initrd.start,

			     _libkvmplat_cfg.heap.start,
			     _libkvmplat_cfg.heap.len)) {
			/* End of initrd within heap range;
			 * Use the remaining left piece as heap */
			heap1_start = ALIGN_UP(_libkvmplat_cfg.initrd.end,
					       __PAGE_SIZE);
			heap1_end   = _libkvmplat_cfg.heap.end;
		}
	} else {
		/* Initrd is not overlapping with heap */
		heap0_start = _libkvmplat_cfg.heap.start;
		heap0_end   = _libkvmplat_cfg.heap.end;
	}
	heap0_len = heap0_end - heap0_start;
	heap1_len = heap1_end - heap1_start;

	/*
	 * Update heap regions
	 * We make sure that in we start filling left heap pieces at
	 * `_libkvmplat_cfg.heap`. Any additional piece will then be
	 * placed to `_libkvmplat_cfg.heap2`.
	 */
	if (heap0_len == 0) {
		/* Heap piece 0 is empty, use piece 1 as only */
		if (heap1_len != 0) {
			_libkvmplat_cfg.heap.start = heap1_start;
			_libkvmplat_cfg.heap.end   = heap1_end;
			_libkvmplat_cfg.heap.len   = heap1_len;
		} else {
			_libkvmplat_cfg.heap.start = 0;
			_libkvmplat_cfg.heap.end   = 0;
			_libkvmplat_cfg.heap.len   = 0;
		}
		 _libkvmplat_cfg.heap2.start = 0;
		 _libkvmplat_cfg.heap2.end   = 0;
		 _libkvmplat_cfg.heap2.len   = 0;
	} else {
		/* Heap piece 0 has memory */
		_libkvmplat_cfg.heap.start = heap0_start;
		_libkvmplat_cfg.heap.end   = heap0_end;
		_libkvmplat_cfg.heap.len   = heap0_len;
		if (heap1_len != 0) {
			_libkvmplat_cfg.heap2.start = heap1_start;
			_libkvmplat_cfg.heap2.end   = heap1_end;
			_libkvmplat_cfg.heap2.len   = heap1_len;
		} else {
			_libkvmplat_cfg.heap2.start = 0;
			_libkvmplat_cfg.heap2.end   = 0;
			_libkvmplat_cfg.heap2.len   = 0;
		}
	}

	/*
	 * Double-check that initrd is not overlapping with previously allocated
	 * boot stack. We crash in such a case because we assume that multiboot
	 * places the initrd close to the beginning of the heap region. One need
	 * to assign just more memory in order to avoid this crash.
	 */
	if (RANGE_OVERLAP(_libkvmplat_cfg.heap.start,
			  _libkvmplat_cfg.heap.len,
			  _libkvmplat_cfg.initrd.start,
			  _libkvmplat_cfg.initrd.len))
		UK_CRASH("Not enough space at end of memory for boot stack\n");
out:
	return;

no_initrd:
	_libkvmplat_cfg.initrd.start = 0;
	_libkvmplat_cfg.initrd.end   = 0;
	_libkvmplat_cfg.initrd.len   = 0;
	_libkvmplat_cfg.heap2.start  = 0;
	_libkvmplat_cfg.heap2.end    = 0;
	_libkvmplat_cfg.heap2.len    = 0;
	return;
}

static inline void initialize_allocator()
{
	struct ukplat_memregion_desc md;
	int rc __maybe_unused = 0;
	struct uk_alloc *a = NULL;

	uk_pr_info("Initialize memory allocator...\n");

	ukplat_memregion_foreach(&md, UKPLAT_MEMRF_ALLOCATABLE) {
#if CONFIG_UKPLAT_MEMRNAME
		uk_pr_debug("Try memory region: %p - %p (flags: 0x%02x, name: %s)...\n",
			    md.base, (void *)((size_t)md.base + md.len),
			    md.flags, md.name);
#else
		uk_pr_debug("Try memory region: %p - %p (flags: 0x%02x)...\n",
			    md.base, (void *)((size_t)md.base + md.len),
			    md.flags);
#endif

		/* try to use memory region to initialize allocator
		 * if it fails, we will try  again with the next region.
		 * As soon we have an allocator, we simply add every
		 * subsequent region to it
		 */
		if (unlikely(!a))
			a = uk_allocbbuddy_init(md.base, md.len);
		else
			uk_alloc_addmem(a, md.base, md.len);
	}

	if (unlikely(!a))
		uk_pr_warn("No suitable memory region for memory allocator. Continue without heap\n");
	else {
		rc = ukplat_memallocator_set(a);
		if (unlikely(rc != 0))
			UK_CRASH("Could not set the platform memory allocator\n");
	}
}

static inline long get_random_addr(int img_len)
{
	long int random_addr, remaining_space;

	/* search for a valid address for the pie kernel */
	while (1) {
		random_addr = uk_swrand_randr() % max_address;
		random_addr = random_addr/__PAGE_SIZE * __PAGE_SIZE;
		/* we don't want for the pie kernel to overlap with the non-pie part or initrd */
		if (random_addr < 0x141000 + img_len)
			continue;

		remaining_space = max_address - random_addr;
		if (remaining_space > img_len)
			break;
	}

	uk_pr_info("Random address for pie kernel: %p\n", (void *)random_addr);

	return random_addr;
}

void _libkvmplat_entry(void *arg)
{
	struct multiboot_info *mi = (struct multiboot_info *)arg;
	struct multiboot_info *my_mi;
	struct ukplat_memregion_desc img;
	char *newcmdline;

	_init_cpufeatures();
	_libkvmplat_init_console();

	uk_pr_info("Entering from KVM (x86)...\n");
	uk_pr_info("     multiboot: %p\n", mi);

	/*
	 * The multiboot structures may be anywhere in memory, so take a copy of
	 * everything necessary before we initialise memory allocation.
	 */
	_mb_get_cmdline(mi);
	_mb_init_mem(mi);
	_mb_init_initrd(mi);

	if (_libkvmplat_cfg.initrd.len)
		uk_pr_info("        initrd: %p\n",
			   (void *) _libkvmplat_cfg.initrd.start);
	uk_pr_info("    heap start: %p\n",
		   (void *) _libkvmplat_cfg.heap.start);
	if (_libkvmplat_cfg.heap2.len)
		uk_pr_info(" heap start (2): %p\n",
			   (void *) _libkvmplat_cfg.heap2.start);
	uk_pr_info("     stack top: %p\n",
		   (void *) _libkvmplat_cfg.bstack.start);

	if (elf_version(EV_CURRENT) == EV_NONE)
		UK_CRASH("Failed to initialize libelf: Version error");

	uk_pr_info("Searching for image...\n");
	int rc = ukplat_memregion_find_initrd0(&img);
	if (rc < 0 || !img.base || !img.len) {
		uk_pr_info("Error1\n");
	}

	uk_pr_info("Image at %p, len %"__PRIsz" bytes\n",
		   img.base, img.len);

	initialize_allocator();

	my_mi = malloc(sizeof(struct multiboot_info));
	memcpy(my_mi, mi, sizeof(struct multiboot_info));
	newcmdline = malloc(MAX_CMDLINE_SIZE);
	strcpy(newcmdline, cmdline);
	my_mi->cmdline = (multiboot_uint64_t)newcmdline;

	/* initialize randomizer */
	aslr_uk_swrand_ctor();

	void *elf_load_address = (void *)get_random_addr(_libkvmplat_cfg.initrd.len);

	/*
	 * Parse image
	 */
	uk_pr_info("Load image...\n");
	struct elf_prog  *prog = load_elf(uk_alloc_get_default(), img.base, img.len,
					"PIE kernel", elf_load_address);
	if (!prog) {
		uk_pr_info("Error loading the elf\n");
	}

	/* Jump to elf entry */
	uk_pr_info("Entry point at %p\n", (void *)prog->entry);
	void (*elf_entry)(void *) = (void (*)(void *))prog->entry;
	elf_entry(my_mi);
}
