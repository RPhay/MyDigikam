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
#include <QTreeWidgetItemIterator>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "spellchecksettings.h"
#include "altlangstredit.h"
#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN LanguagesList : public QTreeWidget
{
    Q_OBJECT

public:

    explicit LanguagesList(QWidget* const parent)
        : QTreeWidget(parent)
    {
        setRootIsDecorated(false);
        setItemsExpandable(false);
        setExpandsOnDoubleClick(false);
        setAlternatingRowColors(true);
        setSelectionMode(QAbstractItemView::NoSelection);
        setAllColumnsShowFocus(true);
        setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        setColumnCount(2);
        setHeaderLabels(QStringList() << i18nc("@title: translator language code", "Code (Language-Country)")
                                      << i18nc("@title: translator language name", "Name"));
        header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        header()->setSectionResizeMode(1, QHeaderView::Stretch);
    }
};

class Q_DECL_HIDDEN LocalizeConfig::Private
{
public:

    explicit Private()
      : translatorCB    (nullptr),
        translatorLabel (nullptr),
        trLangList      (nullptr),
        trLangGroup     (nullptr)
    {
    }

    QStringList    trLangEnabled;
    QComboBox*     translatorCB;
    QLabel*        translatorLabel;
    LanguagesList* trLangList;
    QGroupBox*     trLangGroup;
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
                                                        "an Internet connection to work.</p>"
                                                        "<p>User can select the more appropriate translator engine depending of "
                                                        "desired target language, as not all world-wide languages are supported "
                                                        "by online Web-services.</p>"), this);
    d->translatorLabel->setWordWrap(true);

    // ---

    d->trLangGroup             = new QGroupBox(this);
    QVBoxLayout* const langlay = new QVBoxLayout();
    d->trLangGroup->setLayout(langlay);

    d->trLangList              = new LanguagesList(this);
    langlay->addWidget(d->trLangList);

    // --------------------------------------------------------

    grid->setAlignment(Qt::AlignTop);
    grid->addWidget(trLabel,             0, 0, 1, 1);
    grid->addWidget(d->translatorCB,     0, 1, 1, 1);
    grid->addWidget(d->translatorLabel,  1, 0, 1, 3);
    grid->addWidget(d->trLangGroup,        2, 0, 1, 3);
    grid->setRowStretch(2, 10);
    grid->setColumnStretch(2, 10);
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(spacing);

    // --------------------------------------------------------

    populateTranslatorLanguages();
    readSettings();
    slotTranslatorChanged();

    connect(d->translatorCB, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotTranslatorChanged()));
}

LocalizeConfig::~LocalizeConfig()
{
    delete d;
}

void LocalizeConfig::populateTranslatorLanguages()
{
    d->trLangList->clear();

    QStringList iso = DOnlineTranslator::supportedRFC3066();

    Q_FOREACH (const QString& code, iso)
    {
        new QTreeWidgetItem(d->trLangList,
                            QStringList() << code
                                          << AltLangStrEdit::languageNameRFC3066(code));
    }
}

void LocalizeConfig::slotTranslatorChanged()
{
    int count       = 0;

    QTreeWidgetItemIterator it(d->trLangList);

    while (*it)
    {
        (*it)->setDisabled(!DOnlineTranslator::isSupportTranslation((DOnlineTranslator::Engine)d->translatorCB->currentIndex(),
                                                                    DOnlineTranslator::language(DOnlineTranslator::fromRFC3066((*it)->text(0)))));

        if (!(*it)->isDisabled())
        {
            count++;
        }

        ++it;
    }

    d->trLangGroup->setTitle(i18nc("@title", "Available Translator Languages (%1/%2)",
                             count, d->trLangList->topLevelItemCount()));
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

    QTreeWidgetItemIterator it(d->trLangList);

    while (*it)
    {
        if (((*it)->checkState(0) == Qt::Checked) && !(*it)->isDisabled())
        {
            set.translatorLang << (*it)->text(0);
        }

        ++it;
    }

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

    QTreeWidgetItemIterator it(d->trLangList);

    while (*it)
    {
        if (set.translatorLang.contains((*it)->text(0)))
        {
            (*it)->setCheckState(0, Qt::Checked);
        }
        else
        {
            (*it)->setCheckState(0, Qt::Unchecked);
        }

        ++it;
    }
}

} // namespace Digikam

#include "localizeconfig.moc"

