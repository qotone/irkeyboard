TARGET = qirkeyboardplugin

QT += core-private gui-private input_support-private

SOURCES = main.cpp \
    qirkeyboard.cpp

OTHER_FILES += \
    irkeyboard.json

PLUGIN_TYPE = generic
PLUGIN_EXTENDS = -
PLUGIN_CLASS_NAME = QIrKeyboardPlugin
load(qt_plugin)

HEADERS += \
    qirkeyboard.h \
    qirkeyboard_defaultmap.h \
    hiir.h
