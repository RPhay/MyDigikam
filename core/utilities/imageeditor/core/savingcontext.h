/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-01-20
 * Description : image editor GUI saving context container
 *
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_SAVING_CONTEXT_H
#define DIGIKAM_SAVING_CONTEXT_H

// Qt includes

#include <QUrl>
#include <QString>

// Local includes

#include "digikam_export.h"
#include "filereadwritelock.h"
#include "versionfileoperation.h"

namespace Digikam
{

class DIGIKAM_EXPORT SavingContext
{
public:

    enum SavingState
    {
        SavingStateNone,
        SavingStateSave,
        SavingStateSaveAs,
        SavingStateVersion
    };

    enum SynchronizingState
    {
        NormalSaving,
        SynchronousSaving
    };

public:

    explicit SavingContext()
      : savingState            (SavingStateNone),
        synchronizingState     (NormalSaving),
        synchronousSavingResult(false),
        destinationExisted     (false),
        abortingSaving         (false),
        executedOperation      (SavingStateNone),
        saveTempFile           (nullptr)
    {
    }

public:

    SavingState          savingState;
    SynchronizingState   synchronizingState;
    bool                 synchronousSavingResult;
    bool                 destinationExisted;
    bool                 abortingSaving;
    SavingState          executedOperation;

    QString              originalFormat;
    QString              format;

    QUrl                 srcURL;
    QUrl                 destinationURL;
    QUrl                 moveSrcURL;

    SafeTemporaryFile*   saveTempFile;
    QString              saveTempFileName;

    VersionFileOperation versionFileOperation;
};

} // namespace Digikam

#endif // DIGIKAM_SAVING_CONTEXT_H
