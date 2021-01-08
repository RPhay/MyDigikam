/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-10-12
 * Description : IPTC status settings page.
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

#include "iptcstatus.h"

// Qt includes

#include <QCheckBox>
#include <QLabel>
#include <QGridLayout>
#include <QApplication>
#include <QStyle>
#include <QLineEdit>
#include <QToolTip>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "dmetadata.h"
#include "limitedtextedit.h"
#include "dlayoutbox.h"

using namespace Digikam;

namespace DigikamGenericMetadataEditPlugin
{

class Q_DECL_HIDDEN IPTCStatus::Private
{
public:

    explicit Private()
    {
        statusEdit              = nullptr;
        jobIDEdit               = nullptr;
        statusCheck             = nullptr;
        jobIDCheck              = nullptr;
        specialInstructionNote  = nullptr;
        specialInstructionEdit  = nullptr;
        specialInstructionCheck = nullptr;
        objectNameEdit          = nullptr;
        objectNameCheck         = nullptr;
    }

    QCheckBox*       statusCheck;
    QCheckBox*       jobIDCheck;
    QCheckBox*       specialInstructionCheck;
    QCheckBox*       objectNameCheck;

    QLineEdit*       objectNameEdit;
    QLineEdit*       statusEdit;
    QLineEdit*       jobIDEdit;

    QLabel*          specialInstructionNote;
    LimitedTextEdit* specialInstructionEdit;
};

IPTCStatus::IPTCStatus(QWidget* const parent)
    : QWidget(parent),
      d(new Private)
{
    QGridLayout* const grid = new QGridLayout(this);

    // --------------------------------------------------------

    d->objectNameCheck = new QCheckBox(i18nc("image title", "Title:"), this);
    d->objectNameEdit  = new QLineEdit(this);
    d->objectNameEdit->setClearButtonEnabled(true);
    d->objectNameEdit->setMaxLength(64);
    d->objectNameEdit->setWhatsThis(i18n("Set here the shorthand reference of content. "
                                         "This field is limited to 64 characters."));

    // --------------------------------------------------------

    d->statusCheck = new QCheckBox(i18n("Edit Status:"), this);
    d->statusEdit  = new QLineEdit(this);
    d->statusEdit->setClearButtonEnabled(true);
    d->statusEdit->setMaxLength(64);
    d->statusEdit->setWhatsThis(i18n("Set here the title of content status. This field is limited "
                                     "to 64 characters."));

    // --------------------------------------------------------

    d->jobIDCheck = new QCheckBox(i18n("Job Identifier:"), this);
    d->jobIDEdit  = new QLineEdit(this);
    d->jobIDEdit->setClearButtonEnabled(true);
    d->jobIDEdit->setMaxLength(32);
    d->jobIDEdit->setWhatsThis(i18n("Set here the string that identifies content that recurs. "
                                    "This field is limited to 32 characters."));

    // --------------------------------------------------------

    DHBox* const instHeader    = new DHBox(this);
    d->specialInstructionCheck = new QCheckBox(i18n("Special Instructions:"), instHeader);
    d->specialInstructionNote  = new QLabel(instHeader);
    instHeader->setStretchFactor(d->specialInstructionCheck, 10);

    d->specialInstructionEdit  = new LimitedTextEdit(this);
    d->specialInstructionEdit->setMaxLength(256);
    d->specialInstructionEdit->setWhatsThis(i18n("Enter the editorial usage instructions. "
                                                 "This field is limited to 256 characters."));

    // --------------------------------------------------------

    QLabel* const note = new QLabel(i18n("<b>Note: "
                 "<a href='https://en.wikipedia.org/wiki/IPTC_Information_Interchange_Model'>IPTC</a> "
                 "text tags are limited string sizes. Use contextual help for details. "
                 "Consider to use <a href='https://en.wikipedia.org/wiki/Extensible_Metadata_Platform'>XMP</a> instead.</b>"),
                 this);
    note->setOpenExternalLinks(true);
    note->setWordWrap(true);
    note->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);

    // --------------------------------------------------------

    grid->addWidget(d->objectNameCheck,         0, 0, 1, 1);
    grid->addWidget(d->objectNameEdit,          0, 1, 1, 2);
    grid->addWidget(d->statusCheck,             1, 0, 1, 1);
    grid->addWidget(d->statusEdit,              1, 1, 1, 2);
    grid->addWidget(d->jobIDCheck,              2, 0, 1, 1);
    grid->addWidget(d->jobIDEdit,               2, 1, 1, 2);
    grid->addWidget(instHeader,                 3, 0, 1, 3);
    grid->addWidget(d->specialInstructionEdit,  4, 0, 1, 3);
    grid->addWidget(note,                       9, 0, 1, 3);
    grid->setColumnStretch(2, 10);
    grid->setRowStretch(10, 10);
    grid->setContentsMargins(QMargins());
    grid->setSpacing(QApplication::style()->pixelMetric(QStyle::PM_DefaultLayoutSpacing));

    // --------------------------------------------------------

    connect(d->objectNameCheck, SIGNAL(toggled(bool)),
            d->objectNameEdit, SLOT(setEnabled(bool)));

