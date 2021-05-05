/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-08
 * Description : an option to provide metadata information to the parser
 *
 * Copyright (C) 2009-2012 by Andi Clemens <andi dot clemens at gmail dot com>
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

#ifndef DIGIKAM_METADATA_OPTION_H
#define DIGIKAM_METADATA_OPTION_H

// Qt includes

#include <QString>

// Local includes

#include "option.h"
#include "ruledialog.h"

class QLineEdit;

namespace Digikam
{

class MetadataPanel;

class MetadataOptionDialog : public RuleDialog
{
    Q_OBJECT

public:

    explicit MetadataOptionDialog(Rule* const parent);
    ~MetadataOptionDialog() override;

public:

    MetadataPanel* metadataPanel;
    QLineEdit*     separatorLineEdit;

private:

    // Disable
    explicit MetadataOptionDialog(QWidget*)                      = delete;
    MetadataOptionDialog(const MetadataOptionDialog&)            = delete;
    MetadataOptionDialog& operator=(const MetadataOptionDialog&) = delete;
};

// --------------------------------------------------------

class MetadataOption : public Option
{
    Q_OBJECT

public:

    explicit MetadataOption();
    ~MetadataOption()                               override = default;

protected:

    QString parseOperation(ParseSettings& settings) override;

private Q_SLOTS:

    void slotTokenTriggered(const QString& token)   override;

private:

    QString parseMetadata(const QString& token, ParseSettings& settings);

private:

    // Disable
    explicit MetadataOption(QObject*)                = delete;
    MetadataOption(const MetadataOption&)            = delete;
    MetadataOption& operator=(const MetadataOption&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_METADATA_OPTION_H
