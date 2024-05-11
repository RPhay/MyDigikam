/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-05-20
 * Description : a tool to export images to Onedrive web service
 *
 * SPDX-FileCopyrightText: 2018      by Tarek Talaat <tarektalaat93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_OD_MPFORM_H
#define DIGIKAM_OD_MPFORM_H

// Qt includes

#include <QByteArray>

namespace DigikamGenericOneDrivePlugin
{

class ODMPForm
{

public:

    ODMPForm()  = default;
    ~ODMPForm() = default;

    bool addFile(const QString& imgPath);
    QByteArray formData() const;

private:

    QByteArray m_buffer;
};

} // namespace DigikamGenericOneDrivePlugin

#endif // DIGIKAM_OD_MPFORM_H
