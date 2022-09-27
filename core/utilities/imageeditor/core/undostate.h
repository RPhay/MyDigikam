/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-01-15
 * Description : Undo state container
 *
 * SPDX-FileCopyrightText: 2004-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_UNDO_STATE_H
#define DIGIKAM_UNDO_STATE_H

namespace Digikam
{

class UndoState
{
public:

    explicit UndoState()
      : hasUndo           (false),
        hasRedo           (false),
        hasChanges        (false),
        hasUndoableChanges(false)
    {
    }

    ~UndoState()
    {
    }

public:

    bool hasUndo;
    bool hasRedo;
    bool hasChanges;
    bool hasUndoableChanges;
};

} // namespace Digikam

#endif // DIGIKAM_UNDO_STATE_H
