/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-15
 * Description : multi-languages string editor
 *
 * SPDX-FileCopyrightText: 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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

#include <klazylocalizedstring.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "donlinetranslator.h"
#include "localizesettings.h"
#include "localizeselector.h"
#include "dexpanderbox.h"

namespace Digikam
{

typedef QMap<QString, KLazyLocalizedString> LanguageCodeMap;

extern const LanguageCodeMap s_rfc3066ForXMP;

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
        localizeSelector(nullptr),
        valueEdit       (nullptr),
        languageCB      (nullptr),
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
    LocalizeSelector*              localizeSelector;

    DTextEdit*                     valueEdit;

    QComboBox*                     languageCB;
    DOnlineTranslator*             trengine;
    QString                        trCode;

    MetaEngine::AltLangMap         values;
};

} // namespace Digikam

#endif // DIGIKAM_ALT_LANG_STR_EDIT_P_H
