/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-09-13
 * Description : a widget to provide conflict rules to save image.
 *
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FILE_SAVE_CONFLICT_BOX_H
#define DIGIKAM_FILE_SAVE_CONFLICT_BOX_H

// Qt includes

#include <QWidget>

// Local includes

#include "digikam_export.h"

class KConfigGroup;

namespace Digikam
{

class DIGIKAM_EXPORT FileSaveConflictBox : public QWidget
{
    Q_OBJECT

public:

    enum ConflictRule
    {
        OVERWRITE = 0,
        DIFFNAME,
        SKIPFILE
    };

public:

    explicit FileSaveConflictBox(QWidget* const parent, bool addSkip = false);
    ~FileSaveConflictBox() override;

    ConflictRule conflictRule() const;
    void setConflictRule(ConflictRule r);

    void resetToDefault();

    void readSettings(KConfigGroup& group);
    void writeSettings(KConfigGroup& group);

Q_SIGNALS:

    void signalConflictButtonChanged(int);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FILE_SAVE_CONFLICT_BOX_H
