/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-16
 * Description : Spell-check Config widget.
 *
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "spellcheckconfig.h"

// Qt includes

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "spellchecksettings.h"

namespace Digikam
{

class Q_DECL_HIDDEN SpellCheckConfig::Private
{
public:

    explicit Private()
      : activeSpellCheck(nullptr),
        spellCheckLabel (nullptr)
    {
    }

    QCheckBox*         activeSpellCheck;
    QLabel*            spellCheckLabel;
};

SpellCheckConfig::SpellCheckConfig(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    const int spacing = qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                             QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing));

    // --------------------------------------------------------

    QGridLayout* const grid = new QGridLayout(this);

    d->activeSpellCheck     = new QCheckBox(this);
    d->activeSpellCheck->setText(i18nc("@option:check", "Activate spellcheck when entering text"));

    d->spellCheckLabel      = new QLabel(i18nc("@info", "<p>Turn on this option to activate the background spellcheck "
                                                        "feature on captions, titles, and other text-edit widgets. "
                                                        "Spellchek is able to auto-detect the current language used in "
                                                        "text and will propose alternative with miss-spelled words.</p>"
                                                        "<p>With entries where alternative language can be specified, the "
                                                        "contextual langue will be used to parse text. Spellcheck is "
                                                        "relevant of open-source dictionnaries which must be available "
                                                        "to work properly.</p>"), this);
    d->spellCheckLabel->setWordWrap(true);

    grid->addWidget(d->activeSpellCheck, 0, 0, 1, 1);
    grid->addWidget(d->spellCheckLabel,  1, 0, 1, 1);
    grid->setColumnStretch(0, 10);
    grid->setRowStretch(2, 10);
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(spacing);

    // --------------------------------------------------------

    readSettings();
}

SpellCheckConfig::~SpellCheckConfig()
{
    delete d;
}

void SpellCheckConfig::applySettings()
{
    SpellCheckSettings* const config = SpellCheckSettings::instance();

    if (!config)
    {
        return;
    }

    SpellCheckContainer set;

    set.isActive = d->activeSpellCheck->isChecked();
    config->setSettings(set);
}

void SpellCheckConfig::readSettings()
{
    SpellCheckSettings* const config = SpellCheckSettings::instance();

    if (!config)
    {
        return;
    }

    SpellCheckContainer set = config->settings();

    d->activeSpellCheck->setChecked(set.isActive);
}

} // namespace Digikam
