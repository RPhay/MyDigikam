/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-15
 * Description : localize selector widget
 *
 * Copyright (C) 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "localizeselector.h"

// Qt includes

#include <QLabel>
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QListWidget>
#include <QWidgetAction>
#include <QStyle>
#include <QToolButton>
#include <QApplication>
#include <QScrollBar>
#include <QListWidgetItem>
#include <QGridLayout>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "localizesettings.h"
#include "donlinetranslator.h"
#include "altlangstredit.h"

namespace Digikam
{

class Q_DECL_HIDDEN LocalizeSelector::Private
{
public:

    class Q_DECL_HIDDEN TranslateAction : public QWidgetAction
    {
    public:

        explicit TranslateAction(QObject* const parent)
            : QWidgetAction(parent)
        {
        }

        QWidget* createWidget(QWidget* parent) override
        {
            m_list         = new QListWidget(parent);
            QFontMetrics fontMt(m_list->font());
            QRect fontRect = fontMt.boundingRect(0, 0, m_list->width(), m_list->height(), 0, QLatin1String("XX-XX"));
            int width      =  m_list->contentsMargins().left() + m_list->contentsMargins().right();
            width         += fontRect.width() + m_list->verticalScrollBar()->height();
            m_list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
            m_list->setFixedWidth(width);

            return m_list;
        }

    public:

        QListWidget* m_list = nullptr;
    };

public:

    explicit Private()
      : translateButton (nullptr),
        settingsButton  (nullptr),
        translateAction (nullptr)
    {
    }

public:

    QToolButton*                   translateButton;
    QToolButton*                   settingsButton;

    TranslateAction*               translateAction;
};

LocalizeSelector::LocalizeSelector(QWidget* const parent)
    : DHBox(parent),
      d      (new Private)
{
    d->translateButton = new QToolButton(this);
    d->translateButton->setIcon(QIcon::fromTheme(QLatin1String("language-chooser")));
    d->translateButton->setPopupMode(QToolButton::MenuButtonPopup);
    QMenu* const menu  = new QMenu(d->translateButton);
    d->translateAction = new Private::TranslateAction(d->translateButton);
    menu->addAction(d->translateAction);
    d->translateButton->setMenu(menu);

    d->settingsButton  = new QToolButton(this);
    d->settingsButton->setIcon(QIcon::fromTheme(QLatin1String("configure")));
    d->settingsButton->setToolTip(i18nc("@info", "Open localize setup"));

    setContentsMargins(QMargins());
    setSpacing(qMin(QApplication::style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing),
                    QApplication::style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing)));

    slotLocalizeChanged();

    // --------------------------------------------------------

    connect(d->settingsButton, &QToolButton::clicked,
            this, &LocalizeSelector::slotOpenLocalizeSetup);

    connect(d->translateAction->m_list, &QListWidget::itemClicked,
            this, &LocalizeSelector::slotTranslate);

    connect(d->translateButton, &QToolButton::pressed,
            d->translateButton, &QToolButton::showMenu);

    connect(LocalizeSettings::instance(), &LocalizeSettings::signalSettingsChanged,
            this, &LocalizeSelector::slotLocalizeChanged);
}

LocalizeSelector::~LocalizeSelector()
{
    delete d;
}

void LocalizeSelector::populateTranslationEntries()
{
    d->translateAction->m_list->clear();

    QStringList allRFC3066  = DOnlineTranslator::supportedRFC3066(LocalizeSettings::instance()->settings().translatorEngine);
    LocalizeContainer set   = LocalizeSettings::instance()->settings();

    Q_FOREACH (const QString& lg, set.translatorLang)
    {
        QListWidgetItem* const item = new QListWidgetItem(d->translateAction->m_list);
        item->setText(lg);
        item->setToolTip(i18nc("@info", "Translate to %1", AltLangStrEdit::languageNameRFC3066(lg)));
        d->translateAction->m_list->addItem(item);
    }
}

void LocalizeSelector::slotOpenLocalizeSetup()
{
    LocalizeSettings::instance()->openLocalizeSetup();
}

void LocalizeSelector::slotLocalizeChanged()
{
    populateTranslationEntries();
    d->translateButton->setToolTip(i18nc("@info: language edit widget", "Select language to translate with %1",
                                   DOnlineTranslator::engineName(LocalizeSettings::instance()->settings().translatorEngine)));
}

void LocalizeSelector::slotTranslate(QListWidgetItem* item)
{
    d->translateButton->menu()->close();

    if (item)
    {
        Q_EMIT signalTranslate(item->text());
    }
}

// ------------------------------------------------------------------------

class Q_DECL_HIDDEN LocalizeSelectorList::Private
{
public:

    explicit Private()
      : trLabel   (nullptr),
        trSelector(nullptr),
        trList    (nullptr)
    {
    }

public:

    QLabel*           trLabel;
    LocalizeSelector* trSelector;
    QListWidget*      trList;
};

LocalizeSelectorList::LocalizeSelectorList(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    QGridLayout* const grid  = new QGridLayout(this);

    d->trLabel               = new QLabel(this);
    d->trSelector            = new LocalizeSelector(this);
    d->trList                = new QListWidget(this);
    d->trList->setContextMenuPolicy(Qt::CustomContextMenu);

    grid->addWidget(d->trLabel,      0, 0, 1, 1);
    grid->addWidget(d->trSelector,   0, 1, 1, 1);
    grid->addWidget(d->trList,       1, 0, 1, 2);
    grid->setColumnStretch(0, 10);
    grid->setRowStretch(2, 10);

    connect(d->trSelector, SIGNAL(signalTranslate(QString)),
            this, SLOT(slotAppendTranslation(QString)));

    connect(d->trList, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(slotShowContextMenu(QPoint)));
}

LocalizeSelectorList::~LocalizeSelectorList()
{
    delete d;
}

void LocalizeSelectorList::setTitle(const QString& title)
{
    d->trLabel->setText(title);
}

LocalizeSelector* LocalizeSelectorList::selector() const
{
    return d->trSelector;
}

void LocalizeSelectorList::slotShowContextMenu(const QPoint& pos)
{
    QPoint globalPos = d->trList->mapToGlobal(pos);

    QMenu menu;
    QAction* const rm = menu.addAction(i18nc("@action", "Remove this entry"));
    QAction* const cl = menu.addAction(i18nc("@action", "Clear List"));
    QAction* const ac = menu.exec(globalPos);

    if      (ac == rm)
    {
        delete d->trList->takeItem(d->trList->currentRow());
    }
    else if (ac == cl)
    {
        d->trList->clear();
    }

    Q_EMIT signalSettingsChanged();
}

void LocalizeSelectorList::slotAppendTranslation(const QString& lang)
{
    for (int i = 0 ; i < d->trList->count() ; ++i)
    {
        if (d->trList->item(i)->text().startsWith(lang))
        {
            return;
        }
    }

    d->trList->addItem(QString::fromUtf8("%1 - %2").arg(lang).arg(AltLangStrEdit::languageNameRFC3066(lang)));

    Q_EMIT signalSettingsChanged();
}

} // namespace Digikam

