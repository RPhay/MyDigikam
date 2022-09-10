/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-06-10
 * Description : Progress observer for duplicate scanning
 *
 * SPDX-FileCopyrightText: 2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DUPLICATES_PROGRESS_OBSERVER_H
#define DIGIKAM_DUPLICATES_PROGRESS_OBSERVER_H

#include "haariface.h"
#include "dbjob.h"
#include "digikam_export.h"

namespace Digikam
{

class SearchesJob;

class DIGIKAM_DATABASE_EXPORT DuplicatesProgressObserver : public HaarProgressObserver
{

public:

    explicit DuplicatesProgressObserver(SearchesJob* const thread);
    ~DuplicatesProgressObserver()       override;

    void imageProcessed()               override;
    bool isCanceled()                   override;

private:

    SearchesJob* m_job;

private:

    Q_DISABLE_COPY(DuplicatesProgressObserver)
};

} // namespace Digikam

#endif // DIGIKAM_DUPLICATES_PROGRESS_OBSERVER_H
