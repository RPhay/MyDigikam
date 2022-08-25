/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-15
 * Description : multi-languages string editor
 *
 * Copyright (C) 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "altlangstredit_p.h"

namespace Digikam
{

AltLangStrEdit::AltLangStrEdit(QWidget* const parent, unsigned int lines)
    : QWidget(parent),
      d      (new Private)
{
    d->titleWidget    = new QLabel(this);

    d->delValueButton = new QToolButton(this);
    d->delValueButton->setIcon(QIcon::fromTheme(QLatin1String("edit-clear")));
    d->delValueButton->setToolTip(i18nc("@info: language edit widget", "Remove entry for this language"));
    d->delValueButton->setEnabled(false);

    d->translateButton = new QToolButton(this);
    d->translateButton->setIcon(QIcon::fromTheme(QLatin1String("language-chooser")));
    d->translateButton->setEnabled(false);
    d->translateButton->setPopupMode(QToolButton::MenuButtonPopup);
    QMenu* const menu  = new QMenu(d->translateButton);
    d->translateAction = new TranslateAction(d->translateButton);
    menu->addAction(d->translateAction);
    d->translateButton->setMenu(menu);

    d->settingsButton  = new QToolButton(this);
    d->settingsButton->setIcon(QIcon::fromTheme(QLatin1String("configure")));
    d->settingsButton->setToolTip(i18n("Open localize setup"));

    d->trengine        = new DOnlineTranslator(this);

    d->languageCB      = new QComboBox(this);
    d->languageCB->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    d->languageCB->setWhatsThis(i18nc("@info: language edit widget", "Select item language here."));

    d->valueEdit       = new DTextEdit(lines, this);
    d->valueEdit->setAcceptRichText(false);

    // --------------------------------------------------------

    d->grid            = new QGridLayout(this);
    d->grid->setAlignment(Qt::AlignTop);
    d->grid->addWidget(d->languageCB,      0, 2, 1,  1);
    d->grid->addWidget(d->delValueButton,  0, 3, 1,  1);
    d->grid->addWidget(d->translateButton, 0, 4, 1,  1);
    d->grid->addWidget(d->settingsButton,  0, 5, 1,  1);
    d->grid->addWidget(d->valueEdit,       1, 0, 1, -1);
    d->grid->setColumnStretch(1, 10);
    d->grid->setContentsMargins(QMargins());
    d->grid->setSpacing(qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                             QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing)));

    populateLangAltListEntries();
    slotLocalizeChanged();

    // --------------------------------------------------------

    connect(d->languageCB, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
            this, &AltLangStrEdit::slotSelectionChanged);

    connect(d->delValueButton, &QToolButton::clicked,
            this, &AltLangStrEdit::slotDeleteValue);

    connect(d->settingsButton, &QToolButton::clicked,
            this, &AltLangStrEdit::slotOpenLocalizeSetup);

    connect(d->translateAction->m_list, &QListWidget::itemClicked,
            this, &AltLangStrEdit::slotTranslate);

    connect(d->valueEdit, &QTextEdit::textChanged,
            this, &AltLangStrEdit::slotTextChanged);

    connect(d->trengine, &DOnlineTranslator::signalFinished,
            this, &AltLangStrEdit::slotTranslationFinished);

    connect(d->translateButton, &QToolButton::pressed,
            d->translateButton, &QToolButton::showMenu);

    connect(LocalizeSettings::instance(), &LocalizeSettings::signalSettingsChanged,
            this, &AltLangStrEdit::slotLocalizeChanged);
}

AltLangStrEdit::~AltLangStrEdit()
{
    delete d;
}

QString AltLangStrEdit::languageNameRFC3066(const QString& code)
{
    LanguageCodeMap::const_iterator it = s_rfc3066ForXMP.find(code);

    if (it != s_rfc3066ForXMP.end())
    {
        return it.value();
    }

    return QString();
}

QStringList AltLangStrEdit::allLanguagesRFC3066()
{
    return s_rfc3066ForXMP.keys();
}

void AltLangStrEdit::slotEnabledInternalWidgets(bool b)
{
    d->languageCB->setEnabled(b);
    d->delValueButton->setEnabled(b);
    d->translateButton->setEnabled(b);
    d->valueEdit->setEnabled(b);
}

QString AltLangStrEdit::currentLanguageCode() const
{
    return d->currentLanguage;
}

void AltLangStrEdit::setCurrentLanguageCode(const QString& lang)
{
    if      (d->currentLanguage.isEmpty())
    {
        d->currentLanguage = QLatin1String("x-default");
    }
    else if (!lang.isEmpty())
    {
        d->currentLanguage = lang;
    }
}

QString AltLangStrEdit::languageCode(int index) const
{
    return d->languageCB->itemText(index);
}

void AltLangStrEdit::setTitle(const QString& title)
{
    QLabel* const tlabel = new QLabel(this);
    tlabel->setText(title);
    setTitleWidget(tlabel);
}

