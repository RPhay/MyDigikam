// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#ifndef MARBLE_MARBLEPLUGINSETTINGSWIDGET_H
#define MARBLE_MARBLEPLUGINSETTINGSWIDGET_H

/** @file
 * This file contains the multiple inheritance ui-wrapper for the
 * MarblePluginSettingsWidget ui file.
 *
 * @author Torsten Rahn  <torsten@kde.org>
 */

#include <QWidget>

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
