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

#ifndef AV_PLAYER_AVFILTER_SUBTITLE_H
#define AV_PLAYER_AVFILTER_SUBTITLE_H

// Qt includes

#include <QObject>
#include <QHash>

// Local includes

#include "LibAVFilter.h"
#include "AVPlayerCore.h"

using namespace QtAV;

namespace AVPlayer
{

class AVFilterSubtitle : public LibAVFilterVideo
{
    Q_OBJECT
    Q_PROPERTY(bool autoLoad READ autoLoad WRITE setAutoLoad NOTIFY autoLoadChanged)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)

public:

    explicit AVFilterSubtitle(QObject* const parent = nullptr);

    void setPlayer(QtAV::AVPlayerCore* const player);
    QString setContent(const QString& doc);         ///< return utf8 subtitle path
    bool setFile(const QString& filePath);
    QString file() const;
    bool autoLoad() const;

Q_SIGNALS:

    void loaded();
    void loadError();
    void fileChanged(const QString& path);
    void autoLoadChanged(bool value);

public Q_SLOTS:

    // TODO: enable changed & autoload => load

    void setAutoLoad(bool value);
    void findAndSetFile(const QString& path);
    void onPlayerStart();

private Q_SLOTS:

    void onStatusChanged();

private:

    bool                    m_auto;
    QtAV::AVPlayerCore*     m_player;
    QString                 m_file;

    // convert to utf8 to ensure ffmpeg can open it.

    QHash<QString, QString> m_u8_files;
};

} // namespace AVPlayer

#endif // AV_PLAYER_AVFILTER_SUBTITLE_H
