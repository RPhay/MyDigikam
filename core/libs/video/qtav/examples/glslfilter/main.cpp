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


#include <QtAV>
#include <QtAV/VideoShaderObject.h>
#include <QtAV/OpenGLVideo.h>
#include <QApplication>
#include <QScopedPointer>
#include <qmath.h>
#include <QWidget>
#include "digikam_debug.h"

using namespace QtAV;
#define GLSL(x) #x ""

class WaveShader : public DynamicShaderObject {
    float t;
    qreal A;

    Q_OBJECT
    Q_PROPERTY(qreal u_A READ u_A WRITE setU_A NOTIFY u_AChanged) //you can use meta property
public:
    qreal u_A() const {return A;}
    void setU_A(qreal v) { A=v; Q_EMIT u_AChanged();}
    WaveShader(QObject *parent = 0) : DynamicShaderObject(parent)
      , t(0)
      , A(0.06)
    {
        setProperty("u_t", 0); // you can use dynamic property
        //setProperty("u_A", 0.06);
        setProperty("u_omega", 5);
        setHeader(QLatin1String(GLSL(
                      uniform float u_omega;
                      uniform float u_A;
                      uniform float u_t;
                      )));
        setSample(QLatin1String(GLSL(
                                    vec4 sample2d(sampler2D tex, vec2 pos, int p)
                                    {
                                        vec2 pulse = sin(u_t - u_omega * pos);
                                        vec2 coord = pos + u_A*vec2(pulse.x, -pulse.x);
                                        return texture(tex, coord);
                                    })));
        startTimer(40);
    }
Q_SIGNALS:
    void u_AChanged();
protected:
    void timerEvent(QTimerEvent*) {
        t+=2.0*M_PI/25.0;
        setProperty("u_t", t);
    }
};

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    if (a.arguments().size() < 2) {
        qCDebug(DIGIKAM_TESTS_LOG) << a.arguments().at(0) << " file";
        return 0;
    }
    VideoOutput vo;
    if (!vo.opengl()) {
        qFatal("opengl renderer is required!");
        return 1;
    }
    if (!vo.widget())
        return 1;
    GLSLFilter *f = new GLSLFilter();
    f->setOwnedByTarget();
    f->opengl()->setUserShader(new WaveShader(f));
    f->installTo(&vo);
    vo.widget()->show();
    vo.widget()->resize(800, 500);
    vo.widget()->setWindowTitle(QLatin1String("GLSLFilter example"));
    AVPlayer player;
    player.setRenderer(&vo);

    player.play(a.arguments().last());
    return a.exec();
}
#include "main.moc"
