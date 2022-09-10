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

#include "VideoShaderObject.h"

// Qt includes

#include <QEvent>
#include <QMetaProperty>
#include <QSignalMapper>

// Local includes

#include "VideoShader_p.h"
#include "digikam_debug.h"

namespace QtAV
{

class Q_DECL_HIDDEN VideoShaderObjectPrivate : public VideoShaderPrivate    // clazy:exclude=copyable-polymorphic
{
public:

    ~VideoShaderObjectPrivate()
    {
        qDeleteAll(sigMap[VertexShader]);
        qDeleteAll(sigMap[FragmentShader]);
        sigMap[VertexShader].clear();
        sigMap[FragmentShader].clear();
    }

    QVector<QSignalMapper*> sigMap[ShaderTypeCount];
};

VideoShaderObject::VideoShaderObject(QObject* const parent)
    : QObject    (parent),
      VideoShader(*new VideoShaderObjectPrivate())
{
}

VideoShaderObject::VideoShaderObject(VideoShaderObjectPrivate& d, QObject* const parent)
    : QObject    (parent),
      VideoShader(d)
{
}

bool VideoShaderObject::event(QEvent* event)
{
    DPTR_D(VideoShaderObject);

    if (event->type() != QEvent::DynamicPropertyChange)
        return QObject::event(event);

    QDynamicPropertyChangeEvent* const e = static_cast<QDynamicPropertyChangeEvent*>(event);

    for (int shaderType = VertexShader ; shaderType < ShaderTypeCount ; ++shaderType)
    {
        QVector<Uniform> &uniforms = d.user_uniforms[shaderType];

        for (int i = 0 ; i < uniforms.size() ; ++i)
        {
            if (uniforms.at(i).name == e->propertyName())
            {
                propertyChanged(i | (shaderType << 16));
            }
        }
    }

    return QObject::event(event);
}

void VideoShaderObject::propertyChanged(int id)
{
    DPTR_D(VideoShaderObject);

    const int st     = id >> 16;
    const int idx    = id & 0xffff;
    Uniform& u       = d.user_uniforms[st][idx];
    const QVariant v = property(u.name.constData());
    u.set(v);
/*
    if (u.dirty)
        update();
*/
}

void VideoShaderObject::programReady()
{
    DPTR_D(VideoShaderObject);

    // find property name. if has property, bind to property

    for (int st = VertexShader ; st < ShaderTypeCount ; ++st)
    {
        qDeleteAll(d.sigMap[st]);
        d.sigMap[st].clear();
        const QVector<Uniform>& uniforms = d.user_uniforms[st];

        for (int i = 0 ; i < uniforms.size() ; ++i)
        {
            const Uniform& u = uniforms[i];
            const int idx    = metaObject()->indexOfProperty(u.name.constData());

            if (idx < 0)
            {
                qCDebug(DIGIKAM_QTAV_LOG).noquote()
                    << QString::asprintf("VideoShaderObject has no meta property '%s'. "
                                         "Setting initial value from dynamic property",
                        u.name.constData());

                const_cast<Uniform&>(u).set(property(u.name.constData()));

                continue;
            }

            QMetaProperty mp = metaObject()->property(idx);

            if (!mp.hasNotifySignal())
            {
                qCWarning(DIGIKAM_QTAV_LOG_WARN).noquote()
                     << QString::asprintf("VideoShaderObject property '%s' has no signal",
                        mp.name());

                continue;
            }

            QMetaMethod mm              = mp.notifySignal();
            QSignalMapper* const mapper = new QSignalMapper();
            mapper->setMapping(this, i | (st << 16));

            connect(this, mm,
                    mapper, mapper->metaObject()->method(mapper->metaObject()->indexOfSlot("map()")));

            connect(mapper, SIGNAL(mapped(int)),
                    this, SLOT(propertyChanged(int)));

            d.sigMap[st].append(mapper);

            qCDebug(DIGIKAM_QTAV_LOG) << "set uniform property: "
                                      << u.name << property(u.name.constData());

            propertyChanged(i | (st << 16)); // set the initial value
        }
    }
/*
    ready();
*/
}

class Q_DECL_HIDDEN DynamicShaderObjectPrivate : public VideoShaderObjectPrivate    // clazy:exclude=copyable-polymorphic
{
public:

    QString header;
    QString sampleFunc;
    QString pp;
};

DynamicShaderObject::DynamicShaderObject(QObject* const parent)
    : VideoShaderObject(*new DynamicShaderObjectPrivate(), parent)
{
}

DynamicShaderObject::DynamicShaderObject(DynamicShaderObjectPrivate& d, QObject* const parent)
    : VideoShaderObject(d, parent)
{
}

QString DynamicShaderObject::header() const
{
    return d_func().header;
}

void DynamicShaderObject::setHeader(const QString& text)
{
    DPTR_D(DynamicShaderObject);

    if (d.header == text)
        return;

    d.header = text;

    Q_EMIT headerChanged();

    rebuildLater();
}

QString DynamicShaderObject::sample() const
{
    return d_func().sampleFunc;
}

void DynamicShaderObject::setSample(const QString& text)
{
    DPTR_D(DynamicShaderObject);

    if (d.sampleFunc == text)
        return;

    d.sampleFunc = text;

    Q_EMIT sampleChanged();

    rebuildLater();
}

QString DynamicShaderObject::postProcess() const
{
    return d_func().pp;
}

void DynamicShaderObject::setPostProcess(const QString& text)
{
    DPTR_D(DynamicShaderObject);

    if (d.pp == text)
        return;

    d.pp = text;

    Q_EMIT postProcessChanged();

    rebuildLater();
}

const char* DynamicShaderObject::userShaderHeader(QOpenGLShader::ShaderType st) const
{
    if (st == QOpenGLShader::Vertex)
        return nullptr;

    if (d_func().header.isEmpty())
        return nullptr;

    return d_func().header.toUtf8().constData();
}

const char* DynamicShaderObject::userSample() const
{
    if (d_func().sampleFunc.isEmpty())
        return nullptr;

    return d_func().sampleFunc.toUtf8().constData();
}

const char* DynamicShaderObject::userPostProcess() const
{
    if (d_func().pp.isEmpty())
        return nullptr;

    return d_func().pp.toUtf8().constData();
}

} // namespace QtAV
