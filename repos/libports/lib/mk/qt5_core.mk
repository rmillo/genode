include $(REP_DIR)/lib/import/import-qt5_core.mk

SHARED_LIB = yes

# use default warning level to avoid noise when compiling contrib code
CC_WARN = -Wno-unused-but-set-variable -Wno-deprecated-declarations

include $(REP_DIR)/lib/mk/qt5_core_generated.inc

# add Genode-specific sources
QT_SOURCES += qprocess_genode.cpp \
              qthread_genode.cpp

# remove unsupported UNIX-specific files
QT_SOURCES_FILTER_OUT = \
  qtstorageinfo.cpp \
  qeventdispatcher_glib.cpp \
  qfilesystemwatcher_inotify.cpp \
  qprocess_unix.cpp \
  qthread_unix.cpp \
  adler32.c \
  compress.c \
  crc32.c \
  deflate.c \
  gzclose.c \
  gzlib.c \
  gzread.c \
  gzwrite.c \
  infback.c \
  inffast.c \
  inflate.c \
  inftrees.c \
  trees.c \
  uncompr.c \
  zutil.c \

# remove unneeded files to prevent moc warnings
COMPILER_MOC_HEADER_MAKE_ALL_FILES_FILTER_OUT = \
  moc_qsharedmemory.cpp \
  moc_qeventdispatcher_glib_p.cpp \
  moc_qfilesystemwatcher_inotify_p.cpp \

include $(REP_DIR)/lib/mk/qt5.inc

# reduce 'not implemented yet' noise
SRC_CC += libc_dummies.cc
vpath libc_dummies.cc $(REP_DIR)/src/lib/qt5

INC_DIR += $(REP_DIR)/include/qt5/qtbase/QtCore/private \
           $(REP_DIR)/src/lib/qt5/qtbase/src/corelib/thread \
           $(QT5_CONTRIB_DIR)/qtbase/include/QtCore/$(QT_VERSION) \
           $(QT5_CONTRIB_DIR)/qtbase/include/QtCore/$(QT_VERSION)/QtCore \
           $(QT5_CONTRIB_DIR)/qtbase/include/QtCore/$(QT_VERSION)/QtCore/private

LIBS += qt5_host_tools launchpad zlib icu libc libm alarm libc_lock_pipe pthread
