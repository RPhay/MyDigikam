/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-05-11
 * Description : setup Survey tab.
 *
 * SPDX-FileCopyrightText: 2007-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "setupsurvey.h"

// Qt includes

#include <QCheckBox>
#include <QColor>
#include <QGroupBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_globals.h"
#include "fullscreensettings.h"
#include "dxmlguiwindow.h"

namespace Digikam
{

class Q_DECL_HIDDEN SetupSurvey::Private
{
public:

    Private() = default;

public:

    const QString configGroupName               = QLatin1String("Survey Settings");
    const QString configClearOnCloseEntry       = QLatin1String("Clear On Close");

    QCheckBox*           clearOnClose           = nullptr;

    FullScreenSettings*  fullScreenSettings     = nullptr;
};

// --------------------------------------------------------

SetupSurvey::SetupSurvey(QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    const int spacing         = layoutSpacing();

    QWidget* const panel      = new QWidget(viewport());
    setWidget(panel);
    setWidgetResizable(true);

    QVBoxLayout* const layout = new QVBoxLayout(panel);

    // --------------------------------------------------------

    QGroupBox* const interfaceOptionsGroup = new QGroupBox(i18n("Interface Options"), panel);
    QVBoxLayout* const gLayout             = new QVBoxLayout(interfaceOptionsGroup);

    d->clearOnClose = new QCheckBox(i18n("Clear the Survey on close"));
    d->clearOnClose->setWhatsThis(i18n("Set this option to remove all images "
                                       "from the Survey when you close it, "
                                       "or unset it to preserve the images "
                                       "currently on the Survey."));

    gLayout->addWidget(d->clearOnClose);
    gLayout->setContentsMargins(spacing, spacing, spacing, spacing);
    gLayout->setSpacing(0);

    // --------------------------------------------------------

    d->fullScreenSettings = new FullScreenSettings(FS_SURVEY, panel);

    // --------------------------------------------------------

    layout->addWidget(interfaceOptionsGroup);
    layout->addWidget(d->fullScreenSettings);
    layout->setContentsMargins(QMargins());
    layout->setSpacing(spacing);
    layout->addStretch();

    // --------------------------------------------------------

    readSettings();
}

SetupSurvey::~SetupSurvey()
{
    delete d;
}

void SetupSurvey::readSettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);

    d->fullScreenSettings->readSettings(group);
    d->clearOnClose->setChecked(group.readEntry(d->configClearOnCloseEntry, false));
}

void SetupSurvey::applySettings()
{
    KSharedConfig::Ptr config = KSharedConfig::openConfig();
    KConfigGroup group        = config->group(d->configGroupName);
    d->fullScreenSettings->saveSettings(group);
    group.writeEntry(d->configClearOnCloseEntry, d->clearOnClose->isChecked());
    config->sync();
}

} // namespace Digikam

#include "moc_setupsurvey.cpp"
