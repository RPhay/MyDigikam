/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Image Quality configuration selector widget
 *
 * SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
 * SPDX-FileCopyrightText: 2021-2022 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "imagequalityconfselector.h"

// Qt includes

#include <QRadioButton>
#include <QButtonGroup>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QApplication>
#include <QStyle>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_globals.h"
#include "dlayoutbox.h"
#include "imagequalitysettings.h"

namespace Digikam
{

class Q_DECL_HIDDEN ImageQualityConfSelector::Private
{
public:

    Private() = default;

    QButtonGroup*         selButtonGroup = nullptr;
    QRadioButton*         selDefault     = nullptr;
    QRadioButton*         selCustom      = nullptr;

    QPushButton*          qualitySetup   = nullptr;

    ImageQualitySettings* customView     = nullptr;
};

ImageQualityConfSelector::ImageQualityConfSelector(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    const int spacing         = layoutSpacing();


    d->selButtonGroup       = new QButtonGroup(this);
    d->selButtonGroup->setExclusive(true);

    d->selDefault           = new QRadioButton(i18nc("@option:radio", "Use default settings"), this);
    d->selDefault->setToolTip(i18nc("@info:tooltip", "Use the default quality sorting parameters "
                                    "configured in Setup dialog."));

    d->qualitySetup         = new QPushButton(i18n("Settings..."), this);

    d->selButtonGroup->addButton(d->selDefault, GlobalSettings);

    d->selCustom            = new QRadioButton(i18nc("@option:radio", "Use custom settings"), this);
    d->selCustom->setToolTip(i18nc("@info:tooltip", "Use custom parameters to perform quality "
                                   "sorting instead default one."));

    d->selButtonGroup->addButton(d->selCustom, CustomSettings);
    d->selDefault->setChecked(true);

    d->customView           = new ImageQualitySettings(this);

    QGridLayout* const glay = new QGridLayout(this);
    glay->addWidget(d->selDefault,         0, 0, 1, 1);
    glay->addWidget(d->qualitySetup,       0, 2, 1, 1);
    glay->addWidget(d->selCustom,          1, 0, 1, 3);
    glay->addWidget(d->customView,         2, 0, 1, 3);
    glay->setColumnStretch(1, 10);
    glay->setContentsMargins(spacing, spacing, spacing, spacing);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))

    connect(d->selButtonGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::idClicked),
            this, &ImageQualityConfSelector::slotSelectionChanged);

#else

    connect(d->selButtonGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            this, &ImageQualityConfSelector::slotSelectionChanged);

#endif

    connect(d->qualitySetup, SIGNAL(clicked()),
            this, SIGNAL(signalQualitySetup()));

    connect(d->customView, SIGNAL(signalSettingsChanged()),
            this, SIGNAL(signalSettingsChanged()));

    slotSelectionChanged();
}

ImageQualityConfSelector::~ImageQualityConfSelector()
{
    delete d;
}

ImageQualityConfSelector::SettingsType ImageQualityConfSelector::settingsSelected() const
{
    return ((SettingsType)(d->selButtonGroup->checkedId()));
}

void ImageQualityConfSelector::setSettingsSelected(SettingsType type)
{
    QAbstractButton* const btn = d->selButtonGroup->button((int)(type));

    if (btn)
    {
        btn->setChecked(true);
    }

    slotSelectionChanged();
}

ImageQualityContainer ImageQualityConfSelector::customSettings() const
{
    return d->customView->getImageQualityContainer();
}

void ImageQualityConfSelector::setCustomSettings(const ImageQualityContainer& settings)
{
    d->customView->setImageQualityContainer(settings);
}

void ImageQualityConfSelector::slotSelectionChanged()
{
    d->qualitySetup->setDisabled(d->selCustom->isChecked());
    d->customView->setDisabled(d->selDefault->isChecked());

    Q_EMIT signalSettingsChanged();
}

} // namespace Digikam

#include "moc_imagequalityconfselector.cpp"
