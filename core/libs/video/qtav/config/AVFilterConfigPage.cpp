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

#include "AVFilterConfigPage.h"

// Qt includes

#include <QLayout>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QTextEdit>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "LibAVFilter.h"
#include "AVPlayerConfigMngr.h"

namespace QtAV
{

AVFilterConfigPage::AVFilterConfigPage(QWidget* const parent)
    : ConfigPageBase(parent)
{
    setObjectName(QString::fromLatin1("avfilter"));
    QGridLayout* const gl = new QGridLayout();
    setLayout(gl);
    gl->setSizeConstraint(QLayout::SetFixedSize);

    int r        = 0;
    m_ui[0].type = i18nc("@option: video filter", "Video");
    m_ui[1].type = i18nc("@option: audio filter", "Audio");
    const int mw = 300;

    for (size_t i = 0 ; i < sizeof(m_ui) / sizeof(m_ui[0]) ; ++i)
    {
        m_ui[i].enable      = new QCheckBox(i18nc("@option: enable filter", "Enable %1", m_ui[i].type));
        gl->addWidget(m_ui[i].enable, r++, 0);
        m_ui[i].name        = new QComboBox();
        m_ui[i].name->setToolTip(i18nc("@info", "Registered %1 filters", m_ui[i].type));
        gl->addWidget(m_ui[i].name, r++, 0);
        m_ui[i].description = new QLabel();
        m_ui[i].description->setMaximumWidth(mw);
        gl->addWidget(m_ui[i].description, r++, 0);
        gl->addWidget(new QLabel(i18nc("@label", "Parameters")), r++, 0);
        m_ui[i].options     = new QTextEdit();
        m_ui[i].options->setMaximumWidth(mw);
        m_ui[i].options->setMaximumHeight(mw / 6);
        gl->addWidget(m_ui[i].options, r++, 0);
    }

    m_ui[0].options->setToolTip(QString::fromLatin1("example: negate"));
    m_ui[1].options->setToolTip(QString::fromLatin1("example: volume=volume=2.0"));

    connect(m_ui[0].name, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(videoFilterChanged(QString)));

    m_ui[0].name->addItems(QtAV::LibAVFilter::videoFilters());

    connect(m_ui[1].name, SIGNAL(currentIndexChanged(QString)),
            this, SLOT(audioFilterChanged(QString)));

    m_ui[1].name->addItems(QtAV::LibAVFilter::audioFilters());
}

QString AVFilterConfigPage::name() const
{
    return QString::fromLatin1("AVFilter");
}

void AVFilterConfigPage::applyFromUi()
{
    AVPlayerConfigMngr::instance()
            .avfilterVideoOptions(m_ui[0].options->toPlainText())
            .avfilterVideoEnable(m_ui[0].enable->isChecked())
            .avfilterAudioOptions(m_ui[1].options->toPlainText())
            .avfilterAudioEnable(m_ui[1].enable->isChecked())
    ;
}

void AVFilterConfigPage::applyToUi()
{
    m_ui[0].enable->setChecked(AVPlayerConfigMngr::instance().avfilterVideoEnable());
    m_ui[0].options->setText(AVPlayerConfigMngr::instance().avfilterVideoOptions());
    m_ui[1].enable->setChecked(AVPlayerConfigMngr::instance().avfilterAudioEnable());
    m_ui[1].options->setText(AVPlayerConfigMngr::instance().avfilterAudioOptions());
}

void AVFilterConfigPage::videoFilterChanged(const QString& name)
{
    m_ui[0].description->setText(QtAV::LibAVFilter::filterDescription(name));
}

void AVFilterConfigPage::audioFilterChanged(const QString& name)
{
    m_ui[1].description->setText(QtAV::LibAVFilter::filterDescription(name));
}

} // namespace QtAV
