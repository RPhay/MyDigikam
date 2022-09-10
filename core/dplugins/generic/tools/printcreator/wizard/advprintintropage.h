/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-06-27
 * Description : a tool to print images
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ADV_PRINT_INTRO_PAGE_H
#define DIGIKAM_ADV_PRINT_INTRO_PAGE_H

// Qt includes

#include <QString>

// Local includes

#include "dwizardpage.h"

using namespace Digikam;

namespace DigikamGenericPrintCreatorPlugin
{

class AdvPrintIntroPage : public DWizardPage
{
    Q_OBJECT

public:

    explicit AdvPrintIntroPage(QWizard* const dialog, const QString& title);
    ~AdvPrintIntroPage()    override;

    bool validatePage()     override;
    void initializePage()   override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericPrintCreatorPlugin

#endif // DIGIKAM_ADV_PRINT_INTRO_PAGE_H
