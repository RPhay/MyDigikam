/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-01
 * Description : Two plain text edit widgets with spell checker capabilities based on KF5::Sonnet.
 *               Widgets can be also limited to a number of lines to show text.
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

#include "dtextedit.h"

// Qt includes

#include <QMargins>
#include <QColor>
#include <QFontMetrics>
#include <QFontDatabase>

// KDE includes

#include <sonnet/spellcheckdecorator.h>
#include <sonnet/highlighter.h>

// Local includes

#include "digikam_debug.h"

using namespace Sonnet;

namespace Digikam
{

class Q_DECL_HIDDEN DTextEdit::Private
{
public:

    explicit Private()
      : spellChecker(nullptr),
        lines       (2)
    {
    }

    Sonnet::SpellCheckDecorator* spellChecker;
    unsigned int                 lines;
};

DTextEdit::DTextEdit(QWidget* const parent)
    : QTextEdit(parent),
      d        (new Private)
{
    init();
}

DTextEdit::DTextEdit(const QString& contents, QWidget* const parent)
    : QTextEdit(parent),
      d        (new Private)
{
    init();
    setPlainText(contents);
}

DTextEdit::DTextEdit(unsigned int lines, QWidget* const parent)
    : QTextEdit(parent),
      d        (new Private)
{
    d->lines = lines;
    init();
}

DTextEdit::~DTextEdit()
{
    delete d;
}

void DTextEdit::setLinesVisible(unsigned int lines)
{
    if (lines <= 1)
    {
        return;
    }

    d->lines   = lines;

    QFont fnt;
    setFont(fnt);
    QMargins m = contentsMargins();
    setFixedHeight(m.top() + m.bottom() + frameWidth() + fontMetrics().lineSpacing() * d->lines);
}

unsigned int DTextEdit::linesVisible() const
{
    return d->lines;
}

QString DTextEdit::text() const
{
    return toPlainText();
}

void DTextEdit::setText(const QString& text)
{
    setPlainText(text);
}

void DTextEdit::init()
{
    d->spellChecker = new SpellCheckDecorator(this);
    setLinesVisible(d->lines);
}

void DTextEdit::setCurrentLanguage(const QString& lang)
{
    d->spellChecker->highlighter()->setCurrentLanguage(lang);

    qCDebug(DIGIKAM_WIDGETS_LOG) << "Spell Checker Language:" << currentLanguage();
}

QString DTextEdit::currentLanguage() const
{
    return d->spellChecker->highlighter()->currentLanguage();
}

// ------------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN DPlainTextEdit::Private
{
public:

    explicit Private()
      : spellChecker(nullptr),
        lines       (2)
    {
    }

    Sonnet::SpellCheckDecorator* spellChecker;
    unsigned int                 lines;
};

DPlainTextEdit::DPlainTextEdit(QWidget* const parent)
    : QPlainTextEdit(parent),
      d             (new Private)
{
    init();
}

DPlainTextEdit::DPlainTextEdit(const QString& contents, QWidget* const parent)
    : QPlainTextEdit(parent),
      d             (new Private)
{
    init();
    setPlainText(contents);
}

DPlainTextEdit::DPlainTextEdit(unsigned int lines, QWidget* const parent)
    : QPlainTextEdit(parent),
      d             (new Private)
{
    d->lines = lines;
    init();
}

DPlainTextEdit::~DPlainTextEdit()
{
}

void DPlainTextEdit::setLinesVisible(unsigned int lines)
{
    if (lines <= 1)
    {
        return;
    }

    d->lines   = lines;

    QFont fnt;
    setFont(fnt);
    QMargins m = contentsMargins();
    setFixedHeight(m.top() + m.bottom() + frameWidth() + fontMetrics().lineSpacing() * d->lines);
}

unsigned int DPlainTextEdit::linesVisible() const
{
    return d->lines;
}

QString DPlainTextEdit::text() const
{
    return toPlainText();
}

void DPlainTextEdit::setText(const QString& text)
{
    setPlainText(text);
}

void DPlainTextEdit::init()
{
    d->spellChecker = new SpellCheckDecorator(this);
    setLinesVisible(d->lines);
}

void DPlainTextEdit::setCurrentLanguage(const QString& lang)
{
    d->spellChecker->highlighter()->setCurrentLanguage(lang);

    qCDebug(DIGIKAM_WIDGETS_LOG) << "Spell Checker Language:" << currentLanguage();
}

QString DPlainTextEdit::currentLanguage() const
{
    return d->spellChecker->highlighter()->currentLanguage();
}

} // namespace Digikam
