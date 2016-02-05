include $(TOPDIR)/rules.mk

# Name and release number of this package
PKG_NAME:=oadr
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/oadr
  SECTION:=libs
  CATEGORY:=Libraries
  TITLE:=A library for accessing oadr API's
  DEPENDS:=+libstdcpp +libcurl 
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)/
endef

TARGET_CFLAGS += $(FPIC)
CONFIGURE_VARS += GPP="$(TARGET_CROSS)g++"
CONFIGURE_ARGS += \
	--enable-shared \
	--enable-static \
	--without-readline \
	--disable-fft \


define Build/Compile
	$(call Build/Compile/Default, \
		DESTDIR="$(PKG_INSTALL_DIR)" \
		GPP="$(TARGET_CXX)" \
		all \
	)
endef

define Build/InstallDev
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/liboadr.so $(1)/usr/lib/
endef

define Package/oadr/install
	$(INSTALL_DIR) $(1)/usr/lib
	$(CP) $(PKG_BUILD_DIR)/liboadr.so $(1)/usr/lib/
endef

$(eval $(call BuildPackage,oadr))
