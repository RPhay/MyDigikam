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
#include "digikam_config.h"

// Qt includes

#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QApplication>
#include <QStyle>
#include <QTreeWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QListWidget>
#include <QLineEdit>
#include <QIcon>

// KDE includes

#include <klocalizedstring.h>

#ifdef HAVE_SONNET

#   include <sonnet/speller.h>
using namespace Sonnet;

#endif

// Local includes

#include "spellchecksettings.h"
#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN SpellCheckConfig::Private
{
public:

    explicit Private()
      : activeSpellCheck (nullptr),
        spellCheckLabel  (nullptr),
        dictList         (nullptr),
        backList         (nullptr),
        addWordButton    (nullptr),
        delWordButton    (nullptr),
        repWordButton    (nullptr),
        ignoreWordsBox   (nullptr),
        ignoreWordEdit   (nullptr)
    {
    }

    QCheckBox*   activeSpellCheck;
    QLabel*      spellCheckLabel;
    QTreeWidget* dictList;              ///< Dictionaries list
    QTreeWidget* backList;              ///< Backends list
    QPushButton* addWordButton;
    QPushButton* delWordButton;
    QPushButton* repWordButton;

    QListWidget* ignoreWordsBox;

    QLineEdit*   ignoreWordEdit;
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
    d->activeSpellCheck->setText(i18nc("@option:check", "Activate spellcheck in background when entering text"));

    d->spellCheckLabel      = new QLabel(i18nc("@info", "<p>Turn on this option to activate the background spellcheck "
                                                        "feature on captions, titles, and other text-edit widgets. "
                                                        "Spellchek is able to auto-detect the current language used in "
                                                        "text and will propose alternative with miss-spelled words.</p>"
                                                        "<p>With entries where alternative language can be specified, the "
                                                        "contextual languagee will be used to parse text. Spellcheck "
                                                        "depends of open-source backends, including necessary dictionaries, "
                                                        "to operate sentence analisys in desired languages.</p>"), this);
    d->spellCheckLabel->setWordWrap(true);

    // ---

    QGroupBox* const dictgroup = new QGroupBox(i18n("Available Language Dictionaries"), this);
    QVBoxLayout* const dictlay = new QVBoxLayout();
    dictgroup->setLayout(dictlay);

    d->dictList = new QTreeWidget(this);
    d->dictList->setRootIsDecorated(false);
    d->dictList->setItemsExpandable(false);
    d->dictList->setExpandsOnDoubleClick(false);
    d->dictList->setAlternatingRowColors(true);
    d->dictList->setSelectionMode(QAbstractItemView::NoSelection);
    d->dictList->setAllColumnsShowFocus(true);
    d->dictList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->dictList->setColumnCount(2);
    d->dictList->setHeaderLabels(QStringList() << i18nc("@title: dictionary language code", "Code")
                                               << i18nc("@title: dictionary language name", "Name"));
    d->dictList->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    d->dictList->header()->setSectionResizeMode(1, QHeaderView::Stretch);
    dictlay->addWidget(d->dictList);

    // ---

    QGroupBox* const backgroup = new QGroupBox(i18n("Available Backends"), this);
    QVBoxLayout* const backlay = new QVBoxLayout();
    backgroup->setLayout(backlay);

    d->backList = new QTreeWidget(this);
    d->backList->setRootIsDecorated(false);
    d->backList->setItemsExpandable(false);
    d->backList->setExpandsOnDoubleClick(false);
    d->backList->setAlternatingRowColors(true);
    d->backList->setSelectionMode(QAbstractItemView::NoSelection);
    d->backList->setAllColumnsShowFocus(true);
    d->backList->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->backList->setColumnCount(1);
    d->backList->header()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    d->backList->header()->setVisible(false);
    backlay->addWidget(d->backList);

    // ---

    QGroupBox* const ignoreWordsGroup = new QGroupBox(i18n("Ignored Words"), this);
    QGridLayout* const ignoreWordsLay = new QGridLayout();
    ignoreWordsGroup->setLayout(ignoreWordsLay);

    d->ignoreWordEdit  = new QLineEdit(this);
    d->ignoreWordEdit->setClearButtonEnabled(true);
    d->ignoreWordEdit->setPlaceholderText(i18n("Set here a new word to ignore during spellcheck"));

    d->ignoreWordsBox = new QListWidget(this);
    d->ignoreWordsBox->setWhatsThis(i18n("You can add or remove words to ignore "
                                         "while spell-checking operations."));

    d->ignoreWordsBox->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    d->addWordButton = new QPushButton(i18n("&Add..."),  this);
    d->delWordButton = new QPushButton(i18n("&Remove"),  this);
    d->repWordButton = new QPushButton(i18n("&Replace"), this);

    d->addWordButton->setIcon(QIcon::fromTheme(QLatin1String("list-add")));
    d->delWordButton->setIcon(QIcon::fromTheme(QLatin1String("list-remove")));
    d->repWordButton->setIcon(QIcon::fromTheme(QLatin1String("view-refresh")));
    d->delWordButton->setEnabled(false);
    d->repWordButton->setEnabled(false);

    ignoreWordsLay->setAlignment(Qt::AlignTop);
    ignoreWordsLay->addWidget(d->ignoreWordEdit, 0, 0, 1, 1);
    ignoreWordsLay->addWidget(d->ignoreWordsBox, 1, 0, 5, 1);
    ignoreWordsLay->addWidget(d->addWordButton,  1, 1, 1, 1);
    ignoreWordsLay->addWidget(d->delWordButton,  2, 1, 1, 1);
    ignoreWordsLay->addWidget(d->repWordButton,  3, 1, 1, 1);
    ignoreWordsLay->setRowStretch(4, 10);
    ignoreWordsLay->setColumnStretch(0, 10);
    ignoreWordsLay->setContentsMargins(spacing, spacing, spacing, spacing);
    ignoreWordsLay->setSpacing(spacing);

    // --------------------------------------------------------

    connect(d->ignoreWordsBox, SIGNAL(itemSelectionChanged()),
            this, SLOT(slotIgnoreWordSelectionChanged()));

    connect(d->addWordButton, SIGNAL(clicked()),
            this, SLOT(slotAddWord()));

    connect(d->delWordButton, SIGNAL(clicked()),
            this, SLOT(slotDelWord()));

    connect(d->repWordButton, SIGNAL(clicked()),
            this, SLOT(slotRepWord()));

    // ---

#ifdef HAVE_SONNET

    Speller dict;
    QMap<QString, QString> map = dict.availableDictionaries();

    for (QMap<QString, QString>::const_iterator it = map.constBegin() ; it != map.constEnd() ; ++it)
    {
        new QTreeWidgetItem(d->dictList, QStringList() << it.value() << it.key());
    }

    Q_FOREACH (const QString& b, dict.availableBackends())
    {
        new QTreeWidgetItem(d->backList, QStringList() << b);
    }

#endif

    // ---

    grid->setAlignment(Qt::AlignTop);
    grid->addWidget(d->activeSpellCheck, 0, 0, 1, 2);
    grid->addWidget(d->spellCheckLabel,  1, 0, 1, 2);
    grid->addWidget(dictgroup,           2, 0, 1, 1);
    grid->addWidget(backgroup,           2, 1, 1, 1);
    grid->addWidget(ignoreWordsGroup,    3, 0, 1, 2);
    grid->setRowStretch(4, 10);
    grid->setColumnStretch(0, 10);
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

    set.enableSpellCheck = d->activeSpellCheck->isChecked();

    QStringList ignoredWords;

    for (int i = 0 ; i < d->ignoreWordsBox->count() ; ++i)
    {
        QListWidgetItem* const item = d->ignoreWordsBox->item(i);
        ignoredWords.append(item->text());
    }

    set.ignoredWords = ignoredWords;

    config->setSettings(set);
}

