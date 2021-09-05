/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Showfoto folder view model.
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

#ifndef SHOWFOTO_FOLDER_VIEW_MODEL_H
#define SHOWFOTO_FOLDER_VIEW_MODEL_H

// QT includes

#include <QFileSystemModel>

namespace ShowFoto
{

class ShowfotoFolderViewList;

class ShowfotoFolderViewModel : public QFileSystemModel
{
    Q_OBJECT

public:

    explicit ShowfotoFolderViewModel(ShowfotoFolderViewList* const view);
    ~ShowfotoFolderViewModel()           override;

    QSize iconSize() const;

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_FOLDER_VIEW_MODEL_H
