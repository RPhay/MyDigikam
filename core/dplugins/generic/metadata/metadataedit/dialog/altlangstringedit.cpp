/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-10-18
 * Description : a widget to edit multiple alternative
 *               language string entries with an activation checkbox.
 *
 * Copyright (C) 2007-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013      by Victor Dodon <dodonvictor at gmail dot com>
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

#include "altlangstringedit.h"

// Qt includes

#include <QGridLayout>
#include <QApplication>

// Local includes

#include "squeezedcombobox.h"
#include "metadatacheckbox.h"

namespace DigikamGenericMetadataEditPlugin
{

class Q_DECL_HIDDEN AltLangStringsEdit::Private
{
public:

    explicit Private()
      : valueCheck(nullptr)
    {
    }

    DMetadata::AltLangMap oldValues;
    MetadataCheckBox*     valueCheck;
};

AltLangStringsEdit::AltLangStringsEdit(QWidget* const parent, const QString& title, const QString& desc)
    : AltLangStrEdit(parent),
      d      (new Private)
{
    d->valueCheck = new MetadataCheckBox(title, parent);
    setLinesVisible(4);
    setPlaceholderText(desc);
    setTitleWidget(d->valueCheck);

    // --------------------------------------------------------

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalToggled(bool)));

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            this, SLOT(slotEnabledInternalWidgets(bool)));

    connect(d->valueCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    slotEnabledInternalWidgets(d->valueCheck->isChecked());

    // --------------------------------------------------------

    connect(this, SIGNAL(signalModified(QString,QString)),
            this, SIGNAL(signalModified()));

    connect(this, SIGNAL(signalValueAdded(QString,QString)),
            this, SIGNAL(signalModified()));

    connect(this, SIGNAL(signalValueDeleted(QString)),
            this, SIGNAL(signalModified()));

    connect(this, SIGNAL(signalSelectionChanged(QString)),
            this, SLOT(slotSelectionChanged(QString)));
}

AltLangStringsEdit::~AltLangStringsEdit()
{
    delete d;
}

void AltLangStringsEdit::setValid(bool v)
{
    d->valueCheck->setValid(v);
    d->valueCheck->setChecked(v);
}

bool AltLangStringsEdit::isValid() const
{
    return d->valueCheck->isValid();
}

void AltLangStringsEdit::setValues(const DMetadata::AltLangMap& values)
{
    d->oldValues = values;
    AltLangStrEdit::setValues(values);
}

bool AltLangStringsEdit::getValues(DMetadata::AltLangMap& oldValues,
                                   DMetadata::AltLangMap& newValues) const
{
    oldValues = d->oldValues;
    newValues = values();

    return d->valueCheck->isChecked();
}

void AltLangStringsEdit::slotSelectionChanged(const QString& lang)
{
    Q_EMIT signalDefaultLanguageEnabled(lang == QLatin1String("x-default"));
}

} // namespace DigikamGenericMetadataEditPlugin
