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

#include "ConfigPageBase.h"

namespace QtAV
{

ConfigPageBase::ConfigPageBase(QWidget* const parent)
    : QWidget         (parent),
      mApplyOnUiChange(true)
{
}

void ConfigPageBase::applyOnUiChange(bool a)
{
    mApplyOnUiChange = a;
}

bool ConfigPageBase::applyOnUiChange() const
{
    return mApplyOnUiChange;
}

void ConfigPageBase::apply()
{
    applyFromUi();
}

void ConfigPageBase::cancel()
{
    applyToUi();
}

void ConfigPageBase::reset()
{
    // NOTE: make sure AVPlayerConfigMngr::instance().reset() is called before it. It is called by ConfigDialog.reset()

    applyToUi();
}

} // namespace QtAV
