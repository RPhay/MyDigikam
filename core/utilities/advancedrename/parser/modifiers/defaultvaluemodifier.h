/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-11
 * Description : a modifier for setting a default value if option parsing failed
 *
 * SPDX-FileCopyrightText: 2009-2012 by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DEFAULT_VALUE_MODIFIER_H
#define DIGIKAM_DEFAULT_VALUE_MODIFIER_H

// Local includes

#include "modifier.h"
#include "ruledialog.h"

class QLineEdit;

namespace Digikam
{

class DefaultValueDialog : public RuleDialog
{
    Q_OBJECT

public:

    explicit DefaultValueDialog(Rule* parent);
    ~DefaultValueDialog()                                                                 override;

    QLineEdit* valueInput;

private:

    // Disable
    explicit DefaultValueDialog(QWidget*)                        = delete;
    DefaultValueDialog(const DefaultValueDialog&);
    DefaultValueDialog& operator=(const DefaultValueDialog&);
};

// --------------------------------------------------------

class DefaultValueModifier : public Modifier
{
    Q_OBJECT

public:

    explicit DefaultValueModifier();
    QString parseOperation(ParseSettings& settings, const QRegularExpressionMatch& match) override;

private Q_SLOTS:

    void slotTokenTriggered(const QString& token)                                         override;

private:

    // Disable
    explicit DefaultValueModifier(QObject*)                      = delete;
    DefaultValueModifier(const DefaultValueModifier&)            = delete;
    DefaultValueModifier& operator=(const DefaultValueModifier&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_DEFAULT_VALUE_MODIFIER_H
