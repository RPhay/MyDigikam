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

#ifndef VIDEOEQCONFIGPAGE_H
#define VIDEOEQCONFIGPAGE_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QPushButton;
class QSlider;
QT_END_NAMESPACE
class VideoEQConfigPage : public QWidget
{
    Q_OBJECT
public:
    enum Engine {
        SWScale,
        GLSL,
        XV,
    };
    explicit VideoEQConfigPage(QWidget *parent = 0);
    void setEngines(const QVector<Engine>& engines);
    void setEngine(Engine engine);
    Engine engine() const;

    qreal brightness() const;
    qreal contrast() const;
    qreal hue() const;
    qreal saturation() const;

signals:
    void engineChanged();
    void brightnessChanged(int);
    void contrastChanged(int);
    void hueChanegd(int);
    void saturationChanged(int);

private slots:
    void onGlobalSet(bool);
    void onReset();
    void onEngineChangedByUI();

private:
    QCheckBox *mpGlobal;
    QComboBox *mpEngine;
    QSlider *mpBSlider, *mpCSlider, *mpSSlider;
    QSlider *mpHSlider;
    QPushButton *mpResetButton;
    Engine mEngine;
    QVector<Engine> mEngines;
};

#endif // VIDEOEQCONFIGPAGE_H
