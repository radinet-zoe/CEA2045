include $(TOPDIR)/rules.mk

# Name and release number of this package
PKG_NAME:=cyusbserial
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/cyusbserial
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=Cypress USB Serial Driver
  DEPENDS:=+libusb-1.0
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

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libcyusbserial.so $(1)/usr/lib/
endef

define Package/cyusbserial/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/libcyusbserial.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,cyusbserial))
