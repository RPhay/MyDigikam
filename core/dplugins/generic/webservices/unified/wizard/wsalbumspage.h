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

#ifndef DIGIKAM_WS_ALBUMS_PAGE_H
#define DIGIKAM_WS_ALBUMS_PAGE_H

// Qt includes

#include <QList>
#include <QUrl>
#include <QString>

// Local includes

#include "dwizardpage.h"

using namespace Digikam;

namespace DigikamGenericUnifiedPlugin
{

class WSAlbumsPage : public DWizardPage
{
public:

    explicit WSAlbumsPage(QWizard* const dialog, const QString& title);
    ~WSAlbumsPage();

    bool validatePage();
    bool isComplete() const;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericUnifiedPlugin

#endif // DIGIKAM_WS_ALBUMS_PAGE_H