void AltLangStrEdit::setTitleWidget(QWidget* const twdg)
{
    if (d->titleWidget)
    {
        delete d->titleWidget;
    }

    d->titleWidget = twdg;
    d->grid->addWidget(d->titleWidget, 0, 0, 1, 1);
}

QWidget* AltLangStrEdit::titleWidget() const
{
    return d->titleWidget;
}

void AltLangStrEdit::setPlaceholderText(const QString& msg)
{
    d->valueEdit->setPlaceholderText(msg);
}

void AltLangStrEdit::reset()
{
    setValues(MetaEngine::AltLangMap());
}

void AltLangStrEdit::slotDeleteValue()
{
    d->values.remove(d->currentLanguage);
    setValues(d->values);

    Q_EMIT signalValueDeleted(d->currentLanguage);
}

void AltLangStrEdit::slotSelectionChanged()
{
    d->currentLanguage = d->languageCB->currentText();

    // There are bogus signals caused by spell checking, see bug #141663.
    // so we must block signals here.

    d->valueEdit->blockSignals(true);

    QString text = d->values.value(d->currentLanguage);
    d->valueEdit->setPlainText(text);
    d->delValueButton->setEnabled(!text.isNull());
    d->translateButton->setEnabled(!text.isNull());

    d->valueEdit->blockSignals(false);

    d->languageCB->setToolTip(s_rfc3066ForXMP.value(d->currentLanguage));

    // NOTE: if no specific language is set, spell-checker failback to auto-detection.

    if (d->currentLanguage == QLatin1String("x-default"))
    {
        d->valueEdit->setCurrentLanguage(QString());
    }
    else
    {
        d->valueEdit->setCurrentLanguage(d->currentLanguage.left(2));
    }

    Q_EMIT signalSelectionChanged(d->currentLanguage);
}

void AltLangStrEdit::setValues(const MetaEngine::AltLangMap& values)
{
    d->values    = values;
    populateLangAltListEntries();

    d->valueEdit->blockSignals(true);

    QString text = d->values.value(d->currentLanguage);
    d->valueEdit->setPlainText(text);
    d->delValueButton->setEnabled(!text.isNull());
    d->translateButton->setEnabled(!text.isNull());

    d->valueEdit->blockSignals(false);
}

MetaEngine::AltLangMap& AltLangStrEdit::values() const
{
    return d->values;
}

void AltLangStrEdit::populateLangAltListEntries()
{
    d->languageCB->blockSignals(true);

    d->languageCB->clear();

    // In first we fill already assigned languages.

    QStringList list = d->values.keys();

    if (!list.isEmpty())
    {
        Q_FOREACH (const QString& lg, list)
        {
            d->languageCB->addItem(lg);
            d->languageCB->setItemIcon(d->languageCB->count() - 1,
                                       QIcon::fromTheme(QLatin1String("dialog-ok-apply")).pixmap(16, 16));
            d->languageCB->setItemData(d->languageCB->findText(lg), i18n("Switch to %1", languageNameRFC3066(lg)), Qt::ToolTipRole);
        }

        d->languageCB->insertSeparator(d->languageCB->count());
    }

    // ...and now, all the rest...

    LocalizeContainer set = LocalizeSettings::instance()->settings();
    QStringList lang        = set.alternativeLang;

    Q_FOREACH (const QString& lg, lang)
    {
        if (!list.contains(lg))
        {
            d->languageCB->addItem(lg);
            d->languageCB->setItemData(d->languageCB->findText(lg), i18n("Switch to %1", languageNameRFC3066(lg)), Qt::ToolTipRole);
        }
    }

    d->languageCB->setCurrentIndex(d->languageCB->findText(d->currentLanguage));

    d->languageCB->blockSignals(false);
}

void AltLangStrEdit::populateTranslationEntries()
{
    d->translateAction->m_list->clear();

    QStringList allRFC3066  = DOnlineTranslator::supportedRFC3066();
    LocalizeContainer set = LocalizeSettings::instance()->settings();

    Q_FOREACH (const QString& lg, set.translatorLang)
    {
        QListWidgetItem* const item = new QListWidgetItem(d->translateAction->m_list);
        item->setText(lg);
        item->setToolTip(i18n("Translate to %1", languageNameRFC3066(lg)));
        d->translateAction->m_list->addItem(item);
    }
}

QString AltLangStrEdit::defaultAltLang() const
{
    return d->values.value(QLatin1String("x-default"));
}

bool AltLangStrEdit::asDefaultAltLang() const
{
    return !defaultAltLang().isNull();
}

void AltLangStrEdit::slotTextChanged()
{
    QString editedText   = d->valueEdit->toPlainText();
    QString previousText = d->values.value(d->currentLanguage);
    bool textChanged     = (editedText != previousText);

    if      (editedText.isEmpty() && textChanged)
    {
        slotDeleteValue();
    }
    else if (previousText.isNull() && textChanged)
    {
        addCurrent();
    }
    else if (textChanged)
    {
        // we cannot trust that the text actually changed
        // (there are bogus signals caused by spell checking, see bug #141663)
        // so we have to check before marking the metadata as modified.

        d->values.insert(d->currentLanguage, editedText);

        Q_EMIT signalModified(d->currentLanguage, editedText);
    }
}

