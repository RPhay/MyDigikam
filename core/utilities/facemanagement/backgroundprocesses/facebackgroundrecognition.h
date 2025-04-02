/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2025-03-30
 * Description : Face Background Recognition Controller
 *
 * SPDX-FileCopyrightText: 2024-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2024-2025 by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

 #pragma once

// Qt includes

#include <QObject>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT FaceRecognitionBackgroundController : public QObject
{
    Q_OBJECT
public:
    static FaceRecognitionBackgroundController* instance();

    void stop();
    bool waitForDone();

private Q_SLOTS:

    // void slotScanNotification(const QString& msg, int type);
    bool slotRescan();
    void slotScanDone();

private:

    FaceRecognitionBackgroundController();
    ~FaceRecognitionBackgroundController();

    // Disable
    explicit FaceRecognitionBackgroundController(QObject*)                           = delete;
    FaceRecognitionBackgroundController(const FaceRecognitionBackgroundController&)  = delete;

    class Private;
    Private* const d = nullptr;

    friend class FaceRecognitionBackgroundControllerCreator;


};

} // namespace Digikam