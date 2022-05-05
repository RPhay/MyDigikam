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


#ifndef DECODERCONFIGPAGE_H
#define DECODERCONFIGPAGE_H

#include <QtCore/QVariant>
#include <QWidget>
#include "ConfigPageBase.h"

QT_BEGIN_NAMESPACE
class QListWidget;
class QToolButton;
class QSpinBox;
class QVBoxLayout;
QT_END_NAMESPACE
class DecoderConfigPage : public ConfigPageBase
{
    Q_OBJECT
    class DecoderItemWidget;
public:
    explicit DecoderConfigPage(QWidget *parent = 0);
    virtual QString name() const;
    QVariantHash audioDecoderOptions() const;
    QVariantHash videoDecoderOptions() const;

protected:
    virtual void applyToUi();
    virtual void applyFromUi();

private slots:
    void videoDecoderEnableChanged();
    void priorityUp();
    void priorityDown();
    void onDecSelected(DecoderItemWidget* iw);
    void updateDecodersUi();
    void onConfigChanged();

private:
    QSpinBox *mpThreads;
    QToolButton *mpUp, *mpDown;
    QList<DecoderItemWidget*> mDecItems;
    DecoderItemWidget *mpSelectedDec;
    QVBoxLayout *mpDecLayout;
};

#endif // DECODERCONFIGPAGE_H
