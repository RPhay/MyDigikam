/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2019-09-19
 * Description : digiKam plugin definition for DImg image loader.
 *
 * Copyright (C) 2019 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DPLUGIN_DIMG_H
#define DIGIKAM_DPLUGIN_DIMG_H

// Local includes

#include "dplugin.h"
#include "dimgloader.h"
#include "dpluginloader.h"
#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DPluginDImg : public DPlugin
{
    Q_OBJECT

public:

    /**
     * Constructor with optional parent object
     */
    explicit DPluginDImg(QObject* const parent = nullptr);

    /**
     * Destructor
     */
    ~DPluginDImg() override;

public:

    /** This kind of plugin only provide one tool.
     */
    int count() const override { return 1; };

    /** This kind of plugin do not use a category.
     */
    QStringList categories() const override { return QStringList(); };

    /** This kind of plugin do not have GUI visibility attribute.
     */
    void setVisible(bool) override {};

    /**
     * Return the plugin interface identifier.
     */
    QString ifaceIid() const override { return QLatin1String(DIGIKAM_DPLUGIN_DIMG_IID); };

public:

    /** Return the image loader instance.
     */
    virtual DImgLoader* loader() const = 0;

    /** Return the list of type-mimes supported by the loader,
     *  as a string of file-name suffix separated by spaces.
     *  Ex: "jpeg jpg thm"
     */
    virtual QString typeMimes() const = 0;

    /** Return a single capitalized word to identify the format supported by the loader.
     *  Ex: jpeg => "JPG" ; tiff => "TIF", etc.
     */
    virtual QString format() const = 0;

    /** Return true if bytes from header start with magick number from image format
     *  support by the loader.
     */
    virtual bool checkMagickNumber(const QByteArray& header) const = 0;
};

} // namespace Digikam

Q_DECLARE_INTERFACE(Digikam::DPluginDImg, DIGIKAM_DPLUGIN_DIMG_IID)

#endif // DIGIKAM_DPLUGIN_DIMG_H
