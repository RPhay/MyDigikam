/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-09-19
 * Description : slide properties widget
 *
 * SPDX-FileCopyrightText: 2014-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2019-2020 by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
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
