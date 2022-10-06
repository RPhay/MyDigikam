/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Image Quality setup page
 *
 * SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "setupimagequalitysorter.h"

// Qt includes

#include <QLabel>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "imagequalitysettings.h"
#include "dlayoutbox.h"

namespace Digikam
{

class Q_DECL_HIDDEN SetupImageQualitySorter::Private
{
public:

    explicit Private()
      : settingsWidget(nullptr)
    {
    }

    ImageQualitySettings* settingsWidget;
};

// --------------------------------------------------------

SetupImageQualitySorter::SetupImageQualitySorter(QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    DVBox* const vbox = new DVBox(viewport());

    QLabel* const explanation = new QLabel(i18nc("@label", "These settings determines the quality of an image and convert it into a score, "
                                                 "stored in database as Pick Label property. This information can be evaluated by two ways: "
                                                 "using four basic factors sabotaging the images (blur, noise, exposure, and compression), "
                                                 "or using a deep learning neural network engine. The first one helps to determine whether "
                                                 "images are distorted by the basic factors, however it demands some drawbacks as fine-tuning "
                                                 "from the user’s side and it cannot work along the aesthetic image processing. "
                                                 "The second one uses an IA approach based on %1 model to predict the score. "
                                                 "As deep learning is an end-to-end solution, it doesn’t require hyper-parameter settings, "
                                                 "and make this feature easier to use.",
                                           QString::fromUtf8("<a href='https://expertphotography.com/aesthetic-photography/'>%1</a>")
                                                 .arg(i18nc("@label", "aesthetic image quality"))), vbox);
    explanation->setOpenExternalLinks(true);
    explanation->setWordWrap(true);
    explanation->setTextFormat(Qt::RichText);

    d->settingsWidget = new ImageQualitySettings(vbox);

    setWidget(vbox);
    setWidgetResizable(true);

    d->settingsWidget->readSettings();
}

SetupImageQualitySorter::~SetupImageQualitySorter()
{
    delete d;
}

void SetupImageQualitySorter::applySettings()
{
    d->settingsWidget->applySettings();
}

void SetupImageQualitySorter::readSettings()
{
    d->settingsWidget->readSettings();
}

ImageQualityContainer SetupImageQualitySorter::getImageQualityContainer() const
{
    return d->settingsWidget->getImageQualityContainer();
}

} // namespace Digikam
