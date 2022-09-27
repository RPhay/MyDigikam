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

#include "videogroup.h"

// Qt includes

#include <QApplication>
#include <QEvent>
#include <QFileDialog>
#include <QGridLayout>
#include <QUrl>

// Local includes

#include "AudioOutput.h"
#include "AVPlayerCore.h"
#include "GraphicsItemRenderer.h"
#include "WidgetRenderer.h"
#include "GLWidgetRenderer2.h"
#include "VideoPreviewWidget.h"

#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
#   include <QDesktopWidget>
#   define DESKTOP_RECT() qApp->desktop()->rect()
#else
#   define DESKTOP_RECT() qApp->primaryScreen()->availableGeometry()
#endif

using namespace QtAV;

VideoGroup::VideoGroup(QObject* const parent)
    : QObject(parent)
{
    mpPlayer    = new AVPlayerCore(this);
/*
    mpPlayer->setPlayerEventFilter(0);
*/
    mpBar       = new QWidget(0, Qt::WindowStaysOnTopHint);
    mpBar->setMaximumSize(400, 60);
    mpBar->show();
    mpBar->setLayout(new QHBoxLayout);
    mpOpen      = new QPushButton(QLatin1String("Open"));
    mpPlay      = new QPushButton(QLatin1String("Play"));
    mpStop      = new QPushButton(QLatin1String("Stop"));
    mpPause     = new QPushButton(QLatin1String("Pause"));
    mpPause->setCheckable(true);
    mpAdd       = new QPushButton(QString::fromLatin1("+"));
    mpRemove    = new QPushButton(QString::fromLatin1("-"));
    mp1Window   = new QPushButton(QLatin1String("Single Window"));
    mp1Window->setCheckable(true);
    mp1Frame    = new QPushButton(QLatin1String("Single Frame"));
    mp1Frame->setCheckable(true);
    mp1Frame->setChecked(true);
    mpFrameless = new QPushButton(QLatin1String("no window frame"));
    mpFrameless->setCheckable(true);

    connect(mpOpen, SIGNAL(clicked()),
            this, SLOT(openLocalFile()));

    connect(mpPlay, SIGNAL(clicked()),
            mpPlayer, SLOT(play()));

    connect(mpStop, SIGNAL(clicked()),
            mpPlayer, SLOT(stop()));

    connect(mpPause, SIGNAL(toggled(bool)),
            mpPlayer, SLOT(pause(bool)));

    connect(mpAdd, SIGNAL(clicked()),
            this, SLOT(addRenderer()));

    connect(mpRemove, SIGNAL(clicked()),
            this, SLOT(removeRenderer()));

    connect(mp1Window, SIGNAL(toggled(bool)),
            this, SLOT(setSingleWindow(bool)));

    connect(mp1Frame, SIGNAL(toggled(bool)),
            this, SLOT(toggleSingleFrame(bool)));

    connect(mpFrameless, SIGNAL(toggled(bool)),
            this, SLOT(toggleFrameless(bool)));

    mpBar->layout()->addWidget(mpOpen);
    mpBar->layout()->addWidget(mpPlay);
    mpBar->layout()->addWidget(mpStop);
    mpBar->layout()->addWidget(mpPause);
    mpBar->layout()->addWidget(mpAdd);
    mpBar->layout()->addWidget(mpRemove);
    mpBar->layout()->addWidget(mp1Window);
    mpBar->layout()->addWidget(mp1Frame);
/*
    mpBar->layout()->addWidget(mpFrameless);
*/
    mpBar->resize(200, 25);
}

VideoGroup::~VideoGroup()
{
    delete view;
    delete mpBar;
}

void VideoGroup::setSingleWindow(bool s)
{
    m1Window   = s;
    mRenderers = mpPlayer->videoOutputs();

    if (mRenderers.isEmpty())
        return;

    if (!s)
    {
        if (!view)
            return;

        Q_FOREACH (VideoRenderer* const vo, mRenderers)
        {
            view->layout()->removeWidget(vo->widget());
            vo->widget()->setParent(0);
            Qt::WindowFlags wf = vo->widget()->windowFlags();

            if (mFrameless)
            {
                wf &= ~Qt::FramelessWindowHint;
            }
            else
            {
                wf |= Qt::FramelessWindowHint;
            }

            vo->widget()->setWindowFlags(wf);
            vo->widget()->show();
        }

        delete view;
        view = 0;
    }
    else
    {
        if (view)
            return;

        view                      = new QWidget;
        view->resize(DESKTOP_RECT().size());
        QGridLayout* const layout = new QGridLayout;
        layout->setSizeConstraint(QLayout::SetMaximumSize);
        layout->setSpacing(1);
        layout->setContentsMargins(QMargins());
        view->setLayout(layout);

        for (int i = 0 ; i < mRenderers.size() ; ++i)
        {
            int x = i / cols();
            int y = i % cols();
            ((QGridLayout*)view->layout())->addWidget(mRenderers.at(i)->widget(), x, y);
        }

        view->show();
        mRenderers.last()->widget()->showFullScreen();      // krazy:exclude=qmethods
    }
}

