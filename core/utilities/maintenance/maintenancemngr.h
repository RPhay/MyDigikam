/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-01-31
 * Description : maintenance manager
 *
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_MAINTENANCE_MNGR_H
#define DIGIKAM_MAINTENANCE_MNGR_H

// Qt includes

#include <QObject>

namespace Digikam
{

class MaintenanceSettings;
class ProgressItem;

class MaintenanceMngr : public QObject
{
    Q_OBJECT

public:

    explicit MaintenanceMngr(QObject* const parent);
    ~MaintenanceMngr() override;

    void setSettings(const MaintenanceSettings& settings);

    void start();
    bool isRunning() const;

Q_SIGNALS:

    void signalComplete();

private Q_SLOTS:

    void slotToolCompleted(ProgressItem*);
    void slotToolCanceled(ProgressItem*);

private:

    void stage1();  ///< Find New items
    void stage2();  ///< Update Thumbnails
    void stage3();  ///< Build Similarity Finger-prints
    void stage4();  ///< Find Duplicates
    void stage5();  ///< Faces Management
    void stage6();  ///< Image Quality Sorter
    void stage7();  ///< Metadata Synchronizer
    void stage8();  ///< Database Cleanup

    void done();    ///< Called when all scheduled tools are done.
    void cancel();  ///< Called when a tool is canceled.

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_MAINTENANCE_MNGR_H
