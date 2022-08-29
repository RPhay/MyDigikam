/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-01
 * Description : Text edit widgets with spellcheck support and edition limitations.
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
#include <QUrl>
#include <QToolTip>

// KDE includes

#ifdef HAVE_SONNET
#   include <sonnet/spellcheckdecorator.h>
#   include <sonnet/highlighter.h>

using namespace Sonnet;

#endif

#include <klocalizedstring.h>

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

    int                          maxLength    = 0;
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

    int                          maxLength    = 0;
};

} // namespace Digikam

#endif // DIGIKAM_LINE_EDIT_SPELL_CHECKER_P_H
