LXIP_CONTRIB_DIR := $(call select_from_ports,dde_linux)/src/lib/lxip
SRC_DIR          := $(REP_DIR)/src/lib/lxip

#
# The order of include-search directories is important, we need to look into
# 'contrib' before falling back to our custom 'lx_emul.h' header.
#
INC_DIR += $(SRC_DIR) \
           $(SRC_DIR)/include
#INC_DIR += $(REP_DIR)/src/include
INC_DIR += $(LXIP_CONTRIB_DIR)/include \
           $(LXIP_CONTRIB_DIR)/include/uapi \
INC_DIR += $(LIB_CACHE_DIR)/lxip_include/include/include/include
