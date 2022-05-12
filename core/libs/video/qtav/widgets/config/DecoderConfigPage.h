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

#ifndef QTAV_WIDGETS_DECODER_CONFIG_PAGE_H
#define QTAV_WIDGETS_DECODER_CONFIG_PAGE_H

// Qt includes

#include <QVariant>
#include <QWidget>

// Local includes

#include "QtAVWidgets_Global.h"
#include "ConfigPageBase.h"
#include "VideoDecoder.h"

QT_BEGIN_NAMESPACE
class QListWidget;
class QToolButton;
class QSpinBox;
class QVBoxLayout;
QT_END_NAMESPACE

namespace QtAV
{

class QTAV_WIDGETS_EXPORT DecoderConfigPage : public ConfigPageBase
{
    Q_OBJECT

    class DecoderItemWidget;

public:

    explicit DecoderConfigPage(QWidget* const parent = nullptr);
    virtual QString name()             const override;

    QVariantHash audioDecoderOptions() const;
    QVariantHash videoDecoderOptions() const;

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

    QSpinBox*                   mpThreads;
    QToolButton*                mpUp;
    QToolButton*                mpDown;
    QList<DecoderItemWidget*>   mDecItems;
    DecoderItemWidget*          mpSelectedDec;
    QVBoxLayout*                mpDecLayout;
};

} // namespace QtAV

#endif // QTAV_WIDGETS_DECODER_CONFIG_PAGE_H
