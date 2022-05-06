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

#ifndef QTAV_PLAYER_MISCPAGE_H
#define QTAV_PLAYER_MISCPAGE_H

#include "ConfigPageBase.h"
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QComboBox>

namespace QtAVPlayer
{

class MiscPage : public ConfigPageBase
{
    Q_OBJECT
public:
    MiscPage();
    virtual QString name() const;
protected:
    virtual void applyToUi();
    virtual void applyFromUi();
private:
    QCheckBox *m_preview_on;
    QSpinBox *m_preview_w;
    QSpinBox *m_preview_h;
    QSpinBox *m_notify_interval;
    QDoubleSpinBox *m_fps;
    QSpinBox *m_buffer_value;
    QDoubleSpinBox *m_timeout;
    QCheckBox *m_timeout_abort;
    QComboBox *m_opengl;
    QComboBox *m_angle_platform;
    QCheckBox *m_egl;
    QComboBox *m_log;
};

} // namespace QtAVPlayer

#endif // QTAV_PLAYER_MISCPAGE_H
