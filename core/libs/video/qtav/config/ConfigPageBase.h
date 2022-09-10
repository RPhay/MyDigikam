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

#ifndef QTAV_WIDGETS_CONFIG_PAGE_BASE_H
#define QTAV_WIDGETS_CONFIG_PAGE_BASE_H

// Qt includes

#include <QWidget>

// Local includes

#include "QtAVWidgets_Global.h"

namespace QtAV
{

class DIGIKAM_EXPORT ConfigPageBase : public QWidget
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

} // namespace QtAV

#endif // QTAV_WIDGETS_CONFIG_PAGE_BASE_H
