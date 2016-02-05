include $(TOPDIR)/rules.mk

# Name and release number of this package
PKG_NAME:=venmanager
PKG_RELEASE:=1

PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

include $(INCLUDE_DIR)/package.mk

define Package/venmanager
  SECTION:=utils
  CATEGORY:=Utilities
  TITLE:= venmanager Process
  DEPENDS:= +libpthread +libstdcpp +libcurl +libc +oadr +oadrsd +libsqlite3 +libxerces-c-3.1 +libxerces-c
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

define Package/venmanager/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/venmanager $(1)/bin/
endef

$(eval $(call BuildPackage,venmanager))
