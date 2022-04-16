/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * Copyright (C) 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
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

#ifndef QTAV_SHADERMANAGER_H
#define QTAV_SHADERMANAGER_H

#include <QtCore/QObject>

namespace QtAV
{
class VideoShader;
class VideoMaterial;
/*!
 * \brief The ShaderManager class
 * Cache VideoShader and shader programes for different video material type.
 * TODO: ShaderManager does not change for a given vo, so we can expose VideoRenderer.shaderManager() to set custom shader. It's better than VideoRenderer.opengl() because OpenGLVideo exposes too many apis that may confuse user.
 */
class ShaderManager : public QObject
{
    Q_OBJECT
public:
    ShaderManager(QObject *parent = 0);
    ~ShaderManager();
    VideoShader* prepareMaterial(VideoMaterial *material, qint32 materialType = -1);
//    void setCacheSize(int value);

private:
    class Private;
    Private* d;
};
} // namespace QtAV
#endif // QTAV_SHADERMANAGER_H
