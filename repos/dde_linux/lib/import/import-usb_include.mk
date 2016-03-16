USB_CONTRIB_DIR := $(call select_from_ports,dde_linux)/src/lib/usb

LIB_DIR          := $(REP_DIR)/src/lib/usb
LIB_INC_DIR      := $(LIB_DIR)/include

#
# The order of include-search directories is important, we need to look into
# 'contrib' before falling back to our custom 'lx_emul.h' header.
#
INC_DIR += $(LIB_INC_DIR)
INC_DIR += $(USB_CONTRIB_DIR)/include
INC_DIR += $(USB_CONTRIB_DIR)/drivers/usb/core
INC_DIR += $(LIB_CACHE_DIR)/usb_include/include/include/include
