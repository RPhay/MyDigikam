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

#ifndef QTAV_FRAME_H
#define QTAV_FRAME_H

// Qt includes

#include <QVariant>
#include <QSharedData>

// Local includes

#include "QtAV_Global.h"

// TODO: fromAVFrame() asAVFrame()?

namespace QtAV
{

class FramePrivate;

class DIGIKAM_EXPORT Frame                 // clazy:exclude=copyable-polymorphic
{
    Q_DECLARE_PRIVATE(Frame)

public:

    Frame(const Frame& other);
    virtual ~Frame() = 0;

    Frame& operator =(const Frame& other);

    /*!
     * \brief planeCount
     *  a decoded frame can be packed and planar. packed format has only 1 plane, while planar
     *  format has more than 1 plane. For audio, the number plane equals channel count. For
     *  video, rgb is 1 plane, yuv420p is 3 plane, p means planar
     * \param plane default is the first plane
     * \return
     */
    int planeCount() const;

    /*!
     * \brief channelCount
     * for audio, channel count equals plane count
     * for video, channels >= planes
     * \return
     */
    virtual int channelCount() const;

    /*!
     * \brief bytesPerLine
     *   For video, it's size of each picture line. For audio, it's the whole size of plane
     * \param plane
     * \return line size of plane
     */
    int bytesPerLine(int plane = 0) const;

    // the whole frame data. may be empty unless clone() or allocate is called
    // real data starts with dataAlignment() aligned address

    QByteArray frameData() const;
    int dataAlignment() const;

    uchar* frameDataPtr(int* size = nullptr) const
    {
        const int a      = dataAlignment();
        uchar* const p   = (uchar*)frameData().constData();
        const int offset = (a - ((quintptr)p & (a - 1))) & (a - 1);

        if (size)
            *size = frameData().size() - offset;

        return (p + offset);
    }

    // deep copy 1 plane data

    QByteArray data(int plane = 0) const;
    uchar* bits(int plane = 0);

    const uchar* bits(int plane = 0) const
    {
        return constBits(plane);
    }

    const uchar* constBits(int plane = 0) const;

    /*!
     * \brief setBits
     * does nothing if plane is invalid. if given array size is greater than planeCount(),
     * only planeCount() elements is used
     * \param b slice
     * \param plane color/audio channel
     */

    // TODO: const?

    void setBits(uchar* b, int plane = 0);
    void setBits(const QVector<uchar*>& b);
    void setBits(quint8* slice[]);

    /*!
     * \brief setBytesPerLine
     * does nothing if plane is invalid. if given array size is greater than planeCount(),
     * only planeCount() elements is used
     */
    void setBytesPerLine(int lineSize, int plane = 0);
    void setBytesPerLine(const QVector<int>& lineSize);
    void setBytesPerLine(int stride[]);

    QVariantMap availableMetaData() const;
    QVariant metaData(const QString& key) const;
    void setMetaData(const QString& key, const QVariant& value);
    void setTimestamp(qreal ts);
    qreal timestamp() const;

    void swap(Frame& other);

protected:

    Frame(FramePrivate* const d);

protected:

    QExplicitlySharedDataPointer<FramePrivate> d_ptr;
};

} // namespace QtAV

#endif // QTAV_FRAME_H
