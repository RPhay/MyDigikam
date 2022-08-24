/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-15
 * Description : multi-languages string editor
 *
 * Copyright (C) 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_ALT_LANG_STR_EDIT_P_H
#define DIGIKAM_ALT_LANG_STR_EDIT_P_H

#include "altlangstredit.h"

// Qt includes

#include <QEvent>
#include <QIcon>
#include <QLineEdit>
#include <QFontMetrics>
#include <QRect>
#include <QMenu>
#include <QListWidget>
#include <QWidgetAction>
#include <QStyle>
#include <QLabel>
#include <QToolButton>
#include <QGridLayout>
#include <QApplication>
#include <QComboBox>
#include <QScrollBar>
#include <QListWidgetItem>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "donlinetranslator.h"
#include "spellchecksettings.h"
#include "dexpanderbox.h"

namespace Digikam
{

typedef QMap<QString, QString> LanguageCodeMap;

extern const LanguageCodeMap s_rfc3066ForXMP;

// ----------------------------------------------------------------------

class Q_DECL_HIDDEN TranslateAction : public QWidgetAction
{
    Q_OBJECT

public:

    explicit TranslateAction(QObject* const parent);

    QWidget* createWidget(QWidget* parent) override;

public:

    QListWidget* m_list = nullptr;
};

// ----------------------------------------------------------------------

class Q_DECL_HIDDEN AltLangStrEdit::Private
{
public:

    explicit Private()
      : currentLanguage (QLatin1String("x-default")),
        linesVisible    (0),
        grid            (nullptr),
        titleWidget     (nullptr),
        delValueButton  (nullptr),
        translateButton (nullptr),
        settingsButton  (nullptr),
        valueEdit       (nullptr),
        languageCB      (nullptr),
        translateAction (nullptr),
        trengine        (nullptr)
    {
    }

    ~Private()
    {
    }

public:

    QString                        currentLanguage;

    uint                           linesVisible;

    QGridLayout*                   grid;

    QWidget*                       titleWidget;

    QToolButton*                   delValueButton;
    QToolButton*                   translateButton;
    QToolButton*                   settingsButton;

    DTextEdit*                     valueEdit;

    QComboBox*                     languageCB;
    TranslateAction*               translateAction;
    DOnlineTranslator*             trengine;
    QString                        trCode;

    MetaEngine::AltLangMap         values;
};

} // namespace Digikam

#endif // DIGIKAM_ALT_LANG_STR_EDIT_P_H
