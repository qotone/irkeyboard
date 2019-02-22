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

#ifndef QIRKEYBOARD_H
#define QIRKEYBOARD_H

#include <qobject.h>
#include <QDataStream>
#include <QVector>

QT_BEGIN_NAMESPACE

class QSocketNotifier;


enum {
    Ir_NoKeyMode       = -1
};



namespace QIrKeyboardMap {
    const quint32 FileMagic = 0x514d4150; // 'QMAP'

    struct Mapping {
        quint16 ircode;
        quint16 keycode;
        quint16 unicode;
        quint32 qtcode;
        quint8 modifiers;
        quint8 flags;
        quint16 special;

    };

    enum Flags {
        NoFlags    = 0x00,
        IsLetter   = 0x01,
        IsModifier = 0x02
    };

    enum Modifiers {
        ModPlain   = 0x00,
        ModShift   = 0x01,
        ModAltGr   = 0x02,
        ModControl = 0x04,
        ModAlt     = 0x08,
        ModShiftL  = 0x10,
        ModShiftR  = 0x20,
        ModCtrlL   = 0x40,
        ModCtrlR   = 0x80
        // ModCapsShift = 0x100, // not supported!
    };
}

inline QDataStream &operator>>(QDataStream &ds, QIrKeyboardMap::Mapping &m)
{
    return ds >> m.keycode >> m.unicode >> m.qtcode >> m.modifiers >> m.flags >> m.special;
}

class QIrKeyboardHandler : public QObject
{
    Q_OBJECT

public:
    QIrKeyboardHandler(const QString &key, const QString &specification);
    ~QIrKeyboardHandler() override;


protected:
    Qt::Key IrToQtKey(const int k);
    void processKeycode(quint16 keycode, bool pressed, bool autorepeat);
    void processKeyEvent(int nativecode, int unicode, int qtcode,
                         Qt::KeyboardModifiers modifiers, bool isPress, bool autoRepeat);
    void resetKeymap();
    void readKeyboardData();

private:
    QScopedPointer<QSocketNotifier> m_notifier;
    int m_fd = -1;
    bool m_shouldClose = false;
    QString m_spec;

    // keymap handling
    quint8 m_modifiers = 0;
    bool m_capsLock = false;
    bool m_numLock = false;
    bool m_scrollLock = false;

    QVector<QIrKeyboardMap::Mapping> m_keymap;
};

QT_END_NAMESPACE

#endif // QIRKEYBOARD_H
