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

#ifndef QTAV_WIDGETS_AVFORMAT_CONFIG_PAGE_H
#define QTAV_WIDGETS_AVFORMAT_CONFIG_PAGE_H

// Qt includes

#include <QVariant>

// Local includes

#include "QtAVWidgets_Global.h"
#include "ConfigPageBase.h"

class QCheckBox;
class QSpinBox;
class QLineEdit;

namespace QtAV
{

class DIGIKAM_EXPORT AVFormatConfigPage : public ConfigPageBase
{
    Q_OBJECT

public:

    explicit AVFormatConfigPage(QWidget* const parent = nullptr);
    virtual QString name() const override;

protected:

    virtual void applyToUi()     override;
    virtual void applyFromUi()   override;

private:

    QCheckBox* m_on;
    QCheckBox* m_direct;
    QSpinBox*  m_probeSize;
    QSpinBox*  m_analyzeDuration;
    QLineEdit* m_extra;
};

} // namespace QtAV

#endif // QTAV_WIDGETS_AVFORMAT_CONFIG_PAGE_H