void SpellCheckConfig::readSettings()
{
    SpellCheckSettings* const config = SpellCheckSettings::instance();

    if (!config)
    {
        return;
    }

    SpellCheckContainer set          = config->settings();

    d->activeSpellCheck->setChecked(set.enableSpellCheck);
    d->ignoreWordsBox->insertItems(0, set.ignoredWords);
}

void SpellCheckConfig::slotDelWord()
{
    QListWidgetItem* const item = d->ignoreWordsBox->currentItem();

    if (!item)
    {
        return;
    }

    d->ignoreWordsBox->takeItem(d->ignoreWordsBox->row(item));
    delete item;
}

void SpellCheckConfig::slotRepWord()
{
    QString newWord = d->ignoreWordEdit->text();

    if (newWord.isEmpty())
    {
        return;
    }

    if (!d->ignoreWordsBox->selectedItems().isEmpty())
    {
        d->ignoreWordsBox->selectedItems().at(0)->setText(newWord);
        d->ignoreWordEdit->clear();
    }
}

void SpellCheckConfig::slotIgnoreWordSelectionChanged()
{
    if (!d->ignoreWordsBox->selectedItems().isEmpty())
    {
        d->ignoreWordEdit->setText(d->ignoreWordsBox->selectedItems().at(0)->text());
        d->delWordButton->setEnabled(true);
        d->repWordButton->setEnabled(true);
    }
    else
    {
        d->delWordButton->setEnabled(false);
        d->repWordButton->setEnabled(false);
    }
}

void SpellCheckConfig::slotAddWord()
{
    QString newWord = d->ignoreWordEdit->text();

    if (newWord.isEmpty())
    {
        return;
    }

    bool found = false;

    for (int i = 0 ; i < d->ignoreWordsBox->count() ; ++i)
    {
        QListWidgetItem* const item = d->ignoreWordsBox->item(i);

        if (newWord == item->text())
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        d->ignoreWordsBox->insertItem(d->ignoreWordsBox->count(), newWord);
        d->ignoreWordEdit->clear();
    }
}

} // namespace Digikam
