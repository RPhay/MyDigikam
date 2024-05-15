/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-08-22
 * Description : a generic widget to display a panel to choose
 *               a rectangular image area.
 *
 * SPDX-FileCopyrightText: 1997      by Tim D. Gilman <tdgilman at best dot org>
 * SPDX-FileCopyrightText: 1998-2001 by Mirko Boehm <mirko at kde dot org>
 * SPDX-FileCopyrightText: 2007      by John Layt <john at layt dot net>
 * SPDX-FileCopyrightText: 2004-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "paniconwidget.h"

// C++ includes

#include <cmath>

// Qt includes

#include <QPainter>
#include <QPen>
#include <QTimer>
#include <QToolButton>
#include <QIcon>
#include <QApplication>
#include <QEventLoop>
#include <QKeyEvent>
#include <QScreen>
#include <QWindow>

// KDE includes

#include <klocalizedstring.h>

namespace Digikam
{

class Q_DECL_HIDDEN PanIconFrame::Private
{
public:

    explicit Private(PanIconFrame* const qq);
    ~Private();

public:

    PanIconFrame*        q                      = nullptr;

    /**
     * The result. It is returned from exec() when the popup window closes.
     */
    int                  result                 = 0;    // Rejected

    /**
     * The only subwidget that uses the whole dialog window.
     */
    QWidget*             main                   = nullptr;

    class OutsideClickCatcher;
    OutsideClickCatcher* outsideClickCatcher    = nullptr;
};

// -------------------------------------------------------------------

class Q_DECL_HIDDEN PanIconFrame::Private::OutsideClickCatcher : public QObject
{
    Q_OBJECT

public:

    explicit OutsideClickCatcher(QObject* const parent = nullptr)
        : QObject(parent)
    {
    }

    ~OutsideClickCatcher() override = default;

    void setPopupFrame(PanIconFrame* const popup)
    {
        m_popup = popup;
        popup->installEventFilter(this);
    }

    bool eventFilter(QObject* object, QEvent* event) override
    {
        Q_UNUSED(object);

        // To catch outside clicks, it is sufficient to check for
        // hide events on Qt::Popup type widgets

        if (event->type() == QEvent::Hide && m_popup)
        {
            // do not set d->result here, because the popup
            // hides itself after leaving the event loop.

            Q_EMIT m_popup->leaveModality();
        }

        return false;
    }

public:

    PanIconFrame* m_popup = nullptr;
};

// -------------------------------------------------------------------

PanIconFrame::Private::Private(PanIconFrame* const qq)
    : q                  (qq),
      outsideClickCatcher(new OutsideClickCatcher)
{
    outsideClickCatcher->setPopupFrame(q);
}

PanIconFrame::Private::~Private()
{
    delete outsideClickCatcher;
}

// -------------------------------------------------------------------

PanIconFrame::PanIconFrame(QWidget* const parent)
    : QFrame(parent, Qt::Popup),
      d     (new Private(this))
{
    setFrameStyle(QFrame::Box | QFrame::Raised);
    setMidLineWidth(2);
}

PanIconFrame::~PanIconFrame()
{
    delete d;
}

void PanIconFrame::keyPressEvent(QKeyEvent* e)
{
    if (e->key() == Qt::Key_Escape)
    {
        d->result = 0; // rejected

        Q_EMIT leaveModality();
    }
}

void PanIconFrame::close(int r)
{
    d->result = r;

    Q_EMIT leaveModality();
}

void PanIconFrame::setMainWidget(QWidget* const main)
{
    d->main = main;

    if (d->main)
    {
        resize(d->main->width() + 2 * frameWidth(), d->main->height() + 2 * frameWidth());
    }
}

void PanIconFrame::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);

    if (d->main)
    {
        d->main->setGeometry(frameWidth(), frameWidth(),
                             width() - 2 * frameWidth(), height() - 2 * frameWidth());
    }
}

