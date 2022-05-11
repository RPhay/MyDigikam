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

#ifndef AV_PLAYER_CONFIG_PAGE_BASE_H
#define AV_PLAYER_CONFIG_PAGE_BASE_H

// Qt includes

#include <QWidget>

namespace AVPlayer
{

class ConfigPageBase : public QWidget
{
    Q_OBJECT

public:

    explicit ConfigPageBase(QWidget* const parent = nullptr);

    virtual QString name() const = 0;

    void applyOnUiChange(bool a);

    // default is true. in dialog is false, must call ConfigDialog::apply() to apply

    bool applyOnUiChange() const;

public Q_SLOTS:

    // deprecated. call applyFromUi()

    void apply();

    // deprecated. call applyToUi().

    void cancel();

    // call applyToUi() after AVPlayerConfigMngr::instance().reset();

    void reset();

    /*!
     * \brief applyToUi
     * Apply configurations to UI. Call this in your page ctor when ui layout is ready.
     */
    virtual void applyToUi() = 0;

protected:

    /*!
     * \brief applyFromUi
     * Save configuration values from UI. Call AVPlayerConfigMngr::xxx(value) in your implementation
     */
    virtual void applyFromUi() = 0;

private:

    bool mApplyOnUiChange;
};

} // namespace AVPlayer

#endif // AV_PLAYER_CONFIG_PAGE_BASE_H
