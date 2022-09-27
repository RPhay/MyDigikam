/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-04-04
 * Description : a tool to generate jAlbum image galleries
 *
 * SPDX-FileCopyrightText: 2013-2019 by Andrew Goodbody <ajg zero two at elfringham dot co dot uk>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_JALBUM_FINAL_PAGE_H
#define DIGIKAM_JALBUM_FINAL_PAGE_H

// Qt includes

#include <QString>

// Local includes

#include "dwizardpage.h"

using namespace Digikam;

namespace DigikamGenericJAlbumPlugin
{

class JAlbumFinalPage : public DWizardPage
{
    Q_OBJECT

public:

    explicit JAlbumFinalPage(QWizard* const dialog, const QString& title);
    ~JAlbumFinalPage()      override;

    void initializePage()   override;
    bool isComplete() const override;

private Q_SLOTS:

    void slotProcess();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericJAlbumPlugin

#endif // DIGIKAM_JALBUM_FINAL_PAGE_H
