include $(REP_DIR)/lib/import/import-qt5_websockets.mk

SHARED_LIB = yes

include $(REP_DIR)/lib/mk/qt5_websockets_generated.inc

include $(REP_DIR)/lib/mk/qt5.inc

# remove unsupported specific files
QT_SOURCES_FILTER_OUT = \

INC_DIR += $(QT5_CONTRIB_DIR)/qtwebsockets/include/QtWebSockets/$(QT_VERSION)/QtWebSockets \
           $(QT5_CONTRIB_DIR)/qtbase/include/QtCore/$(QT_VERSION)/QtCore

LIBS += qt5_core qt5_network libc libssl pthread lwip
