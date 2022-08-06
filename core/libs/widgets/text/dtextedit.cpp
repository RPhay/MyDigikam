/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-01
 * Description : Two plain text edit widgets with spell checker capabilities based on KF5::Sonnet (optional).
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
#include "digikam_config.h"

// Qt includes

#include <QMargins>
#include <QColor>
#include <QFontMetrics>
#include <QFontDatabase>

// KDE includes

#ifdef HAVE_SONNET
#   include <sonnet/spellcheckdecorator.h>
#   include <sonnet/highlighter.h>

using namespace Sonnet;

#endif

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN DTextEdit::Private
{
public:

    explicit Private()
    {
    }

#ifdef HAVE_SONNET

    Sonnet::SpellCheckDecorator* spellChecker = nullptr;

#endif

    unsigned int                 lines        = 2;
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
#ifdef HAVE_SONNET

    d->spellChecker = new SpellCheckDecorator(this);

#endif

    setLinesVisible(d->lines);
}

void DTextEdit::setCurrentLanguage(const QString& lang)
{

#ifdef HAVE_SONNET

    d->spellChecker->highlighter()->setCurrentLanguage(lang);

    qCDebug(DIGIKAM_WIDGETS_LOG) << "Spell Checker Language:" << currentLanguage();

#else

    Q_UNUSED(lang);

#endif

}

QString DTextEdit::currentLanguage() const
{

#ifdef HAVE_SONNET

    return d->spellChecker->highlighter()->currentLanguage();

#else

    return QString();

#endif

}

// ------------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN DPlainTextEdit::Private
{
public:

    explicit Private()
    {
    }

#ifdef HAVE_SONNET

    Sonnet::SpellCheckDecorator* spellChecker = nullptr;

#endif

    unsigned int                 lines        = 2;
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
    delete d;
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

#ifdef HAVE_SONNET

    d->spellChecker = new SpellCheckDecorator(this);

#endif

    setLinesVisible(d->lines);
}

void DPlainTextEdit::setCurrentLanguage(const QString& lang)
{

#ifdef HAVE_SONNET

    d->spellChecker->highlighter()->setCurrentLanguage(lang);

    qCDebug(DIGIKAM_WIDGETS_LOG) << "Spell Checker Language:" << currentLanguage();

#else

    Q_UNUSED(lang);

#endif

}

QString DPlainTextEdit::currentLanguage() const
{

#ifdef HAVE_SONNET

    return d->spellChecker->highlighter()->currentLanguage();

#else

    return QString();

#endif

}

} // namespace Digikam
