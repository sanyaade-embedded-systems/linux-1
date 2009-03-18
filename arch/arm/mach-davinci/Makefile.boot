ifdef CONFIG_ARCH_DAVINCI
   zreladdr-y	:= 0x80008000
params_phys-y	:= 0x80000100
initrd_phys-y	:= 0x80800000
endif

ifdef CONFIG_ARCH_DA830
   zreladdr-y	:= 0xC0008000
params_phys-y	:= 0xC0000100
initrd_phys-y	:= 0xC0800000
endif
