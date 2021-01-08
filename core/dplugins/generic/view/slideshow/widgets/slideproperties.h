/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-09-19
 * Description : slide properties widget
 *
 * Copyright (C) 2014-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2019-2020 by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#ifndef DIGIKAM_SLIDE_PROPERTIES_PLUGIN_H
#define DIGIKAM_SLIDE_PROPERTIES_PLUGIN_H

#include <QWidget>
#include <QPainter>
#include <QString>
#include <QColor>
#include <QUrl>

// Local includes

#include "slideshowsettings.h"

namespace DigikamGenericSlideShowPlugin
{

class SlideProperties : public QWidget
{
    Q_OBJECT

public:

    explicit SlideProperties(SlideShowSettings* const settings, QWidget* const parent);
    ~SlideProperties()              override;

    void setCurrentUrl(const QUrl& url);
    void togglePaintEnabled();

private:

    void printInfoText(QPainter& p, int& offset, const QString& str, const QColor& pcol = Qt::white);
    void printComments(QPainter& p, int& offset, const QString& comments);
    void printTags(QPainter& p, int& offset, QStringList& tags);
    void paintEvent(QPaintEvent*)   override;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericSlideShowPlugin

#endif // DIGIKAM_SLIDE_PROPERTIES_PLUGIN_H
