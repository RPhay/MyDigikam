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

#ifndef QTAV_PLAYER_AVFILTERCONFIGPAGE_H
#define QTAV_PLAYER_AVFILTERCONFIGPAGE_H

// Local includes

#include "ConfigPageBase.h"

QT_BEGIN_NAMESPACE
class QCheckBox;
class QComboBox;
class QLabel;
class QTextEdit;
QT_END_NAMESPACE

namespace QtAVPlayer
{

class AVFilterConfigPage : public ConfigPageBase
{
    Q_OBJECT

public:

    explicit AVFilterConfigPage(QWidget* const parent = nullptr);
    virtual QString name() const;

protected:

    virtual void applyToUi();
    virtual void applyFromUi();

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

} // namespace QtAVPlayer

#endif // QTAV_PLAYER_AVFILTERCONFIGPAGE_H
