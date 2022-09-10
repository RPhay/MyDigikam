/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2015-05-29
 * Description : a combobox with time zones.
 *
 * SPDX-FileCopyrightText: 2015      by Maik Qualmann <metzpinguin at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_TIMEZONE_COMBOBOX_H
#define DIGIKAM_TIMEZONE_COMBOBOX_H

// Qt includes

#include <QComboBox>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT TimeZoneComboBox : public QComboBox
{
    Q_OBJECT

public:

    explicit TimeZoneComboBox(QWidget* const parent);
    ~TimeZoneComboBox()            override;

    void setToUTC();
    void setTimeZone(const QString& timeStr);

    QString getTimeZone()    const;
    int     timeZoneOffset() const;
};

} // namespace Digikam

#endif // DIGIKAM_TIMEZONE_COMBOBOX_H
