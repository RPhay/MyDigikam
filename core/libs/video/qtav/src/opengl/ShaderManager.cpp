/******************************************************************************
    QtAV:  Multimedia framework based on Qt and FFmpeg
    Copyright (C) 2012-2016 Wang Bin <wbsecg1@gmail.com>

*   This file is part of QtAV (from 2014)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
******************************************************************************/

#include "ShaderManager.h"
#include "QtAV/VideoShader.h"

namespace QtAV {
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
} //namespace QtAV