    connect(d->statusCheck, SIGNAL(toggled(bool)),
            d->statusEdit, SLOT(setEnabled(bool)));

    connect(d->jobIDCheck, SIGNAL(toggled(bool)),
            d->jobIDEdit, SLOT(setEnabled(bool)));

    connect(d->specialInstructionCheck, SIGNAL(toggled(bool)),
            d->specialInstructionEdit, SLOT(setEnabled(bool)));

    // --------------------------------------------------------

    connect(d->objectNameCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->statusCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->jobIDCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    connect(d->specialInstructionCheck, SIGNAL(toggled(bool)),
            this, SIGNAL(signalModified()));

    // --------------------------------------------------------

    connect(d->objectNameEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(signalModified()));

    connect(d->objectNameEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotLineEditModified()));

    connect(d->statusEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(signalModified()));

    connect(d->statusEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotLineEditModified()));

    connect(d->jobIDEdit, SIGNAL(textChanged(QString)),
            this, SIGNAL(signalModified()));

    connect(d->jobIDEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotLineEditModified()));

    connect(d->specialInstructionEdit, SIGNAL(textChanged()),
            this, SIGNAL(signalModified()));

    connect(d->specialInstructionEdit, SIGNAL(textChanged()),
            this, SLOT(slotSpecialInstructionLeftCharacters()));
}

IPTCStatus::~IPTCStatus()
{
    delete d;
}

void IPTCStatus::slotSpecialInstructionLeftCharacters()
{
    QToolTip::showText(d->specialInstructionCheck->mapToGlobal(QPoint(0, -16)),
                       i18np("%1 character left", "%1 characters left", d->specialInstructionEdit->maxLength() - d->specialInstructionEdit->toPlainText().size()),
                       d->specialInstructionEdit);
}

void IPTCStatus::slotLineEditModified()
{
    QLineEdit* const ledit = dynamic_cast<QLineEdit*>(sender());

    if (!ledit)
    {
        return;
    }

    QToolTip::showText(ledit->mapToGlobal(QPoint(0, (-1)*(ledit->height() + 16))),
                       i18np("%1 character left", "%1 characters left", ledit->maxLength() - ledit->text().size()),
                       ledit);
}

void IPTCStatus::readMetadata(QByteArray& iptcData)
{
    blockSignals(true);
    QScopedPointer<DMetadata> meta(new DMetadata);
    meta->setIptc(iptcData);

    QString     data;
    QStringList list;

    d->objectNameEdit->clear();
    d->objectNameCheck->setChecked(false);
    data = meta->getIptcTagString("Iptc.Application2.ObjectName", false);

    if (!data.isNull())
    {
        d->objectNameEdit->setText(data);
        d->objectNameCheck->setChecked(true);
    }

    d->objectNameEdit->setEnabled(d->objectNameCheck->isChecked());

    d->statusEdit->clear();
    d->statusCheck->setChecked(false);
    data = meta->getIptcTagString("Iptc.Application2.EditStatus", false);

    if (!data.isNull())
    {
        d->statusEdit->setText(data);
        d->statusCheck->setChecked(true);
    }

    d->statusEdit->setEnabled(d->statusCheck->isChecked());

    d->jobIDEdit->clear();
    d->jobIDCheck->setChecked(false);
    data = meta->getIptcTagString("Iptc.Application2.FixtureId", false);

    if (!data.isNull())
    {
        d->jobIDEdit->setText(data);
        d->jobIDCheck->setChecked(true);
    }

    d->jobIDEdit->setEnabled(d->jobIDCheck->isChecked());

    d->specialInstructionEdit->clear();
    d->specialInstructionCheck->setChecked(false);
    data = meta->getIptcTagString("Iptc.Application2.SpecialInstructions", false);

    if (!data.isNull())
    {
        d->specialInstructionEdit->setPlainText(data);
        d->specialInstructionCheck->setChecked(true);
    }

    d->specialInstructionEdit->setEnabled(d->specialInstructionCheck->isChecked());
    slotSpecialInstructionLeftCharacters();

    blockSignals(false);
}

void IPTCStatus::applyMetadata(QByteArray& iptcData)
{
    QScopedPointer<DMetadata> meta(new DMetadata);
    meta->setIptc(iptcData);

    if (d->objectNameCheck->isChecked())
        meta->setIptcTagString("Iptc.Application2.ObjectName", d->objectNameEdit->text());
    else
        meta->removeIptcTag("Iptc.Application2.ObjectName");

    if (d->statusCheck->isChecked())
        meta->setIptcTagString("Iptc.Application2.EditStatus", d->statusEdit->text());
    else
        meta->removeIptcTag("Iptc.Application2.EditStatus");

    if (d->jobIDCheck->isChecked())
        meta->setIptcTagString("Iptc.Application2.FixtureId", d->jobIDEdit->text());
    else
        meta->removeIptcTag("Iptc.Application2.FixtureId");

    if (d->specialInstructionCheck->isChecked())
        meta->setIptcTagString("Iptc.Application2.SpecialInstructions", d->specialInstructionEdit->toPlainText());
    else
        meta->removeIptcTag("Iptc.Application2.SpecialInstructions");

    iptcData = meta->getIptc();
}

} // namespace DigikamGenericMetadataEditPlugin