void PanIconFrame::popup(const QPoint& pos)
{
    // Make sure the whole popup is visible.

    QScreen* screen = qApp->primaryScreen();

    if (QWidget* const widget = nativeParentWidget())
    {
        if (QWindow* const window = widget->windowHandle())
        {
            screen = window->screen();
        }
    }

    QRect desktopGeometry = screen->geometry();

    int x = pos.x();
    int y = pos.y();
    int w = width();
    int h = height();

    if ((x + w) > (desktopGeometry.x() + desktopGeometry.width()))
    {
        x = desktopGeometry.width() - w;
    }

    if ((y + h) > (desktopGeometry.y() + desktopGeometry.height()))
    {
        y = desktopGeometry.height() - h;
    }

    if (x < desktopGeometry.x())
    {
        x = 0;
    }

    if (y < desktopGeometry.y())
    {
        y = 0;
    }

    // Pop the thingy up.

    move(x, y);
    show();
    d->main->setFocus();
}

int PanIconFrame::exec(const QPoint& pos)
{
    popup(pos);
    repaint();
    d->result = 0; // rejected
    QEventLoop eventLoop;

    connect(this, SIGNAL(leaveModality()),
            &eventLoop, SLOT(quit()));

    eventLoop.exec();
    hide();

    return d->result;
}

int PanIconFrame::exec(int x, int y)
{
    return exec(QPoint(x, y));
}

// -------------------------------------------------------------------

class Q_DECL_HIDDEN PanIconWidget::Private
{

public:

    Private() = default;

    bool    moveSelection       = false;
    bool    flicker             = false;

    int     width               = 0;
    int     height              = 0;
    int     zoomedOrgWidth      = 0;
    int     zoomedOrgHeight     = 0;
    int     orgWidth            = 0;
    int     orgHeight           = 0;
    int     xpos                = 0;
    int     ypos                = 0;

    double  zoomFactor          = 1.0;

    QRect   regionSelection;                ///< Original size image selection.
    QTimer* timer               = nullptr;

    QRect   rect;
    QRect   localRegionSelection;           ///< Thumbnail size selection.

    QPixmap pixmap;
};

PanIconWidget::PanIconWidget(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    d->timer = new QTimer(this);
    d->timer->setInterval(800);

    setMouseTracking(true);
    setAttribute(Qt::WA_DeleteOnClose);

    connect(d->timer, SIGNAL(timeout()),
            this, SLOT(slotFlickerTimer()));
}

PanIconWidget::~PanIconWidget()
{
    delete d;
}

QToolButton* PanIconWidget::button()
{
    QToolButton* const btn = new QToolButton;
    btn->setToolButtonStyle(Qt::ToolButtonIconOnly);
    btn->setIcon(QIcon::fromTheme(QLatin1String("transform-move")));
    btn->hide();
    btn->setToolTip(i18n("Pan the image to a region"));

    return btn;
}

void PanIconWidget::setImage(int previewWidth, int previewHeight, const QImage& image)
{
    QSize sz(image.width(), image.height());
    sz.scale(previewWidth, previewHeight, Qt::KeepAspectRatio);
    QImage scaledImg = image.scaled(sz.width(), sz.height(),
                                    Qt::IgnoreAspectRatio, Qt::SmoothTransformation);

    setImage(scaledImg, image.size());
}

void PanIconWidget::setImage(const QImage& scaledPreviewImage, const QSize& fullImageSize)
{
    d->width           = scaledPreviewImage.width();
    d->height          = scaledPreviewImage.height();
    d->orgWidth        = fullImageSize.width();
    d->orgHeight       = fullImageSize.height();
    d->zoomedOrgWidth  = fullImageSize.width();
    d->zoomedOrgHeight = fullImageSize.height();
    d->pixmap          = QPixmap(d->width, d->height);
    d->pixmap.fill(palette().color(QPalette::Window));
    QPainter p(&d->pixmap);
    p.drawImage(0, 0, scaledPreviewImage);

    setFixedSize(d->width, d->height);

    d->rect = QRect(width() / 2 - d->width / 2, height() / 2 - d->height / 2, d->width, d->height);
    update();
}

