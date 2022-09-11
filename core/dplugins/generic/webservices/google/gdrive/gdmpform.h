/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-18
 * Description : a tool to export items to Google web services
 *
 * SPDX-FileCopyrightText: 2013      by Pankaj Kumar <me at panks dot me>
 * SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_GD_MPFORM_H
#define DIGIKAM_GD_MPFORM_H

// Qt includes

#include <QByteArray>
#include <QString>

namespace DigikamGenericGoogleServicesPlugin
{

class GDMPForm
{
public:

    explicit GDMPForm();
    ~GDMPForm();

    void finish();
    void reset();

    void addPair(const QString& name,
                 const QString& description,
                 const QString& mimetype,
                 const QString& id);

    bool addFile(const QString& path);

    QString contentType() const;
    QByteArray formData() const;
    QString boundary()    const;
    QString getFileSize() const;

private:

    QByteArray m_buffer;
    QByteArray m_boundary;
    QString    m_file_size;
};

} // namespace DigikamGenericGoogleServicesPlugin

#endif // DIGIKAM_GD_MPFORM_H
