/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * Copyright (C)      2022 by Gilles Caulier, <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef QTAV_PLAYER_COMMON_H
#define QTAV_PLAYER_COMMON_H

// Qt includes

#include <QObject>
#include <QStringList>
#include <QUrl>

// Local includes

#include "qoptions.h"
#include "Config.h"
#include "ScreenSaver.h"

namespace QtAVPlayer
{

QOptions get_common_options();

void do_common_options_before_qapp(const QOptions& options);

/// help, log file, ffmpeg log level

void do_common_options(const QOptions& options, const QString& appName = QString());

// if appname ends with 'desktop', 'es', 'angle', software', 'sw', set by appname. otherwise set by command line option glopt, or Config file

// TODO: move to do_common_options_before_qapp

void set_opengl_backend(const QString& glopt = QString::fromLatin1("auto"), const QString& appname = QString());

QString appDataDir();

class AppEventFilter : public QObject
{
public:

    AppEventFilter(QObject* player = nullptr, QObject* parent = nullptr);

    QUrl url() const;
    virtual bool eventFilter(QObject* obj, QEvent* ev);

private:

    QObject* m_player;
};

} // namespace QtAVPlayer

#endif // QTAV_PLAYER_COMMON_H
