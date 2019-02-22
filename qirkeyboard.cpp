/****************************************************************************
**
** Copyright (C) 2015-2016 Oleksandr Tymoshenko <gonzo@bluezbox.com>
** Contact: https://www.qt.io/licensing/
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qirkeyboard.h"

#include <QByteArray>
#include <QFile>
#include <QGuiApplication>
#include <QPoint>
#include <QSocketNotifier>
#include <QString>
#include <QStringList>

#include <QtCore/qglobal.h>
#include <qpa/qwindowsysteminterface.h>
#include <private/qcore_unix_p.h>
#include <private/qguiapplication_p.h>
#include <private/qinputdevicemanager_p_p.h>

#include <qdebug.h>
#include <cstdio>

#include <cerrno>
#include <fcntl.h>
#include <unistd.h>

#include "hiir.h"


#define QT_IR_KEYBOARD_DEBUG

#ifdef QT_IR_KEYBOARD_DEBUG
#include <qdebug.h>
#endif

QT_BEGIN_NAMESPACE


#include "qirkeyboard_defaultmap.h"





Qt::Key QIrKeyboardHandler::IrToQtKey(const int k)
{
    //Qt::Key key_code;
    switch(k){
    case REMOTE_POWER: return Qt::Key_PowerOff;
    case REMOTE_MUTE: return Qt::Key_MicMute;
    case REMOTE_KEY_ONE:return Qt::Key_1;
    case REMOTE_KEY_TWO:return Qt::Key_2;
    case REMOTE_KEY_THREE:return Qt::Key_3;
    case REMOTE_KEY_FOUR:return Qt::Key_4;
    case REMOTE_KEY_FIVE:return Qt::Key_5;
    case REMOTE_KEY_SIX:return Qt::Key_6;
    case REMOTE_KEY_SEVEN:return Qt::Key_7;
    case REMOTE_KEY_EIGHT:return Qt::Key_8;
    case REMOTE_KEY_NINE:return Qt::Key_9;
    case REMOTE_KEY_ZERO:return Qt::Key_0;
    case REMOTE_SWITH_1_2:return Qt::Key_Shift;
    case REMOTE_BACKSPACE:return Qt::Key_Backspace;
    case REMOTE_YAYIN:return Qt::Key_Yacute;
    case REMOTE_LEFT_CURSOR:return Qt::Key_Left;
    case REMOTE_RIGHT_CURSOR:return Qt::Key_Right;
    case REMOTE_UP_CURSOR:return Qt::Key_Up;
    case REMOTE_DOWN_CURSOR:return Qt::Key_Down;
    case REMOTE_ENTER:return Qt::Key_Enter;
    case REMOTE_MENU:return Qt::Key_Menu;
    case REMOTE_GO_BACK:return Qt::Key_BackForward;
    default: return Qt::Key_unknown;
    }
}


QIrKeyboardHandler::QIrKeyboardHandler(const QString &key, const QString &specification)
{
    Q_UNUSED(key);

    setObjectName(QLatin1String("IR Keyboard Handler"));

    QByteArray device;
    if (specification.startsWith("/dev/"))
        device = QFile::encodeName(specification);

    if (device.isEmpty()) {
        device = QByteArrayLiteral("STDIN");
        m_fd = fileno(stdin);
    }
    else {
        m_fd = QT_OPEN(device.constData(), O_RDONLY);
        if (!m_fd) {
            qErrnoWarning(errno, "open(%s) failed", device.constData());
            return;
        }
        m_shouldClose = true;
         qWarning("open(%s) ok", device.constData());
    }
    resetKeymap();

    m_notifier.reset(new QSocketNotifier(m_fd, QSocketNotifier::Read, this));
    connect(m_notifier.data(), &QSocketNotifier::activated, this, &QIrKeyboardHandler::readKeyboardData);
    QInputDeviceManagerPrivate::get(QGuiApplicationPrivate::inputDeviceManager())->setDeviceCount(
        QInputDeviceManager::DeviceTypeKeyboard, 1);
}

QIrKeyboardHandler::~QIrKeyboardHandler()
{
    if (m_fd >= 0) {

        if (m_shouldClose)
            close(m_fd);
        m_fd = -1;
    }
}

void QIrKeyboardHandler::readKeyboardData()
{
        irkey_info_s rcv_irkey_info[4];
        int count;
        int bytesRead = qt_safe_read(m_fd, rcv_irkey_info, sizeof(rcv_irkey_info));


        if (!bytesRead) {
            qWarning("Got EOF from the input device.");
            return;
        } else if (bytesRead < 0) {
            if (errno != EINTR && errno != EAGAIN)
                qWarning("Could not read from input device: %s", strerror(errno));
            return;
        }
        count = bytesRead/sizeof(irkey_info_s);

        for (int i = 0; i < count; ++i) {
            const quint16 code = (quint16) (rcv_irkey_info[i].irkey_datal >> 16);
            const quint16 ir_code = (quint16) (rcv_irkey_info[i].irkey_datal);
            const bool pressed = (rcv_irkey_info[i].irkey_state_code ) ? false : true;
#ifdef QT_IR_KEYBOARD_DEBUG
            qWarning("IR read\tkeyvalue=H/L/S 0x%x/0x%x/0x%x\n",i,rcv_irkey_info[i].irkey_datah,rcv_irkey_info[i].irkey_datal,rcv_irkey_info[i].irkey_state_code);
#endif
            if(ir_code != IR_USER_CODE){
                qWarning("IR USER CODE ERROR!\tCode = %04x\n",ir_code);
            }else{
                processKeycode(code, pressed, false);
            }
        }
}

void QIrKeyboardHandler::processKeyEvent(int nativecode, int unicode, int qtcode,
                                          Qt::KeyboardModifiers modifiers, bool isPress,
                                          bool autoRepeat)
{
    const QString text = (unicode != 0xffff ) ? QString(unicode) : QString();
    const QEvent::Type eventType = isPress ? QEvent::KeyPress : QEvent::KeyRelease;

    QWindowSystemInterface::handleExtendedKeyEvent(0, eventType, qtcode, modifiers, nativecode, 0,
                                                   int(modifiers), text, autoRepeat);
}

void QIrKeyboardHandler::processKeycode(quint16 keycode, bool pressed, bool autorepeat)
{
    const bool first_press = pressed && !autorepeat;

    const QIrKeyboardMap::Mapping *map_plain = nullptr;


    // get a specific and plain mapping for the keycode and the current modifiers

    for (const QIrKeyboardMap::Mapping &m : m_keymap) {
        if (m.ircode == keycode) {
            if (m.modifiers == 0)
                map_plain = &m;
        }
    }

    const QIrKeyboardMap::Mapping *it =  map_plain;

    if (!it) {
#ifdef QT_IR_KEYBOARD_DEBUG
        // we couldn't even find a plain mapping
        qWarning("Could not find a suitable mapping for keycode: 0x%4x", keycode);
#endif
        return;
    }

    quint16 unicode = it->unicode;
    quint32 qtcode = it->qtcode;
    quint32 _keycode = it->keycode;

        // a normal key was pressed
    const int modmask = Qt::NoModifier;

#ifdef QT_IR_KEYBOARD_DEBUG
    qWarning("Processing: uni=%04x, qt=%08x, qtmod=%08x", unicode, qtcode & ~modmask, (qtcode & modmask));
#endif
        // send the result to the server
    if(pressed)
        processKeyEvent(_keycode, unicode, qtcode & ~modmask,Qt::KeyboardModifiers(qtcode & modmask), pressed, autorepeat);
}


void QIrKeyboardHandler::resetKeymap()
{
#ifdef QT_IR_KEYBOARD_DEBUG
    qWarning() << "Unload current keymap and restore built-in";
#endif

    m_keymap.clear();

    const size_t mappingSize = sizeof(keymapDefault) / sizeof(keymapDefault[0]);
    m_keymap.resize(mappingSize);
    std::copy_n( &keymapDefault[0], mappingSize, m_keymap.begin() );

    // reset state, so we could switch keymaps at runtime
    m_modifiers = 0;
    m_capsLock = false;
    m_numLock = false;
    m_scrollLock = false;

#ifdef QT_IR_KEYBOARD_DEBUG
    qWarning()<<"Hello This is my evdevkeyboard.";
#endif

}
