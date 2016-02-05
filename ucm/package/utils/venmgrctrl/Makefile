include $(TOPDIR)/rules.mk

# Name and release number of this package
PKG_NAME:=venmgrctrl
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/venmgrctrl
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:=Ven Manager Controls
#  DEPENDS:=+cyusbserial
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

TARGET_CFLAGS += $(FPIC)
CONFIGURE_VARS += GCC="$(TARGET_CROSS)gcc"
CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--without-readline \
	--disable-fft \


define Build/Compile
	$(call Build/Compile/Default, \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		GCC="$(TARGET_CC)" \
		all \
	)
endef

define Package/venmgrctrl/install
	$(INSTALL_DIR) $(1)/bin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/venmgrctrl $(1)/bin/
endef

$(eval $(call BuildPackage,venmgrctrl))
