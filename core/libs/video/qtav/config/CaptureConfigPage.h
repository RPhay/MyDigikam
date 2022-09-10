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

#ifndef QTAV_WIDGETS_CAPTURE_CONFIG_PAGE_H
#define QTAV_WIDGETS_CAPTURE_CONFIG_PAGE_H

// Qt includes

#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

// Local includes

#include "QtAVWidgets_Global.h"
#include "ConfigPageBase.h"

namespace QtAV
{

/*
 * TODO: ConfigPageBase: auto save(true for menu ui, false for dialog ui)
 * virtual public slot: apply()
 */

class AVPlayerSlider;

class DIGIKAM_EXPORT CaptureConfigPage : public ConfigPageBase
{
    Q_OBJECT

public:

    explicit CaptureConfigPage(QWidget* const parent = nullptr);
    virtual QString name() const override;

protected:

    virtual void applyToUi()     override;
    virtual void applyFromUi()   override;

private Q_SLOTS:

    // only emit signals. no value stores.

    void selectSaveDir();
    void browseCaptureDir();

private:

    QLineEdit*      mpDir;
    QComboBox*      mpFormat;
    AVPlayerSlider* mpQuality;
};

} // namespace QtAV

#endif // QTAV_WIDGETS_CAPTURE_CONFIG_PAGE_H
