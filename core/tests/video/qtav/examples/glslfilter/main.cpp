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

#include <QApplication>
#include <QScopedPointer>
#include <qmath.h>
#include <QWidget>

// Local includes

#include "QtAV.h"
#include "VideoShaderObject.h"
#include "OpenGLVideo.h"
#include "digikam_debug.h"

using namespace QtAV;

#define GLSL(x) #x ""

class WaveShader : public DynamicShaderObject
{
    Q_OBJECT
    Q_PROPERTY(qreal u_A READ u_A WRITE setU_A NOTIFY u_AChanged) // you can use meta property

public:

    WaveShader(QObject* const parent = nullptr)
        : DynamicShaderObject(parent),
          t                  (0.0),
          A                  (0.06)
    {
        setProperty("u_t", 0); // you can use dynamic property
/*
        setProperty("u_A", 0.06);
*/
        setProperty("u_omega", 5);

        setHeader(QLatin1String(GLSL(
                                     uniform float u_omega;
                                     uniform float u_A;
                                     uniform float u_t;
                                    )
        ));

        setSample(QLatin1String(GLSL(
                                     vec4 sample2d(sampler2D tex, vec2 pos, int p)
                                     {
                                         vec2 pulse = sin(u_t - u_omega * pos);
                                         vec2 coord = pos + u_A * vec2(pulse.x, -pulse.x);

                                         return texture(tex, coord);
                                     }
                                    )
        ));

        startTimer(40);
    }

    qreal u_A() const
    {
        return A;
    }

    void setU_A(qreal v)
    {
        A = v;

        Q_EMIT u_AChanged();
    }

Q_SIGNALS:

    void u_AChanged();

protected:

    void timerEvent(QTimerEvent*)
    {
        t += 2.0 * M_PI / 25.0;
        setProperty("u_t", t);
    }

private:

    float t = 0.0;
    qreal A = 0.06;
};

int main(int argc, char** argv)
{
    QApplication a(argc, argv);

    if (a.arguments().size() < 2)
    {
        qCDebug(DIGIKAM_TESTS_LOG) << a.arguments().at(0) << " file";

        return 0;
    }

    VideoOutput vo;

    if (!vo.opengl())
    {
        qFatal("opengl renderer is required!");

        return 1;
    }

    if (!vo.widget())
        return 1;

    GLSLFilter* const f = new GLSLFilter();

    f->setOwnedByTarget();
    f->opengl()->setUserShader(new WaveShader(f));
    f->installTo(&vo);

    vo.widget()->show();
    vo.widget()->resize(800, 500);
    vo.widget()->setWindowTitle(QLatin1String("GLSLFilter Example"));

    AVPlayerCore player;
    player.setRenderer(&vo);

    player.play(a.arguments().last());

    return a.exec();
}

#include "main.moc"
