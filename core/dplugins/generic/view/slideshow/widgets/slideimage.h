/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-09-18
 * Description : slideshow image widget
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

#ifndef DIGIKAM_SLIDE_IMAGE_PLUGIN_H
#define DIGIKAM_SLIDE_IMAGE_PLUGIN_H

// Qt includes

#include <QWidget>
#include <QUrl>

// Local includes

#include "digikam_export.h"
#include "loadingdescription.h"
#include "previewsettings.h"
#include "dimg.h"

using namespace Digikam;

namespace DigikamGenericSlideShowPlugin
{

class SlideImage : public QWidget
{
    Q_OBJECT

public:

    explicit SlideImage(QWidget* const parent = nullptr);
    ~SlideImage() override;

    void setPreviewSettings(const PreviewSettings& settings);
    void setLoadUrl(const QUrl& url);
    void setPreloadUrl(const QUrl& url);

Q_SIGNALS:

    void signalImageLoaded(bool);

private Q_SLOTS:

    void slotGotImagePreview(const LoadingDescription&, const DImg&);

private:

    void paintEvent(QPaintEvent*) override;
    void updatePixmap();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericSlideShowPlugin

#endif // DIGIKAM_SLIDE_IMAGE_PLUGIN_H
