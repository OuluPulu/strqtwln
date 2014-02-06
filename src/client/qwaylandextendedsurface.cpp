/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the plugins of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qwaylandextendedsurface_p.h"

#include "qwaylandwindow_p.h"

#include "qwaylanddisplay_p.h"

#include "qwaylandnativeinterface_p.h"

#include <QtGui/QGuiApplication>
#include <qpa/qplatformnativeinterface.h>
#include <qpa/qwindowsysteminterface.h>

QT_BEGIN_NAMESPACE

QWaylandExtendedSurface::QWaylandExtendedSurface(QWaylandWindow *window, struct ::qt_extended_surface *extended_surface)
    : QtWayland::qt_extended_surface(extended_surface)
    , m_window(window)
{
}

QWaylandExtendedSurface::~QWaylandExtendedSurface()
{
    qt_extended_surface_destroy(object());
}

void QWaylandExtendedSurface::updateGenericProperty(const QString &name, const QVariant &value)
{
    QByteArray byteValue;
    QDataStream ds(&byteValue, QIODevice::WriteOnly);
    ds << value;

    update_generic_property(name, byteValue);

    m_properties.insert(name, value);
    QWaylandNativeInterface *nativeInterface = static_cast<QWaylandNativeInterface *>(
                QGuiApplication::platformNativeInterface());
    nativeInterface->emitWindowPropertyChanged(m_window, name);
}

static int32_t waylandRotationFromScreenOrientation(Qt::ScreenOrientation orientation)
{
    switch (orientation) {
    case Qt::PortraitOrientation: return QT_EXTENDED_SURFACE_ORIENTATION_PORTRAITORIENTATION;
    case Qt::InvertedPortraitOrientation: return QT_EXTENDED_SURFACE_ORIENTATION_INVERTEDPORTRAITORIENTATION;
    case Qt::LandscapeOrientation: return QT_EXTENDED_SURFACE_ORIENTATION_LANDSCAPEORIENTATION;
    case Qt::InvertedLandscapeOrientation: return QT_EXTENDED_SURFACE_ORIENTATION_INVERTEDLANDSCAPEORIENTATION;
    default: return QT_EXTENDED_SURFACE_ORIENTATION_PRIMARYORIENTATION;
    }
}

void QWaylandExtendedSurface::setContentOrientation(Qt::ScreenOrientation orientation)
{
    set_content_orientation(waylandRotationFromScreenOrientation(orientation));
}

QVariantMap QWaylandExtendedSurface::properties() const
{
    return m_properties;
}

QVariant QWaylandExtendedSurface::property(const QString &name)
{
    return m_properties.value(name);
}

QVariant QWaylandExtendedSurface::property(const QString &name, const QVariant &defaultValue)
{
    return m_properties.value(name,defaultValue);
}

void QWaylandExtendedSurface::extended_surface_onscreen_visibility(int32_t visibility)
{
    m_window->window()->setVisibility(static_cast<QWindow::Visibility>(visibility));
}

void QWaylandExtendedSurface::extended_surface_set_generic_property(const QString &name, wl_array *value)
{
    QByteArray data = QByteArray::fromRawData(static_cast<char *>(value->data), value->size);

    QVariant variantValue;
    QDataStream ds(data);
    ds >> variantValue;

    m_properties.insert(name, variantValue);

    QWaylandNativeInterface *nativeInterface = static_cast<QWaylandNativeInterface *>(
        QGuiApplication::platformNativeInterface());
    nativeInterface->emitWindowPropertyChanged(m_window, name);
}

void QWaylandExtendedSurface::extended_surface_close()
{
    QWindowSystemInterface::handleCloseEvent(m_window->window());
}

Qt::WindowFlags QWaylandExtendedSurface::setWindowFlags(Qt::WindowFlags flags)
{
    uint wlFlags = 0;

    if (flags & Qt::WindowStaysOnTopHint) wlFlags |= QT_EXTENDED_SURFACE_WINDOWFLAG_STAYSONTOP;
    if (flags & Qt::WindowOverridesSystemGestures) wlFlags |= QT_EXTENDED_SURFACE_WINDOWFLAG_OVERRIDESSYSTEMGESTURES;
    if (flags & Qt::BypassWindowManagerHint) wlFlags |= QT_EXTENDED_SURFACE_WINDOWFLAG_BYPASSWINDOWMANAGER;

    set_window_flags(wlFlags);

    return flags & (Qt::WindowStaysOnTopHint | Qt::WindowOverridesSystemGestures | Qt::BypassWindowManagerHint);
}

QT_END_NAMESPACE
