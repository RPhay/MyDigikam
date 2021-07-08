/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-08
 * Description : Dummy Widget if no Webkit is found.
 *
 * Copyright (C) 2021 by Anjani Kumar <anjanik012 at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#pragma once

#ifndef DIGIKAM_DNOWEBWIDGET_H
#define DIGIKAM_DNOWEBWIDGET_H

#include <QLabel>

namespace Digikam
{
class DNoWebWidget : public QLabel
{
    Q_OBJECT

public:
    explicit DNoWebWidget(QWidget *parent);

    /* Dummy to imitate QWebEngineView::setUrl()
     * https://doc.qt.io/qt-5.15/qwebengineview.html#url-prop
     */
    void setUrl(const QUrl &);

 Q_SIGNALS:

    /*
     * Dummy signals to be called in vkontake_authencticationdialog.cpp
     * https://doc.qt.io/qt-5.15/qwebengineview.html#signals
     * */
    void urlChanged(const QUrl &);
    void loadStarted();
    void loadFinished(bool);
    void loadProgress(int);

};
}


#endif //DIGIKAM_DNOWEBWIDGET_H
