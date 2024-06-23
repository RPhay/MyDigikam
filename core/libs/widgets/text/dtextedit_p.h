/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-08-01
 * Description : Text edit widgets with spellcheck support and edition limitations.
 *
 * SPDX-FileCopyrightText: 2021-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

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
#include <QEvent>

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

    Private() = default;

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

    unsigned int                 lines          = 3;

    DTextEditClearButton*        clrBtn         = nullptr;

    LocalizeContainer            container;                 ///< Spell checking settings container.

    int                          maxLength      = 0;

    bool                         clearBtnEnable = true;
};

// ------------------------------------------------------------------------------------------------

class Q_DECL_HIDDEN DPlainTextEdit::Private
{
public:

    Private() = default;

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

    unsigned int                 lines          = 3;

    DTextEditClearButton*        clrBtn         = nullptr;

    LocalizeContainer            container;                 ///< Spell checking settings container.

    int                          maxLength      = 0;

    bool                         clearBtnEnable = true;
};

} // namespace Digikam
