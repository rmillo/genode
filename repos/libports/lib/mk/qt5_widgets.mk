include $(REP_DIR)/lib/import/import-qt5_widgets.mk

SHARED_LIB = yes

include $(REP_DIR)/lib/mk/qt5_widgets_generated.inc

# UI headers
qfiledialog.o: ui_qfiledialog.h

include $(REP_DIR)/lib/mk/qt5.inc

# remove unsupported specific files
QT_SOURCES_FILTER_OUT = \
  complexwidgets.cpp \
  itemviews.cpp \
  qaccessiblemenu.cpp \
  qaccessiblewidget.cpp \
  qaccessiblewidgetfactory.cpp \
  qaccessiblewidgets.cpp \
  rangecontrols.cpp \
  simplewidgets.cpp 

INC_DIR += $(REP_DIR)/include/qt5/qtbase/QtWidgets/private \
           $(QT5_CONTRIB_DIR)/qtbase/include/QtWidgets/$(QT_VERSION)/QtWidgets \
           $(QT5_CONTRIB_DIR)/qtbase/include/QtGui/$(QT_VERSION) \
           $(QT5_CONTRIB_DIR)/qtbase/include/QtGui/$(QT_VERSION)/QtGui \
           $(QT5_CONTRIB_DIR)/qtbase/include/QtCore/$(QT_VERSION) \
           $(QT5_CONTRIB_DIR)/qtbase/include/QtCore/$(QT_VERSION)/QtCore

LIBS += qt5_core libc
