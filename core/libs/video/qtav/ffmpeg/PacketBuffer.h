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

#ifndef QTAV_PACKET_BUFFER_H
#define QTAV_PACKET_BUFFER_H

// Qt includes

#include <QQueue>

// Local includes

#include "Packet.h"
#include "QtAV_BlockingQueue.h"
#include "QtAV_ring.h"

namespace QtAV
{

/*
 * take empty: start buffering, block at next take if still empty
 * take enough: start to put more packets
 * put enough: end buffering, end take block
 * put full: stop putting more packets
 */
class PacketBuffer : public BlockingQueue<Packet, QQueue>
{
public:

    PacketBuffer();
    ~PacketBuffer();

    void setBufferMode(BufferMode mode);
    BufferMode bufferMode()     const;

    /*!
     * \brief setBufferValue
     * Ensure the buffered msecs/bytes/packets in queue is at least the given value
     * \param value BufferBytes: bytes, BufferTime: msecs, BufferPackets: packets count
     */
    void setBufferValue(qint64 value);
    qint64 bufferValue()        const;

    /*!
     * \brief setBufferMax
     * stop buffering if max value reached. Real value is bufferValue()*bufferMax()
     * \param max the ratio to bufferValue(). always >= 1.0
     */
    void setBufferMax(qreal max);
    qreal bufferMax()           const;

    /*!
     * \brief buffered
     * Current buffered value in the queue
     */
    qint64 buffered()           const;
    bool isBuffering()          const;

    /*!
     * \brief bufferProgress
     * How much of the data buffer is currently filled, from 0.0 (empty) to 1.0 (enough or full).
     * bufferProgress() can be less than 1 if not isBuffering();
     * \return Percent of buffered time, bytes or packets.
     */
    qreal bufferProgress()      const;

    /*!
     * \brief bufferSpeed
     * Depending on BufferMode, the result is delta_pts/s, packets/s or bytes/s
     * \return
     */
    qreal bufferSpeed()         const;
    qreal bufferSpeedInBytes()  const;

protected:

    bool checkEnough()          const override;
    bool checkFull()            const override;
    void onTake(const Packet&)        override;
    void onPut(const Packet&)         override;

protected:

    typedef BlockingQueue<Packet, QQueue> PQ;

    using PQ::setCapacity;
    using PQ::setThreshold;
    using PQ::capacity;
    using PQ::threshold;

private:

    qreal calc_speed(bool use_bytes) const;

private:

    BufferMode          m_mode;
    bool                m_buffering;
    qreal               m_max;

    // bytes or count

    qint64              m_buffer;
    qint64              m_value0;
    qint64              m_value1;

    typedef struct BufferInfo_
    {
        qint64 v     = 0;       ///< pts, total packes or total bytes
        qint64 bytes = 0;       ///< total bytes
        qint64 t     = 0;
    } BufferInfo;

    ring<BufferInfo>    m_history;
};

} // namespace QtAV

#endif // QTAV_PACKET_BUFFER_H