void PanIconWidget::setImage(int previewWidth, int previewHeight, const DImg& image)
{
    DImg img = DImg(image).smoothScale(previewWidth, previewHeight, Qt::KeepAspectRatio);
    setImage(img.copyQImage(), image.size());
}

void PanIconWidget::slotZoomFactorChanged(double factor)
{
    if (d->zoomFactor == factor)
    {
        return;
    }

    d->zoomFactor      = factor;
    d->zoomedOrgWidth  = (int)(d->orgWidth  * factor);
    d->zoomedOrgHeight = (int)(d->orgHeight * factor);
    update();
}

void PanIconWidget::setRegionSelection(const QRect& regionSelection)
{
    d->regionSelection = regionSelection;
    d->localRegionSelection.setX(d->rect.x() + (int)((float)d->regionSelection.x() *
                                 ((float)d->width / (float)d->zoomedOrgWidth)));

    d->localRegionSelection.setY(d->rect.y() + (int)((float)d->regionSelection.y() *
                                 ((float)d->height / (float)d->zoomedOrgHeight)));

    d->localRegionSelection.setWidth((int)((float)d->regionSelection.width() *
                                           ((float)d->width / (float)d->zoomedOrgWidth)));

    d->localRegionSelection.setHeight((int)((float)d->regionSelection.height() *
                                      ((float)d->height / (float)d->zoomedOrgHeight)));

    update();
}

QRect PanIconWidget::getRegionSelection() const
{
    return (d->regionSelection);
}

void PanIconWidget::setCursorToLocalRegionSelectionCenter()
{
    QCursor::setPos(mapToGlobal(d->localRegionSelection.center()));
}

void PanIconWidget::setCenterSelection()
{
    setRegionSelection(QRect((int)(((float)d->zoomedOrgWidth / 2.0) - ((float)d->regionSelection.width()  / 2.0)),
                            (int)(((float)d->zoomedOrgHeight / 2.0) - ((float)d->regionSelection.height() / 2.0)),
                            d->regionSelection.width(),
                            d->regionSelection.height()));
}

void PanIconWidget::regionSelectionMoved(bool targetDone)
{
    if (targetDone)
    {
        update();
    }

    int x = (int)lround(((float)d->localRegionSelection.x() - (float)d->rect.x() ) *
                        ((float)d->zoomedOrgWidth / (float)d->width));

    int y = (int)lround(((float)d->localRegionSelection.y() - (float)d->rect.y() ) *
                        ((float)d->zoomedOrgHeight / (float)d->height));

    int w = (int)lround((float)d->localRegionSelection.width() *
                        ((float)d->zoomedOrgWidth / (float)d->width));

    int h = (int)lround((float)d->localRegionSelection.height() *
                        ((float)d->zoomedOrgHeight / (float)d->height));

    d->regionSelection.setX(x);
    d->regionSelection.setY(y);
    d->regionSelection.setWidth(w);
    d->regionSelection.setHeight(h);

    Q_EMIT signalSelectionMoved(d->regionSelection, targetDone);
}

void PanIconWidget::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    p.drawPixmap(d->rect.x(), d->rect.y(), d->pixmap);

    // Drawing selection border

    if (d->flicker)
    {
        p.setPen(QPen(Qt::white, 1, Qt::SolidLine));
    }
    else
    {
        p.setPen(QPen(Qt::red, 1, Qt::SolidLine));
    }

    QRect r(d->localRegionSelection);

    // Clamp to widget size. Selection area must always be visible

    if (r.left() < 0)
    {
        r.setLeft(0);
    }

    if (r.top() < 0)
    {
        r.setTop(0);
    }

    if (r.right() > (width() - 2))
    {
        r.setRight(width() - 2);
    }

    if (r.bottom() > (height() - 2))
    {
        r.setBottom(height() - 2);
    }

    p.drawRect(r.x(), r.y(), r.width(), r.height());

    if (d->flicker)
    {
        p.setPen(QPen(Qt::red, 1, Qt::DotLine));
    }
    else
    {
        p.setPen(QPen(Qt::white, 1, Qt::DotLine));
    }

    p.drawRect(r.x(), r.y(), r.width(), r.height());
}

