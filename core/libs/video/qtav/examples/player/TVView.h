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

#ifndef QTAV_PLAYER_TVVIEW_H
#define QTAV_PLAYER_TVVIEW_H

#include <QWidget>


QT_BEGIN_NAMESPACE
class QTreeWidget;
class QTreeWidgetItem;
QT_END_NAMESPACE

class TVView : public QWidget
{
    Q_OBJECT
public:
    explicit TVView(QWidget *parent = 0);

signals:
    void clicked(const QString& key, const QString& value);

private slots:
    void load();
    void onItemDoubleClick( QTreeWidgetItem * item, int column);
private:
    QTreeWidgetItem* createNodeWithItems(QTreeWidget* view, const QString& name, const QStringList& itemNames, QList<QTreeWidgetItem*>* items = 0);

    QTreeWidget *mpView;

};

#endif // QTAV_PLAYER_TVVIEW_H
