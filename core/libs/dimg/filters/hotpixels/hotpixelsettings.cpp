/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-08-05)
 * Description : HotPixel settings view.
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "hotpixelsettings.h"

// Qt includes

#include <QGridLayout>
#include <QLabel>
#include <QString>
#include <QIcon>
#include <QTextStream>
#include <QProgressBar>
#include <QPushButton>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// Local includes
 
#include "digikam_debug.h"
#include "hotpixelfixer.h"
#include "blackframelistview.h"
#include "dcombobox.h"

namespace Digikam
{

class Q_DECL_HIDDEN HotPixelSettings::Private
{
public:

    explicit Private()
      : blackFrameButton(nullptr),
        progressBar(nullptr),
        filterMethodCombo(nullptr),
        blackFrameListView(nullptr)
    {
    }

    static const QString configGroupName;
    static const QString configLastBlackFrameFileEntry;
    static const QString configFilterMethodEntry;

    QPushButton*         blackFrameButton;
    QProgressBar*        progressBar;
    DComboBox*           filterMethodCombo;
    BlackFrameListView*  blackFrameListView;
};

const QString HotPixelSettings::Private::configGroupName(QLatin1String("hotpixels Tool"));
const QString HotPixelSettings::Private::configLastBlackFrameFileEntry(QLatin1String("Last Black Frame File"));
const QString HotPixelSettings::Private::configFilterMethodEntry(QLatin1String("Filter Method"));

// --------------------------------------------------------

HotPixelSettings::HotPixelSettings(QWidget* const parent)
    : QWidget(parent),
      d(new Private)
{
    const int spacing       = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);    QGridLayout* const grid = new QGridLayout(parent);

    QLabel* const filterMethodLabel = new QLabel(i18n("Filter:"), this);
    d->filterMethodCombo            = new DComboBox(this);
    d->filterMethodCombo->addItem(i18nc("average filter mode", "Average"));
    d->filterMethodCombo->addItem(i18nc("linear filter mode", "Linear"));
    d->filterMethodCombo->addItem(i18nc("quadratic filter mode", "Quadratic"));
    d->filterMethodCombo->addItem(i18nc("cubic filter mode", "Cubic"));
    d->filterMethodCombo->setDefaultIndex(HotPixelFixer::QUADRATIC_INTERPOLATION);

    d->blackFrameButton = new QPushButton(i18n("Black Frame..."), this);
    d->blackFrameButton->setIcon(QIcon::fromTheme(QLatin1String("document-open")));
    d->blackFrameButton->setWhatsThis(i18n("Use this button to add a new black frame file which will "
                                           "be used by the hot pixels removal filter.") );

    d->blackFrameListView = new BlackFrameListView(this);

    // -------------------------------------------------------------

    grid->addWidget(filterMethodLabel,     0, 0, 1, 1);
    grid->addWidget(d->filterMethodCombo,  0, 1, 1, 1);
    grid->addWidget(d->blackFrameButton,   0, 2, 1, 1);
    grid->addWidget(d->blackFrameListView, 1, 0, 2, 3);
    grid->setRowStretch(3, 10);
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(spacing);

    // -------------------------------------------------------------

    connect(d->filterMethodCombo, SIGNAL(activated(int)),
            this, SLOT(slotPreview()));

    connect(d->blackFrameButton, SIGNAL(clicked()),
            this, SLOT(slotAddBlackFrame()));

    connect(d->blackFrameListView, SIGNAL(signalBlackFrameSelected(QList<HotPixelProps>,QUrl)),
            this, SLOT(slotBlackFrame(QList<HotPixelProps>,QUrl)));
}

HotPixelSettings::~HotPixelSettings()
{
    delete d;
}

HotPixelContainer HotPixelSettings::settings() const
{
    HotPixelContainer prm;
    prm.filterMethod = (HotPixelFixer::InterpolationMethod)d->filterMethodCombo->currentIndex();
    return prm;
}

void HotPixelSettings::setSettings(const HotPixelContainer& settings)
{
    d->filterMethodCombo->blockSignals(true);
    d->filterMethodCombo->setCurrentIndex(settings.filterMethod);
    d->filterMethodCombo->blockSignals(false);
}

void HotPixelSettings::resetToDefault()
{
    d->filterMethodCombo->blockSignals(true);
    d->filterMethodCombo->slotReset();
    d->filterMethodCombo->blockSignals(false);
}

HotPixelContainer HotPixelSettings::defaultSettings() const
{
    HotPixelContainer prm;
    return prm;
}

void HotPixelSettings::readSettings(KConfigGroup& group)
{
    HotPixelContainer prm;
    HotPixelContainer defaultPrm = defaultSettings();

    prm.blackFramePath           = group.readEntry(d->configLastBlackFrameFileEntry, defaultPrm.blackFramePath);
    prm.filterMethod             = (HotPixelFixer::InterpolationMethod)group.readEntry(d->configFilterMethodEntry,       (int)defaultPrm.filterMethod);
    setSettings(prm);
}

void HotPixelSettings::writeSettings(KConfigGroup& group)
{
    HotPixelContainer prm = settings();
    group.writeEntry(d->configLastBlackFrameFileEntry, prm.blackFramePath);
    group.writeEntry(d->configFilterMethodEntry,       (int)prm.filterMethod);
}

} // namespace Digikam
