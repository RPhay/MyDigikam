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

#include "ShaderPage.h"

// Qt includes

#include <QLabel>
#include <QLayout>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "AVPlayerConfigMngr.h"

namespace QtAV
{

ShaderPage::ShaderPage(QWidget* const parent)
    : ConfigPageBase(parent)
{
    QVBoxLayout* const gl = new QVBoxLayout();
    setLayout(gl);
    gl->setSizeConstraint(QLayout::SetMaximumSize);

    const int mw          = 600;
    m_enable              = new QCheckBox(i18nc("@option: enable shader", "Enable"));
    gl->addWidget(m_enable);
    m_fbo                 = new QCheckBox(i18nc("@option", "Intermediate FBO"));
    gl->addWidget(m_fbo);
    gl->addWidget(new QLabel(i18nc("@label", "Fragment shader header")));
    m_header              = new QTextEdit();

    //m_header->setMaximumWidth(mw);

    m_header->setMaximumHeight(mw / 6);
    m_header->setToolTip(i18nc("@info", "Additional header code"));
    gl->addWidget(m_header);
    gl->addWidget(new QLabel(i18nc("@label", "Fragment shader texel sample function")));
    m_sample              = new QTextEdit();

    //m_sample->setMaximumWidth(mw);

    m_sample->setMaximumHeight(mw / 6);
    m_sample->setToolTip(QLatin1String("vec4 sample2d(sampler2D tex, vec2 pos, int p)"));
    gl->addWidget(m_sample);
    gl->addWidget(new QLabel(i18nc("@label", "Fragment shader RGB post process code")));
    m_pp                  = new QTextEdit();

    //m_pp->setMaximumWidth(mw);

    m_pp->setMaximumHeight(mw / 6);
    gl->addWidget(m_pp);
    gl->addStretch();
}

QString ShaderPage::name() const
{
    return i18nc("@title", "Shader");
}

void ShaderPage::applyToUi()
{
    m_enable->setChecked(AVPlayerConfigMngr::instance().userShaderEnabled());
    m_fbo->setChecked(AVPlayerConfigMngr::instance().intermediateFBO());
    m_header->setText(AVPlayerConfigMngr::instance().fragHeader());
    m_sample->setText(AVPlayerConfigMngr::instance().fragSample());
    m_pp->setText(AVPlayerConfigMngr::instance().fragPostProcess());
}

void ShaderPage::applyFromUi()
{
    AVPlayerConfigMngr::instance()
            .setUserShaderEnabled(m_enable->isChecked())
            .setIntermediateFBO(m_fbo->isChecked())
            .setFragHeader(m_header->toPlainText())
            .setFragSample(m_sample->toPlainText())
            .setFragPostProcess(m_pp->toPlainText())
    ;
}

} // namespace QtAV
