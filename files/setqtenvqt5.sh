#!/bin/sh
export QT_INSTALL_PATH=/nfsroot/app/hi3531d/Qt-hisi-test
export LD_LIBRARY_PATH=/lib:/usr/lib:${QT_INSTALL_PATH}/lib:${LD_LIBRARY_PATH}
export QT_QPA_PLATFORM_PLUGIN_PATH=${QT_INSTALL_PATH}/plugins
export QT_QPA_FONTDIR=${QT_INSTALL_PATH}/fonts
# 720P
# export QT_QPA_PLATFORM=linuxfb:fb=/dev/fb0:size=1320x820:offset=600x250
# 1080P
export QT_QPA_PLATFORM=linuxfb:fb=/dev/fb0:size=1500x972:offset=420x108
# export QT_QPA_PLATFORM=linuxfb:fb=/dev/fb0:size=1920x1080:offset=0x0
export QT_PLUGIN_PATH=${QT_INSTALL_PATH}/plugins
export QT_QPA_GENERIC_PLUGINS=irkeytboard:/dev/Hi_IR
# export QT_QPA_GENERIC_PLUGINS=evdevkeyboard:/dev/input/event1
# export QML2_IMPORT_PATH=${QT_INSTALL_PATH}/qml
