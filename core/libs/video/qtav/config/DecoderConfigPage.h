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

#ifndef QTAV_WIDGETS_DECODER_CONFIG_PAGE_H
#define QTAV_WIDGETS_DECODER_CONFIG_PAGE_H

// Qt includes

#include <QVariant>
#include <QWidget>

// Local includes

#include "QtAVWidgets_Global.h"
#include "ConfigPageBase.h"
#include "VideoDecoder.h"

class QListWidget;
class QToolButton;
class QSpinBox;
class QVBoxLayout;

namespace QtAV
{

class DIGIKAM_EXPORT DecoderConfigPage : public ConfigPageBase
{
    Q_OBJECT

    class DecoderItemWidget;

public:

    explicit DecoderConfigPage(QWidget* const parent = nullptr, bool advOptVisible = true);
    virtual QString name()             const override;

    QVariantHash audioDecoderOptions() const;
    QVariantHash videoDecoderOptions() const;

public:

    static QStringList idsToNames(QVector<VideoDecoderId> ids);
    static QVector<VideoDecoderId> idsFromNames(const QStringList& names);

protected:

    virtual void applyToUi()                 override;
    virtual void applyFromUi()               override;

private Q_SLOTS:

    void videoDecoderEnableChanged();
    void priorityUp();
    void priorityDown();
    void onDecSelected(DecoderItemWidget* iw);
    void updateDecodersUi();
    void onConfigChanged();

private:

    QSpinBox*                   mpThreads       = nullptr;
    QToolButton*                mpUp            = nullptr;
    QToolButton*                mpDown          = nullptr;
    QList<DecoderItemWidget*>   mDecItems;
    DecoderItemWidget*          mpSelectedDec   = nullptr;
    QVBoxLayout*                mpDecLayout     = nullptr;
};

} // namespace QtAV

#endif // QTAV_WIDGETS_DECODER_CONFIG_PAGE_H
