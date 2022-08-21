/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-20
 * Description : Localization of Strings Config widget.
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

#include "localizeconfig.h"

// Qt includes

#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QApplication>
#include <QStyle>
#include <QTreeWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QListWidget>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "spellchecksettings.h"
#include "altlangstredit.h"
#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN LocalizeConfig::Private
{
public:

    explicit Private()
      : translatorCB    (nullptr),
        translatorLabel (nullptr),
        langList        (nullptr),
        langGroup       (nullptr)
    {
    }

    QComboBox*   translatorCB;
    QLabel*      translatorLabel;
    QTreeWidget* langList;
    QGroupBox*   langGroup;
};

LocalizeConfig::LocalizeConfig(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    const int spacing = qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                             QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing));

    // --------------------------------------------------------

    QGridLayout* const grid = new QGridLayout(this);

    QLabel* const trLabel   = new QLabel(i18n("Online Translator:"), this);
    d->translatorCB         = new QComboBox(this);
    d->translatorCB->setToolTip(i18nc("@option", "Select here the online engine to translate text"));
    d->translatorCB->insertItem(DOnlineTranslator::Google,         QLatin1String("Google"));
    d->translatorCB->insertItem(DOnlineTranslator::Yandex,         QLatin1String("Yandex"));
    d->translatorCB->insertItem(DOnlineTranslator::Bing,           QLatin1String("Bing"));
    d->translatorCB->insertItem(DOnlineTranslator::LibreTranslate, QLatin1String("Libre Translate"));
    d->translatorCB->insertItem(DOnlineTranslator::Lingva,         QLatin1String("Lingva"));

    d->translatorLabel      = new QLabel(i18nc("@info", "<p>The translator engine is an online Web-service used to translate "
                                                        "text from one language to another one. This feature is available on "
                                                        "metadata option where alternative language is supported, as Title, "
                                                        "Caption, etc. The translation process is fully automatized but it requires "
                                                        "an Internet connexion to work.</p>"
                                                        "<p>User can select the more appropriate translator engine depending of "
                                                        "desired target language, as not all world-wide langues are supported "
                                                        "by online Web-services.</p>"), this);
    d->translatorLabel->setWordWrap(true);

    // ---

    d->langGroup               = new QGroupBox(this);
    QVBoxLayout* const langlay = new QVBoxLayout();
    d->langGroup->setLayout(langlay);

    d->langList = new QTreeWidget(this);
    d->langList->setRootIsDecorated(false);
    d->langList->setItemsExpandable(false);
    d->langList->setExpandsOnDoubleClick(false);
    d->langList->setAlternatingRowColors(true);
    d->langList->setSelectionMode(QAbstractItemView::NoSelection);
    d->langList->setAllColumnsShowFocus(true);
    d->langList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->langList->setColumnCount(2);
    d->langList->setHeaderLabels(QStringList() << i18nc("@title: translator language code", "Code")
                                               << i18nc("@title: translator language name", "Name"));
    d->langList->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    d->langList->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    langlay->addWidget(d->langList);

    // --------------------------------------------------------

    grid->setAlignment(Qt::AlignTop);
    grid->addWidget(trLabel,             0, 0, 1, 1);
    grid->addWidget(d->translatorCB,     0, 1, 1, 1);
    grid->addWidget(d->translatorLabel,  1, 0, 1, 3);
    grid->addWidget(d->langGroup,        2, 0, 1, 3);
    grid->setRowStretch(3, 10);
    grid->setColumnStretch(2, 10);
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(spacing);

    // --------------------------------------------------------

    readSettings();
    slotPopulateLanguage();

    connect(d->translatorCB, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotPopulateLanguage()));
}

LocalizeConfig::~LocalizeConfig()
{
    delete d;
}

void LocalizeConfig::slotPopulateLanguage()
{
    d->langList->clear();

    int count       = 0;
    QStringList iso = DOnlineTranslator::supportedRFC3066();

    Q_FOREACH (const QString& code, iso)
    {
        QTreeWidgetItem* const item = new QTreeWidgetItem(d->langList,
                                                          QStringList() << code
                                                                        << AltLangStrEdit::languageNameRFC3066(code));

        item->setDisabled(!DOnlineTranslator::isSupportTranslation((DOnlineTranslator::Engine)d->translatorCB->currentIndex(),
                                                                   DOnlineTranslator::language(DOnlineTranslator::fromRFC3066(code))));

        if (!item->isDisabled())
        {
            count++;
        }
    }

    d->langGroup->setTitle(i18nc("@title", "Available Languages (%1/%2)", count, d->langList->topLevelItemCount()));
}

void LocalizeConfig::applySettings()
{
    SpellCheckSettings* const config = SpellCheckSettings::instance();

    if (!config)
    {
        return;
    }

    SpellCheckContainer set;

    set.translatorEngine = (DOnlineTranslator::Engine)d->translatorCB->currentIndex();

    config->setSettings(set);
}

void LocalizeConfig::readSettings()
{
    SpellCheckSettings* const config = SpellCheckSettings::instance();

    if (!config)
    {
        return;
    }

    SpellCheckContainer set          = config->settings();

    d->translatorCB->setCurrentIndex(set.translatorEngine);
}

} // namespace Digikam
