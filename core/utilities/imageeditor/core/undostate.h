/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-01-15
 * Description : Undo state container
 *
 * SPDX-FileCopyrightText: 2004-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

    UndoState()  = default;
    ~UndoState() = default;

public:

    bool hasUndo            = false;
    bool hasRedo            = false;
    bool hasChanges         = false;
    bool hasUndoableChanges = false;
};

} // namespace Digikam

#endif // DIGIKAM_UNDO_STATE_H
