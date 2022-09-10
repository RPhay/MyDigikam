/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-25
 * Description : a tool to generate video slideshow from images.
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_VIDSLIDE_IMAGES_PAGE_H
#define DIGIKAM_VIDSLIDE_IMAGES_PAGE_H

// Qt includes

#include <QList>
#include <QUrl>
#include <QString>

// Local includes

#include "dwizardpage.h"

using namespace Digikam;

namespace DigikamGenericVideoSlideShowPlugin
{

class VidSlideImagesPage : public DWizardPage
{
    Q_OBJECT

public:

    explicit VidSlideImagesPage(QWizard* const dialog, const QString& title);
    ~VidSlideImagesPage()   override;

    void initializePage()   override;
    bool validatePage()     override;
    bool isComplete() const override;

    void setItemsList(const QList<QUrl>& urls);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericVideoSlideShowPlugin

#endif // DIGIKAM_VIDSLIDE_IMAGES_PAGE_H