void VideoGroup::toggleSingleFrame(bool s)
{
    if (m1Frame == s)
        return;

    m1Frame = s;
    updateROI();
}

void VideoGroup::toggleFrameless(bool f)
{
    mFrameless = f;

    if (mRenderers.isEmpty())
        return;

    VideoRenderer* const renderer = mRenderers.first();
    Qt::WindowFlags wf            = renderer->widget()->windowFlags();

    if (f)
    {
        wf &= ~Qt::FramelessWindowHint;
    }
    else
    {
        wf |= Qt::FramelessWindowHint;
    }

    Q_FOREACH (VideoRenderer* const rd, mRenderers)
    {
        rd->widget()->setWindowFlags(wf);
    }
}

void VideoGroup::setVideoRendererTypeString(const QString& vt)
{
    vid = vt.toLower();
}

void VideoGroup::setRows(int n)
{
    r = n;
}

void VideoGroup::setCols(int n)
{
    c = n;
}

int VideoGroup::rows() const
{
    return r;
}

int VideoGroup::cols() const
{
    return c;
}

void VideoGroup::show()
{
    for (int i = 0 ; i < r ; ++i)
    {
        for (int j = 0 ; j < c ; ++j)
        {
            addRenderer();
        }
    }
}

void VideoGroup::play(const QString &file)
{
    mpPlayer->play(file);
}

void VideoGroup::openLocalFile()
{
    QString file = QFileDialog::getOpenFileName(nullptr,
                                                QLatin1String("Open a video"));

    if (file.isEmpty())
        return;

    mpPlayer->stop();
    mpPlayer->play(file);
}

void VideoGroup::addRenderer()
{
    VideoRendererId v = VideoRendererId_Widget;

    if      (vid == QLatin1String("gl"))
        v = VideoRendererId_GLWidget2;
    else if (vid == QLatin1String("d2d"))
        v = VideoRendererId_Direct2D;
    else if (vid == QLatin1String("gdi"))
        v = VideoRendererId_GDI;
    else if (vid == QLatin1String("xv"))
        v = VideoRendererId_XV;

    VideoRenderer* const renderer = VideoRenderer::create(v);
    mRenderers                    = mpPlayer->videoOutputs();
    mRenderers.append(renderer);
    renderer->widget()->setAttribute(Qt::WA_DeleteOnClose);
    Qt::WindowFlags wf            = renderer->widget()->windowFlags();

    if (mFrameless)
    {
        wf &= ~Qt::FramelessWindowHint;
    }
    else
    {
        wf |= Qt::FramelessWindowHint;
    }

    renderer->widget()->setWindowFlags(wf);
    int w = (view ? view->frameGeometry().width()  / c : DESKTOP_RECT().width()  / c);
    int h = (view ? view->frameGeometry().height() / r : DESKTOP_RECT().height() / r);
    renderer->widget()->resize(w, h);
    mpPlayer->addVideoRenderer(renderer);
    int i = (mRenderers.size()-1) / cols();
    int j = (mRenderers.size()-1) % cols();

    if (view)
    {
        view->resize(DESKTOP_RECT().size());
        ((QGridLayout*)view->layout())->addWidget(renderer->widget(), i, j);
        view->show();
    }
    else
    {
        renderer->widget()->move(j * w, i * h);
        renderer->widget()->show();
    }

    updateROI();
}

void VideoGroup::removeRenderer()
{
    mRenderers = mpPlayer->videoOutputs();

    if (mRenderers.isEmpty())
        return;

    VideoRenderer* const r = mRenderers.takeLast();
    mpPlayer->removeVideoRenderer(r);

    if (view)
    {
        view->layout()->removeWidget(r->widget());
    }

    delete r;
    updateROI();
}

void VideoGroup::updateROI()
{
    if (mRenderers.isEmpty())
        return;

    if (!m1Frame)
    {
        Q_FOREACH (VideoRenderer* const renderer, mRenderers)
        {
            renderer->setRegionOfInterest(0, 0, 0, 0);
        }

        return;
    }

    int W = (view ? view->frameGeometry().width()  : DESKTOP_RECT().width());
    int H = (view ? view->frameGeometry().height() : DESKTOP_RECT().height());
    int w = W / c;
    int h = H / r;

    for (int i = 0 ; i < mRenderers.size() ; ++i)
    {
        VideoRenderer* const renderer = mRenderers.at(i);
        renderer->setRegionOfInterest(qreal((i % c) * w) / qreal(W), qreal((i / c ) * h) / qreal(H),
                                      qreal(w) / qreal(W), qreal(h) / qreal(H));
    }
}
