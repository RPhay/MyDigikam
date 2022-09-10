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

#ifndef QTAV_WIDGETS_SHADER_PAGE_H
#define QTAV_WIDGETS_SHADER_PAGE_H

// Qt includes

#include <QTextEdit>
#include <QCheckBox>

// Local includes

#include "QtAVWidgets_Global.h"
#include "ConfigPageBase.h"

namespace QtAV
{

class DIGIKAM_EXPORT ShaderPage : public ConfigPageBase
{
    Q_OBJECT

public:

    explicit ShaderPage(QWidget* const parent = nullptr);

    virtual QString name() const    override;

protected:

    virtual void applyToUi()        override;
    virtual void applyFromUi()      override;

private:

    QCheckBox* m_enable = nullptr;
    QCheckBox* m_fbo    = nullptr;
    QTextEdit* m_header = nullptr;
    QTextEdit* m_sample = nullptr;
    QTextEdit* m_pp     = nullptr;
};

} // namespace QtAV

#endif // QTAV_WIDGETS_SHADER_PAGE_H
