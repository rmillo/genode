include $(REP_DIR)/lib/import/import-libc_lwip_wrapper.mk

SRC_CC = lwip_wrapper.cc

LIBS += libc lwip libc_lwip

vpath % $(REP_DIR)/src/lib/libc_lwip_wrapper