void PanIconWidget::setMouseFocus()
{
    raise();
    d->xpos          = d->localRegionSelection.center().x();
    d->ypos          = d->localRegionSelection.center().y();
    d->moveSelection = true;
    setCursor(Qt::SizeAllCursor);

    Q_EMIT signalSelectionTakeFocus();
}

void PanIconWidget::showEvent(QShowEvent* e)
{
    QWidget::showEvent(e);

    d->timer->start();
}

void PanIconWidget::hideEvent(QHideEvent* e)
{
    QWidget::hideEvent(e);

    d->timer->stop();

    if (d->moveSelection)
    {
        d->moveSelection = false;
        setCursor(Qt::ArrowCursor);

        Q_EMIT signalHidden();
    }
}

void PanIconWidget::mousePressEvent(QMouseEvent* e)
{
    if (
        ((e->button() == Qt::LeftButton) || (e->button() == Qt::MiddleButton)) &&

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

         d->localRegionSelection.contains(e->position().toPoint().x(), e->position().toPoint().y())

#else

         d->localRegionSelection.contains(e->x(), e->y())

#endif

       )
    {

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

        d->xpos          = e->position().toPoint().x();
        d->ypos          = e->position().toPoint().y();

#else

        d->xpos          = e->x();
        d->ypos          = e->y();

#endif

        d->moveSelection = true;
        setCursor(Qt::SizeAllCursor);

        Q_EMIT signalSelectionTakeFocus();
    }
}

void PanIconWidget::mouseMoveEvent(QMouseEvent* e)
{
    if (
        d->moveSelection &&
        ((e->buttons() == Qt::LeftButton) || (e->buttons() == Qt::MiddleButton))
       )
    {

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

        int newxpos = e->position().toPoint().x();
        int newypos = e->position().toPoint().y();

#else

        int newxpos = e->x();
        int newypos = e->y();

#endif

        d->localRegionSelection.translate(newxpos - d->xpos, newypos - d->ypos);

        d->xpos = newxpos;
        d->ypos = newypos;

        // Perform normalization of selection area.

        if (d->localRegionSelection.left() < d->rect.left())
        {
            d->localRegionSelection.moveLeft(d->rect.left());
        }

        if (d->localRegionSelection.top() < d->rect.top())
        {
            d->localRegionSelection.moveTop(d->rect.top());
        }

        if (d->localRegionSelection.right() > d->rect.right())
        {
            d->localRegionSelection.moveRight(d->rect.right());
        }

        if (d->localRegionSelection.bottom() > d->rect.bottom())
        {
            d->localRegionSelection.moveBottom(d->rect.bottom());
        }

        update();
        regionSelectionMoved(false);

        return;
    }
    else
    {

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

        if (d->localRegionSelection.contains(e->position().toPoint().x(), e->position().toPoint().y()))

#else

        if (d->localRegionSelection.contains(e->x(), e->y()))

#endif

        {
            setCursor(Qt::PointingHandCursor);
        }
        else
        {
            setCursor(Qt::ArrowCursor);
        }
    }
}

void PanIconWidget::mouseReleaseEvent(QMouseEvent*)
{
    if (d->moveSelection)
    {
        d->moveSelection = false;
        setCursor(Qt::ArrowCursor);
        regionSelectionMoved(true);
    }
}

void PanIconWidget::slotFlickerTimer()
{
    d->flicker = !d->flicker;
    update();
}

} // namespace Digikam

#include "paniconwidget.moc"

#include "moc_paniconwidget.cpp"
