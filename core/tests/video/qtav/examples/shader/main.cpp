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

// Qt includes

#include <qmath.h>
#include <QApplication>
#include <QScopedPointer>
#include <QWidget>

// Local includes

#include "QtAV.h"
#include "OpenGLVideo.h"
#include "ConvolutionShader.h"
#include "VideoShaderObject.h"
#include "digikam_debug.h"

using namespace QtAV;

#define GLSL(x) #x ""

class MyShader : public VideoShaderObject
{
    Q_OBJECT

public:

    MyShader()
    {
        startTimer(50);
    }

protected:

    void timerEvent(QTimerEvent*)
    {
        static int t  = 0;
        const float b = float(50 -     t % 100) / 50.0;
        const float c = float(50 - 4 * t % 100) / 50.0;
        ++t;
        setProperty("bs", QVariant::fromValue(QVector<float>() << b << c));
    }

private:

    const char* userShaderHeader(QOpenGLShader::ShaderType type) const override
    {
        if (type == QOpenGLShader::Vertex)
            return nullptr;

        return GLSL( uniform vec2 bs; );
    }

    const char* userPostProcess()                                const override
    {
        return GLSL(
                    float lr     = 0.3086;
                    float lg     = 0.6094;
                    float lb     = 0.0820;
                    float s      = bs.g + 1.0;
                    float is     = 1.0 - s;
                    float ilr    = is * lr;
                    float ilg    = is * lg;
                    float ilb    = is * lb;

                    mat4 m       = mat4
                    (
                        ilr + s, ilg,     ilb,     0.0,
                        ilr,     ilg + s, ilb,     0.0,
                        ilr,     ilg,     ilb + s, 0.0,
                        0.0,     0.0,     0.0,     1.0
                    );

                    gl_FragColor = m * gl_FragColor + bs.r;
                   );
    }

private:

    // Disable

    MyShader(QObject*);
};

class MediumBlurShader : public ConvolutionShader
{
public:

    MediumBlurShader()
    {
        setKernelRadius(2);
    }

protected:

    virtual const float* kernel() const override
    {
        const float v = 1.0 / (float)kernelSize(); // radius 1
        static QVector<float> k;
        k.resize(kernelSize());
        k.fill(v);

        return k.constData();
    }
};

class WaveShader : public QObject,
                   public VideoShader
{
    Q_OBJECT

public:

    WaveShader(QObject* const parent = nullptr)
        : QObject(parent),
          t      (0.0),
          A      (0.06),
          omega  (5.0)
    {
        startTimer(20);
    }

protected:

    void timerEvent(QTimerEvent*)
    {
        t += 2.0 * M_PI / 50.0;
    }

private:

    const char* userShaderHeader(QOpenGLShader::ShaderType type) const override
    {
        if (type == QOpenGLShader::Vertex)
            return nullptr;

        return GLSL(
                    uniform float u_omega;
                    uniform float u_A;
                    uniform float u_t;
        );
    }

    const char* userSample()                                     const override
    {
        return GLSL(
                    vec4 sample2d(sampler2D tex, vec2 pos, int p)
                    {
                        vec2 pulse = sin(u_t - u_omega * pos);
                        vec2 coord = pos + u_A * vec2(pulse.x, -pulse.x);

                        return texture(tex, coord);
                    }
        );
    }

    bool setUserUniformValues()                                        override
    {
        // return false; // enable this line to call setUserUniformValue(Uniform&)

        program()->setUniformValue("u_A",     A);
        program()->setUniformValue("u_omega", omega);
        program()->setUniformValue("u_t",     t);

        return true;
    }

    // setUserUniformValues() must return false to call setUserUniformValue(Uniform&)

    void setUserUniformValue(Uniform& u)                               override
    {
        if      (u.name == "u_A")
        {
            u.set(A);
        }
        else if (u.name == "u_omega")
        {
            u.set(omega);
        }
        else if (u.name == "u_t")
        {
            u.set(t);
        }
    }

private:

    float t;
    float A;
    float omega;
};

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    if (a.arguments().size() < 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG).noquote()
            << QString::asprintf("./shader file");

        return 0;
    }

    VideoOutput vo[4];
    QScopedPointer<VideoShader> shaders[4];
    AVPlayerCore player;

    struct
    {
        QByteArray   title;
        VideoShader* shader = nullptr;
    }
    shader_list[] =
    {
        {"No shader",                                                     nullptr                 },
        {"Wave effect shader",                                            new WaveShader()        },
        {"Blur shader",                                                   new MediumBlurShader()  },
        {"Brightness+Saturation. (VideoShaderObject dynamic properties)", new MyShader()          }
    };

    vo[0].widget()->move(0, 0);

    for (int i = 0 ; i < 4 ; ++i)
    {
        if (!vo[i].opengl())
            qFatal("No opengl in the renderer");

        player.addVideoRenderer(&vo[i]);
        vo[i].widget()->setWindowTitle(QString::fromLatin1(shader_list[i].title));
        vo[i].widget()->show();
        vo[i].widget()->resize(500, 300);
        vo[i].widget()->move(vo[0].widget()->x() + (i % 2) * vo[0].widget()->width(),
                             vo[0].widget()->y() + (i / 2) * vo[0].widget()->height());
        vo[i].opengl()->setUserShader(shader_list[i].shader);
        shaders[i].reset(shader_list[i].shader);
    }

    player.play(a.arguments().at(1));

    return a.exec();
}

#include "main.moc"
