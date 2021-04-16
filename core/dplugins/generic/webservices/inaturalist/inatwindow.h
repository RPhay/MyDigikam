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

#ifndef DIGIKAM_INAT_WINDOW_H
#define DIGIKAM_INAT_WINDOW_H

// Qt includes

#include <QUrl>
#include <QHash>
#include <QList>
#include <QPair>
#include <QLabel>
#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QTreeWidgetItem>

// Local includes

#include "wscomboboxintermediate.h"
#include "dinfointerface.h"
#include "wstooldialog.h"
#include "inattaxon.h"
#include "inattalker.h"

using namespace Digikam;

namespace DigikamGenericINatPlugin
{

class INatWindow : public WSToolDialog
{
    Q_OBJECT

public:

    explicit INatWindow(DInfoInterface* const iface,
                        QWidget* const parent,
                        const QString& serviceName = QLatin1String("iNaturalist"));
    ~INatWindow()                                                   override;

    /**
     * Use this method to (re-)activate the dialog after it has been created
     * to display it. This also loads the currently selected images.
     */
    void reactivate();

    void setItemsList(const QList<QUrl>& urls);

private Q_SLOTS:

    void slotLinkingSucceeded(const QString& login, const QString& name,
                              const QUrl& iconUrl);
    void slotLinkingFailed(const QString& error);
    void slotLoadUrlSucceeded(const QUrl&, const QByteArray&);
    void slotApiTokenExpires();
    void slotBusy(bool val);
    void slotError(const QString& msg);
    void slotFinished();
    void slotUser1();
    void slotCancelClicked();

    void slotTaxonSelected(const Taxon&, bool fromVision);
    void slotTaxonDeselected();
    void slotComputerVision();
    void slotObservationCreated(const INatTalker::PhotoUploadRequest&);
    void slotPhotoUploaded(const INatTalker::PhotoUploadResult&);
    void cancelUpload(const INatTalker::PhotoUploadRequest&);
    void slotObservationDeleted(int id);
    void slotUserChangeRequest();
    void slotRemoveAccount();
    void slotAuthCancel();
    void slotImageListChanged();
    void slotValueChanged(int);
    void slotClosestChanged(int);
    void slotMoreOptionsButton(bool);
    void slotNearbyPlaces(const QStringList&);
    void slotNearbyObservation(const INatTalker::NearbyObservation&);

private:

    void closeEvent(QCloseEvent*)                               override;
    void readSettings(const QString& uname);
    void writeSettings();

    void setUiInProgressState(bool inProgress);
    void updateProgressBarValue(unsigned);
    void updateProgressBarMaximum(unsigned);
    void switchUser(bool restoreToken = true);

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericINatPlugin

#endif // DIGIKAM_INAT_WINDOW_H
