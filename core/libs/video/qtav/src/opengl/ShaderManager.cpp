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

#include "ShaderManager.h"
#include "VideoShader.h"

namespace QtAV
{
class ShaderManager::Private
{
public:
    ~Private() {
        // TODO: thread safe required?
        qDeleteAll(shader_cache.values());
        shader_cache.clear();
    }

    QHash<qint32, VideoShader*> shader_cache;
};

ShaderManager::ShaderManager(QObject *parent) :
    QObject(parent)
  , d(new Private())
{
}

ShaderManager::~ShaderManager()
{
    delete d;
    d = 0;
}

VideoShader* ShaderManager::prepareMaterial(VideoMaterial *material, qint32 materialType)
{
    const qint32 type = materialType != -1 ? materialType : material->type();
    VideoShader *shader = d->shader_cache.value(type, 0);
    if (shader)
        return shader;
    qDebug() << QStringLiteral("[ShaderManager] cache a new shader material type(%1): %2").arg(type).arg(VideoMaterial::typeName(type));
    shader = material->createShader();
    shader->initialize();
    d->shader_cache[type] = shader;
    return shader;
}
} // namespace QtAV
