/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-11-05
 * Description : simple text labels to display item
 *               properties meta infos
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "itempropertiestxtlabel.h"

// Qt includes

#include <QMargins>
#include <QPalette>
#include <QColor>
#include <QFontMetrics>
#include <QFontDatabase>

namespace Digikam
{

DTextLabelName::DTextLabelName(const QString& name, QWidget* const parent)
    : DAdjustableLabel(parent)
{
    setAdjustedText(name);
    QFont fnt;
    fnt.setItalic(true);
    setFont(fnt);
    setAlignment(Qt::AlignRight | Qt::AlignTop);
    setWordWrap(false);
}

DTextLabelName::~DTextLabelName()
{
}

// -------------------------------------------------------------------

DTextLabelValue::DTextLabelValue(const QString& value, QWidget* const parent)
    : DAdjustableLabel(parent)
{
    setAdjustedText(value);
    setAlignment(Qt::AlignLeft | Qt::AlignTop);
    setWordWrap(false);
    setElideMode(Qt::ElideRight);
}

DTextLabelValue::~DTextLabelValue()
{
}

// -------------------------------------------------------------------

DTextBrowser::DTextBrowser(const QString& text, QWidget* const parent)
    : QTextBrowser(parent)
{
    setOpenExternalLinks(false);
    setOpenLinks(false);
    setText(text);
    setLinesNumber(3);
    setFocusPolicy(Qt::NoFocus);
}

DTextBrowser::~DTextBrowser()
{
}

void DTextBrowser::setLinesNumber(int l)
{
    QFont fnt;
    document()->setDefaultFont(fnt);
    QMargins m = contentsMargins();
    setFixedHeight(m.top() + m.bottom() + frameWidth() + fontMetrics().lineSpacing()*l);
}

// -------------------------------------------------------------------

DTextList::DTextList(const QStringList& list, QWidget* const parent)
    : QListWidget(parent)
{
    addItems(list);
    setLinesNumber(6);
    setFocusPolicy(Qt::NoFocus);
    sortItems();
}

DTextList::~DTextList()
{
}

void DTextList::setLinesNumber(int l)
{
    QFont fnt;
    setFont(fnt);
    QMargins m = contentsMargins();
    setFixedHeight(m.top() + m.bottom() + frameWidth() + fontMetrics().lineSpacing()*l);
}

} // namespace Digikam
