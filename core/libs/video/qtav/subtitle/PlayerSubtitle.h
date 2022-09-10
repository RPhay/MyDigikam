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

#ifndef QTAV_PLAYER_SUBTITLE_H
#define QTAV_PLAYER_SUBTITLE_H

// Qt includes

#include <QObject>
#include <QVariant>
#include <QVector>

// Local includes

#include "QtAV_Global.h"
#include "Packet.h"

namespace QtAV
{

class AVPlayerCore;
class Subtitle;

/*!
 * \brief The PlayerSubtitle class
 * Bind Subtitle to AVPlayerCore. Used by SubtitleFilter and QuickSubtitle.
 * Subtitle load priority: user specified file (setFile(...)) > auto load
 * external (autoLoad() must be true) > embedded subtitle
 */
class DIGIKAM_EXPORT PlayerSubtitle : public QObject
{
    Q_OBJECT

public:

    explicit PlayerSubtitle(QObject* const parent = nullptr);

    void setPlayer(AVPlayerCore* player);
    Subtitle* subtitle();

    /*!
     * \brief setFile
     * Load user selected subtitle. The subtitle will not change unless
     * you manually setFile(QString()).
     */
    void setFile(const QString& file);
    QString file() const;

    /*!
     * \brief autoLoad
     * Auto find and load a suitable external subtitle if file() is not empty.
     */
    void setAutoLoad(bool value);
    bool autoLoad() const;

Q_SIGNALS:

    void autoLoadChanged(bool value);
    void fileChanged();

public Q_SLOTS:

    void onEnabledChanged(bool value);

private Q_SLOTS:

    void onPlayerSourceChanged();
    void onPlayerPositionChanged();
    void onPlayerStart();
    void tryReload();
    void tryReloadInternalSub();
    void updateInternalSubtitleTracks(const QVariantList& tracks);
    void processInternalSubtitlePacket(int track, const QtAV::Packet& packet);
    void processInternalSubtitleHeader(const QByteArray& codec, const QByteArray& data); // TODO: remove

private:

    void connectSignals();
    void disconnectSignals();
    void tryReload(int flag);   ///< 1: internal, 2: external, 3: internal+external

private:

    bool            m_auto;
    bool            m_enabled;  // TODO: m_enable_external
    AVPlayerCore*   m_player = nullptr;
    Subtitle*       m_sub    = nullptr;
    QString         m_file;
    QVariantList    m_tracks;
    QVector<Packet> m_current_pkt;
};

} // namespace QtAV

#endif // QTAV_PLAYER_SUBTITLE_H
