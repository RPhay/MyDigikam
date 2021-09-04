/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Side Bar Widget for the Showfoto folder view.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef SHOWFOTO_FOLDER_VIEW_H
#define SHOWFOTO_FOLDER_VIEW_H

// QT includes

#include <QListView>
#include <QPixmap>
#include <QWidget>
#include <QModelIndex>

// Local includes

#include "statesavingobject.h"

using namespace Digikam;

namespace ShowFoto
{

class ShowfotoFolderView : public QWidget,
                           public StateSavingObject
{
    Q_OBJECT

public:

    explicit ShowfotoFolderView(QWidget* const parent);
    ~ShowfotoFolderView() override;

    void          setActive(bool active);
    void          doLoadState();
    void          doSaveState();
    void          applySettings();
    const QIcon   getIcon();
    const QString getCaption();

    QString currentFolder()     const;
    QString currentPath()       const;
    void setCurrentPath(const QString& newPathNative);
    void setCurrentPathWithoutUndo(const QString& newPath);

    QListView* listView()       const;

private Q_SLOTS:

    void slotItemDoubleClicked(const QModelIndex& index);
    void slotGoHome();
    void slotGoUp();
    void slotCustomPathChanged(const QString&);

Q_SIGNALS:

    void signalCurrentPathChanged(const QString& path);

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_FOLDER_VIEW_H
