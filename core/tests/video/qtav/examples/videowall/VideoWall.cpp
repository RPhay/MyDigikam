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

#include "VideoWall.h"

// Qt includes

#include <QApplication>
#include <QEvent>
#include <QFileDialog>
#include <QGridLayout>
#include <QInputDialog>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QUrl>

// Local includes

#include "AudioOutput.h"
#include "digikam_debug.h"

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#   include <QDesktopWidget>
#   define DESKTOP_RECT() qApp->desktop()->rect()
#else
#   define DESKTOP_RECT() qApp->primaryScreen()->availableGeometry()
#endif

using namespace QtAV;

const int kSyncInterval = 2000;

VideoWall::VideoWall(QObject* const parent)
    : QObject(parent)
{
    QtAV::Widgets::registerRenderers();
    clock = new AVClock(this);
    clock->setClockType(AVClock::ExternalClock);
    view  = new QWidget;

    if (view)
    {
        qCDebug(DIGIKAM_TESTS_LOG).noquote()
            << QString::asprintf("WA_OpaquePaintEvent=%d",
                view->testAttribute(Qt::WA_OpaquePaintEvent));

        view->resize(DESKTOP_RECT().size());
        view->move(QPoint(0, 0));
        view->show();
    }

    view->installEventFilter(this);
}

VideoWall::~VideoWall()
{
    if (menu)
    {
        delete menu;
        menu = 0;
    }

    if (!players.isEmpty())
    {
        Q_FOREACH (AVPlayerCore* const player, players)
        {
            player->stop();
            VideoRenderer* const renderer = player->renderer();

            if (renderer->widget())
            {
                renderer->widget()->close(); // TODO: rename

                if (!renderer->widget()->testAttribute(Qt::WA_DeleteOnClose) &&
                    !renderer->widget()->parent())
                {
                    delete renderer;
                }

                delete player;
            }
        }

        players.clear();
    }

    delete view;
}

void VideoWall::setVideoRendererTypeString(const QString &vt)
{
    vid = vt.toLower();
}

void VideoWall::setRows(int n)
{
    r = n;
}

void VideoWall::setCols(int n)
{
    c = n;
}

int VideoWall::rows() const
{
    return r;
}

int VideoWall::cols() const
{
    return c;
}

void VideoWall::show()
{
    if (!players.isEmpty())
    {
        Q_FOREACH (AVPlayerCore* const player, players)
        {
            player->stop();
            VideoRenderer* const renderer = player->renderer();

            if (renderer->widget())
            {
                renderer->widget()->close();

                if (!renderer->widget()->testAttribute(Qt::WA_DeleteOnClose) &&
                    !renderer->widget()->parent())
                {
                    delete renderer;
                }

                delete player;
            }
        }

        players.clear();
    }

    qCDebug(DIGIKAM_TESTS_LOG).noquote()
        << QString::asprintf("show wall: %d x %d", r, c);

    int w = (view ? view->frameGeometry().width()  / c : DESKTOP_RECT().width()  / c);
    int h = (view ? view->frameGeometry().height() / r : DESKTOP_RECT().height() / r);

    if (view)
    {
        QGridLayout* const layout = new QGridLayout;
        layout->setSizeConstraint(QLayout::SetMaximumSize);
        layout->setSpacing(1);
        layout->setContentsMargins(QMargins());
        view->setLayout(layout);
    }

    VideoRendererId v = VideoRendererId_Widget;

    if      (vid == QLatin1String("gl"))
        v = VideoRendererId_GLWidget2;
    else if (vid == QLatin1String("opengl"))
        v = VideoRendererId_OpenGLWidget;
    else if (vid == QLatin1String("d2d"))
        v = VideoRendererId_Direct2D;
    else if (vid == QLatin1String("gdi"))
        v = VideoRendererId_GDI;
    else if (vid == QLatin1String("x11"))
        v = VideoRendererId_X11;
    else if (vid == QLatin1String("xv"))
        v = VideoRendererId_XV;

    for (int i = 0 ; i < r ; ++i)
    {
        for (int j = 0 ; j < c ; ++j)
        {
            VideoRenderer* const renderer = VideoRenderer::create(v);
            renderer->widget()->setWindowFlags(renderer->widget()->windowFlags() | Qt::FramelessWindowHint);
            renderer->widget()->setAttribute(Qt::WA_DeleteOnClose);
            renderer->widget()->resize(w, h);
            renderer->widget()->move(j * w, i * h);
            AVPlayerCore* const player    = new AVPlayerCore;
            player->setRenderer(renderer);

            connect(player, SIGNAL(started()),
                    this, SLOT(changeClockType()));

            players.append(player);

            if (view)
                ((QGridLayout*)view->layout())->addWidget(renderer->widget(), i, j);
        }
    }
}

