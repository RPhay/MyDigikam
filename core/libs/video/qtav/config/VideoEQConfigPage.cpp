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

#include "VideoEQConfigPage.h"

// C++ includes

#include <algorithm>

// Qt includes

#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QLayout>
#include <QVector>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "AVPlayerSlider.h"

namespace QtAV
{

VideoEQConfigPage::VideoEQConfigPage(QWidget* const parent)
    : QWidget(parent)
{
    mEngine               = SWScale;
    QGridLayout* const gl = new QGridLayout();
    setLayout(gl);

    QLabel* const label   = new QLabel();
    label->setText(i18nc("@label", "Engine"));
    mpEngine              = new QComboBox();
    setEngines(QVector<Engine>(1, SWScale));

    connect(mpEngine, SIGNAL(currentIndexChanged(int)),
            this, SLOT(onEngineChangedByUI()));

    int r = 0, c = 0;
    gl->addWidget(label, r, c);
    gl->addWidget(mpEngine, r, c+1);
    r++;

    struct
    {
        QSlider** slider;
        QString   text;
    } sliders[] =
    {
        { &mpBSlider, i18nc("@option", "Brightness")  },
        { &mpCSlider, i18nc("@option", "Contrast")   },
        { &mpHSlider, i18nc("@option", "Hue")         },
        { &mpSSlider, i18nc("@option", "Saturation")  },
        { 0,          QString()         }
    };

    for (int i = 0 ; sliders[i].slider ; ++i)
    {
        QLabel* const slabel  = new QLabel(sliders[i].text);
        *sliders[i].slider    = new AVPlayerSlider();
        QSlider* const slider = *sliders[i].slider;
        slider->setOrientation(Qt::Horizontal);
        slider->setTickInterval(2);
        slider->setRange(-100, 100);
        slider->setValue(0);

        gl->addWidget(slabel, r, c);
        gl->addWidget(slider, r, c + 1);
        r++;
    }

    mpGlobal      = new QCheckBox(i18nc("@option: global settings", "Global"));
    mpGlobal->setEnabled(false);
    mpGlobal->setChecked(false);
    mpResetButton = new QPushButton(i18nc("@action", "Reset"));

    gl->addWidget(mpGlobal,      r, c,     Qt::AlignLeft);
    gl->addWidget(mpResetButton, r, c + 1, Qt::AlignRight);
    r++;

    connect(mpBSlider, SIGNAL(valueChanged(int)),
            this, SIGNAL(brightnessChanged(int)));

    connect(mpCSlider, SIGNAL(valueChanged(int)),
            this, SIGNAL(contrastChanged(int)));

    connect(mpHSlider, SIGNAL(valueChanged(int)),
            this, SIGNAL(hueChanegd(int)));

    connect(mpSSlider, SIGNAL(valueChanged(int)),
            this, SIGNAL(saturationChanged(int)));

    connect(mpGlobal, SIGNAL(toggled(bool)),
            this, SLOT(onGlobalSet(bool)));

    connect(mpResetButton, SIGNAL(clicked()),
            this, SLOT(onReset()));
}

void VideoEQConfigPage::onGlobalSet(bool g)
{
    Q_UNUSED(g);
}

void VideoEQConfigPage::setEngines(const QVector<Engine>& engines)
{
    mpEngine->clear();
    QVector<Engine> es(engines);
    std::sort(es.begin(), es.end());
    mEngines = es;

    Q_FOREACH (Engine e, es)
    {
        if      (e == SWScale)
        {
            mpEngine->addItem(QString::fromLatin1("libswscale"));
        }
        else if (e == GLSL)
        {
            mpEngine->addItem(QString::fromLatin1("GLSL"));
        }
        else if (e == XV)
        {
            mpEngine->addItem(QString::fromLatin1("XV"));
        }
    }
}

void VideoEQConfigPage::setEngine(Engine engine)
{
    if (engine == mEngine)
        return;

    mEngine = engine;

    if (!mEngines.isEmpty())
    {
        mpEngine->setCurrentIndex(mEngines.indexOf(engine));
    }

    Q_EMIT engineChanged();
}

VideoEQConfigPage::Engine VideoEQConfigPage::engine() const
{
    return mEngine;
}

qreal VideoEQConfigPage::brightness() const
{
    return (qreal)mpBSlider->value() / 100.0;
}

qreal VideoEQConfigPage::contrast() const
{
    return (qreal)mpCSlider->value() / 100.0;
}

qreal VideoEQConfigPage::hue() const
{
    return (qreal)mpHSlider->value() / 100.0;
}

qreal VideoEQConfigPage::saturation() const
{
    return (qreal)mpSSlider->value() / 100.0;
}

void VideoEQConfigPage::onReset()
{
    mpBSlider->setValue(0);
    mpCSlider->setValue(0);
    mpHSlider->setValue(0);
    mpSSlider->setValue(0);
}

void VideoEQConfigPage::onEngineChangedByUI()
{
    if ((mpEngine->currentIndex() >= mEngines.size()) || (mpEngine->currentIndex() < 0))
        return;

    mEngine = mEngines.at(mpEngine->currentIndex());

    Q_EMIT engineChanged();
}

} // namespace QtAV
