/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2018-07-30
 * Description : a plugin to render slideshow.
 *
 * Copyright (C) 2018-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2020      by Minh Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#ifndef DIGIKAM_SLIDESHOW_PLUGIN_H
#define DIGIKAM_SLIDESHOW_PLUGIN_H

// Local includes

#include "dplugingeneric.h"
#include "iteminfolist.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.generic.SlideShow"

using namespace Digikam;

namespace DigikamGenericSlideShowPlugin
{

class SlideShowSettings;

class SlideShowPlugin : public DPluginGeneric
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginGeneric)

public:

    explicit SlideShowPlugin(QObject* const parent = nullptr);
    ~SlideShowPlugin()                   override;

    QString name()                 const override;
    QString iid()                  const override;
    QIcon   icon()                 const override;
    QString details()              const override;
    QString description()          const override;
    QList<DPluginAuthor> authors() const override;

    void setup(QObject* const)           override;

    void addConnectionSlideEnd(QObject* const obj);

private Q_SLOTS:

    void slotMenuSlideShow();
    void slotMenuSlideShowAll();
    void slotMenuSlideShowSelection();
    void slotMenuSlideShowRecursive();
    void slotShowRecursive(const QList<QUrl>& imageList);
    void slotShowManual();

private:

    void slideshow(SlideShowSettings* const settings,
                   bool autoPlayEnabled = true,
                   const QUrl& startFrom = QUrl());
};

} // namespace DigikamGenericSlideShowPlugin

#endif // DIGIKAM_SLIDESHOW_PLUGIN_H
