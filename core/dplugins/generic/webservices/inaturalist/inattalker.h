/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-03-20
 * Description : a tool to export images to iNaturalist web service
 *
 * Copyright (C) 2021      by Joerg Lohse <joergmlpts at gmail dot com>
 * Copyright (C) 2005-2009 by Vardhman Jain <vardhman at gmail dot com>
 * Copyright (C) 2009-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2017-2019 by Maik Qualmann <metzpinguin at gmail dot com>
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

#ifndef DIGIKAM_INAT_TALKER_H
#define DIGIKAM_INAT_TALKER_H

// Qt includes

#include <QUrl>
#include <QList>
#include <QPair>
#include <QString>
#include <QObject>
#include <QNetworkReply>
#include <QNetworkCookie>

// Local includes

#include "dinfointerface.h"
#include "ditemslist.h"
#include "inattaxon.h"
#include "inatscore.h"

class QProgressDialog;

using namespace Digikam;

namespace DigikamGenericINatPlugin
{

class INatPhotoInfo;
class INatPhotoSet;

class AuthRequest;
class ApiTokenRequest;

typedef QPair<QString, QList<Taxon> > AutoCompletions;

class INatTalker : public QObject
{
    Q_OBJECT

public:

    /**
     * This struct contains all the data needed for photo uploads.
     */
    struct PhotoUploadRequest
    {
        PhotoUploadRequest()
            : m_observationId(-1),
              m_updateIds    (false),
              m_rescale      (false),
              m_maxDim       (-1),
              m_quality      (-1)
        {
        }

        PhotoUploadRequest(const QList<QUrl>& imgs,
                           bool updId,
                           bool resize,
                           int mxDim,
                           int q)
            : m_observationId(-1),
              m_images       (imgs),
              m_updateIds    (updId),
              m_rescale      (resize),
              m_maxDim       (mxDim),
              m_quality      (q)
        {
        }

        int         m_observationId;
        QList<QUrl> m_images;
        QString     m_apiKey;
        bool        m_updateIds;
        bool        m_rescale;
        int         m_maxDim;
        int         m_quality;
    };

    /**
     * This struct is sent after each photo has been uploaded.
     */
    struct PhotoUploadResult
    {
        PhotoUploadResult()
            : m_observationPhotoId(-1),
              m_photoId           (-1)
        {
        }

        PhotoUploadResult(const PhotoUploadRequest& req, int obsPhId, int phId)
            : m_request           (req),
              m_observationPhotoId(obsPhId),
              m_photoId           (phId)
        {
        }

        PhotoUploadRequest m_request;
        int                m_observationPhotoId;
        int                m_photoId;
    };

    struct NearbyObservation
    {

        NearbyObservation()
            : m_observationId     (-1),
              m_latitude          (0.0),
              m_longitude         (0.0),
              m_distanceMeters    (-1.0),
              m_obscured          (false),
              m_referenceTaxon    (0),
              m_referenceLatitude (0.0),
              m_referenceLongitude(0.0)
        {
        }

        NearbyObservation(int id,
                          double latitude,
                          double longitude,
                          double distanceMeters,
                          bool obscured,
                          uint taxon,
                          double referenceLatitude,
                          double referenceLongitude)
            : m_observationId       (id),
              m_latitude            (latitude),
              m_longitude           (longitude),
              m_distanceMeters      (distanceMeters),
              m_obscured            (obscured),
              m_referenceTaxon      (taxon),
              m_referenceLatitude   (referenceLatitude),
              m_referenceLongitude  (referenceLongitude)
        {
        }

        void updateObservation(int id,
                               double latitude,
                               double longitude,
                               double distanceMeters)
        {
            m_observationId  = id;
            m_latitude       = latitude;
            m_longitude      = longitude;
            m_distanceMeters = distanceMeters;
        }

        bool isValid() const
        {
            return (m_observationId != -1);
        }

        int    m_observationId;
        double m_latitude;
        double m_longitude;
        double m_distanceMeters;
        bool   m_obscured;
        uint   m_referenceTaxon;
        double m_referenceLatitude;
        double m_referenceLongitude;
    };

    INatTalker(QWidget* const parent,
               const QString& serviceName,
               DInfoInterface* const iface);
    ~INatTalker() override;

    void    unLink();
    void    cancel();
    void    removeUserName(const QString& userName);

    /**
     * Are we still uploading observations or photos?
     */
    bool    stillUploading() const;

    /**
     * Returns -1 if there is no valid token; number of seconds otherwise.
     */
    int     apiTokenExpiresIn() const;

    /**
     * Try to restore a valid API token; they are good for 24 hours.
     */
    bool    restoreApiToken(const QString& username,
                            QList<QNetworkCookie>&,
                            bool emitSignal);

    /**
     * Download an image from iNaturalist servers.
     */
    void    loadUrl(const QUrl& url);

    /**
     * Obtain auto completions for taxa from iNaturalist servers.
     */
    void    taxonAutoCompletions(const QString& partialName);

    /**
     * Retrieve login, name, and icon of current user.
     */
    void    userInfo(const QList<QNetworkCookie>&);

    /**
     * Get list of nearby places.
     */
    void    nearbyPlaces(double latitude, double longitude);

    /**
     * Get closest nearby observation.
     */
    void    closestObservation(uint taxon, double latitude, double longitude,
                               double radius_km = 10.0,
                               const QString& origQuery = QString());

    /**
     * Identify taxa from a given photo.
     */
    void    computerVision(const QUrl& localImage);

    /**
     * Create an iNaturalist observation; photo uploads follow.
     */
    void    createObservation(const QByteArray&, const PhotoUploadRequest&);

    /**
     * Delete an observation; called when canceling uploads.
     */
    void    deleteObservation(int id, const QString& apiKey);

    /**
     * Upload another photo to previously created observation.
     */
    void    uploadNextPhoto(const PhotoUploadRequest&);

public:

    QProgressDialog* m_authProgressDlg;

Q_SIGNALS:

    void signalBusy(bool val);

    void signalLoadUrlSucceeded(const QUrl&, const QByteArray&);
    void signalTaxonAutoCompletions(const AutoCompletions&);
    void signalNearbyObservation(const INatTalker::NearbyObservation&);
    void signalComputerVisionResults(const ImageScores&);
    void signalNearbyPlaces(const QStringList&);
    void signalObservationCreated(const INatTalker::PhotoUploadRequest&);
    void signalObservationDeleted(int id);
    void signalPhotoUploaded(const INatTalker::PhotoUploadResult&);

    void signalLinkingSucceeded(const QString& login,
                                const QString& name, const QUrl& iconUrl);
    void signalLinkingFailed(const QString& error);

private:

    QString tmpFileName(const QString& path);

private Q_SLOTS:

    void slotApiToken(const QString&, const QList<QNetworkCookie>&);
    void slotFinished(QNetworkReply* reply);

private:

    class Private;
    Private* const d;

    friend class UserRequest;
    friend class LoadUrlRequest;
    friend class AutoCompletionRequest;
    friend class NearbyPlacesRequest;
    friend class NearbyObservationRequest;
    friend class ComputerVisionRequest;
};

} // namespace DigikamGenericINatPlugin

#endif // DIGIKAM_INAT_TALKER_H
