/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-09-13
 * Description : a widget to provide conflict rules to save image.
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "filesaveconflictbox.h"

// Qt includes

#include <QButtonGroup>
#include <QGridLayout>
#include <QLabel>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QApplication>
#include <QStyle>

// KDE includes

#include <klocalizedstring.h>
#include <kconfiggroup.h>

namespace Digikam
{

class Q_DECL_HIDDEN FileSaveConflictBox::Private
{
public:

    explicit Private()
      : conflictLabel       (nullptr),
        conflictButtonGroup (nullptr),
        storeDiffButton     (nullptr),
        overwriteButton     (nullptr),
        skipFileButton      (nullptr)
    {
    }

    QLabel*       conflictLabel;

    QButtonGroup* conflictButtonGroup;

    QRadioButton* storeDiffButton;
    QRadioButton* overwriteButton;
    QRadioButton* skipFileButton;
};

FileSaveConflictBox::FileSaveConflictBox(QWidget* const parent, bool addSkip)
    : QWidget(parent),
      d      (new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);

    const int spacing          = QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing);
    QGridLayout* const grid    = new QGridLayout(this);
    d->conflictLabel           = new QLabel(i18n("If Target File Exists:"), this);
    QWidget* const conflictBox = new QWidget(this);
    QVBoxLayout* const vlay    = new QVBoxLayout(conflictBox);
    d->conflictButtonGroup     = new QButtonGroup(conflictBox);
    d->storeDiffButton         = new QRadioButton(i18n("Store as a different name"), conflictBox);
    d->overwriteButton         = new QRadioButton(i18n("Overwrite automatically"),   conflictBox);
    d->skipFileButton          = new QRadioButton(i18n("Skip automatically"),        conflictBox);

    d->conflictButtonGroup->addButton(d->overwriteButton, OVERWRITE);
    d->conflictButtonGroup->addButton(d->storeDiffButton, DIFFNAME);
    d->conflictButtonGroup->addButton(d->skipFileButton, SKIPFILE);

    d->conflictButtonGroup->setExclusive(true);
    d->storeDiffButton->setChecked(true);

    if (!addSkip)
    {
        d->skipFileButton->hide();
    }

    vlay->setContentsMargins(spacing, spacing, spacing, spacing);
    vlay->setSpacing(spacing);
    vlay->addWidget(d->storeDiffButton);
    vlay->addWidget(d->overwriteButton);
    vlay->addWidget(d->skipFileButton);

    grid->addWidget(d->conflictLabel, 1, 0, 1, 2);
    grid->addWidget(conflictBox,      2, 0, 1, 2);
    grid->setRowStretch(3, 10);
    grid->setContentsMargins(spacing, spacing, spacing, spacing);
    grid->setSpacing(spacing);

    connect(d->conflictButtonGroup, static_cast<void (QButtonGroup::*)(int)>(&QButtonGroup::buttonClicked),
            this, &FileSaveConflictBox::signalConflictButtonChanged);
}

FileSaveConflictBox::~FileSaveConflictBox()
{
    delete d;
}

void FileSaveConflictBox::resetToDefault()
{
    setConflictRule(OVERWRITE);
}

FileSaveConflictBox::ConflictRule FileSaveConflictBox::conflictRule() const
{
    return ((ConflictRule)(d->conflictButtonGroup->checkedId()));
}

void FileSaveConflictBox::setConflictRule(ConflictRule r)
{
    QAbstractButton* const bt = d->conflictButtonGroup->button((int)r);

    if (bt)
    {
        bt->setChecked(true);
    }
}

void FileSaveConflictBox::readSettings(KConfigGroup& group)
{
    setConflictRule((FileSaveConflictBox::ConflictRule)group.readEntry("Conflict",
        (int)(FileSaveConflictBox::DIFFNAME)));
}

void FileSaveConflictBox::writeSettings(KConfigGroup& group)
{
    group.writeEntry("Conflict", (int)conflictRule());
}

} // namespace Digikam
