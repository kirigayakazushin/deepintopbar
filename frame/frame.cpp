#include "frame.h"
#include <QDesktopWidget>
#include <QApplication>
#include <DPlatformWindowHandle>
#include <QX11Info>
#include <xcb/xcb.h>
#include <xcb/xcb_ewmh.h>

DWIDGET_USE_NAMESPACE

Frame::Frame(QWidget *parent) : DBlurEffectWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowDoesNotAcceptFocus);
    setAttribute(Qt::WA_TranslucentBackground);

    setBlendMode(DBlurEffectWidget::InWindowBlend);
    setMaskColor(DBlurEffectWidget::LightColor);

    DPlatformWindowHandle *handle = new DPlatformWindowHandle(this);
    handle->setBorderWidth(0);
    handle->setWindowRadius(0);
    handle->setEnableSystemMove(true);
    handle->setEnableSystemResize(true);
    handle->setEnableBlurWindow(true);

    QRect screen = QApplication::desktop()->screenGeometry(QApplication::desktop()->primaryScreen());
    resize(screen.width(), 25);
    move(screen.x(), 0);
}

void Frame::registerDesktop()
{
    //register type to Desktop

    xcb_ewmh_connection_t m_ewmh_connection;
    xcb_intern_atom_cookie_t * cookie = xcb_ewmh_init_atoms(QX11Info::connection(), &m_ewmh_connection);
    xcb_ewmh_init_atoms_replies(&m_ewmh_connection, cookie, NULL);

    // clear strut partial
    xcb_ewmh_wm_strut_partial_t strutPartial;
    memset(&strutPartial, 0, sizeof(xcb_ewmh_wm_strut_partial_t));
    xcb_ewmh_set_wm_strut_partial(&m_ewmh_connection, winId(), strutPartial);

    //s set desktop type
    xcb_ewmh_init_atoms_replies(&m_ewmh_connection, cookie, NULL);

    xcb_atom_t atoms[1];
    atoms[0] = m_ewmh_connection._NET_WM_WINDOW_TYPE_DESKTOP;
    xcb_ewmh_set_wm_window_type(&m_ewmh_connection, winId(), 1, atoms);

    // set strct partial
    xcb_ewmh_wm_strut_partial_t strut_partial;
    memset(&strut_partial, 0, sizeof(xcb_ewmh_wm_strut_partial_t));

    QRect screen = QApplication::desktop()->screenGeometry(QApplication::desktop()->primaryScreen());
    const QPoint p(screen.x(), 0);
    const QRect r = QRect(p, size());

    strut_partial.top = r.bottom() + 2;
    strut_partial.top_start_x = r.left();
    strut_partial.top_end_x = r.right();

    xcb_ewmh_set_wm_strut_partial(&m_ewmh_connection, winId(), strut_partial);
}