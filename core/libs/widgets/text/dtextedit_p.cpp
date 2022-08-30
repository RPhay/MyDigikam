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

#include "dtextedit_p.h"

namespace Digikam
{

DTextEditClearButton::DTextEditClearButton(QWidget* const parent)
    : QLabel(parent)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    setFocusPolicy(Qt::NoFocus);
    setContentsMargins(QMargins());
    setScaledContents(false);
    setMouseTracking(false);
    QIcon ico = qApp->style()->standardIcon(QStyle::SP_LineEditClearButton);
    int s     = qApp->style()->pixelMetric(QStyle::PM_SliderLength) -
                qApp->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    setPixmap(ico.pixmap(QSize(s, s)));
}

void DTextEditClearButton::mousePressEvent(QMouseEvent* e)
{
    Q_EMIT clicked();
    QLabel::mousePressEvent(e);
}

// -------------------------------------------------------------------------------

DTextEdit::Private::Private()
{
}

void DTextEdit::Private::init(DTextEdit* const parent)
{

#ifdef HAVE_SONNET

    spellChecker                   = new SpellCheckDecorator(parent);

    // Auto-detect language enabled by default.

    spellChecker->highlighter()->setAutoDetectLanguageDisabled(false);

    LocalizeSettings* const config = LocalizeSettings::instance();

    if (config)
    {
        parent->setLocalizeSettings(config->settings());

        QObject::connect(config, &LocalizeSettings::signalSettingsChanged,
                         parent, [=]()
            {
                parent->setLocalizeSettings(config->settings());
            }
        );
    }

#endif

    parent->setLinesVisible(lines);

    clrBtn = new DTextEditClearButton(parent);
    parent->setCornerWidget(clrBtn);

    connect(clrBtn, &DTextEditClearButton::clicked,
            parent, &QTextEdit::clear);

    // Mimic QLineEdit

    QObject::connect(parent, &QTextEdit::textChanged,
                     parent, [=]()
        {
            if (clearBtnEnable && (lines == 1))
            {
                clrBtn->setVisible(!parent->text().isEmpty());
            }
        }
    );

    QObject::connect(parent, &QTextEdit::textChanged,
                     parent, &DTextEdit::slotChanged);
}

// ------------------------------------------------------------------------------------------------

DPlainTextEdit::Private::Private()
{
}

void DPlainTextEdit::Private::init(DPlainTextEdit* const parent)
{

#ifdef HAVE_SONNET

    spellChecker                   = new SpellCheckDecorator(parent);

    // Auto-detect language enabled by default.

    spellChecker->highlighter()->setAutoDetectLanguageDisabled(false);

    LocalizeSettings* const config = LocalizeSettings::instance();

    if (config)
    {
        parent->setLocalizeSettings(config->settings());

        QObject::connect(config, &LocalizeSettings::signalSettingsChanged,
                         parent, [=]()
            {
                parent->setLocalizeSettings(config->settings());
            }
        );
    }

#endif

    parent->setLinesVisible(lines);

    clrBtn = new DTextEditClearButton(parent);
    parent->setCornerWidget(clrBtn);

    connect(clrBtn, &DTextEditClearButton::clicked,
            parent, &QPlainTextEdit::clear);

    // Mimic QLineEdit

    QObject::connect(parent, &QPlainTextEdit::textChanged,
                     parent, [=]()
        {
            if (clearBtnEnable && (lines == 1))
            {
                clrBtn->setVisible(!parent->text().isEmpty());
            }
        }
    );

    QObject::connect(parent, &QPlainTextEdit::textChanged,
                     parent, &DPlainTextEdit::slotChanged);
}

} // namespace Digikam
