/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2020-08-29
 * Description : a widget to edit a long text with limited string size.
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "limitedtextedit.h"

// Qt includes

#include <QUrl>

// KDE includes

#include <klocalizedstring.h>

namespace DigikamGenericMetadataEditPlugin
{

LimitedTextEdit::LimitedTextEdit(QWidget* const parent)
    : QPlainTextEdit(parent),
      m_maxLength   (0)
{
}

LimitedTextEdit::~LimitedTextEdit()
{
}

int LimitedTextEdit::maxLength() const
{
    return m_maxLength;
}

int LimitedTextEdit::leftCharacters() const
{
    int left = m_maxLength - toPlainText().length();

    return ((left > 0) ? left : 0);
}

void LimitedTextEdit::setMaxLength(int length)
{
    m_maxLength     = length;
    QString curText = toPlainText();

    if ((m_maxLength > 0) && (curText.length() > m_maxLength))
    {
        curText = curText.left(m_maxLength);
        setPlainText(curText);
    }
}

void LimitedTextEdit::keyPressEvent(QKeyEvent* e)
{
    int csize = toPlainText().length();

    if ((m_maxLength <= 0) || (csize < m_maxLength))
    {
        QPlainTextEdit::keyPressEvent(e);
    }
    else
    {
        QString txt       = e->text();
        int key           = e->key();
        bool delCondition = (key == Qt::Key_Delete)    ||
                            (key == Qt::Key_Backspace) ||
                            (key == Qt::Key_Cancel);

        bool asciiCtrl    = false;

        if (txt.length() == 1)
        {
            int sym = txt.toLatin1().at(0);

            if (((sym >= 0) && (sym <= 31)) || sym == 127)
            {
                asciiCtrl = true;
            }
        }

        if (txt.isEmpty() || delCondition || asciiCtrl)
        {
            QPlainTextEdit::keyPressEvent(e);
        }
    }
}

void LimitedTextEdit::insertFromMimeData(const QMimeData* source)
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

    if ((m_maxLength > 0) && source->hasText())
    {
        QString textToPaste = source->text();
        QString curText     = toPlainText();
        int totalLength     = curText.length() + textToPaste.length();

        if      (curText.length() == m_maxLength)
        {
            scopy.setText(QString());
        }
        else if (totalLength > m_maxLength)
        {
            int numToDelete = totalLength - m_maxLength;
            textToPaste     = textToPaste.left(textToPaste.length() - numToDelete);
            scopy.setText(textToPaste);
        }
    }

    QPlainTextEdit::insertFromMimeData(&scopy);
}

} // namespace DigikamGenericMetadataEditPlugin
