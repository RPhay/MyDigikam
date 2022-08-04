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

DTextEdit::DTextEdit(QWidget* const parent)
    : QTextEdit(parent)
{
    init();
}

DTextEdit::DTextEdit(const QString& contents, QWidget* const parent)
    : QTextEdit(parent)
{
    init();
    setPlainText(contents);
}

DTextEdit::DTextEdit(unsigned int lines, QWidget* const parent)
    : QTextEdit(parent)
{
    m_lines = lines;
    init();
}

DTextEdit::~DTextEdit()
{
}

void DTextEdit::setLinesVisible(unsigned int lines)
{
    if (lines <= 1)
    {
        return;
    }

    m_lines    = lines;

    QFont fnt;
    setFont(fnt);
    QMargins m = contentsMargins();
    setFixedHeight(m.top() + m.bottom() + frameWidth() + fontMetrics().lineSpacing() * m_lines);
}

unsigned int DTextEdit::linesVisible() const
{
    return m_lines;
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
    m_spellChecker = new SpellCheckDecorator(this);
    setLinesVisible(m_lines);
}

void DTextEdit::setCurrentLanguage(const QString& lang)
{
    m_spellChecker->highlighter()->setCurrentLanguage(lang);

    qCDebug(DIGIKAM_WIDGETS_LOG) << "Spell Checker Language:" << currentLanguage();
}

QString DTextEdit::currentLanguage() const
{
    return m_spellChecker->highlighter()->currentLanguage();
}

// ------------------------------------------------------------------------------------------------

DPlainTextEdit::DPlainTextEdit(QWidget* const parent)
    : QPlainTextEdit(parent)
{
    init();
}

DPlainTextEdit::DPlainTextEdit(const QString& contents, QWidget* const parent)
    : QPlainTextEdit(parent)
{
    init();
    setPlainText(contents);
}

DPlainTextEdit::DPlainTextEdit(unsigned int lines, QWidget* const parent)
    : QPlainTextEdit(parent)
{
    m_lines = lines;
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

    m_lines    = lines;

    QFont fnt;
    setFont(fnt);
    QMargins m = contentsMargins();
    setFixedHeight(m.top() + m.bottom() + frameWidth() + fontMetrics().lineSpacing() * m_lines);
}

unsigned int DPlainTextEdit::linesVisible() const
{
    return m_lines;
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
    m_spellChecker = new SpellCheckDecorator(this);
    setLinesVisible(m_lines);
}

void DPlainTextEdit::setCurrentLanguage(const QString& lang)
{
    m_spellChecker->highlighter()->setCurrentLanguage(lang);

    qCDebug(DIGIKAM_WIDGETS_LOG) << "Spell Checker Language:" << currentLanguage();
}

QString DPlainTextEdit::currentLanguage() const
{
    return m_spellChecker->highlighter()->currentLanguage();
}

} // namespace Digikam
