/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the config.tests of the Qt Toolkit.
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

#ifndef QWAYLANDWINDOW_H
#define QWAYLANDWINDOW_H

#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>
#include <QtGui/QIcon>

#include <qpa/qplatformwindow.h>

#include <QtWaylandClient/private/qwaylanddisplay_p.h>

#include <QtWaylandClient/private/qwayland-wayland.h>

struct wl_egl_window;

QT_BEGIN_NAMESPACE

class QWaylandDisplay;
class QWaylandBuffer;
class QWaylandShellSurface;
class QWaylandExtendedSurface;
class QWaylandSubSurface;
class QWaylandDecoration;

class Q_WAYLAND_CLIENT_EXPORT QWaylandWindowConfigure
{
public:
    QWaylandWindowConfigure()
        : width(0)
        , height(0)
        , edges(0)
    { }

    void clear()
    { width = height = edges = 0; }

    bool isEmpty() const
    { return !height || !width; }

    int width;
    int height;
    uint32_t edges;
};

class Q_WAYLAND_CLIENT_EXPORT QWaylandWindow : public QObject, public QPlatformWindow, public QtWayland::wl_surface
{
    Q_OBJECT
public:
    enum WindowType {
        Shm,
        Egl
    };

    QWaylandWindow(QWindow *window);
    ~QWaylandWindow();

    virtual WindowType windowType() const = 0;
    WId winId() const;
    void setVisible(bool visible);
    void setParent(const QPlatformWindow *parent);

    void setWindowTitle(const QString &title);

    inline QIcon windowIcon() const;
    void setWindowIcon(const QIcon &icon);

    void setGeometry(const QRect &rect);

    void configure(uint32_t edges, int32_t width, int32_t height);

    using QtWayland::wl_surface::attach;
    void attach(QWaylandBuffer *buffer, int x, int y);
    void attachOffset(QWaylandBuffer *buffer);
    QWaylandBuffer *attached() const;
    QPoint attachOffset() const;

    using QtWayland::wl_surface::damage;
    void damage(const QRect &rect);

    void waitForFrameSync();

    QMargins frameMargins() const;

    static QWaylandWindow *fromWlSurface(::wl_surface *surface);

    QWaylandShellSurface *shellSurface() const;
    QWaylandExtendedSurface *extendedWindow() const;
    QWaylandSubSurface *subSurfaceWindow() const;

    void handleContentOrientationChange(Qt::ScreenOrientation orientation);

    void setWindowState(Qt::WindowState state);
    void setWindowFlags(Qt::WindowFlags flags);

    void raise() Q_DECL_OVERRIDE;
    void lower() Q_DECL_OVERRIDE;

    void requestActivateWindow() Q_DECL_OVERRIDE;

    QWaylandDecoration *decoration() const;
    void setDecoration(QWaylandDecoration *decoration);


    void handleMouse(QWaylandInputDevice *inputDevice,
                     ulong timestamp,
                     const QPointF & local,
                     const QPointF & global,
                     Qt::MouseButtons b,
                     Qt::KeyboardModifiers mods);
    void handleMouseEnter(QWaylandInputDevice *inputDevice);
    void handleMouseLeave(QWaylandInputDevice *inputDevice);

    bool createDecoration();

    inline bool isMaximized() const { return mState == Qt::WindowMaximized; }
    inline bool isFullscreen() const { return mState == Qt::WindowFullScreen; }

    void setMouseCursor(QWaylandInputDevice *device, Qt::CursorShape shape);
    void restoreMouseCursor(QWaylandInputDevice *device);

    QWaylandWindow *transientParent() const;

    QMutex *resizeMutex() { return &mResizeLock; }
    void doResize();
    void setCanResize(bool canResize);

    bool setMouseGrabEnabled(bool grab);
    static QWaylandWindow *mouseGrab() { return mMouseGrab; }

public slots:
    void requestResize();

protected:
    QWaylandScreen *mScreen;
    QWaylandDisplay *mDisplay;
    QWaylandShellSurface *mShellSurface;
    QWaylandExtendedSurface *mExtendedWindow;
    QWaylandSubSurface *mSubSurfaceWindow;

    QWaylandDecoration *mWindowDecoration;
    bool mMouseEventsInContentArea;
    Qt::MouseButtons mMousePressedInContentArea;
    Qt::CursorShape m_cursorShape;

    QWaylandBuffer *mBuffer;
    WId mWindowId;
    bool mWaitingForFrameSync;
    struct wl_callback *mFrameCallback;
    QWaitCondition mFrameSyncWait;

    QMutex mResizeLock;
    QWaylandWindowConfigure mConfigure;
    bool mRequestResizeSent;
    bool mCanResize;
    bool mResizeDirty;
    bool mResizeAfterSwap;

    bool mSentInitialResize;
    QPoint mOffset;

    QIcon mWindowIcon;
    QWaylandInputDevice *mMouseDevice;
    int mMouseSerial;

    Qt::WindowState mState;

private:
    bool setWindowStateInternal(Qt::WindowState flags);

    void handleMouseEventWithDecoration(QWaylandInputDevice *inputDevice,
                                        ulong timestamp,
                                        const QPointF & local,
                                        const QPointF & global,
                                        Qt::MouseButtons b,
                                        Qt::KeyboardModifiers mods);

    static const wl_callback_listener callbackListener;
    static void frameCallback(void *data, struct wl_callback *wl_callback, uint32_t time);

    static QMutex mFrameSyncMutex;
    static QWaylandWindow *mMouseGrab;
};

inline QIcon QWaylandWindow::windowIcon() const
{
    return mWindowIcon;
}

inline QPoint QWaylandWindow::attachOffset() const
{
    return mOffset;
}

QT_END_NAMESPACE

#endif // QWAYLANDWINDOW_H
