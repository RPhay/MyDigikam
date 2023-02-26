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

#ifndef AV_PLAYER_STATISTICS_VIEW_H
#define AV_PLAYER_STATISTICS_VIEW_H

// Qt includes

#include <QDialog>

// Local includes

#include "QtAV_Statistics.h"

using namespace QtAV;

class QTreeWidget;
class QTreeWidgetItem;

namespace AVPlayer
{

class StatisticsView : public QDialog
{
    Q_OBJECT

public:

    explicit StatisticsView(QWidget* const parent = nullptr);

    void setStatistics(const Statistics& s);

protected:

    virtual void hideEvent(QHideEvent* e);
    virtual void showEvent(QShowEvent* e);
    virtual void timerEvent(QTimerEvent* e);

Q_SIGNALS:

private:

    void initBaseItems(QList<QTreeWidgetItem*>* items);

    QTreeWidgetItem* createNodeWithItems(QTreeWidget* view,
                                         const QString& name,
                                         const QStringList& itemNames,
                                         QList<QTreeWidgetItem*>* items = 0);

    void setMetadataItem(QTreeWidgetItem* parent,
                         const QHash<QString, QString>& metadata);

private:

    QTreeWidget*            mpView;
    QList<QTreeWidgetItem*> mBaseItems;
    QList<QTreeWidgetItem*> mVideoItems;

    // TODO: multiple streams

    QList<QTreeWidgetItem*> mAudioItems;
    Statistics              mStatistics;
    int                     mTimer;

    QTreeWidgetItem*        mpFPS;
    QTreeWidgetItem*        mpAudioBitRate;
    QTreeWidgetItem*        mpVideoBitRate;
    QTreeWidgetItem*        mpMetadata;
    QTreeWidgetItem*        mpAudioMetadata;
    QTreeWidgetItem*        mpVideoMetadata;
};

} // namespace AVPlayer

#endif // AV_PLAYER_STATISTICS_VIEW_H
