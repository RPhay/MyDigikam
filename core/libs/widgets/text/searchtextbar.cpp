/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-11-25
 * Description : a bar used to search a string - version not based on database models
 *
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009-2010 by Johannes Wienke <languitar at semipol dot de>
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

#include "searchtextbar.h"

// Qt includes

#include <QContextMenuEvent>
#include <QMenu>
#include <QColor>
#include <QPalette>
#include <QString>
#include <QMap>
#include <QTimer>

// KDE includes

#include <klocalizedstring.h>
#include <kconfiggroup.h>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

bool operator==(const SearchTextSettings& a, const SearchTextSettings& b)
{
    return ((a.caseSensitive == b.caseSensitive) && (a.text == b.text));
}

class Q_DECL_HIDDEN SearchTextBar::Private
{
public:

    explicit Private()
      : optionAutoCompletionModeEntry(QLatin1String("AutoCompletionMode")),
        optionCaseSensitiveEntry     (QLatin1String("CaseSensitive")),
        textQueryCompletion          (false),
        hasCaseSensitive             (true),
        highlightOnResult            (true),
        hasResultColor               (200, 255, 200),
        hasNoResultColor             (255, 200, 200),
        completer                    (nullptr),
        searchTimer                  (nullptr)
    {
    }

    QString            optionAutoCompletionModeEntry;
    QString            optionCaseSensitiveEntry;

    bool               textQueryCompletion;
    bool               hasCaseSensitive;
    bool               highlightOnResult;

    QColor             hasResultColor;
    QColor             hasNoResultColor;

    ModelCompleter*    completer;

    SearchTextSettings settings;
    QTimer*            searchTimer;   ///< Timer to delay search processing and not signals bombarding at each key pressed.
};

SearchTextBar::SearchTextBar(QWidget* const parent, const QString& name, const QString& msg)
    : QLineEdit(parent),
      StateSavingObject(this),
      d                (new Private)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    setObjectName(name + QLatin1String(" Search Text Tool"));
    setClearButtonEnabled(true);
    setPlaceholderText(msg.isNull() ? i18nc("@info: search text bar", "Search...") : msg);

    d->completer   = new ModelCompleter(this);
    setCompleter(d->completer);

    d->searchTimer = new QTimer(this);
    d->searchTimer->setInterval(500);
    d->searchTimer->setSingleShot(true);

    connect(d->searchTimer, SIGNAL(timeout()),
            this, SLOT(slotTextChanged()));

    connect(this, SIGNAL(textChanged(QString)),
            d->searchTimer, SLOT(start()));

    connect(d->completer, QOverload<>::of(&ModelCompleter::signalActivated),
            [=]()
            {
                emit completerActivated();
            }
    );

    connect(d->completer, QOverload<const int>::of(&ModelCompleter::signalHighlighted),
            [=](const int albumId)
            {
                emit completerHighlighted(albumId);
            }
    );

    loadState();
}

SearchTextBar::~SearchTextBar()
{
    saveState();
    delete d;
}

void SearchTextBar::doLoadState()
{
    KConfigGroup group        = getConfigGroup();
    completer()->setCompletionMode((QCompleter::CompletionMode)group.readEntry(entryName(d->optionAutoCompletionModeEntry),
                                                                               (int)QCompleter::PopupCompletion));
    d->settings.caseSensitive = (Qt::CaseSensitivity)group.readEntry(entryName(d->optionCaseSensitiveEntry),
                                                                     (int)Qt::CaseInsensitive);
    setIgnoreCase(d->settings.caseSensitive == Qt::CaseInsensitive);
}

void SearchTextBar::doSaveState()
{
    KConfigGroup group = getConfigGroup();

    if (completer()->completionMode() != QCompleter::PopupCompletion)
    {
        group.writeEntry(entryName(d->optionAutoCompletionModeEntry), (int)completer()->completionMode());
    }
    else
    {
        group.deleteEntry(entryName(d->optionAutoCompletionModeEntry));
    }

    group.writeEntry(entryName(d->optionCaseSensitiveEntry), (int)d->settings.caseSensitive);
    group.sync();
}

