/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef QTAV_PLAYER_STATISTICSVIEW_H
#define QTAV_PLAYER_STATISTICSVIEW_H

#include <QDialog>
#include "QtAV_Statistics.h"

using namespace QtAV;

QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE
class StatisticsView : public QDialog
{
    Q_OBJECT
public:
    explicit StatisticsView(QWidget *parent = 0);
    void setStatistics(const Statistics &s);

protected:
    virtual void hideEvent(QHideEvent* e);
    virtual void showEvent(QShowEvent* e);
    virtual void timerEvent(QTimerEvent *e);

signals:
    
public slots:
    
private:
    void initBaseItems(QList<QTreeWidgetItem*>* items);
    QTreeWidgetItem* createNodeWithItems(QTreeWidget* view, const QString& name, const QStringList& itemNames, QList<QTreeWidgetItem*>* items = 0);
    void setMetadataItem(QTreeWidgetItem* parent, const QHash<QString,QString>& metadata);
    QTreeWidget *mpView;
    QList<QTreeWidgetItem*> mBaseItems;
    QList<QTreeWidgetItem*> mVideoItems;
    //TODO: multiple streams
    QList<QTreeWidgetItem*> mAudioItems;
    Statistics mStatistics;
    int mTimer;

    QTreeWidgetItem *mpFPS, *mpAudioBitRate, *mpVideoBitRate;
    QTreeWidgetItem *mpMetadata, *mpAudioMetadata, *mpVideoMetadata;
};

#endif // QTAV_PLAYER_STATISTICSVIEW_H
