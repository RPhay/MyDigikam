/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-08-08
 * Description : an option to add a sequence number to the parser
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

#ifndef DIGIKAM_SEQUENCE_NUMBER_OPTION_H
#define DIGIKAM_SEQUENCE_NUMBER_OPTION_H

// Local includes

#include "option.h"
#include "ruledialog.h"

namespace Ui
{
    class SequenceNumberOptionDialogWidget;
}

namespace Digikam
{

class SequenceNumberDialog : public RuleDialog
{
    Q_OBJECT

public:

    explicit SequenceNumberDialog(Rule* const parent);
    ~SequenceNumberDialog() override;

    Ui::SequenceNumberOptionDialogWidget* const ui;

private:

    // Disable
    explicit SequenceNumberDialog(QWidget*)                      = delete;
    SequenceNumberDialog(const SequenceNumberDialog&)            = delete;
    SequenceNumberDialog& operator=(const SequenceNumberDialog&) = delete;
};

// --------------------------------------------------------

class SequenceNumberOption : public Option
{
    Q_OBJECT

public:

    explicit SequenceNumberOption();
    ~SequenceNumberOption()                             override;

protected:

    QString parseOperation(ParseSettings& settings)     override;

private Q_SLOTS:

    void slotTokenTriggered(const QString& token)       override;

private:

    // Disable
    explicit SequenceNumberOption(QObject*)                      = delete;
    SequenceNumberOption(const SequenceNumberOption&)            = delete;
    SequenceNumberOption& operator=(const SequenceNumberOption&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_SEQUENCE_NUMBER_OPTION_H
