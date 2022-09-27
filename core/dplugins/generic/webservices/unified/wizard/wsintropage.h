/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-06-27
 * Description : intro page to export tool where user can choose web service to export,
 *               existent accounts and function mode (export/import).
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2018 by Thanh Trung Dinh <dinhthanhtrung1996 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_WS_INTRO_PAGE_H
#define DIGIKAM_WS_INTRO_PAGE_H

// Qt includes

#include <QString>

// Local includes

#include "dwizardpage.h"

using namespace Digikam;

namespace DigikamGenericUnifiedPlugin
{

class WSIntroPage : public DWizardPage
{
    Q_OBJECT

public:

    explicit WSIntroPage(QWizard* const dialog, const QString& title);
    ~WSIntroPage();

    void initializePage();
    bool validatePage();

private Q_SLOTS:

    void slotImageGetOptionChanged(int index);
    void slotWebServiceOptionChanged(const QString& serviceName);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericUnifiedPlugin

#endif // DIGIKAM_WS_INTRO_PAGE_H
