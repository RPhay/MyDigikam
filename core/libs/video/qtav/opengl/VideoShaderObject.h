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

#ifndef QTAV_VIDEO_SHADER_OBJECT_H
#define QTAV_VIDEO_SHADER_OBJECT_H

// Qt includes

#include <QObject>
#include <QVector>

// Local includes

#include "VideoShader.h"

namespace QtAV
{

// check and auto update properties in shader

class VideoShaderObjectPrivate;

/*!
 * \brief The VideoShaderObject class
 * User defined uniform names are bound to class meta properties (property signals are required)
 * and object dynamic properties.
 * Property value type T is limited to float, int, unsigned(ES3.0) and QVector<T>
 */
class DIGIKAM_EXPORT VideoShaderObject : public QObject,
                                         public VideoShader
{
    DPTR_DECLARE_PRIVATE(VideoShaderObject)
    Q_OBJECT

public:

    explicit VideoShaderObject(QObject* const parent = nullptr);

protected:

    explicit VideoShaderObject(VideoShaderObjectPrivate& d, QObject* const parent = nullptr);

    bool event(QEvent* event) override;

private Q_SLOTS:

    void propertyChanged(int id);

private:

    void programReady() override final;
};

// ---------------------------------------------------------------------------------------------------

class DynamicShaderObjectPrivate;

/*!
 * \brief The DynamicShaderObject class
 * Able to set custom shader code
 */
class DIGIKAM_EXPORT DynamicShaderObject : public VideoShaderObject
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(DynamicShaderObject)
    Q_PROPERTY(QString header READ header WRITE setHeader NOTIFY headerChanged)
    Q_PROPERTY(QString sample READ sample WRITE setSample NOTIFY sampleChanged)
    Q_PROPERTY(QString postProcess READ postProcess WRITE setPostProcess NOTIFY postProcessChanged)

public:

    explicit DynamicShaderObject(QObject* const parent = nullptr);

    QString header()        const;
    void setHeader(const QString& text);

    QString sample()        const;
    void setSample(const QString& text);

    QString postProcess()   const;
    void setPostProcess(const QString& text);

Q_SIGNALS:

    void headerChanged();
    void sampleChanged();
    void postProcessChanged();

protected:

    explicit DynamicShaderObject(DynamicShaderObjectPrivate& d, QObject* const parent = nullptr);

private:

    const char* userShaderHeader(QOpenGLShader::ShaderType st)  const override;
    const char* userSample()                                    const override;
    const char* userPostProcess()                               const override;
};

} // namespace QtAV

#endif //QTAV_VIDEO_SHADER_OBJECT_H
