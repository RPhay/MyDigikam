/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-03-30
 * Description : abstract class to host DImg loader settings.
 *
 * SPDX-FileCopyrightText: 2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

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
