/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-12-28
 * Description : a tool to export items to Facebook web service
 *
 * SPDX-FileCopyrightText: 2005-2008 by Vardhman Jain <vardhman at gmail dot com>
 * SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2008-2009 by Luka Renko <lure at kubuntu dot org>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FB_MPFORM_H
#define DIGIKAM_FB_MPFORM_H

// Qt includes

#include <QByteArray>
#include <QString>

namespace DigikamGenericFaceBookPlugin
{

class FbMPForm
{

public:

    explicit FbMPForm();
    ~FbMPForm();

    void finish();
    void reset();

    void addPair(const QString& name, const QString& value);
    bool addFile(const QString& name, const QString& path);

    QString    contentType() const;
    QByteArray formData()    const;
    QString    boundary()    const;

private:

    QByteArray m_buffer;
    QByteArray m_boundary;
};

} // namespace DigikamGenericFaceBookPlugin

#endif // DIGIKAM_FB_MPFORM_H
