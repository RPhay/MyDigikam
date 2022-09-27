/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-02-02
 * Description : a tool to export items to ImageShack web service
 *
 * SPDX-FileCopyrightText: 2012      by Dodon Victor <dodonvictor at gmail dot com>
 * SPDX-FileCopyrightText: 2013-2018 by Caulier Gilles <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_IMAGE_SHACK_WIDGET_H
#define DIGIKAM_IMAGE_SHACK_WIDGET_H

// Qt includes

#include <QString>
#include <QWidget>

// Local includes

#include "dprogresswdg.h"
#include "wssettingswidget.h"
#include "dinfointerface.h"

using namespace Digikam;

namespace DigikamGenericImageShackPlugin
{

class ImageShackSession;

class ImageShackWidget : public WSSettingsWidget
{
    Q_OBJECT

public:

    explicit ImageShackWidget(QWidget* const parent,
                              ImageShackSession* const session,
                              DInfoInterface* const iface,
                              const QString& toolName);
    ~ImageShackWidget() override;

Q_SIGNALS:

    void signalReloadGalleries();

private:

    void updateLabels(const QString& name = QString(),
                      const QString& url = QString()) override;

private Q_SLOTS:

    void slotGetGalleries(const QStringList& gTexts, const QStringList& gNames);
    void slotReloadGalleries();

private:

    class Private;
    Private* const d;

    friend class ImageShackWindow;
};

} // namespace DigikamGenericImageShackPlugin

#endif // DIGIKAM_IMAGE_SHACK_WIDGET_H
