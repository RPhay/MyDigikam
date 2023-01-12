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

#include "playerwindow.h"

// Qt includes

#include <QPushButton>
#include <QSlider>
#include <QLabel>
#include <QLayout>
#include <QMessageBox>
#include <QFileDialog>

// Local includes

#include "GraphicsItemRenderer.h"
#include "WidgetRenderer.h"
#include "GLWidgetRenderer2.h"
#include "VideoPreviewWidget.h"

using namespace QtAV;

PlayerWindow::PlayerWindow(QWidget* const parent)
    : QWidget  (parent),
      m_preview(nullptr)
{
    QtAV::Widgets::registerRenderers();
    setWindowTitle(QString::fromLatin1("QtAV Simple Player Example"));
    m_player              = new AVPlayerCore(this);
    QVBoxLayout* const vl = new QVBoxLayout();
    setLayout(vl);
    m_vo                  = new VideoOutput(this);

    if (!m_vo->widget())
    {
        QMessageBox::warning(nullptr,
                             QString::fromLatin1("QtAV error"),
                             QString::fromLatin1("Can not create video renderer"));

        return;
    }

    m_player->setRenderer(m_vo);
    vl->addWidget(m_vo->widget());
    m_slider              = new QSlider();
    m_slider->setOrientation(Qt::Horizontal);

    connect(m_slider, SIGNAL(sliderMoved(int)),
            this, SLOT(seek(int)));

    connect(m_player, SIGNAL(positionChanged(qint64)),
            this, SLOT(updateSlider()));

    connect(m_player, SIGNAL(started()),
            this, SLOT(updateSlider()));

    vl->addWidget(m_slider);
    QHBoxLayout* const hb = new QHBoxLayout();
    vl->addLayout(hb);
    m_openBtn             = new QPushButton(QLatin1String("Open"));
    m_captureBtn          = new QPushButton(QLatin1String("Capture video frame"));
    hb->addWidget(m_openBtn);
    hb->addWidget(m_captureBtn);

    m_preview             = new QLabel(QLatin1String("Capture preview"));
    m_preview->setFixedSize(200, 150);
    hb->addWidget(m_preview);

    connect(m_openBtn, SIGNAL(clicked()),
            this, SLOT(openMedia()));

    connect(m_captureBtn, SIGNAL(clicked()),
            this, SLOT(capture()));

    connect(m_player->videoCapture(), SIGNAL(imageCaptured(QImage)),
            this, SLOT(updatePreview(QImage)));

    connect(m_player->videoCapture(), SIGNAL(saved(QString)),
            this, SLOT(onCaptureSaved(QString)));

    connect(m_player->videoCapture(), SIGNAL(failed()),
            this, SLOT(onCaptureError()));
}

void PlayerWindow::openMedia()
{
    QString file = QFileDialog::getOpenFileName(nullptr,
                                                QLatin1String("Open a video"));

    if (file.isEmpty())
        return;

    m_player->play(file);
}

void PlayerWindow::seek(int pos)
{
    if (!m_player->isPlaying())
        return;

    m_player->seek(pos * 1000LL); // to msecs
}

void PlayerWindow::updateSlider()
{
    m_slider->setRange(0, int(m_player->duration() / 1000LL));
    m_slider->setValue(int(m_player->position() / 1000LL));
}

void PlayerWindow::updatePreview(const QImage &image)
{
    m_preview->setPixmap(QPixmap::fromImage(image).scaled(m_preview->size()));
}

void PlayerWindow::capture()
{
/*
    m_player->captureVideo();
*/
    m_player->videoCapture()->capture();
}

void PlayerWindow::onCaptureSaved(const QString &path)
{
    setWindowTitle(QLatin1String("saved to: ") + path);
}

void PlayerWindow::onCaptureError()
{
    QMessageBox::warning(nullptr, QString::fromLatin1("QtAV video capture"),
                         QLatin1String("Failed to capture video frame"));
}
