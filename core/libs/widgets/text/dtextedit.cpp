/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-01
 * Description : Two plain text edit widgets with spell checker capabilities based on KF5::Sonnet (optional).
 *               Widgets can be also limited to a number of lines to show text.
 *               A single line constraint will mimic QLineEdit.
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
#include <QTextDocument>
#include <QFontMetrics>
#include <QFontDatabase>
#include <QMimeData>
#include <QPushButton>
#include <QIcon>
#include <QStyle>
#include <QPainter>
#include <QApplication>
#include <QScrollBar>

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

class Q_DECL_HIDDEN DTextEditClearButton : public QPushButton
{
public:

    explicit DTextEditClearButton(QWidget* const parent)
        : QPushButton(parent)
    {
        setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        setFocusPolicy(Qt::NoFocus);
        setFlat(true);
        setIcon(qApp->style()->standardIcon(QStyle::SP_LineEditClearButton));
    }
};

class Q_DECL_HIDDEN DTextEdit::Private
{
public:

    explicit Private()
    {
    }

#ifdef HAVE_SONNET

    Sonnet::SpellCheckDecorator* spellChecker = nullptr;

#endif

    unsigned int                 lines        = 3;
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
    if (lines <= 0)
    {
        return;
    }

    d->lines    = lines;

    QFont fnt;
    setFont(fnt);
    QMargins m  = contentsMargins();
    qreal md    = document()->documentMargin();
    setFixedHeight(m.top() + m.bottom() + md +
                   frameWidth() * 2          +
                   fontMetrics().lineSpacing() * d->lines);

    // Mimic QLineEdit

    if (d->lines == 1)
    {
        setLineWrapMode(QTextEdit::NoWrap);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        verticalScrollBar()->setFixedHeight(0);
        DTextEditClearButton* const clr = new DTextEditClearButton(this);
        setCornerWidget(clr);

        connect(clr, SIGNAL(clicked()),
                this, SLOT(clear()));
    }
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

void DTextEdit::keyPressEvent(QKeyEvent* e)
{
    if (d->lines == 1)
    {
        int key = e->key();

        if ((key == Qt::Key_Return) || (key == Qt::Key_Enter))
        {
            e->ignore();
            return;
        }
    }

    QTextEdit::keyPressEvent(e);
}

void DTextEdit::insertFromMimeData(const QMimeData* source)
{
    QMimeData scopy;

    if (source->hasHtml())
    {
        scopy.setHtml(source->html());
    }

    if (source->hasText())
    {
        scopy.setText(source->text());
    }

    if (source->hasUrls())
    {
        scopy.setUrls(source->urls());
    }

    if ((d->lines == 1) && source->hasText())
    {
        QString textToPaste = source->text();
        textToPaste.replace(QLatin1String("\n\r"), QLatin1String(" "));
        textToPaste.replace(QLatin1Char('\n'),     QLatin1Char(' '));
        scopy.setText(textToPaste);
    }

    QTextEdit::insertFromMimeData(&scopy);
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

    unsigned int                 lines        = 3;
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
    if (lines <= 0)
    {
        return;
    }

    d->lines    = lines;

    QFont fnt;
    setFont(fnt);
    QMargins m  = contentsMargins();
    qreal md    = document()->documentMargin();
    setFixedHeight(m.top() + m.bottom() + md +
                   frameWidth() * 2          +
                   fontMetrics().lineSpacing() * d->lines);

    // Mimic QLineEdit

    if (d->lines == 1)
    {
        setLineWrapMode(QPlainTextEdit::NoWrap);
        setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        verticalScrollBar()->setFixedHeight(0);
        DTextEditClearButton* const clr = new DTextEditClearButton(this);
        setCornerWidget(clr);

        connect(clr, SIGNAL(clicked()),
                this, SLOT(clear()));
    }
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

void DPlainTextEdit::keyPressEvent(QKeyEvent* e)
{
    if (d->lines == 1)
    {
        int key = e->key();

        if ((key == Qt::Key_Return) || (key == Qt::Key_Enter))
        {
            e->ignore();
            return;
        }
    }

    QPlainTextEdit::keyPressEvent(e);
}

void DPlainTextEdit::insertFromMimeData(const QMimeData* source)
{
    QMimeData scopy;

    if (source->hasHtml())
    {
        scopy.setHtml(source->html());
    }

    if (source->hasText())
    {
        scopy.setText(source->text());
    }

    if (source->hasUrls())
    {
        scopy.setUrls(source->urls());
    }

    if ((d->lines == 1) && source->hasText())
    {
        QString textToPaste = source->text();
        textToPaste.replace(QLatin1String("\n\r"), QLatin1String(" "));
        textToPaste.replace(QLatin1Char('\n'),     QLatin1Char(' '));
        scopy.setText(textToPaste);
    }

    QPlainTextEdit::insertFromMimeData(&scopy);
}

} // namespace Digikam
