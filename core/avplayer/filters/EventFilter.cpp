/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "EventFilter.h"

// Qt includes

#include <QApplication>
#include <QUrl>
#include <QEvent>
#include <QFileDialog>
#include <QGraphicsObject>
#include <QGraphicsSceneContextMenuEvent>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QWindowStateChangeEvent>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "QtAVWidgets_Global.h"
#include "AVPlayerCore.h"
#include "AudioOutput.h"
#include "VideoCapture.h"
#include "VideoRenderer.h"
#include "digikam_debug.h"

using namespace QtAV;

namespace AVPlayer
{

// TODO: watch main window

EventFilter::EventFilter(QtAV::AVPlayerCore* const player)
    : QObject(player),
      menu   (nullptr)
{
}

EventFilter::~EventFilter()
{
    if (menu)
    {
        delete menu;
        menu = nullptr;
    }
}

void EventFilter::openLocalFile()
{
    QString file = QFileDialog::getOpenFileName(nullptr, i18nc("@title:window", "Open a Video"));

    if (file.isEmpty())
        return;

    QtAV::AVPlayerCore* const player = static_cast<QtAV::AVPlayerCore*>(parent());
    player->play(file);
}

void EventFilter::openUrl()
{
    QString url = QInputDialog::getText(nullptr,
                                        i18nc("@title:window", "Open an Url"),
                                        i18nc("@info", "Url"));

    if (url.isEmpty())
        return;

    QtAV::AVPlayerCore* const player = static_cast<QtAV::AVPlayerCore*>(parent());
    player->play(url);
}

void EventFilter::about()
{
    QtAV::about();
}

void EventFilter::aboutFFmpeg()
{
    QtAV::aboutFFmpeg();
}

void EventFilter::help()
{
    static QString help = QLatin1String("<qt><h4>")                                                                                    +
        i18nc("@info", "Drag and drop a file to player\n")                               + QLatin1String("</h4><p>") +
        i18nc("@info", "A: switch aspect ratio")                                         + QLatin1String("</p><p>")  +
        i18nc("@info", "Double click to switch fullscreen")                              + QLatin1String("</p><p>")  +
        i18nc("@info", "Shortcut:\n")                                                    + QLatin1String("</p><p>")  +
        i18nc("@info", "Space: pause/continue\n")                                        + QLatin1String("</p><p>")  +
        i18nc("@info", "F: fullscreen on/off\n")                                         + QLatin1String("</p><p>")  +
        i18nc("@info", "T: stays on top on/off\n")                                       + QLatin1String("</p><p>")  +
        i18nc("@info", "N: show next frame. Continue the playing by pressing 'Space'\n") + QLatin1String("</p><p>")  +
        i18nc("@info", "Ctrl+O: open a file\n")                                          + QLatin1String("</p><p>")  +
        i18nc("@info", "O: On Screen Display\n")                                         + QLatin1String("</p><p>")  +
        i18nc("@info", "P: replay\n")                                                    + QLatin1String("</p><p>")  +
        i18nc("@info", "Q/ESC: quit\n")                                                  + QLatin1String("</p><p>")  +
        i18nc("@info", "S: stop\n")                                                      + QLatin1String("</p><p>")  +
        i18nc("@info", "R: rotate 90 degrees")                                           + QLatin1String("</p><p>")  +
        i18nc("@info", "M: mute on/off\n")                                               + QLatin1String("</p><p>")  +
        i18nc("@info", "C: capture video")                                               + QLatin1String("</p><p>")  +
        i18nc("@info", "Up/Down: volume +/-\n")                                          + QLatin1String("</p><p>")  +
        i18nc("@info", "Ctrl+Up/Down: speed +/-\n")                                      + QLatin1String("</p><p>")  +
        i18nc("@info", "-&gt;/&lt;-: seek forward/backward\n")                                                             +
        QLatin1String("</qt>");

    QMessageBox::about(nullptr, i18nc("@title:window", "Help"), help);
}

bool EventFilter::eventFilter(QObject* watched, QEvent* event)
{
    Q_UNUSED(watched);

    QtAV::AVPlayerCore* const player = static_cast<QtAV::AVPlayerCore*>(parent());

    if (!player || !player->renderer() || !player->renderer()->widget())
        return false;

    if (qobject_cast<QWidget*>(watched) != player->renderer()->widget())
    {
        return false;
    }

#ifndef QT_NO_DYNAMIC_CAST // dynamic_cast is defined as a macro to force a compile error

    if (player->renderer() != dynamic_cast<VideoRenderer*>(watched))
    {
/*
       return false;
*/
    }

#endif

    QEvent::Type type = event->type();

    switch (type)
    {
        case QEvent::KeyPress:
        {
            QKeyEvent* const key_event      = static_cast<QKeyEvent*>(event);
            int key                         = key_event->key();
            Qt::KeyboardModifiers modifiers = key_event->modifiers();

            switch (key)
            {
                case Qt::Key_0:
                {
                    player->seek(0LL);

                    break;
                }

                case Qt::Key_C:     // capture
                {
                    player->videoCapture()->capture();

                    break;
                }

                case Qt::Key_N:     // check playing?
                {
                    player->stepForward();

                    break;
                }

                case Qt::Key_B:
                {
                    player->stepBackward();

                    break;
                }

                case Qt::Key_P:
                {
                    player->stop();
                    player->play();

                    break;
                }

                case Qt::Key_Q:
                case Qt::Key_Escape:
                {
                    qApp->quit();

                    break;
                }

                case Qt::Key_S:
                {
                    player->stop(); // check playing?

                    break;
                }

                case Qt::Key_Space:
                {
                    // check playing?

                    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()
                        << QString::asprintf("isPaused = %d",
                            player->isPaused());

                    player->pause(!player->isPaused());

                    break;
                }

                case Qt::Key_F:
                {
                    // TODO: move to gui

                    QWidget* const w = qApp->activeWindow();

                    if (!w)
                        return false;

                    w->setWindowState(w->windowState() ^ Qt::WindowFullScreen);

                    break;
                }

                case Qt::Key_U:
                {
                    player->setNotifyInterval(player->notifyInterval() + 100);

                    break;
                }

                case Qt::Key_D:
                {
                    player->setNotifyInterval(player->notifyInterval() - 100);

                    break;
                }

                case Qt::Key_Up:
                {
                    AudioOutput* const ao = player->audio();

                    if (modifiers == Qt::ControlModifier)
                    {
                        qreal s = player->speed();

                        if (s < 1.4)
                            s += 0.02;
                        else
                            s += 0.05;

                        if (qAbs<qreal>(s - 1.0) <= 0.01)
                            s = 1.0;

                        player->setSpeed(s);

                        return true;
                    }

                    if (ao && ao->isAvailable())
                    {
                        qreal v = player->audio()->volume();

                        if      (v > 0.5)
                            v += 0.1;
                        else if (v > 0.1)
                            v += 0.05;
                        else
                            v += 0.025;

                        player->audio()->setVolume(v);

                        qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()
                            << QString::asprintf("vol = %.3f",
                                player->audio()->volume());
                    }

                    break;
                }

                case Qt::Key_Down:
                {
                    AudioOutput* const ao = player->audio();

                    if (modifiers == Qt::ControlModifier)
                    {
                        qreal s = player->speed();

                        if (s < 1.4)
                            s -= 0.02;
                        else
                            s -= 0.05;

                        if (qAbs<qreal>(s - 1.0) <= 0.01)
                            s = 1.0;

                        s = qMax<qreal>(s, 0.0);
                        player->setSpeed(s);

                        return true;
                    }

                    if (ao && ao->isAvailable())
                    {
                        qreal v = player->audio()->volume();

                        if      (v > 0.5)
                           v -= 0.1;
                        else if (v > 0.1)
                            v -= 0.05;
                        else
                            v -= 0.025;

                        player->audio()->setVolume(v);

                        qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()
                            << QString::asprintf("vol = %.3f",
                                player->audio()->volume());
                    }

                    break;
                }

                case Qt::Key_O:
                {
                    if (modifiers == Qt::ControlModifier)
                    {
                        // TODO: Q_EMIT a signal so we can use custome dialogs?

                        openLocalFile();
                    }
                    else /* if (m == Qt::NoModifier) */
                    {
                        Q_EMIT showNextOSD();
                    }

                    break;
                }

                case Qt::Key_Left:
                {
                    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()
                        << QString::asprintf("<-");

                    player->setSeekType(key_event->isAutoRepeat() ? KeyFrameSeek : AccurateSeek);
                    player->seekBackward();

                    break;
                }

                case Qt::Key_Right:
                {
                    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()
                        << QString::asprintf("->");

                    player->setSeekType(key_event->isAutoRepeat() ? KeyFrameSeek : AccurateSeek);
                    player->seekForward();

                    break;
                }

                case Qt::Key_M:
                {
                    if (player->audio())
                    {
                        player->audio()->setMute(!player->audio()->isMute());
                    }

                    break;
                }

                case Qt::Key_A:
                {
                    VideoRenderer* const renderer       = player->renderer();
                    QtAV::OutAspectRatioMode r          = renderer->outAspectRatioMode();
                    renderer->setOutAspectRatioMode(QtAV::OutAspectRatioMode(((int)r + 1) % 2));

                    break;
                }

                case Qt::Key_R:
                {
                    VideoRenderer* const renderer = player->renderer();
                    renderer->setOrientation(renderer->orientation() + 90);

                    qCDebug(DIGIKAM_AVPLAYER_LOG).noquote()
                        << QString::asprintf("orientation: %d",
                            renderer->orientation());

                    break;
                }

                case Qt::Key_T:
                {
                    QWidget* const w = qApp->activeWindow();

                    if (!w)
                        return false;

                    w->setWindowFlags(w->window()->windowFlags() ^ Qt::WindowStaysOnTopHint);

                    // call setParent() when changing the flags, causing the widget to be hidden

                    w->show();

                    break;
                }

                case Qt::Key_F1:
                {
                    help();

                    break;
                }

                default:
                {
                    return false;
                }
            }

            break;
        }

        case QEvent::DragEnter:
        case QEvent::DragMove:
        {
            QDropEvent* const e = static_cast<QDropEvent*>(event);

            if (e->mimeData()->hasUrls())
                e->acceptProposedAction();
            else
                e->ignore();

            break;
        }

        case QEvent::Drop:
        {
            QDropEvent* const e = static_cast<QDropEvent*>(event);

            if (e->mimeData()->hasUrls())
            {
                QString path = e->mimeData()->urls().first().toLocalFile();
                player->stop();
                player->play(path);
                e->acceptProposedAction();
            }
            else
            {
                e->ignore();
            }

            break;
        }

        case QEvent::MouseButtonDblClick:
        {
            QMouseEvent* const me   = static_cast<QMouseEvent*>(event);
            Qt::MouseButton mbt     = me->button();
            QWidget* const mpWindow = static_cast<QWidget*>(player->parent());

            if (mbt == Qt::LeftButton)
            {
                if (Qt::WindowFullScreen == mpWindow->windowState())
                {
                    mpWindow->setWindowState(mpWindow->windowState() ^ Qt::WindowFullScreen);
                }
                else
                {
                    mpWindow->showFullScreen();     // krazy:exclude=qmethods
                }
            }

            break;
        }

        case QEvent::GraphicsSceneContextMenu:
        {
            QGraphicsSceneContextMenuEvent* const e = static_cast<QGraphicsSceneContextMenuEvent*>(event);
            showMenu(e->screenPos());

            break;
        }

        case QEvent::ContextMenu:
        {
            QContextMenuEvent* const e = static_cast<QContextMenuEvent*>(event);
            showMenu(e->globalPos());

            break;
        }

        default:
        {
            return false;
        }
    }

    return true; // false: for text input
}

void EventFilter::showMenu(const QPoint& p)
{
    if (!menu)
    {
        menu = new QMenu();
        menu->addAction(i18nc("@action: open new media",  "Open"),     this, SLOT(openLocalFile()));
        menu->addAction(i18nc("@action: open new stream", "Open Url"), this, SLOT(openUrl()));
        menu->addSeparator();
        menu->addAction(i18nc("@action: about AVPlayer",  "About"),    this, SLOT(about()));
        menu->addAction(i18nc("@action: AVPlayer help",   "Help"),     this, SLOT(help()));
        menu->addSeparator();
    }

    menu->exec(p);
}

// -----------------------------------------------------------------------------

WindowEventFilter::WindowEventFilter(QWidget* const window)
    : QObject (window),
      mpWindow(window)
{
}

bool WindowEventFilter::eventFilter(QObject* watched, QEvent* event)
{
    if (watched != mpWindow)
        return false;

    if (event->type() == QEvent::WindowStateChange)
    {
        QWindowStateChangeEvent* const e = static_cast<QWindowStateChangeEvent*>(event);
        mpWindow->updateGeometry();

        if (mpWindow->windowState().testFlag(Qt::WindowFullScreen) ||
            e->oldState().testFlag(Qt::WindowFullScreen))
        {
            Q_EMIT fullscreenChanged();
        }

        return false;
    }

    if (event->type() ==  QEvent::MouseButtonPress)
    {
        QMouseEvent* const me = static_cast<QMouseEvent*>(event);
        Qt::MouseButton mbt   = me->button();

        if (mbt == Qt::LeftButton)
        {

#if (QT_VERSION > QT_VERSION_CHECK(5, 99, 0))

            gMousePos = me->globalPosition().toPoint();

#else

            gMousePos = me->globalPos();

#endif
            iMousePos = me->pos();
        }

        return false;
    }

    if (event->type() == QEvent::MouseButtonRelease)
    {
        QMouseEvent* const me = static_cast<QMouseEvent*>(event);
        Qt::MouseButton mbt   = me->button();

        if (mbt != Qt::LeftButton)
            return false;

        iMousePos = QPoint();
        gMousePos = QPoint();

        return false;
    }

    if (event->type() == QEvent::MouseMove)
    {
        if (iMousePos.isNull() || gMousePos.isNull())
            return false;

        QMouseEvent* const me = static_cast<QMouseEvent*>(event);
        int x                 = mpWindow->pos().x();
        int y                 = mpWindow->pos().y();

#if (QT_VERSION > QT_VERSION_CHECK(5, 99, 0))

        int dx                = me->globalPosition().toPoint().x() - gMousePos.x();
        int dy                = me->globalPosition().toPoint().y() - gMousePos.y();
        gMousePos             = me->globalPosition().toPoint();

#else

        int dx                = me->globalPos().x() - gMousePos.x();
        int dy                = me->globalPos().y() - gMousePos.y();
        gMousePos             = me->globalPos();

#endif


        mpWindow->move(x + dx, y + dy);

        return false;
    }

    return false;
}

} // namespace AVPlayer
