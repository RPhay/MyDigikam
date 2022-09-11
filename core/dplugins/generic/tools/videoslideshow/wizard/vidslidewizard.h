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

#ifndef DIGIKAM_VIDSLIDE_WIZARD_H
#define DIGIKAM_VIDSLIDE_WIZARD_H

// Qt includes

#include <QList>
#include <QUrl>

// Local includes

#include "dwizarddlg.h"
#include "dinfointerface.h"
#include "vidslidesettings.h"

using namespace Digikam;

namespace DigikamGenericVideoSlideShowPlugin
{

class VidSlideWizard : public DWizardDlg
{
    Q_OBJECT

public:

    explicit VidSlideWizard(QWidget* const parent, DInfoInterface* const iface = nullptr);
    ~VidSlideWizard()                  override;

    bool validateCurrentPage()         override;
    int  nextId()                const override;

    DInfoInterface*   iface()    const;
    VidSlideSettings* settings() const;

    void setItemsList(const QList<QUrl>& urls);

private Q_SLOTS:

    void slotCurrentIdChanged(int);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericVideoSlideShowPlugin

#endif // DIGIKAM_VIDSLIDE_WIZARD_H