void SearchTextBar::setTextQueryCompletion(bool b)
{
    d->textQueryCompletion = b;
}

bool SearchTextBar::hasTextQueryCompletion() const
{
    return d->textQueryCompletion;
}

void SearchTextBar::setHighlightOnResult(bool highlight)
{
    d->highlightOnResult = highlight;

    if (!highlight)
    {
        setPalette(QPalette());
    }
}

SearchTextBar::HighlightState SearchTextBar::getCurrentHighlightState() const
{
    if      (palette() == QPalette())
    {
        return NEUTRAL;
    }
    else if (palette().color(QPalette::Active, QPalette::Base) == d->hasResultColor)
    {
        return HAS_RESULT;
    }
    else if (palette().color(QPalette::Active, QPalette::Base) == d->hasNoResultColor)
    {
        return NO_RESULT;
    }

    qCDebug(DIGIKAM_WIDGETS_LOG) << "Impossible highlighting state";

    return NEUTRAL;
}

void SearchTextBar::setCaseSensitive(bool b)
{
    d->hasCaseSensitive = b;

    // Reset settings if selecting case sensitivity is not allowed

    if (!b)
    {
        d->settings.caseSensitive = Qt::CaseInsensitive;
    }

    // Re-emit signal with changed settings

    if (!text().isEmpty())
    {
        emit signalSearchTextSettings(d->settings);
    }
}

bool SearchTextBar::hasCaseSensitive() const
{
    return d->hasCaseSensitive;
}

void SearchTextBar::setSearchTextSettings(const SearchTextSettings& settings)
{
    d->settings = settings;
}

SearchTextSettings SearchTextBar::searchTextSettings() const
{
    return d->settings;
}

ModelCompleter* SearchTextBar::completerModel() const
{
    return d->completer;
}

void SearchTextBar::slotTextChanged()
{
    QString txt = text();

    if (txt.isEmpty())
    {
        setPalette(QPalette());
    }

    d->settings.text = txt;

    emit signalSearchTextSettings(d->settings);
}

void SearchTextBar::slotSearchResult(bool match)
{
    // only highlight if text is not empty or highlighting is disabled.

    if (text().isEmpty() || !d->highlightOnResult)
    {
        setPalette(QPalette());
        return;
    }

    QPalette pal = palette();
    pal.setColor(QPalette::Active, QPalette::Base,
                 match ? d->hasResultColor :
                 d->hasNoResultColor);
    pal.setColor(QPalette::Active, QPalette::Text, Qt::black);
    setPalette(pal);
}

void SearchTextBar::contextMenuEvent(QContextMenuEvent* e)
{
    QAction* cs       = nullptr;
    QMenu* const menu = createStandardContextMenu();

    if (d->hasCaseSensitive)
    {
        cs = menu->addAction(i18nc("@info: search text bar", "Case sensitive"));
        cs->setCheckable(true);
        cs->setChecked(d->settings.caseSensitive == Qt::CaseInsensitive ? false : true);
    }

    menu->exec(e->globalPos());

    if (d->hasCaseSensitive)
    {
        setIgnoreCase(!cs->isChecked());
    }

    delete menu;
}

void SearchTextBar::setIgnoreCase(bool ignore)
{
    if (hasCaseSensitive())
    {
        if (ignore)
        {
            completer()->setCaseSensitivity(Qt::CaseInsensitive);
            d->settings.caseSensitive = Qt::CaseInsensitive;
        }
        else
        {
            completer()->setCaseSensitivity(Qt::CaseSensitive);
            d->settings.caseSensitive = Qt::CaseSensitive;
        }
    }
    else
    {
        completer()->setCaseSensitivity(Qt::CaseInsensitive);
        d->settings.caseSensitive = Qt::CaseInsensitive;
    }

    emit signalSearchTextSettings(d->settings);
}

} // namespace Digikam
