deps_config := \
	/home/daniel/Faculty/BachelorThesis/apps/helloworld/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//vfscore/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//uktimeconv/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//uktime/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//ukswrand/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//uksp/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//uksglist/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//ukschedcoop/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//uksched/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//ukring/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//uknetdev/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//ukmpi/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//ukmmap/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//uklock/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//uklibparam/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//ukdebug/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//ukbus/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//ukboot/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//ukblkdev/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//ukargparse/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//ukallocregion/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//ukallocbbuddy/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//ukalloc/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//uk9p/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//syscall_shim/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//ramfs/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//posix-user/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//posix-sysinfo/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//posix-process/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//posix-libdl/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//nolibc/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//fdt/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//devfs/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/lib//9pfs/Config.uk \
	/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/libs.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/plat/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/plat//xen/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/plat//linuxu/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/plat//kvm/Config.uk \
	/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/plat.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/arch/arm/arm64/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/arch/arm/arm/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/arch/x86/x86_64/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/arch/Config.uk \
	/home/daniel/Faculty/BachelorThesis/unikraft/Config.uk

/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/auto.conf: \
	$(deps_config)

ifneq "$(UK_FULLVERSION)" "0.4.0~fd5fe1e"
/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/auto.conf: FORCE
endif
ifneq "$(UK_CODENAME)" "Rhea"
/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/auto.conf: FORCE
endif
ifneq "$(UK_ARCH)" "x86_64"
/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/auto.conf: FORCE
endif
ifneq "$(UK_BASE)" "/home/daniel/Faculty/BachelorThesis/unikraft"
/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/auto.conf: FORCE
endif
ifneq "$(UK_APP)" "/home/daniel/Faculty/BachelorThesis/apps/helloworld"
/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/auto.conf: FORCE
endif
ifneq "$(UK_NAME)" "helloworld"
/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/auto.conf: FORCE
endif
ifneq "$(CC)" "gcc"
/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/auto.conf: FORCE
endif
ifneq "$(KCONFIG_PLAT_DIR)" "/home/daniel/Faculty/BachelorThesis/unikraft/plat//kvm /home/daniel/Faculty/BachelorThesis/unikraft/plat//linuxu /home/daniel/Faculty/BachelorThesis/unikraft/plat//xen  /home/daniel/Faculty/BachelorThesis/unikraft/plat/"
/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/auto.conf: FORCE
endif
ifneq "$(KCONFIG_PLAT_IN)" "/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/plat.uk"
/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/auto.conf: FORCE
endif
ifneq "$(KCONFIG_LIB_DIR)" "/home/daniel/Faculty/BachelorThesis/unikraft/lib//9pfs /home/daniel/Faculty/BachelorThesis/unikraft/lib//devfs /home/daniel/Faculty/BachelorThesis/unikraft/lib//fdt /home/daniel/Faculty/BachelorThesis/unikraft/lib//nolibc /home/daniel/Faculty/BachelorThesis/unikraft/lib//posix-libdl /home/daniel/Faculty/BachelorThesis/unikraft/lib//posix-process /home/daniel/Faculty/BachelorThesis/unikraft/lib//posix-sysinfo /home/daniel/Faculty/BachelorThesis/unikraft/lib//posix-user /home/daniel/Faculty/BachelorThesis/unikraft/lib//ramfs /home/daniel/Faculty/BachelorThesis/unikraft/lib//syscall_shim /home/daniel/Faculty/BachelorThesis/unikraft/lib//uk9p /home/daniel/Faculty/BachelorThesis/unikraft/lib//ukalloc /home/daniel/Faculty/BachelorThesis/unikraft/lib//ukallocbbuddy /home/daniel/Faculty/BachelorThesis/unikraft/lib//ukallocregion /home/daniel/Faculty/BachelorThesis/unikraft/lib//ukargparse /home/daniel/Faculty/BachelorThesis/unikraft/lib//ukblkdev /home/daniel/Faculty/BachelorThesis/unikraft/lib//ukboot /home/daniel/Faculty/BachelorThesis/unikraft/lib//ukbus /home/daniel/Faculty/BachelorThesis/unikraft/lib//ukdebug /home/daniel/Faculty/BachelorThesis/unikraft/lib//uklibparam /home/daniel/Faculty/BachelorThesis/unikraft/lib//uklock /home/daniel/Faculty/BachelorThesis/unikraft/lib//ukmmap /home/daniel/Faculty/BachelorThesis/unikraft/lib//ukmpi /home/daniel/Faculty/BachelorThesis/unikraft/lib//uknetdev /home/daniel/Faculty/BachelorThesis/unikraft/lib//ukring /home/daniel/Faculty/BachelorThesis/unikraft/lib//uksched /home/daniel/Faculty/BachelorThesis/unikraft/lib//ukschedcoop /home/daniel/Faculty/BachelorThesis/unikraft/lib//uksglist /home/daniel/Faculty/BachelorThesis/unikraft/lib//uksp /home/daniel/Faculty/BachelorThesis/unikraft/lib//ukswrand /home/daniel/Faculty/BachelorThesis/unikraft/lib//uktime /home/daniel/Faculty/BachelorThesis/unikraft/lib//uktimeconv /home/daniel/Faculty/BachelorThesis/unikraft/lib//vfscore /home/daniel/Faculty/BachelorThesis/unikraft/lib "
/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/auto.conf: FORCE
endif
ifneq "$(KCONFIG_LIB_IN)" "/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/libs.uk"
/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/auto.conf: FORCE
endif
ifneq "$(KCONFIG_APP_DIR)" "/home/daniel/Faculty/BachelorThesis/apps/helloworld"
/home/daniel/Faculty/BachelorThesis/apps/helloworld/build/kconfig/auto.conf: FORCE
endif

$(deps_config): ;
