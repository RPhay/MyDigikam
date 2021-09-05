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

#include "showfotofolderviewundo.h"

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"

namespace ShowFoto
{

ShowfotoFolderViewUndo::ShowfotoFolderViewUndo(ShowfotoFolderView* const view,
                                               const QString& newPath)
    : QUndoCommand(),
      m_view(view)
{
    m_oldPath = m_view->currentFolder();
    m_newPath = newPath;
}

ShowfotoFolderViewUndo::~ShowfotoFolderViewUndo()
{
}

QString ShowfotoFolderViewUndo::undoPath() const
{
    return m_oldPath;
}

void ShowfotoFolderViewUndo::undo()
{
    m_view->setCurrentPathWithoutUndo(m_oldPath);
}

void ShowfotoFolderViewUndo::redo()
{
    m_view->setCurrentPathWithoutUndo(m_newPath);
}

} // namespace ShowFoto