void AltLangStrEdit::addCurrent()
{
    QString text = d->valueEdit->toPlainText();

    d->values.insert(d->currentLanguage, text);
    populateLangAltListEntries();
    d->delValueButton->setEnabled(true);
    d->translateButton->setEnabled(true);

    Q_EMIT signalValueAdded(d->currentLanguage, text);
}

void AltLangStrEdit::setLinesVisible(uint lines)
{
    d->linesVisible = lines;

    if (d->linesVisible == 0)
    {
        d->valueEdit->setFixedHeight(QWIDGETSIZE_MAX); // reset
    }
    else
    {
        d->valueEdit->setFixedHeight(d->valueEdit->fontMetrics().lineSpacing() * d->linesVisible         +
                                     d->valueEdit->contentsMargins().top()                               +
                                     d->valueEdit->contentsMargins().bottom()                            +
                                     1                                                                   +
                                     2*(d->valueEdit->style()->pixelMetric(QStyle::PM_DefaultFrameWidth) +
                                        d->valueEdit->style()->pixelMetric(QStyle::PM_FocusFrameVMargin))
                                    );
    }

    // It's not possible to display scrollbar properly if size is too small

    if (d->linesVisible < 3)
    {
        d->valueEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    }
}

uint AltLangStrEdit::linesVisible() const
{
    return d->linesVisible;
}

void AltLangStrEdit::changeEvent(QEvent* e)
{
    if (e->type() == QEvent::FontChange)
    {
        setLinesVisible(linesVisible());
    }

    QWidget::changeEvent(e);
}

DTextEdit* AltLangStrEdit::textEdit() const
{
    return d->valueEdit;
}

void AltLangStrEdit::slotOpenLocalizeSetup()
{
    LocalizeSettings::instance()->openLocalizeSetup();
}

void AltLangStrEdit::slotTranslate(QListWidgetItem* item)
{
    d->translateButton->menu()->close();

    if (d->trengine->isRunning())
    {
        return;
    }

    setDisabled(true);

    if (item)
    {
        d->trCode       = item->text();
        QString srcCode = currentLanguageCode();
        DOnlineTranslator::Language trLang;
        DOnlineTranslator::Language srcLang;

        if (srcCode == QLatin1String("x-default"))
        {
            srcLang = DOnlineTranslator::Auto;
        }
        else
        {
            srcLang = DOnlineTranslator::language(DOnlineTranslator::fromRFC3066(srcCode));
        }

        trLang       = DOnlineTranslator::language(DOnlineTranslator::fromRFC3066(d->trCode));
        QString text = textEdit()->text();

        qCDebug(DIGIKAM_WIDGETS_LOG) << "Request to translate with Web-service:";
        qCDebug(DIGIKAM_WIDGETS_LOG) << "Text to translate        :" << text;
        qCDebug(DIGIKAM_WIDGETS_LOG) << "To target language       :" << trLang;
        qCDebug(DIGIKAM_WIDGETS_LOG) << "With source language     :" << srcLang;

        d->trengine->translate(text,                                                            // String to translate
                               LocalizeSettings::instance()->settings().translatorEngine,     // Web service
                               trLang,                                                          // Target language
                               srcLang,                                                         // Source langage
                               DOnlineTranslator::Auto);
    }
}

void AltLangStrEdit::slotTranslationFinished()
{
    setDisabled(false);

    if (d->trengine->error() == DOnlineTranslator::NoError)
    {
        if (d->trCode.isEmpty())
        {
            return;
        }

        QString translation = d->trengine->translation();
        qCDebug(DIGIKAM_WIDGETS_LOG) << "Text translated          :" << translation;

        MetaEngine::AltLangMap vals = values();
        vals.insert(d->trCode, translation);
        setValues(vals);

        Q_EMIT signalValueAdded(d->trCode, translation);

        d->languageCB->setCurrentText(d->trCode);
        d->trCode.clear();
    }
    else
    {
        qCDebug(DIGIKAM_WIDGETS_LOG) << "Translation Error       :" << d->trengine->error();

        QMessageBox::information(qApp->activeWindow(),
                                 i18n("Failed to translate string with %1 Web-service",
                                 DOnlineTranslator::engineName(LocalizeSettings::instance()->settings().translatorEngine)),
                                 i18n("Error message: %1",
                                 d->trengine->errorString()));

    }
}

void AltLangStrEdit::slotLocalizeChanged()
{
    populateTranslationEntries();
    d->translateButton->setToolTip(i18nc("@info: language edit widget", "Select language to translate with %1",
                                   DOnlineTranslator::engineName(LocalizeSettings::instance()->settings().translatorEngine)));
}

} // namespace Digikam
