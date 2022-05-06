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

#ifndef QTAV_PLAYER_CONFIGPAGEBASE_H
#define QTAV_PLAYER_CONFIGPAGEBASE_H

#include <QWidget>

namespace QtAVPlayer
{

class ConfigPageBase : public QWidget
{
    Q_OBJECT
public:
    explicit ConfigPageBase(QWidget *parent = 0);
    virtual QString name() const = 0;
    void applyOnUiChange(bool a);
    // default is true. in dialog is false, must call ConfigDialog::apply() to apply
    bool applyOnUiChange() const;

public Q_SLOTS:
    // deprecated. call applyFromUi()
    void apply();
    // deprecated. call applyToUi().
    void cancel();
    //call applyToUi() after Config::instance().reset();
    void reset();
    /*!
     * \brief applyToUi
     * Apply configurations to UI. Call this in your page ctor when ui layout is ready.
     */
    virtual void applyToUi() = 0;
protected:
    /*!
     * \brief applyFromUi
     * Save configuration values from UI. Call Config::xxx(value) in your implementation
     */
    virtual void applyFromUi() = 0;
private:
    bool mApplyOnUiChange;
};

} // namespace QtAVPlayer

#endif // QTAV_PLAYER_CONFIGPAGEBASE_H
