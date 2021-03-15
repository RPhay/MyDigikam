/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a tool to blend bracketed images.
 *
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2012      by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "expoblendinglastpage.h"

// Qt includes

#include <QDir>
#include <QLabel>
#include <QPixmap>
#include <QStandardPaths>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "enfusebinary.h"
#include "expoblendingmanager.h"
#include "dlayoutbox.h"

namespace DigikamGenericExpoBlendingPlugin
{

class Q_DECL_HIDDEN ExpoBlendingLastPage::Private
{
public:

    explicit Private()
        : mngr(nullptr)
    {
    }

    ExpoBlendingManager* mngr;
};

ExpoBlendingLastPage::ExpoBlendingLastPage(ExpoBlendingManager* const mngr, QWizard* const dlg)
    : DWizardPage(dlg, QString::fromLatin1("<b>%1</b>").arg(i18nc("@title:window", "Pre-Processing is Complete"))),
      d          (new Private)
{
    d->mngr                 = mngr;
    DVBox* const vbox       = new DVBox(this);
    QLabel* const title     = new QLabel(vbox);
    title->setOpenExternalLinks(true);
    title->setWordWrap(true);
    title->setText(QString::fromUtf8("<qt>"
                                     "<p><h1><b>%1</b></h1></p>"
                                     "<p>%2</p>"
                                     "<p>%3</p>"
                                     "<p>%4</p>"
                                     "</qt>")
                   .arg(i18nc("@info", "Bracketed Images Pre-Processing is Done"))
                   .arg(i18nc("@info", "Congratulations. Your images are ready to be fused."))
                   .arg(i18nc("@info", "To perform this operation, Enblend program from will be used."))
                   .arg(i18nc("@info", "Press \"Finish\" button to fuse your items and make a pseudo HDR image.")));

    vbox->setStretchFactor(new QWidget(vbox), 10);

    setPageWidget(vbox);

    QPixmap leftPix(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("digikam/data/assistant-enfuse.png")));
    setLeftBottomPix(leftPix.scaledToWidth(128, Qt::SmoothTransformation));
}

ExpoBlendingLastPage::~ExpoBlendingLastPage()
{
    delete d;
}

} // namespace DigikamGenericExpoBlendingPlugin
