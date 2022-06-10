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

#ifndef QTAV_WIDGETS_AVFILTER_CONFIG_PAGE_H
#define QTAV_WIDGETS_AVFILTER_CONFIG_PAGE_H

// Local includes

#include "QtAVWidgets_Global.h"
#include "ConfigPageBase.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QTextEdit;

namespace QtAV
{

class DIGIKAM_EXPORT AVFilterConfigPage : public ConfigPageBase
{
    Q_OBJECT

public:

    explicit AVFilterConfigPage(QWidget* const parent = nullptr);
    virtual QString name() const override;

protected:

    virtual void applyToUi()     override;
    virtual void applyFromUi()   override;

private Q_SLOTS:

    void audioFilterChanged(const QString& name);
    void videoFilterChanged(const QString& name);

private:

    struct
    {
        QString    type;
        QCheckBox* enable;
        QComboBox* name;
        QLabel*    description;
        QTextEdit* options;
    } m_ui[2];                  ///< 0: video, 1: audio
};

} // namespace QtAV

#endif // QTAV_WIDGETS_AVFILTER_CONFIG_PAGE_H
