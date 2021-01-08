/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-10-15
 * Description : IPTC subjects settings page.
 *
 * Copyright (C) 2006-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2009      by Andi Clemens <andi dot clemens at googlemail dot com>
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

#include "iptcsubjects.h"

// Qt includes

#include <QToolTip>

// KDE includes

#include <klocalizedstring.h>

namespace DigikamGenericMetadataEditPlugin
{

IPTCSubjects::IPTCSubjects(QWidget* const parent)
    : SubjectWidget(parent, true)
{
    m_iprDefault = QLatin1String("IPTC");

    // --------------------------------------------------------

    m_iprEdit->setText(m_iprDefault);
    m_iprEdit->setWhatsThis(i18n("Enter here the Informative Provider Reference. "
                                 "I.P.R is a name registered with the IPTC/NAA, identifying the "
                                 "provider that provides an indicator of the content. "
                                 "The default value for the I.P.R is \"IPTC\" if a standard Reference "
                                 "Code is used. This field is limited to 32 characters."));

    // --------------------------------------------------------

    m_refEdit->setWhatsThis(i18n("Enter here the Subject Reference Number. "
                                 "Provides a numeric code to indicate the Subject Name plus "
                                 "optional Subject Matter and Subject Detail Names in the "
                                 "language of the service. Subject Reference is a number "
                                 "from the range 01000000 to 17999999 and represent a "
                                 "language independent international reference to "
                                 "a Subject. A Subject is identified by its Reference Number "
                                 "and corresponding Names taken from a standard lists given "
                                 "by IPTC/NAA. If a standard reference code is used, these lists "
                                 "are the English language reference versions. "
                                 "This field is limited to 8 digit code."));

    // --------------------------------------------------------

    m_nameEdit->setWhatsThis(i18n("Enter here the Subject Name. English language is used "
                                  "if you selected a standard IPTC/NAA reference code. "
                                  "This field is limited to 64 characters."));

    // --------------------------------------------------------

    m_matterEdit->setWhatsThis(i18n("Enter here the Subject Matter Name. English language is used "
                                    "if you selected a standard IPTC/NAA reference code. "
                                    "This field is limited to 64 characters."));

    // --------------------------------------------------------

    m_detailEdit->setWhatsThis(i18n("Enter here the Subject Detail Name. English language is used "
                                    "if you selected a standard IPTC/NAA reference code. "
                                    "This field is limited to 64 characters."));

    m_note->setText(i18n("<b>Note: "
                 "<a href='https://en.wikipedia.org/wiki/IPTC_Information_Interchange_Model'>IPTC</a> "
                 "text tags are limited string sizes. Use contextual help for details. "
                 "Consider to use <a href='https://en.wikipedia.org/wiki/Extensible_Metadata_Platform'>XMP</a> instead.</b>"));

    m_subjectsCheck->setVisible(true);
    m_subjectsCheck->setEnabled(true);

    connect(m_iprEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotLineEditModified()));

    connect(m_refEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotLineEditModified()));

    connect(m_nameEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotLineEditModified()));

    connect(m_matterEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotLineEditModified()));

    connect(m_detailEdit, SIGNAL(textChanged(QString)),
            this, SLOT(slotLineEditModified()));
}

IPTCSubjects::~IPTCSubjects()
{
}

void IPTCSubjects::readMetadata(QByteArray& iptcData)
{
    QScopedPointer<DMetadata> meta(new DMetadata);
    meta->setIptc(iptcData);
    setSubjectsList(meta->getIptcSubjects());
}

void IPTCSubjects::applyMetadata(QByteArray& iptcData)
{
    QScopedPointer<DMetadata> meta(new DMetadata);
    meta->setIptc(iptcData);
    QStringList newSubjects = subjectsList();

    if (m_subjectsCheck->isChecked())
    {
        meta->setIptcSubjects(meta->getIptcSubjects(), newSubjects);
    }
    else
    {
        meta->setIptcSubjects(meta->getIptcSubjects(), QStringList());
    }

    iptcData = meta->getIptc();
}

void IPTCSubjects::slotLineEditModified()
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

} // namespace DigikamGenericMetadataEditPlugin
