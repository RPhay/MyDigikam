/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-08-27
 * Description : Showfoto folder view undo command.
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

#ifndef SHOWFOTO_FOLDER_VIEW_UNDO_H
#define SHOWFOTO_FOLDER_VIEW_UNDO_H

// Qt includes

#include <QUndoCommand>
#include <QString>

// Local includes

#include "showfotofolderviewsidebar.h"

namespace ShowFoto
{

class ShowfotoFolderViewUndo : public QUndoCommand
{

public:

    ShowfotoFolderViewUndo(ShowfotoFolderViewSideBar* const view,
                           const QString& newPath);
    ~ShowfotoFolderViewUndo();

    QString undoPath() const;

    void undo();
    void redo();

private:

    class Private;
    Private* const d;
};

} // namespace ShowFoto

#endif // SHOWFOTO_FOLDER_VIEW_UNDO_H
