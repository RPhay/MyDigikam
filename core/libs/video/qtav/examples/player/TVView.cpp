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

#include "TVView.h"
#include <QTimer>
#include <QFile>
#include <QLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>
#include <QApplication>
#include <QTextStream>

TVView::TVView(QWidget *parent) :
    QWidget(parent)
{
    setWindowTitle(tr("Online TV channels"));
    //setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    mpView = new QTreeWidget();
    mpView->setAnimated(true);
    mpView->setHeaderHidden(true);
    mpView->setColumnCount(1);
    connect(mpView, SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(onItemDoubleClick (QTreeWidgetItem*,int)));
    QVBoxLayout *vl = new QVBoxLayout();
    vl->addWidget(mpView);
    setLayout(vl);
    QTimer::singleShot(0, this, SLOT(load()));
}

void TVView::load()
{
    /*
        //codec problem
        QSettings tv(qApp->applicationDirPath() + "/tv.ini", QSettings::IniFormat);
        tv.setIniCodec("UTF-8");
        foreach (QString key, tv.allKeys()) {
            subMenu->addAction(key)->setData(tv.value(key).toString());
        }
    */
    QFile tv_file(qApp->applicationDirPath() + QString::fromLatin1("/tv.ini"));
    if (!tv_file.exists())
        tv_file.setFileName(QString::fromLatin1(":/tv.ini"));
    if (!tv_file.open(QIODevice::ReadOnly))
        return;
    QTextStream ts(&tv_file);
#if QT_VERSION < QT_VERSION_CHECK(6,0,0)
    ts.setCodec("UTF-8");
#endif
    QTreeWidgetItem *nodeItem = new QTreeWidgetItem(mpView);
    nodeItem->setData(0, Qt::DisplayRole, QString());
    mpView->addTopLevelItem(nodeItem);
    nodeItem->setExpanded(true);
    QString line;
    while (!ts.atEnd()) {
        line = ts.readLine();
        if (line.isEmpty() || line.startsWith(QLatin1String("#")))
            continue;
        if (!line.contains(QLatin1String("="))) {
            nodeItem = new QTreeWidgetItem(mpView);
            nodeItem->setData(0, Qt::DisplayRole, line);
            mpView->addTopLevelItem(nodeItem);
            continue;
        }
        QString key = line.section(QLatin1Char('='), 0, 0);
        QString value = line.section(QLatin1Char('='), 1);
        QTreeWidgetItem *item = new QTreeWidgetItem(nodeItem);
        item->setData(0, Qt::DisplayRole, key);
        item->setData(1, Qt::EditRole, value);
    }
    mpView->resizeColumnToContents(0); //call this after content is done
}

void TVView::onItemDoubleClick(QTreeWidgetItem *item, int column)
{
    Q_UNUSED(column);
    emit clicked(item->data(0, Qt::DisplayRole).toString(), item->data(1, Qt::EditRole).toString());
}
