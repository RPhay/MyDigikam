/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-06-27
 * Description : a tool to export items to web services.
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_WS_SETTINGS_PAGE_H
#define DIGIKAM_WS_SETTINGS_PAGE_H

// Qt includes

#include <QString>

// Local includes

#include "dwizardpage.h"

using namespace Digikam;

namespace DigikamGenericUnifiedPlugin
{

class WSSettingsPage : public DWizardPage
{
    Q_OBJECT

public:

    explicit WSSettingsPage(QWizard* const dialog, const QString& title);
    ~WSSettingsPage();

    void initializePage();
    bool validatePage();

private Q_SLOTS:

    void slotImagesFormatChanged(int);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericUnifiedPlugin

#endif // DIGIKAM_WS_SETTINGS_PAGE_H
