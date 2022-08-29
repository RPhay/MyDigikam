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

#ifndef DIGIKAM_DTEXT_EDIT_P_H
#define DIGIKAM_DTEXT_EDIT_P_H

#include "dtextedit.h"
#include "digikam_config.h"

// Qt includes

#include <QMargins>
#include <QColor>
#include <QTextDocument>
#include <QFontMetrics>
#include <QFontDatabase>
#include <QMimeData>
#include <QLabel>
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
#include "localizesettings.h"
#include "localizecontainer.h"

namespace Digikam
{

class Q_DECL_HIDDEN DTextEditClearButton : public QLabel
{
    Q_OBJECT

public:

    explicit DTextEditClearButton(QWidget* const parent);

Q_SIGNALS:

    void clicked();

protected:

    void mousePressEvent(QMouseEvent* e) override;
};

// -------------------------------------------------------------------------------

class Q_DECL_HIDDEN DTextEdit::Private
{
public:

    explicit Private();

    /**
     * Init the text widget with the spell-checker engine (optional).
     */
    void init(DTextEdit* const parent);

public:

#ifdef HAVE_SONNET

    Sonnet::SpellCheckDecorator* spellChecker = nullptr;

#endif

    QString                      ignoredMask;               ///< Mask of ignored characters in text editor.
    QString                      acceptedMask;              ///< Mask of accepted characters in text editor.

    unsigned int                 lines        = 3;

    DTextEditClearButton*        clrBtn       = nullptr;

    LocalizeContainer            container;                 ///< Spell checking settings container.
};

// ------------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN DPlainTextEdit::Private
{
public:

    explicit Private();

    /**
     * Init the text widget with the spell-checker engine (optional).
     */
    void init(DPlainTextEdit* const parent);

public:

#ifdef HAVE_SONNET

    Sonnet::SpellCheckDecorator* spellChecker = nullptr;

#endif

    QString                      ignoredMask;               ///< Mask of ignored characters in text editor.
    QString                      acceptedMask;              ///< Mask of accepted characters in text editor.

    unsigned int                 lines        = 3;

    DTextEditClearButton*        clrBtn       = nullptr;

    LocalizeContainer            container;                 ///< Spell checking settings container.
};

} // namespace Digikam

#endif // DIGIKAM_LINE_EDIT_SPELL_CHECKER_P_H
