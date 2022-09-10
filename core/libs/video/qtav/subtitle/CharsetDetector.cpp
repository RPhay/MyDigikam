/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "CharsetDetector.h"

// Libuchardet includes

#ifdef LINK_UCHARDET
#   include <uchardet/uchardet.h>
#   define HAVE_UCHARDET
#endif

#ifndef HAVE_UCHARDET

typedef struct uchardet* uchardet_t;

#endif

namespace QtAV
{

class Q_DECL_HIDDEN CharsetDetector::Private
{
public:

    Private()
        : m_det(nullptr)
    {

#ifdef HAVE_UCHARDET

        m_det = uchardet_new();

#endif

    }

    ~Private()
    {
        if (!m_det)
            return;

#ifdef HAVE_UCHARDET

        uchardet_delete(m_det);

#endif

        m_det = nullptr;
    }

    QByteArray detect(const QByteArray& data)
    {

#ifdef HAVE_UCHARDET

        if (!m_det)
            return QByteArray();

        if (uchardet_handle_data(m_det, data.constData(), data.size()) != 0)
            return QByteArray();

        uchardet_data_end(m_det);
        QByteArray cs(uchardet_get_charset(m_det));
        uchardet_reset(m_det);

        return cs.trimmed();

#else

        Q_UNUSED(data);

        return QByteArray();

#endif

    }

    uchardet_t m_det;
};

CharsetDetector::CharsetDetector()
    : d(new Private())
{
}

CharsetDetector::~CharsetDetector()
{
    if (d)
    {
        delete d;
        d = nullptr;
    }
}

bool CharsetDetector::isAvailable() const
{
    return !!d->m_det;
}

QByteArray CharsetDetector::detect(const QByteArray& data)
{
    return d->detect(data);
}

} // namespace QtAV
