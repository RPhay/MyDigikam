/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-03-20
 * Description : a tool to export images to iNaturalist web service
 *
 * Copyright (C) 2021      by Joerg Lohse <joergmlpts at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_INAT_WIDGET_H
#define DIGIKAM_INAT_WIDGET_H

// Qt includes

#include <QWidget>
#include <QTabWidget>
#include <QLineEdit>
#include <QComboBox>

// Local includes

#include "wscomboboxintermediate.h"
#include "ditemslist.h"
#include "wssettingswidget.h"
#include "dinfointerface.h"

using namespace Digikam;

namespace DigikamGenericINatPlugin
{

class INatWidget : public WSSettingsWidget
{
    Q_OBJECT

public:

    explicit INatWidget(QWidget* const parent,
                        DInfoInterface* const iface,
                        const QString& serviceName);
    ~INatWidget()                                       override;

    void updateLabels(const QString& name,
                      const QString& url = QString())   override;

private:

    class Private;
    Private* const d;

    friend class INatWindow;
};

} // namespace DigikamGenericINatPlugin

#endif // DIGIKAM_INAT_WIDGET_H