void VideoWall::play(const QString& file)
{
    if (players.isEmpty())
        return;

    clock->reset();
    clock->start();

    Q_FOREACH (AVPlayerCore* const player, players)
    {
        player->play(file);
    }

    timer_id = startTimer(kSyncInterval);
}

void VideoWall::stop()
{
    clock->reset();
    killTimer(timer_id);

    Q_FOREACH (AVPlayerCore* const player, players)
    {
        player->stop();         // check playing?
    }
}

void VideoWall::openLocalFile()
{
    QString file = QFileDialog::getOpenFileName(nullptr, QLatin1String("Open a video"));

    if (file.isEmpty())
        return;

    stop();
    clock->reset();
    clock->start();
    timer_id = startTimer(kSyncInterval);

    Q_FOREACH (AVPlayerCore* const player, players)
    {
        player->setFile(file);  // TODO: load all players before play
        player->play();
    }
}

void VideoWall::openUrl()
{
    QString url = QInputDialog::getText(nullptr, QLatin1String("Open an url"), QLatin1String("Url"));

    if (url.isEmpty())
        return;

    stop();
    clock->reset();
    clock->start();
    timer_id = startTimer(kSyncInterval);

    Q_FOREACH (AVPlayerCore* const player, players)
    {
        player->setFile(url);
        player->play();         // TODO: load all players before play
    }
}

void VideoWall::about()
{
    QMessageBox::about(nullptr, QLatin1String("About QtAV"), QString::fromLatin1("<h3>%1</h3>\n\n%2")
                       .arg(QLatin1String("This is a demo for playing and synchronising multiple players"))
                       .arg(aboutQtAV_HTML()));
}

void VideoWall::help()
{
    QMessageBox::about(nullptr, QLatin1String("Help"),
                       QLatin1String("Command line: %1 [-r rows=3] [-c cols=3] path/of/video\n").arg(qApp->applicationFilePath())
                       + QLatin1String("Drag and drop a file to player\n")
                       + QLatin1String("Shortcut:\n")
                       + QLatin1String("Space: pause/continue\n")
                       + QLatin1String("N: show next frame. Continue the playing by pressing 'Space'\n")
                       + QLatin1String("O: open a file\n")
                       + QLatin1String("P: replay\n")
                       + QLatin1String("S: stop\n")
                       + QLatin1String("M: mute on/off\n")
                       + QLatin1String("C: capture video")
                       + QLatin1String("Up/Down: volume +/-\n")
                       + QLatin1String("->/<-: seek forward/backward\n"));
}

