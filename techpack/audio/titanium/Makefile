# auto-detect subdirs
ifeq ($(CONFIG_ARCH_SDM450), y)
include $(srctree)/techpack/audio/titanium/config/sdm450auto.conf
export $(shell sed 's/=.*//' $(srctree)/techpack/audio/titanium/config/sdm450auto.conf)
endif
ifeq ($(CONFIG_ARCH_MSM8953), y)
include $(srctree)/techpack/audio/titanium/config/msm8953auto.conf
export $(shell sed 's/=.*//' $(srctree)/techpack/audio/titanium/config/msm8953auto.conf)
endif

# Use USERINCLUDE when you must reference the UAPI directories only.
USERINCLUDE     += \
                -I$(srctree)/techpack/audio/titanium/include/uapi \

# Use LINUXINCLUDE when you must reference the include/ directory.
# Needed to be compatible with the O= option
LINUXINCLUDE    += \
                -I$(srctree)/techpack/audio/titanium/include/uapi \
                -I$(srctree)/techpack/audio/titanium/include

ifeq ($(CONFIG_ARCH_SDM450), y)
LINUXINCLUDE    += \
                -include $(srctree)/techpack/audio/titanium/config/sdm450autoconf.h
endif
ifeq ($(CONFIG_ARCH_MSM8953), y)
LINUXINCLUDE    += \
                -include $(srctree)/techpack/audio/titanium/config/msm8953autoconf.h
endif

obj-y += asoc/
obj-y += dsp/
obj-y += ipc/
obj-y += soc/
