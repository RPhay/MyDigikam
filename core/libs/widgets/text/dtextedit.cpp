/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-01
 * Description : Two text edit widgets with spell checker capabilities based on KF5::Sonnet (optional).
 *               Widgets can be also limited to a number of lines to show text.
 *               A single line constraint will mimic QLineEdit. See setLinesVisible() for details.
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

// -------------------------------------------------------------------------------

class Q_DECL_HIDDEN DTextEdit::Private
{
public:

    explicit Private()
    {
    }

    /**
     * Init the text widget with the spell-checker engine (optional).
     */
    void init(DTextEdit* const parent)
    {

#ifdef HAVE_SONNET

        spellChecker = new SpellCheckDecorator(parent);

#endif

        parent->setLinesVisible(lines);

        // Mimic QLineEdit

        QObject::connect(parent, &QTextEdit::textChanged,
                         parent, [=]()
                {
                    if (lines == 1)
                    {
                        clrBtn->setVisible(!parent->text().isEmpty());
                    }
                }
        );
    }

public:

#ifdef HAVE_SONNET

    Sonnet::SpellCheckDecorator* spellChecker = nullptr;

#endif

    QString                      mask;                      ///< Mask of ignored character in text editor.

    unsigned int                 lines        = 3;

    DTextEditClearButton*        clrBtn       = nullptr;
};

DTextEdit::DTextEdit(QWidget* const parent)
    : QTextEdit(parent),
      d        (new Private)
{
    d->init(this);
}

DTextEdit::DTextEdit(const QString& contents, QWidget* const parent)
    : QTextEdit(parent),
      d        (new Private)
{
    d->init(this);
    setPlainText(contents);
}

DTextEdit::DTextEdit(unsigned int lines, QWidget* const parent)
    : QTextEdit(parent),
      d        (new Private)
{
    d->lines = lines;
    d->init(this);
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
        d->clrBtn = new DTextEditClearButton(this);
        setCornerWidget(d->clrBtn);

        connect(d->clrBtn, SIGNAL(clicked()),
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

QString DTextEdit::ignoreCharacters() const
{
    return d->mask;
}

void DTextEdit::setIgnoreCharacters(const QString& mask)
{
    d->mask = mask;
}

void DTextEdit::setCurrentLanguage(const QString& lang)
{

#ifdef HAVE_SONNET

    d->spellChecker->highlighter()->setCurrentLanguage(lang);

    qCDebug(DIGIKAM_WIDGETS_LOG) << "Spell Checker Language:" << currentLanguage();

    d->spellChecker->highlighter()->rehighlight();

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

            Q_EMIT returnPressed();

            return;
        }

        for (int i = 0 ; i < d->mask.size() ; ++i)
        {
            if (key == d->mask[i])
            {
                e->ignore();
                return;
            }
        }

        Q_EMIT textEdited(text());
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

    QString maskedTxt = scopy.text();

    for (int i = 0 ; i < d->mask.size() ; ++i)
    {
        maskedTxt.remove(d->mask[i]);
    }

    scopy.setText(maskedTxt);

    QTextEdit::insertFromMimeData(&scopy);
}

void DTextEdit::slotActiveSpellChecking(bool active)
{

#ifdef HAVE_SONNET

    d->spellChecker->highlighter()->setAutomatic(active);
    d->spellChecker->highlighter()->setActive(active);

#else

    Q_UNUSED(active);

#endif

}

bool DTextEdit::spellCheckingIsActive() const
{

#ifdef HAVE_SONNET

    return d->spellChecker->highlighter()->isActive();

#else

    return false;

#endif

}

// ------------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN DPlainTextEdit::Private
{
public:

    explicit Private()
    {
    }

    /**
     * Init the text widget with the spell-checker engine (optional).
     */
    void init(DPlainTextEdit* const parent)
    {

#ifdef HAVE_SONNET

        spellChecker = new SpellCheckDecorator(parent);

#endif

        parent->setLinesVisible(lines);

        // Mimic QLineEdit

        QObject::connect(parent, &QPlainTextEdit::textChanged,
                         parent, [=]()
                {
                    if (lines == 1)
                    {
                        clrBtn->setVisible(!parent->text().isEmpty());
                    }
                }
        );
    }

public:

#ifdef HAVE_SONNET

    Sonnet::SpellCheckDecorator* spellChecker = nullptr;

#endif

    QString                      mask;                      ///< Mask of ignored character in text editor.

    unsigned int                 lines        = 3;

    DTextEditClearButton*        clrBtn       = nullptr;
};

DPlainTextEdit::DPlainTextEdit(QWidget* const parent)
    : QPlainTextEdit(parent),
      d             (new Private)
{
    d->init(this);
}

DPlainTextEdit::DPlainTextEdit(const QString& contents, QWidget* const parent)
    : QPlainTextEdit(parent),
      d             (new Private)
{
    d->init(this);
    setPlainText(contents);
}

DPlainTextEdit::DPlainTextEdit(unsigned int lines, QWidget* const parent)
    : QPlainTextEdit(parent),
      d             (new Private)
{
    d->lines = lines;
    d->init(this);
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
        d->clrBtn = new DTextEditClearButton(this);
        setCornerWidget(d->clrBtn);

        connect(d->clrBtn, SIGNAL(clicked()),
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

QString DPlainTextEdit::ignoreCharacters() const
{
    return d->mask;
}

void DPlainTextEdit::setIgnoreCharacters(const QString& mask)
{
    d->mask = mask;
}

void DPlainTextEdit::setCurrentLanguage(const QString& lang)
{

#ifdef HAVE_SONNET

    d->spellChecker->highlighter()->setCurrentLanguage(lang);

    qCDebug(DIGIKAM_WIDGETS_LOG) << "Spell Checker Language:" << currentLanguage();

    d->spellChecker->highlighter()->rehighlight();

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

            Q_EMIT returnPressed();

            return;
        }

        for (int i = 0 ; i < d->mask.size() ; ++i)
        {
            if (key == d->mask[i])
            {
                e->ignore();
                return;
            }
        }

        Q_EMIT textEdited(text());
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

    QString maskedTxt = scopy.text();

    for (int i = 0 ; i < d->mask.size() ; ++i)
    {
        maskedTxt.remove(d->mask[i]);
    }

    scopy.setText(maskedTxt);

    QPlainTextEdit::insertFromMimeData(&scopy);
}

void DPlainTextEdit::slotActiveSpellChecking(bool active)
{

#ifdef HAVE_SONNET

    d->spellChecker->highlighter()->setAutomatic(active);
    d->spellChecker->highlighter()->setActive(active);

#else

    Q_UNUSED(active);

#endif

}

bool DPlainTextEdit::spellCheckingIsActive() const
{

#ifdef HAVE_SONNET

    return d->spellChecker->highlighter()->isActive();

#else

    return false;

#endif

}

} // namespace Digikam
