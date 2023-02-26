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

#ifndef AV_PLAYER_COMMON_H
#define AV_PLAYER_COMMON_H

// Qt includes

#include <QObject>
#include <QStringList>
#include <QUrl>

// Local includes

#include "qoptions.h"

namespace AVPlayer
{

QOptions get_common_options();

void do_common_options_before_qapp(const QOptions& options);

/// help, log file, ffmpeg log level

void do_common_options(const QOptions& options,
                       const QString& appName = QString());

// if appname ends with 'desktop', 'es', 'angle', software', 'sw', set by appname.
// otherwise set by command line option glopt, or Config file

// TODO: move to do_common_options_before_qapp

void set_opengl_backend(const QString& glopt = QString::fromLatin1("auto"),
                        const QString& appname = QString());

// -----------------------------------------------------------------------------------

class AppEventFilter : public QObject
{
    Q_OBJECT

public:

    explicit AppEventFilter(QObject* const player = nullptr,
                            QObject* const parent = nullptr);

    QUrl url() const;
    virtual bool eventFilter(QObject* obj, QEvent* ev);

private:

    QObject* m_player;
};

} // namespace AVPlayer

#endif // AV_PLAYER_COMMON_H
