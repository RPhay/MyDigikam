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

#include "dtextedit_p.h"

namespace Digikam
{

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
    if (lines == 0)
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
    QString maskedTxt = text;

    for (int i = 0 ; i < d->ignoredMask.size() ; ++i)
    {
        maskedTxt.remove(d->ignoredMask[i]);
    }

    if (!d->acceptedMask.isEmpty())
    {
        QString maskedTxt2;

        for (int i = 0 ; i < maskedTxt.size() ; ++i)
        {
            if (d->acceptedMask.contains(maskedTxt[i]))
            {
                maskedTxt2.append(maskedTxt[i]);
            }
        }

        maskedTxt = maskedTxt2;
    }

    setPlainText(maskedTxt);
}

QString DTextEdit::ignoredCharacters() const
{
    return d->ignoredMask;
}

void DTextEdit::setIgnoredCharacters(const QString& mask)
{
    d->ignoredMask = mask;
}

QString DTextEdit::acceptedCharacters() const
{
    return d->acceptedMask;
}

void DTextEdit::setAcceptedCharacters(const QString& mask)
{
    d->acceptedMask = mask;
}

void DTextEdit::setCurrentLanguage(const QString& lang)
{

#ifdef HAVE_SONNET

    if (!lang.isEmpty())
    {
        d->spellChecker->highlighter()->setAutoDetectLanguageDisabled(true);
        d->spellChecker->highlighter()->setCurrentLanguage(lang);

        qCDebug(DIGIKAM_WIDGETS_LOG) << "Spell Checker Language:" << currentLanguage();

        d->spellChecker->highlighter()->rehighlight();
    }
    else
    {
        d->spellChecker->highlighter()->setAutoDetectLanguageDisabled(false);

        qCDebug(DIGIKAM_WIDGETS_LOG) << "Spell Checker Language auto-detection enabled";

        d->spellChecker->highlighter()->rehighlight();
    }

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

        for (int i = 0 ; i < d->ignoredMask.size() ; ++i)
        {
            if (key == d->ignoredMask[i])
            {
                e->ignore();
                return;
            }
        }

        for (int i = 0 ; i < d->acceptedMask.size() ; ++i)
        {
            if (key != d->acceptedMask[i])
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

    for (int i = 0 ; i < d->ignoredMask.size() ; ++i)
    {
        maskedTxt.remove(d->ignoredMask[i]);
    }

    scopy.setText(maskedTxt);

    if (!d->acceptedMask.isEmpty())
    {
        QString maskedTxt2;

        for (int i = 0 ; i < maskedTxt.size() ; ++i)
        {
            if (d->acceptedMask.contains(maskedTxt[i]))
            {
                maskedTxt2.append(maskedTxt[i]);
            }
        }

        scopy.setText(maskedTxt2);
    }

    QTextEdit::insertFromMimeData(&scopy);
}

void DTextEdit::setLocalizeSettings(const LocalizeContainer& settings)
{
    d->container = settings;

#ifdef HAVE_SONNET

    // Automatic disable spellcheck if too many spelling errors are detected.

    d->spellChecker->highlighter()->setAutomatic(!d->container.enableSpellCheck);

    // Enable spellchecker globally.

    d->spellChecker->highlighter()->setActive(d->container.enableSpellCheck);

    Q_FOREACH (const QString& word, d->container.ignoredWords)
    {
        d->spellChecker->highlighter()->ignoreWord(word);
    }

    d->spellChecker->highlighter()->rehighlight();

#endif

}

LocalizeContainer DTextEdit::spellCheckSettings() const
{
    return d->container;
}

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
    if (lines == 0)
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
    QString maskedTxt = text;

    for (int i = 0 ; i < d->ignoredMask.size() ; ++i)
    {
        maskedTxt.remove(d->ignoredMask[i]);
    }

    if (!d->acceptedMask.isEmpty())
    {
        QString maskedTxt2;

        for (int i = 0 ; i < maskedTxt.size() ; ++i)
        {
            if (d->acceptedMask.contains(maskedTxt[i]))
            {
                maskedTxt2.append(maskedTxt[i]);
            }
        }

        maskedTxt = maskedTxt2;
    }

    setPlainText(maskedTxt);
}

QString DPlainTextEdit::ignoredCharacters() const
{
    return d->ignoredMask;
}

void DPlainTextEdit::setIgnoredCharacters(const QString& mask)
{
    d->ignoredMask = mask;
}

QString DPlainTextEdit::acceptedCharacters() const
{
    return d->acceptedMask;
}

void DPlainTextEdit::setAcceptedCharacters(const QString& mask)
{
    d->acceptedMask = mask;
}

void DPlainTextEdit::setCurrentLanguage(const QString& lang)
{

#ifdef HAVE_SONNET

    if (!lang.isEmpty())
    {
        d->spellChecker->highlighter()->setAutoDetectLanguageDisabled(true);
        d->spellChecker->highlighter()->setCurrentLanguage(lang);

        qCDebug(DIGIKAM_WIDGETS_LOG) << "Spell Checker Language:" << currentLanguage();

        d->spellChecker->highlighter()->rehighlight();
    }
    else
    {
        d->spellChecker->highlighter()->setAutoDetectLanguageDisabled(false);

        qCDebug(DIGIKAM_WIDGETS_LOG) << "Spell Checker Language auto-detection enabled";

        d->spellChecker->highlighter()->rehighlight();
    }

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

        for (int i = 0 ; i < d->ignoredMask.size() ; ++i)
        {
            if (key == d->ignoredMask[i])
            {
                e->ignore();
                return;
            }
        }

        for (int i = 0 ; i < d->acceptedMask.size() ; ++i)
        {
            if (key != d->acceptedMask[i])
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

    for (int i = 0 ; i < d->ignoredMask.size() ; ++i)
    {
        maskedTxt.remove(d->ignoredMask[i]);
    }

    scopy.setText(maskedTxt);

    if (!d->acceptedMask.isEmpty())
    {
        QString maskedTxt2;

        for (int i = 0 ; i < maskedTxt.size() ; ++i)
        {
            if (d->acceptedMask.contains(maskedTxt[i]))
            {
                maskedTxt2.append(maskedTxt[i]);
            }
        }

        scopy.setText(maskedTxt2);
    }

    QPlainTextEdit::insertFromMimeData(&scopy);
}

void DPlainTextEdit::setLocalizeSettings(const LocalizeContainer& settings)
{
    d->container = settings;

#ifdef HAVE_SONNET

    // Automatic disable spellcheck if too many spelling errors are detected.

    d->spellChecker->highlighter()->setAutomatic(!d->container.enableSpellCheck);

    // Enable spellchecker globally.

    d->spellChecker->highlighter()->setActive(d->container.enableSpellCheck);

    Q_FOREACH (const QString& word, d->container.ignoredWords)
    {
        d->spellChecker->highlighter()->ignoreWord(word);
    }

    d->spellChecker->highlighter()->rehighlight();

#endif

}

LocalizeContainer DPlainTextEdit::spellCheckSettings() const
{
    return d->container;
}

} // namespace Digikam

#include "dtextedit.moc"
