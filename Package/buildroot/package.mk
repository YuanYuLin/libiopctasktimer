################################################################################
#
# libiopctasktimer
#
################################################################################

LIBIOPCTASKTIMER_VERSION       = <BUILDROOT_VERSION>
LIBIOPCTASKTIMER_VERSION_MAJOR = 1
LIBIOPCTASKTIMER_VERSION_MINOR = 0
LIBIOPCTASKTIMER_SITE          = $(call github,YuanYuLin,libiopctasktimer,$(LIBIOPCTASKTIMER_VERSION))
#LIBIOPCTASKTIMER_SITE          = file:///tmp
#LIBIOPCTASKTIMER_SOURCE        = libiopctasktimer.tar.bz2
LIBIOPCTASKTIMER_LICENSE       = GPLv2+
LIBIOPCTASKTIMER_LICENSE_FILES = COPYING

LIBIOPCTASKTIMER_PACKAGE_DIR	= $(BASE_DIR)/packages/libiopctasktimer

LIBIOPCTASKTIMER_DEPENDENCIES  = libiopccommon

LIBIOPCTASKTIMER_EXTRA_CFLAGS =                                        \
	-DTARGET_LINUX -DTARGET_POSIX                           \


LIBIOPCTASKTIMER_MAKE_ENV =                       \
	CROSS_COMPILE=$(TARGET_CROSS)       \
	BUILDROOT=$(TOP_DIR)                \
	SDKSTAGE=$(STAGING_DIR)             \
	TARGETFS=$(LIBIOPCTASKTIMER_PACKAGE_DIR)  \
	TOOLCHAIN=$(HOST_DIR)/usr           \
	HOST=$(GNU_TARGET_NAME)             \
	SYSROOT=$(STAGING_DIR)              \
	JOBS=$(PARALLEL_JOBS)               \
	$(TARGET_CONFIGURE_OPTS)            \
	CFLAGS="$(TARGET_CFLAGS) $(LIBIOPCTASKTIMER_EXTRA_CFLAGS)"

define LIBIOPCTASKTIMER_BUILD_CMDS
	$(LIBIOPCTASKTIMER_MAKE_ENV) $(MAKE) -C $(@D)
endef

define LIBIOPCTASKTIMER_INSTALL_TARGET_DIR
	cp -avr $(LIBIOPCTASKTIMER_PACKAGE_DIR)/usr/local/lib/* $(TARGET_DIR)/usr/local/lib/
endef

define LIBIOPCTASKTIMER_INSTALL_TARGET_CMDS
	rm -rf $(LIBIOPCTASKTIMER_PACKAGE_DIR)
	mkdir -p $(LIBIOPCTASKTIMER_PACKAGE_DIR)/usr/local/lib/
	$(INSTALL) -m 0755 -D $(@D)/libiopctasktimer.so $(LIBIOPCTASKTIMER_PACKAGE_DIR)/usr/local/lib/
	$(LIBIOPCTASKTIMER_INSTALL_TARGET_DIR)
endef

$(eval $(generic-package))
