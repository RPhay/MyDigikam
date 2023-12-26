/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-12-21
 * Description : Marble Settings View.
 *
 * SPDX-FileCopyrightText: 2007      by Inge Wallin <ingwa at kde dot org>
 * SPDX-FileCopyrightText: 2008      by Jens-Michael Hoffmann <jensmh at gmx dot de>
 * SPDX-FileCopyrightText: 2022-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#ifndef MARBLE_MARBLEPLUGINSETTINGSWIDGET_H
#define MARBLE_MARBLEPLUGINSETTINGSWIDGET_H

// Qt includes

#include <QWidget>

// Local includes

#include "digikam_export.h"

class QModelIndex;

/**
 * @short A public class that adds methods to the UI Plugins Settings Widget.
 */
namespace Marble
{

class RenderPluginModel;

class DIGIKAM_EXPORT MarblePluginSettingsWidget : public QWidget
{
    Q_OBJECT

public:

    explicit MarblePluginSettingsWidget(QWidget* const parent = nullptr);
    ~MarblePluginSettingsWidget() override;

    void setModel(RenderPluginModel* const pluginModel);

    void setAboutIcon(const QIcon& icon);
    void setConfigIcon(const QIcon& icon);

Q_SIGNALS:

    void pluginListViewClicked();

private Q_SLOTS:

    void slotPluginAboutDialog(const QModelIndex&);
    void slotPluginConfigDialog(const QModelIndex&);

private:

    class Private;
    Private* const d;
};

} // namespace Marble

#endif
