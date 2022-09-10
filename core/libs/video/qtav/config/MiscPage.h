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

#ifndef QTAV_WIDGETS_MISC_PAGE_H
#define QTAV_WIDGETS_MISC_PAGE_H

#include "ConfigPageBase.h"

// Qt includes

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QComboBox>

// Local includes

#include "QtAVWidgets_Global.h"

namespace QtAV
{

class DIGIKAM_EXPORT MiscPage : public ConfigPageBase
{
    Q_OBJECT

public:

    explicit MiscPage(QWidget* const parent = nullptr, bool previewOpt = true);

    virtual QString name() const override;

protected:

    virtual void applyToUi()     override;
    virtual void applyFromUi()   override;

private:

    QCheckBox*      m_preview_on      = nullptr;
    QSpinBox*       m_preview_w       = nullptr;
    QSpinBox*       m_preview_h       = nullptr;
    QSpinBox*       m_notify_interval = nullptr;
    QDoubleSpinBox* m_fps             = nullptr;
    QSpinBox*       m_buffer_value    = nullptr;
    QDoubleSpinBox* m_timeout         = nullptr;
    QCheckBox*      m_timeout_abort   = nullptr;
    QComboBox*      m_opengl          = nullptr;
    QComboBox*      m_angle_platform  = nullptr;
    QCheckBox*      m_egl             = nullptr;
};

} // namespace QtAV

#endif // QTAV_WIDGETS_MISC_PAGE_H
