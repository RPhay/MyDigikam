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

#include "StatisticsView.h"

// Qt includes

#include <QTimerEvent>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLayout>
#include <QHeaderView>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

namespace AVPlayer
{

QStringList getBaseInfoKeys()
{
    return QStringList()
            << QString::fromUtf8("Url")
            << QString::fromUtf8("Format")
            << QString::fromUtf8("Bit rate")
            << QString::fromUtf8("Start time")
            << QString::fromUtf8("Duration")
    ;
}

QStringList getCommonInfoKeys()
{
    return QStringList()
            << QString::fromUtf8("Available")
            << QString::fromUtf8("Codec")
            << QString::fromUtf8("Decoder")
            << QString::fromUtf8("Decoder detail")
            << QString::fromUtf8("Total time")
            << QString::fromUtf8("Start time")
            << QString::fromUtf8("Bit rate")
            << QString::fromUtf8("Frames")
            << QString::fromUtf8("FPS")          // avg_frame_rate. guessed by FFmpeg
    ;
}

QStringList getVideoInfoKeys()
{
    return getCommonInfoKeys()
            << QString::fromUtf8("FPS Now")       // current display fps
            << QString::fromUtf8("Pixel format")
            << QString::fromUtf8("Size")          // w x h
            << QString::fromUtf8("Coded size")    // w x h
            << QString::fromUtf8("GOP size")
    ;
}

QStringList getAudioInfoKeys()
{
    return getCommonInfoKeys()
            << QString::fromUtf8("Sample format")
            << QString::fromUtf8("Sample rate")
            << QString::fromUtf8("Channels")
            << QString::fromUtf8("Channel layout")
            << QString::fromUtf8("Frame size")
    ;
}

QVariantList getBaseInfoValues(const Statistics& s)
{
    return QVariantList()
            << s.url
            << s.format
            << QString::number(s.bit_rate / 1000).append(QString::fromLatin1(" Kb/s"))
            << s.start_time.toString(QString::fromLatin1("HH:mm:ss"))
            << s.duration.toString(QString::fromLatin1("HH:mm:ss"))
    ;
}

QList<QVariant> getVideoInfoValues(const Statistics& s)
{
    return QList<QVariant>()
            << s.video.available
            << QString::fromLatin1("%1 (%2)").arg(s.video.codec).arg(s.video.codec_long)
            << s.video.decoder
            << s.video.decoder_detail
            << s.video.total_time.toString(QString::fromLatin1("HH:mm:ss"))
            << s.video.start_time.toString(QString::fromLatin1("HH:mm:ss"))
            << QString::number(s.video.bit_rate / 1000).append(QString::fromLatin1(" Kb/s"))
            << s.video.frames
            << s.video.frame_rate
            << s.video.frame_rate
            << s.video_only.pix_fmt
            << QString::fromLatin1("%1x%2").arg(s.video_only.width).arg(s.video_only.height)
            << QString::fromLatin1("%1x%2").arg(s.video_only.coded_width).arg(s.video_only.coded_height)
            << s.video_only.gop_size
    ;
}

QList<QVariant> getAudioInfoValues(const Statistics& s)
{
    return QList<QVariant>()
            << s.audio.available
            << QString::fromLatin1("%1 (%2)").arg(s.audio.codec).arg(s.audio.codec_long)
            << s.audio.decoder
            << s.audio.decoder_detail
            << s.audio.total_time.toString(QString::fromLatin1("HH:mm:ss"))
            << s.audio.start_time.toString(QString::fromLatin1("HH:mm:ss"))
            << QString::number(s.audio.bit_rate / 1000).append(QString::fromLatin1(" Kb/s"))
            << s.audio.frames
            << s.audio.frame_rate
            << s.audio_only.sample_fmt
            << QString::number(s.audio_only.sample_rate).append(QString::fromLatin1(" Hz"))
            << s.audio_only.channels
            << s.audio_only.channel_layout
            << s.audio_only.frame_size
    ;
}

StatisticsView::StatisticsView(QWidget* const parent)
    : QDialog       (parent),
      mTimer        (0),
      mpFPS         (nullptr),
      mpAudioBitRate(nullptr),
      mpVideoBitRate(nullptr)
{
    setWindowTitle(QString::fromUtf8("Media Info"));
    setModal(false);
    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    mpView                = new QTreeWidget();
    mpView->setAnimated(true);
    mpView->setHeaderHidden(false);
    mpView->setColumnCount(2);
    mpView->headerItem()->setText(0, QString::fromUtf8("Key"));
    mpView->headerItem()->setText(1, QString::fromUtf8("Value"));
    initBaseItems(&mBaseItems);
    mpView->addTopLevelItems(mBaseItems);
    mpMetadata            = new QTreeWidgetItem();
    mpMetadata->setText(0, QString::fromUtf8("Metadata"));
    mpView->addTopLevelItem(mpMetadata);
    QTreeWidgetItem* item = createNodeWithItems(mpView, QString::fromUtf8("Video"),
                                                getVideoInfoKeys(), &mVideoItems);
    mpFPS                 = item->child(9);

    // mpVideoBitRate =

    mpVideoMetadata = new QTreeWidgetItem(item);
    mpVideoMetadata->setText(0, QString::fromUtf8("Metadata"));
    mpView->addTopLevelItem(item);
    item            = createNodeWithItems(mpView, QString::fromUtf8("Audio"),
                                          getAudioInfoKeys(), &mAudioItems);

    // mpAudioBitRate =

    mpAudioMetadata              = new QTreeWidgetItem(item);
    mpAudioMetadata->setText(0, QString::fromUtf8("Metadata"));
    mpView->addTopLevelItem(item);
    mpView->resizeColumnToContents(0); // call this after content is done

    QPushButton* const btn       = new QPushButton(QString::fromUtf8("Ok"));
    QHBoxLayout* const btnLayout = new QHBoxLayout;
    btnLayout->addStretch();
    btnLayout->addWidget(btn);

    QObject::connect(btn, SIGNAL(clicked()),
                     this, SLOT(accept()));

    QVBoxLayout* const vl        = new QVBoxLayout();
    vl->addWidget(mpView);
    vl->addLayout(btnLayout);
    setLayout(vl);
}

void StatisticsView::setStatistics(const Statistics& s)
{
    mStatistics    = s;
    QVariantList v = getBaseInfoValues(s);
    int i          = 0;

    Q_FOREACH (QTreeWidgetItem* const item, mBaseItems)
    {
        if (item->data(1, Qt::DisplayRole) != v.at(i))
        {
            item->setData(1, Qt::DisplayRole, v.at(i));
        }

        ++i;
    }

    v = getVideoInfoValues(s);
    i = 0;

    Q_FOREACH (QTreeWidgetItem* const item, mVideoItems)
    {
        if (item->data(1, Qt::DisplayRole) != v.at(i))
        {
            item->setData(1, Qt::DisplayRole, v.at(i));
        }

        ++i;
    }

    v = getAudioInfoValues(s);
    i = 0;

    Q_FOREACH (QTreeWidgetItem* const item, mAudioItems)
    {
        if (item->data(1, Qt::DisplayRole) != v.at(i))
        {
            item->setData(1, Qt::DisplayRole, v.at(i));
        }

        ++i;
    }

    setMetadataItem(mpMetadata,      s.metadata);
    setMetadataItem(mpVideoMetadata, s.video.metadata);
    setMetadataItem(mpAudioMetadata, s.audio.metadata);
}

void StatisticsView::hideEvent(QHideEvent* e)
{
    QDialog::hideEvent(e);
    killTimer(mTimer);
}

void StatisticsView::showEvent(QShowEvent* e)
{
    QDialog::showEvent(e);
    mTimer = startTimer(1000);
}

void StatisticsView::timerEvent(QTimerEvent* e)
{
    if (e->timerId() != mTimer)
        return;

    if (mpFPS)
    {
        mpFPS->setData(1, Qt::DisplayRole,
                       QString::number(mStatistics.video_only.currentDisplayFPS(), 'f', 2));
    }
}

void StatisticsView::initBaseItems(QList<QTreeWidgetItem*>* items)
{
    QTreeWidgetItem* item = nullptr;

    Q_FOREACH (const QString& key, getBaseInfoKeys())
    {
        item = new QTreeWidgetItem(0);
        item->setData(0, Qt::DisplayRole, key);
        items->append(item);
    }
}

QTreeWidgetItem* StatisticsView::createNodeWithItems(QTreeWidget* view,
                                                     const QString& name,
                                                     const QStringList& itemNames,
                                                     QList<QTreeWidgetItem*>* items)
{
    QTreeWidgetItem* const nodeItem = new QTreeWidgetItem(view);
    nodeItem->setData(0, Qt::DisplayRole, name);
    QTreeWidgetItem* item           = nullptr;

    Q_FOREACH (const QString& key, itemNames)
    {
        item = new QTreeWidgetItem(nodeItem);
        item->setData(0, Qt::DisplayRole, key);
        nodeItem->addChild(item);

        if (items)
            items->append(item);
    }

    nodeItem->setExpanded(true);

    return nodeItem;
}

void StatisticsView::setMetadataItem(QTreeWidgetItem* parent,
                                     const QHash<QString, QString>& metadata)
{
    if (parent->childCount() > 0)
    {
        QList<QTreeWidgetItem*> children(parent->takeChildren());
        qDeleteAll(children);
    }

    QHash<QString, QString>::const_iterator it = metadata.constBegin();

    for ( ; it != metadata.constEnd() ; ++it)
    {
        QTreeWidgetItem* const item = new QTreeWidgetItem(parent);
        item->setText(0, it.key());
        item->setText(1, it.value());
    }
}

} // namespace AVPlayer

#include "moc_StatisticsView.cpp"
