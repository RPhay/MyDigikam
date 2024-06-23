/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-31
 * Description : digiKam global static QNetworkAccessManager
 *
 * SPDX-FileCopyrightText: 2022 by Maik Qualmann <metzpinguin at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QObject>
#include <QNetworkAccessManager>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT NetworkManager : public QObject
{
    Q_OBJECT

public:

    /**
     * Global instance of internal network manager.
     * All accessor methods are thread-safe.
     */
    static NetworkManager* instance();

    /**
     * Get the current QNetworkAccessManager or create
     * a new QNetworkAccessManager if the passed
     * QObject runs on a different thread.
     */
    QNetworkAccessManager* getNetworkManager(QObject* const object) const;

private:

    // Disable

    NetworkManager();
    explicit NetworkManager(QObject*) = delete;

    ~NetworkManager() override;

private:

    class Private;
    Private* const d = nullptr;

    friend class NetworkManagerCreator;
};

} // namespace Digikam
