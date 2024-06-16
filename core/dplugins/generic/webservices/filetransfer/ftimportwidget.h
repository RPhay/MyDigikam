/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 04-10-2009
 * Description : main widget of the import dialog
 *
 * SPDX-FileCopyrightText: 2009      by Johannes Wienke <languitar at semipol dot de>
 * SPDX-FileCopyrightText: 2011-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QUrl>
#include <QWidget>

// Local includes

#include "dinfointerface.h"
#include "ditemslist.h"

using namespace Digikam;

namespace DigikamGenericFileTransferPlugin
{

/**
 * Helper widget for the import tool.
 */
class FTImportWidget: public QWidget
{
    Q_OBJECT

public:

    /**
     * Constructor.
     *
     * @param parent the parent widget
     * @param interface application interface to use
     */
    explicit FTImportWidget(QWidget* const parent, DInfoInterface* const iface);

    /**
     * Destructor.
     */
    ~FTImportWidget() override;

    /**
     * Returns the current list of source urls to import contained in the
     * image list.
     *
     * @return list of urls, potentially from remote system
     */
    QList<QUrl> sourceUrls() const;

    /**
     * Returns the image list used to manage images to import.
     *
     * @return pointer to the image list
     */
    DItemsList* imagesList() const;

    /**
     * Returns the upload widget for specifying the target location.
     *
     * @return pointer to the widget
     */
    QWidget* uploadWidget() const;

private Q_SLOTS:

    void slotShowImportDialogClicked(bool);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace DigikamGenericFileTransferPlugin
