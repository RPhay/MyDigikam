/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-20
 * Description : QImage DImg plugin.
 *
 * Copyright (C) 2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_QIMAGE_DIMG_PLUGIN_H
#define DIGIKAM_QIMAGE_DIMG_PLUGIN_H

// Qt includes

#include <QByteArray>
#include <QWidget>

// Local includes

#include "dplugindimg.h"
#include "dimg.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.dimg.QImage"

using namespace Digikam;

namespace DigikamDImgQImagePlugin
{

class QImageDImgPlugin : public DPluginDImg
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginDImg)

public:

    explicit QImageDImgPlugin(QObject* const parent = nullptr);
    ~QImageDImgPlugin();

    QString name()                 const override;
    QString iid()                  const override;
    QIcon   icon()                 const override;
    QString details()              const override;
    QString description()          const override;
    QList<DPluginAuthor> authors() const override;

    void setup(QObject* const)           override;

    QString     loaderName()                         const;
    QString     typeMimes()                          const;
    bool        isSupported(const QString& filePath) const;
    DImgLoader* loader(DImg* const)                  const;
    QWidget*    settingsWidget()                     const;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamDImgQImagePlugin

#endif // DIGIKAM_QIMAGE_DIMG_PLUGIN_H
