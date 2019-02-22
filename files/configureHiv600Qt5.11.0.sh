#!/bin/bash

unset QMAKESPEC
unset XQMAKESPEC
unset QMAKEPATH
unset QMAKEFEATURES

../configure \
    --prefix=QT5.11.0 \
    -opensource \
    -confirm-license \
    -xplatform linux-arm-hisiv600-g++ \
    -qt-freetype \
    -feature-accessibility \
    -accessibility \
    -skip qtandroidextras -skip qtactiveqt -skip qtdoc  \
    -skip qtserialport  -skip qtwayland \
    -skip qtmacextras -skip qtlocation \
    -skip qtx11extras -skip qtwinextras \
    -skip qttools \
    -skip qtenginio -skip qtactiveqt \
    -qt-pcre \
    -release \
    -no-cups \
    -no-dbus \
    -no-sm \
    -no-pch \
    -no-glib \
    -no-harfbuzz \
    -no-qml-debug \
    -no-opengl \
    -no-separate-debug-info \
    -reduce-exports \
    -no-android-style-assets \
    -no-compile-examples \
    -nomake tests -nomake examples  \
    -v
