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

#ifndef QTAV_PACKET_H
#define QTAV_PACKET_H

// Qt includes

#include <QSharedData>

// Local includes

#include "QtAV_Global.h"

struct AVPacket;

namespace QtAV
{

class PacketPrivate;

class DIGIKAM_EXPORT Packet
{
public:

    static Packet fromAVPacket(const AVPacket* avpkt, double time_base);
    static bool fromAVPacket(Packet* pkt, const AVPacket* avpkt, double time_base);
    static Packet createEOF();

    Packet();
    ~Packet();

    // required if no defination of PacketPrivate

    Packet(const Packet& other);
    Packet& operator =(const Packet& other);

    bool isEOF() const;
    inline bool isValid() const;

    /*!
     * \brief asAVPacket
     * If Packet is constructed from AVPacket, then data and properties are the same as that AVPacket.
     * Otherwise, Packet's data and properties are used and no side data.
     * Packet takes the owner ship. time unit is always ms even constructed from AVPacket.
     */
    const AVPacket* asAVPacket() const;

    /*!
     * \brief skip
     * Skip bytes of packet data. User has to update pts, dts etc to new values.
     * Useful for asAVPakcet(). When asAVPakcet() is called, AVPacket->pts/dts will be updated to new values.
     */
    void skip(int bytes);

public:

    bool        hasKeyFrame;
    bool        isCorrupt;
    QByteArray  data;

    // time unit is s.

    qreal       pts, duration;
    qreal       dts;
    qint64      position; ///< position in source file byte stream

private:

    // we must define  default/copy ctor, dtor and operator= so that we can provide
    // only forward declaration of PacketPrivate

    mutable QSharedDataPointer<PacketPrivate> d;
};

bool Packet::isValid() const
{
    return (!isCorrupt && !data.isEmpty() && (pts >= 0) && (duration >= 0)); // !data.isEmpty() ?
}

#ifndef QT_NO_DEBUG_STREAM

DIGIKAM_EXPORT QDebug operator<<(QDebug debug, const Packet &pkt);

#endif

} // namespace QtAV

Q_DECLARE_METATYPE(QtAV::Packet)

#endif // QTAV_PACKET_H
