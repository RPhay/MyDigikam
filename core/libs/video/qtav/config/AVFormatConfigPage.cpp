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

#include "AVFormatConfigPage.h"

// C++ includes

#include <limits>

// Qt includes

#include <QLayout>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QLabel>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "AVPlayerConfigMngr.h"

namespace QtAV
{

AVFormatConfigPage::AVFormatConfigPage(QWidget* const parent)
    : ConfigPageBase(parent)
{
    setObjectName(QString::fromLatin1("avformat"));
    QGridLayout* const gl   = new QGridLayout();
    setLayout(gl);
    gl->setSizeConstraint(QLayout::SetFixedSize);
    int r             = 0;

    m_on              = new QCheckBox(QString::fromLatin1("%1 avformat %2").arg(i18n("Enable")).arg(i18n("options")));
    gl->addWidget(m_on, r++, 0);
    m_direct          = new QCheckBox(i18n("Reduce buffering"));
    gl->addWidget(m_direct, r++, 0);
    gl->addWidget(new QLabel(i18n("Probe size")), r, 0, Qt::AlignRight);
    m_probeSize       = new QSpinBox();
    m_probeSize->setMaximum(std::numeric_limits<int>::max());
    m_probeSize->setMinimum(0);
    m_probeSize->setToolTip(i18n("0: auto"));
    gl->addWidget(m_probeSize, r++, 1, Qt::AlignLeft);
    gl->addWidget(new QLabel(i18n("Max analyze duration")), r, 0, Qt::AlignRight);
    m_analyzeDuration = new QSpinBox();
    m_analyzeDuration->setMaximum(std::numeric_limits<int>::max());
    m_analyzeDuration->setToolTip(i18n("0: auto. how many microseconds are analyzed to probe the input"));
    gl->addWidget(m_analyzeDuration, r++, 1, Qt::AlignLeft);

    gl->addWidget(new QLabel(i18n("Extra")), r, 0, Qt::AlignRight);
    m_extra           = new QLineEdit();
    m_extra->setToolTip(QString::fromLatin1("key1=value1 key2=value2 ..."));
    gl->addWidget(m_extra, r++, 1, Qt::AlignLeft);
}

QString AVFormatConfigPage::name() const
{
    return i18n("AVFormat");
}

void AVFormatConfigPage::applyFromUi()
{
    AVPlayerConfigMngr::instance()
            .setAvformatOptionsEnabled(m_on->isChecked())
            .probeSize(m_probeSize->value())
            .analyzeDuration(m_analyzeDuration->value())
            .reduceBuffering(m_direct->isChecked())
            .avformatExtra(m_extra->text());
}

void AVFormatConfigPage::applyToUi()
{
    m_on->setChecked(AVPlayerConfigMngr::instance().avformatOptionsEnabled());
    m_direct->setChecked(AVPlayerConfigMngr::instance().reduceBuffering());
    m_probeSize->setValue(AVPlayerConfigMngr::instance().probeSize());
    m_analyzeDuration->setValue(AVPlayerConfigMngr::instance().analyzeDuration());
    m_extra->setText(AVPlayerConfigMngr::instance().avformatExtra());
}

} // namespace QtAV
