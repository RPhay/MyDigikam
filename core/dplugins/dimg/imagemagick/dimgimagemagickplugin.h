/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-21
 * Description : ImageMagick DImg plugin.
 *
 * Copyright (C) 2020 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DIMG_IMAGE_MAGICK_PLUGIN_H
#define DIGIKAM_DIMG_IMAGE_MAGICK_PLUGIN_H

// Qt includes

#include <QByteArray>
#include <QWidget>

// Local includes

#include "dplugindimg.h"
#include "dimg.h"

#define DPLUGIN_IID "org.kde.digikam.plugin.dimg.ImageMagick"

using namespace Digikam;

namespace DigikamImageMagickDImgPlugin
{

class DImgImageMagickPlugin : public DPluginDImg
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID DPLUGIN_IID)
    Q_INTERFACES(Digikam::DPluginDImg)

public:

    explicit DImgImageMagickPlugin(QObject* const parent = nullptr);
    ~DImgImageMagickPlugin() override;

    QString name()                                                                          const override;
    QString iid()                                                                           const override;
    QIcon   icon()                                                                          const override;
    QString details()                                                                       const override;
    QString description()                                                                   const override;
    QList<DPluginAuthor> authors()                                                          const override;

    void setup(QObject* const)                                                                    override;
    void cleanUp()                                                                                override;
    QMap<QString, QString> extraAboutData()                                                 const override;

    QString     loaderName()                                                                const override;
    QString     typeMimes()                                                                 const override;
    int         canRead(const QFileInfo& fileInfo, bool magic)                              const override;
    int         canWrite(const QString& format)                                             const override;
    DImgLoader* loader(DImg* const image, const DRawDecoding& rawSettings = DRawDecoding()) const override;

private:

    QStringList decoderFormats() const;
    QStringList encoderFormats() const;

private:

    QStringList m_readFormats;
    QStringList m_writeFormats;

};

} // namespace DigikamImageMagickDImgPlugin

#endif // DIGIKAM_DIMG_IMAGE_MAGICK_PLUGIN_H
