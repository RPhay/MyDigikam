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

#include "MiscPage.h"

// Qt includes

#include <QLabel>
#include <QLayout>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "AVPlayerConfigMngr.h"

namespace QtAV
{

MiscPage::MiscPage(QWidget* const parent, bool previewOpt)
    : ConfigPageBase(parent)
{
    QGridLayout* const gl = new QGridLayout();
    setLayout(gl);
    gl->setSizeConstraint(QLayout::SetFixedSize);
    int r                 = 0;

    m_preview_on          = new QCheckBox(i18n("Preview Over Time-Slider"));
    gl->addWidget(m_preview_on, r++, 0);
    m_preview_w           = new QSpinBox();
    m_preview_w->setRange(1, 1920);
    m_preview_h           = new QSpinBox();
    m_preview_h->setRange(1, 1080);
    QLabel* const slbl    =  new QLabel(QString::fromLatin1("%1 %2: ").arg(i18n("Preview")).arg(i18n("size")));
    gl->addWidget(slbl, r, 0);
    QHBoxLayout* hb       = new QHBoxLayout();
    hb->addWidget(m_preview_w);
    QLabel* const xlbl    = new QLabel(QString::fromLatin1("x"));
    hb->addWidget(xlbl);
    hb->addWidget(m_preview_h);
    gl->addLayout(hb, r, 1);

    m_preview_on->setVisible(previewOpt);
    m_preview_w->setVisible(previewOpt);
    m_preview_h->setVisible(previewOpt);
    slbl->setVisible(previewOpt);
    xlbl->setVisible(previewOpt);

    r++;
    gl->addWidget(new QLabel(i18n("Force fps")), r, 0);
    m_fps                 = new QDoubleSpinBox();
    m_fps->setMinimum(-m_fps->maximum());
    m_fps->setToolTip(QString::fromLatin1("<= 0: ") + i18n("Ignore"));
    gl->addWidget(m_fps, r++, 1);

    QLabel* const lbl = new QLabel(i18n("Progress update interval") + QString::fromLatin1("(ms)"));
    lbl->setVisible(false);
    gl->addWidget(lbl, r, 0);
    m_notify_interval     = new QSpinBox();
    m_notify_interval->setVisible(false);
    gl->addWidget(m_notify_interval, r++, 1);

    gl->addWidget(new QLabel(i18n("Buffer frames")), r, 0);
    m_buffer_value        = new QSpinBox();
    m_buffer_value->setRange(-1, 32767);
    m_buffer_value->setToolTip(QString::fromLatin1("-1: auto"));
    gl->addWidget(m_buffer_value, r++, 1);

    gl->addWidget(new QLabel(QString::fromLatin1("%1(%2)").arg(i18n("Timeout")).arg(i18n("s"))), r, 0);
    m_timeout             = new QDoubleSpinBox();
    m_timeout->setDecimals(3);
    m_timeout->setSingleStep(1.0);
    m_timeout->setMinimum(-0.5);
    m_timeout->setToolTip(QString::fromLatin1("<=0: never"));
    m_timeout_abort       = new QCheckBox(i18n("Abort"));
    hb                    = new QHBoxLayout();
    hb->addWidget(m_timeout);
    hb->addWidget(m_timeout_abort);
    gl->addLayout(hb, r++, 1);

    gl->addWidget(new QLabel(i18n("OpenGL type")), r, 0);
    m_opengl              = new QComboBox();
    m_opengl->addItem(QString::fromLatin1("Auto"), AVPlayerConfigMngr::Auto);
    m_opengl->addItem(QString::fromLatin1("Desktop"), AVPlayerConfigMngr::Desktop);
    m_opengl->addItem(QString::fromLatin1("OpenGLES"), AVPlayerConfigMngr::OpenGLES);
    m_opengl->addItem(QString::fromLatin1("Software"), AVPlayerConfigMngr::Software);
    m_opengl->setToolTip(i18n("Windows only") + QLatin1String(" Qt>=5.4 + dynamicgl") +
                         QString::fromLatin1("\n") + i18n("OpenGLES is Used by DXVA Zero Copy"));
    gl->addWidget(m_opengl, r, 1);
    m_angle_platform      = new QComboBox();
    m_angle_platform->setToolTip(i18n("D3D9 has performance if ZeroCopy is disabled or for software decoders") +
                                QString::fromLatin1("\n") + i18n("RESTART REQUIRED"));
    m_angle_platform->addItems(QStringList() << QString::fromLatin1("D3D9")
                                             << QString::fromLatin1("D3D11")
                                             << QString::fromLatin1("AUTO")
                                             << QString::fromLatin1("WARP"));

#ifndef QT_OPENGL_DYNAMIC

    m_opengl->setEnabled(false);
    m_angle_platform->setEnabled(false);

#endif

    gl->addWidget(m_angle_platform, r++, 2);

    gl->addWidget(new QLabel(QLatin1String("EGL")), r, 0);
    m_egl                 = new QCheckBox();
    m_egl->setToolTip(i18n("Currently only works for Qt>=5.5 XCB build"));

    gl->addWidget(m_egl, r++, 1);
}

QString MiscPage::name() const
{
    return i18n("Misc");
}

void MiscPage::applyFromUi()
{
    AVPlayerConfigMngr::instance().setPreviewEnabled(m_preview_on->isChecked())
            .setPreviewWidth(m_preview_w->value())
            .setPreviewHeight(m_preview_h->value())
            .setEGL(m_egl->isChecked())
            .setOpenGLType((AVPlayerConfigMngr::OpenGLType)m_opengl->itemData(m_opengl->currentIndex()).toInt())
            .setANGLEPlatform(m_angle_platform->currentText().toLower())
            .setForceFrameRate(m_fps->value())
            .setBufferValue(m_buffer_value->value())
            .setTimeout(m_timeout->value())
            .setAbortOnTimeout(m_timeout_abort->isChecked())
    ;
}

void MiscPage::applyToUi()
{
    m_preview_on->setChecked(AVPlayerConfigMngr::instance().previewEnabled());
    m_preview_w->setValue(AVPlayerConfigMngr::instance().previewWidth());
    m_preview_h->setValue(AVPlayerConfigMngr::instance().previewHeight());
    m_opengl->setCurrentIndex(m_opengl->findData(AVPlayerConfigMngr::instance().openGLType()));
    m_angle_platform->setCurrentIndex(m_angle_platform->findText(AVPlayerConfigMngr::instance().getANGLEPlatform().toUpper()));
    m_egl->setChecked(AVPlayerConfigMngr::instance().isEGL());
    m_fps->setValue(AVPlayerConfigMngr::instance().forceFrameRate());
/*
    m_notify_interval->setValue(AVPlayerConfigMngr::instance().avfilterOptions());
*/
    m_buffer_value->setValue(AVPlayerConfigMngr::instance().bufferValue());
    m_timeout->setValue(AVPlayerConfigMngr::instance().timeout());
    m_timeout_abort->setChecked(AVPlayerConfigMngr::instance().abortOnTimeout());
}

} // namespace QtAV
