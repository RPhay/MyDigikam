/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-03-30
 * Description : abstract class to host DImg loader settings.
 *
 * Copyright (C) 2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_DIMG_LOADER_SETTINGS_H
#define DIGIKAM_DIMG_LOADER_SETTINGS_H

// Qt includes

#include <QWidget>
#include <QMap>
#include <QVariant>
#include <QStringList>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

/**
 * Map container of widget parameter name/value.
 */
typedef QMap<QString, QVariant> DImgLoaderPrms;

class DIGIKAM_EXPORT DImgLoaderSettings : public QWidget
{
    Q_OBJECT

public:

    explicit DImgLoaderSettings(QWidget* const parent = nullptr);
    ~DImgLoaderSettings() override;

    /**
     * Set the parameters values in the widget from DImgLoaderPrms map container.
     */
    virtual void setSettings(const DImgLoaderPrms& set) = 0;

    /**
     * Return the DImgLoaderPrms map container of parameters/values from the Widget.
     */
    virtual DImgLoaderPrms settings() const             = 0;

    /**
     * Return the list of supported parameter names.
     */
     QStringList parameters()         const;

Q_SIGNALS:

    /**
     * Signal to emit when a settings is changed from the widget.
     */
    void signalSettingsChanged();
};

} // namespace Digikam

#endif // DIGIKAM_DIMG_LOADER_SETTINGS_H