bool VideoWall::eventFilter(QObject* watched, QEvent* event)
{
/*
    qCDebug(DIGIKAM_TESTS_LOG).noquote()
        << QString::asprintf("EventFilter::eventFilter to %p", watched);
*/
    Q_UNUSED(watched);

    if (players.isEmpty())
        return false;

    QEvent::Type type = event->type();

    switch (type)
    {
        case QEvent::KeyPress:
        {
/*
            qCDebug(DIGIKAM_TESTS_LOG).noquote()
                << QString::asprintf("Event target = %p %p",
                    watched, player->renderer);
*/
            // avoid receive an event multiple times

            QKeyEvent* const key_event      = static_cast<QKeyEvent*>(event);
            int key                         = key_event->key();
            Qt::KeyboardModifiers modifiers = key_event->modifiers();

            switch (key)
            {
                case Qt::Key_F:
                {
                    QWidget* const w = qApp->activeWindow();

                    if (!w)
                        return false;

                    if (w->isFullScreen())
                        w->showNormal();        // krazy:exclude=qmethods
                    else
                        w->showFullScreen();    // krazy:exclude=qmethods

                    break;
                }

                case Qt::Key_N: // check playing?
                {
                    Q_FOREACH (AVPlayerCore* const player, players)
                    {
                        player->stepForward();
                    }

                    break;
                }

                case Qt::Key_O:
                {
                    if (modifiers == Qt::ControlModifier)
                    {
                        openLocalFile();

                        return true;
                    }
                    else /* if (m == Qt::NoModifier) */
                    {
                        return false;
                    }

                    break;
                }

                case Qt::Key_P:
                {
                    clock->reset();
                    clock->start();

                    Q_FOREACH (AVPlayerCore* const player, players)
                    {
                        player->play();
                    }

                    timer_id = startTimer(kSyncInterval);

                    break;
                }

                case Qt::Key_S:
                {
                    stop();

                    break;
                }

                case Qt::Key_Space: // check playing?
                {
                    clock->pause(!clock->isPaused());

                    Q_FOREACH (AVPlayerCore* const player, players)
                    {
                        player->pause(!player->isPaused());
                    }

                    break;
                }

                case Qt::Key_Up:
                {
                    Q_FOREACH (AVPlayerCore* const player, players)
                    {
                        if (player->audio())
                        {
                            qreal v = player->audio()->volume();

                            if      (v > 0.5)
                                v += 0.1;
                            else if (v > 0.1)
                                v += 0.05;
                            else
                                v += 0.025;

                            player->audio()->setVolume(v);
                        }
                    }

                    break;
                }

                case Qt::Key_Down:
                {
                    Q_FOREACH (AVPlayerCore* const player, players)
                    {
                        if (player->audio())
                        {
                            qreal v = player->audio()->volume();

                            if      (v > 0.5)
                                v -= 0.1;
                            else if (v > 0.1)
                                v -= 0.05;
                            else
                                v -= 0.025;

                            player->audio()->setVolume(v);
                        }
                    }

                    break;
                }

                case Qt::Key_Left:
                {
                    qCDebug(DIGIKAM_TESTS_LOG).noquote()
                        << QString::asprintf("<-");

                    const qint64 newPos = clock->value() * 1000.0 - 2000.0;
                    clock->updateExternalClock(newPos);

                    Q_FOREACH (AVPlayerCore* const player, players)
                    {
                        player->setPosition(newPos);
                    }

                    break;
                }

                case Qt::Key_Right:
                {
                    qCDebug(DIGIKAM_TESTS_LOG).noquote()
                        << QString::asprintf("->");

                    const qint64 newPos = clock->value() * 1000.0 + 2000.0;
                    clock->updateExternalClock(newPos);

                    Q_FOREACH (AVPlayerCore* const player, players)
                    {
                        player->setPosition(newPos);
                    }

                    break;
                }

                case Qt::Key_M:
                {
                    Q_FOREACH (AVPlayerCore* const player, players)
                    {
                        if (player->audio())
                        {
                            player->audio()->setMute(!player->audio()->isMute());
                        }
                    }

                    break;
                }

                default:
                {
                    return false;
                }
            }

            break;
        }

        case QEvent::ContextMenu:
        {
            QContextMenuEvent* const e = static_cast<QContextMenuEvent*>(event);

            if (!menu)
            {
                menu = new QMenu();
                menu->addAction(QLatin1String("Open"),     this, SLOT(openLocalFile()));
                menu->addAction(QLatin1String("Open Url"), this, SLOT(openUrl()));
                menu->addSeparator();
                menu->addAction(QLatin1String("About"),    this, SLOT(about()));
                menu->addAction(QLatin1String("Help"),     this, SLOT(help()));
                menu->addSeparator();
                menu->addAction(QLatin1String("About Qt"), qApp, SLOT(aboutQt()));
            }

            menu->popup(e->globalPos());
            menu->exec();

            break;
        }

        case QEvent::DragEnter:
        case QEvent::DragMove:
        {
            QDropEvent* const e = static_cast<QDropEvent*>(event);
            e->acceptProposedAction();

            break;
        }

        case QEvent::Drop:
        {
            QDropEvent* const e = static_cast<QDropEvent*>(event);
            QString path        = e->mimeData()->urls().first().toLocalFile();
            stop();
            play(path);
            e->acceptProposedAction();

            break;
        }

        default:
        {
            return false;
        }
    }

    return true; // false: for text input
}

void VideoWall::timerEvent(QTimerEvent* e)
{
    if (e->timerId() != timer_id)
    {
        qCDebug(DIGIKAM_TESTS_LOG).noquote()
            << QString::asprintf("Not clock id");

        return;
    }

    if (!clock->isActive())
    {
        qCDebug(DIGIKAM_TESTS_LOG).noquote()
            << QString::asprintf("clock not running");

        return;
    }

    Q_FOREACH (AVPlayerCore* const player, players)
    {
        player->masterClock()->updateExternalClock(*clock);
    }
}

void VideoWall::changeClockType()
{
    AVPlayerCore* const player = qobject_cast<AVPlayerCore*>(sender());
    player->masterClock()->setClockAuto(false);
    player->masterClock()->setClockType(AVClock::ExternalClock);
}
