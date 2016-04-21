SHARED_LIB = yes

LIB_DIR     = $(REP_DIR)/src/lib/vfs/ext4
LIB_INC_DIR = $(LIB_DIR)/include

LIBS += ext4_include

LX_CONTRIB_DIR := $(call select_from_ports,dde_linux)/src/lib/ext4
EXT4_DIR       := $(LX_CONTRIB_DIR)/fs/ext4

#CC_OLEVEL = -O2

CC_OPT += -U__linux__ -D__KERNEL__

CC_WARN = -Wall -Wno-unused-variable -Wno-uninitialized \
          -Wno-unused-function -Wno-overflow -Wno-pointer-arith \
          -Wno-sign-compare

CC_C_OPT  += -Wno-unused-but-set-variable -Wno-pointer-sign

CC_C_OPT  += -include $(LIB_INC_DIR)/lx_emul.h
CC_CXX_OPT = -fpermissive

# lx_kit
SRC_CC += malloc.cc printf.cc

# SRC_C += $(addprefix fs/ext4/,$(notdir $(wildcard $(EXT4_DIR)/*.c)))
SRC_C_ext4  = balloc.c bitmap.c dir.c file.c fsync.c ialloc.c inode.c page-io.c \
              ioctl.c namei.c super.c symlink.c hash.c resize.c extents.c \
              ext4_jbd2.c migrate.c mballoc.c block_validity.c move_extent.c \
              mmp.c indirect.c extents_status.c xattr.c xattr_user.c \
              xattr_trusted.c inline.c readpage.c sysfs.c
SRC_C      += $(addprefix fs/ext4/,$(SRC_C_ext4))

# SRC_C_ext4_acl  = xattr_security.c
# SRC_C          += $(addprefix fs/ext4/,$(SRC_C_ext4_acl))

# SRC_C_ext4_crypto  = crypto_policy.c crypto.c crypto_key.c crypto_fname.c
# SRC_C             += $(addprefix fs/ext4/,$(SRC_C_ext4_acl))


vpath %.c  $(LX_CONTRIB_DIR)
vpath %.c  $(LIB_DIR)
vpath %.cc $(LIB_DIR)
vpath %.cc $(REP_DIR)/src/lx_kit
