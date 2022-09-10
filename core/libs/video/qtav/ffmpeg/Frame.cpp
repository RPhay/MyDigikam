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

#include "Frame_p.h"

// C++ includes

#include "algorithm"

// Local includes

#include "digikam_debug.h"

namespace QtAV
{

Frame::Frame(const Frame& other)
    : d_ptr(other.d_ptr)
{
}

Frame::Frame(FramePrivate* const d)
    : d_ptr(d)
{
}

Frame::~Frame()
{
}

Frame& Frame::operator =(const Frame& other)
{
    d_ptr = other.d_ptr;

    return *this;
}

int Frame::bytesPerLine(int plane) const
{
    if ((plane < 0) || (plane >= planeCount()))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Invalid plane! Valid range is [0, %d)",
                planeCount());

        return 0;
    }

    return d_func()->line_sizes[plane];
}

QByteArray Frame::frameData() const
{
    return d_func()->data;
}

int Frame::dataAlignment() const
{
    return d_func()->data_align;
}

QByteArray Frame::data(int plane) const
{
    if ((plane < 0) || (plane >= planeCount()))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Invalid plane! Valid range is [0, %d)",
                planeCount());

        return QByteArray();
    }

    return QByteArray((char*)d_func()->planes[plane], bytesPerLine(plane));
}

uchar* Frame::bits(int plane)
{
    if ((plane < 0) || (plane >= planeCount()))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Invalid plane! Valid range is [0, %d)",
                planeCount());

        return nullptr;
    }

    return d_func()->planes[plane];
}

const uchar* Frame::constBits(int plane) const
{
    if ((plane < 0) || (plane >= planeCount()))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Invalid plane! Valid range is [0, %d)",
                planeCount());

        return nullptr;
    }

    return d_func()->planes[plane];
}

void Frame::setBits(uchar* b, int plane)
{
    if ((plane < 0) || (plane >= planeCount()))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Invalid plane! Valid range is [0, %d)",
                planeCount());

        return;
    }

    Q_D(Frame);

    d->planes[plane] = b;
}

void Frame::setBits(const QVector<uchar*>& b)
{
    Q_D(Frame);

    const int nb_planes = planeCount();
    d->planes           = b;

    if (d->planes.size() > nb_planes)
    {
        d->planes.reserve(nb_planes);
        d->planes.resize(nb_planes);
    }
}

void Frame::setBits(quint8* slice[])
{
    for (int i = 0 ; i < planeCount() ; ++i)
    {
        setBits(slice[i], i);
    }
}

void Frame::setBytesPerLine(int lineSize, int plane)
{
    if ((plane < 0) || (plane >= planeCount()))
    {
        qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
            << QString::asprintf("Invalid plane! Valid range is [0, %d)",
                planeCount());

        return;
    }

    Q_D(Frame);

    d->line_sizes[plane] = lineSize;
}

void Frame::setBytesPerLine(const QVector<int>& lineSize)
{
    Q_D(Frame);

    const int nb_planes = planeCount();
    d->line_sizes       = lineSize;

    if (d->line_sizes.size() > nb_planes)
    {
        d->line_sizes.reserve(nb_planes);
        d->line_sizes.resize(nb_planes);
    }
}

void Frame::setBytesPerLine(int stride[])
{
    for (int i = 0 ; i < planeCount() ; ++i)
    {
        setBytesPerLine(stride[i], i);
    }
}

int Frame::planeCount() const
{
    Q_D(const Frame);

    return d->planes.size();
}

int Frame::channelCount() const
{
    return planeCount();
}

/*!
 * Returns any extra metadata associated with this frame.
 */
QVariantMap Frame::availableMetaData() const
{
    Q_D(const Frame);

    return d->metadata;
}

/*!
 * Returns any metadata for this frame for the given \a key.
 *
 * This might include frame specific information from
 * a camera, or subtitles from a decoded video stream.
 *
 * See the documentation for the relevant video frame
 * producer for further information about available metadata.
 */
QVariant Frame::metaData(const QString& key) const
{
    Q_D(const Frame);

    return d->metadata.value(key);
}

/*!
 * Sets the metadata for the given \a key to \a value.
 *
 * If \a value is a null variant, any metadata for this key will be removed->
 *
 * The producer of the video frame might use this to associate
 * certain data with this frame, or for an intermediate processor
 * to add information for a consumer of this frame.
 */
void Frame::setMetaData(const QString& key, const QVariant& value)
{
    Q_D(Frame);

    if (!value.isNull())
        d->metadata.insert(key, value);
    else
        d->metadata.remove(key);
}

qreal Frame::timestamp() const
{
    return d_func()->timestamp;
}

void Frame::setTimestamp(qreal ts)
{
    d_func()->timestamp = ts;
}

void Frame::swap(Frame& other)
{
    std::swap(d_ptr, other.d_ptr);
}

} // namespace QtAV
