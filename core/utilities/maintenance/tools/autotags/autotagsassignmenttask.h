/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-09-02
 * Description : Thread actions task for autotags assignment.
 *
 * SPDX-FileCopyrightText: 2023 by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_AUTOTAGS_ASSIGNMENT_TASK_H
#define DIGIKAM_AUTOTAGS_ASSIGNMENT_TASK_H

// Qt includes

#include <QImage>

// Local includes

#include "actionthreadbase.h"

namespace Digikam
{

class MaintenanceData;

class AutotagsAssignmentTask : public ActionJob
{
    Q_OBJECT

public:

    explicit AutotagsAssignmentTask();
    ~AutotagsAssignmentTask()     override;

    void assignTags(const QString& pathImage, const QList<QString>& tagsList);
    void setMaintenanceData(MaintenanceData* const data = nullptr);
    void setLanguages(const QStringList& langs);
    void setModelType(int modelType);

Q_SIGNALS:

    void signalFinished(const QImage&);

protected:

    void run()                    override;

private:

    // Disable
    AutotagsAssignmentTask(QObject*) = delete;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam

#endif // DIGIKAM_AUTOTAGS_ASSIGNMENT_TASK_H
