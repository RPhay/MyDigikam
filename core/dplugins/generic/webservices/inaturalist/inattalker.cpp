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

#include "inattalker.h"

// Qt includes

#include <QByteArray>
#include <QMap>
#include <QUrlQuery>
#include <QHttpMultiPart>
#include <QStringList>
#include <QMessageBox>
#include <QApplication>
#include <QProgressDialog>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "wstoolutils.h"
#include "inatutils.h"
#include "digikam_debug.h"
#include "digikam_config.h"
#include "digikam_version.h"
#include "previewloadthread.h"
#include "inatbrowserdlg.h"

// OAuth2 library includes

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic push
#   pragma clang diagnostic ignored "-Wextra-semi"
#endif

#include "o0globals.h"
#include "o0settingsstore.h"

#if defined(Q_CC_CLANG)
#   pragma clang diagnostic pop
#endif

namespace DigikamGenericINatPlugin
{

static const char COOKIE_SEPARATOR          = '\n';

enum
{
    INAT_API_TOKEN_EXPIRATION               = 86000, ///< api tokens are valid for 24 hours
    GEOLOCATION_PRECISION                   =     8, ///< # digits after decimal point
    RADIUS_PRECISION                        =     6, ///< # digits after decimal point
    EARTH_RADIUS_KM                         =  6371  ///< Earth radius in kilometers
};

/**
 * Fields in server responses received from iNaturalist.
 */
static const QString API_TOKEN              = QLatin1String("api_token");
static const QString TOTAL_RESULTS          = QLatin1String("total_results");
static const QString PER_PAGE               = QLatin1String("per_page");
static const QString RESULTS                = QLatin1String("results");
static const QString NAME                   = QLatin1String("name");
static const QString TAXON                  = QLatin1String("taxon");
static const QString ID                     = QLatin1String("id");
static const QString PARENT_ID              = QLatin1String("parent_id");
static const QString RANK                   = QLatin1String("rank");
static const QString RANK_LEVEL             = QLatin1String("rank_level");
static const QString PREFERRED_COMMON_NAME  = QLatin1String("preferred_common_name");
static const QString ENGLISH_COMMON_NAME    = QLatin1String("english_common_name");
static const QString MATCHED_TERM           = QLatin1String("matched_term");
static const QString DEFAULT_PHOTO          = QLatin1String("default_photo");
static const QString SQUARE_URL             = QLatin1String("square_url");
static const QString ANCESTORS              = QLatin1String("ancestors");
static const QString OBSCURED               = QLatin1String("obscured");
static const QString GEOJSON                = QLatin1String("geojson");
static const QString COORDINATES            = QLatin1String("coordinates");
static const QString LOGIN                  = QLatin1String("login");
static const QString ICON                   = QLatin1String("icon");

static QJsonObject parseJsonResponse(const QByteArray& data)
{
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(data, &err);

    if (err.error != QJsonParseError::NoError)
    {
        qCWarning(DIGIKAM_WEBSERVICES_LOG) << "parseJsonResponse: Failed to parse json response:"
                   << err.errorString();

        return QJsonObject();
    }

    if (!doc.isObject())
    {
        qCWarning(DIGIKAM_WEBSERVICES_LOG) << "parseJsonResponse: Json response is not an object!";

        return QJsonObject();
    }

    return doc.object();
}

static Taxon parseTaxon(const QJsonObject& taxon)
{
    int id        = -1;
    int parentId  = -1;
    int rankLevel = -1;
    QString name;
    QString rank;
    QString commonName;
    QString matchedTerm;
    QUrl squareUrl;
    QList<Taxon> ancestors;

    if (taxon.contains(NAME))
    {
        name = taxon[NAME].toString();
    }

    if (taxon.contains(ID))
    {
        id = taxon[ID].toInt();
    }

    if (taxon.contains(PARENT_ID))
    {
        parentId = taxon[PARENT_ID].toInt();
    }

    if (taxon.contains(RANK))
    {
        rank = taxon[RANK].toString();
    }

    if (taxon.contains(RANK_LEVEL))
    {
        rankLevel = taxon[RANK_LEVEL].toInt();
    }

    if      (taxon.contains(PREFERRED_COMMON_NAME))
    {
        commonName = taxon[PREFERRED_COMMON_NAME].toString();
    }
    else if (isEnglish && taxon.contains(ENGLISH_COMMON_NAME))
    {
        commonName = taxon[ENGLISH_COMMON_NAME].toString();
    }

    if (taxon.contains(MATCHED_TERM))
    {
        matchedTerm = taxon[MATCHED_TERM].toString();
    }

    if (taxon.contains(DEFAULT_PHOTO) &&
        taxon[DEFAULT_PHOTO].toObject().contains(SQUARE_URL))
    {
        squareUrl = QUrl(taxon[DEFAULT_PHOTO].toObject()
                         [SQUARE_URL].toString());
    }

    if (taxon.contains(ANCESTORS))
    {
        for (auto ancestorTaxon : taxon[ANCESTORS].toArray())
        {
            ancestors << parseTaxon(ancestorTaxon.toObject());
        }
    }

    return Taxon(id, parentId, name, rank, rankLevel, commonName,
                 matchedTerm, squareUrl, ancestors);
}

/**
 * A request consists in state and a function that is called with the response.
 */
class Request
{
public:

    Request()
    {
    }

    virtual ~Request()
    {
    }

    virtual void reportError(INatTalker&, int, const QString& errorString) const
    {
        QMessageBox::critical(QApplication::activeWindow(),
                              i18n("Error"), errorString);
    }

    virtual void parseResponse(INatTalker& talker,
                               const QByteArray& data) const = 0;
};

// --------------------------------------------------------------------------------

class Q_DECL_HIDDEN INatTalker::Private
{
public:

    explicit Private()
        : parent          (nullptr),
          netMngr         (nullptr),
          settings        (nullptr),
          iface           (nullptr),
          store           (nullptr),
          apiTokenExpires (0)
    {
        QString cryptId(QLatin1String("119b0b8a57644341fe03eca486a341"));

        apiUrl     = QLatin1String("https://api.inaturalist.org/v1/");
        keyToken   = QString(QLatin1String(O2_KEY_TOKEN)).arg(cryptId);
        keyExpires = QString(QLatin1String(O2_KEY_EXPIRES)).arg(cryptId);
        keyCookies = QString(QLatin1String("cookies.%1")).arg(cryptId);
    }

    void clear()
    {
        apiTokenExpires = 0;
        apiToken        = QString();
    }

    ~Private()
    {
        delete netMngr;
        delete store;
    }

    QWidget*                          parent;
    QNetworkAccessManager*            netMngr;
    QSettings*                        settings;
    DInfoInterface*                   iface;
    O0SettingsStore*                  store;

    QString                           serviceName;
    QString                           apiUrl;

    /// keys used in O0SettingsStore
    QString                           keyToken;
    QString                           keyExpires;
    QString                           keyCookies;

    /// the api token and its expiration time in seconds since January 1st, 1970
    QString                           apiToken;
    uint                              apiTokenExpires;

    /// this hash table allows us to serve multiple requests concurrently
    QHash<QNetworkReply*, Request*>   pendingRequests;

    /// cached api call results
    QHash<QString, AutoCompletions>   cachedAutoCompletions;
    QHash<QUrl, QByteArray>           cachedLoadUrls;
    QHash<QString, ImageScores>       cachedImageScores;
    QHash<QString, QStringList>       cachedNearbyPlaces;
    QHash<QString, NearbyObservation> cachedNearbyObservations;
};

INatTalker::INatTalker(QWidget* const parent, const QString& serviceName,
                       DInfoInterface* const iface)
    : d(new Private)
{
    d->parent          = parent;
    d->serviceName     = serviceName;
    d->iface           = iface;
    m_authProgressDlg  = nullptr;

    d->netMngr         = new QNetworkAccessManager(this);

    connect(d->netMngr, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(slotFinished(QNetworkReply*)));

    d->settings = WSToolUtils::getOauthSettings(this);
    d->store    = new O0SettingsStore(d->settings,
                                      QLatin1String(O2_ENCRYPTION_KEY), this);
    d->store->setGroupKey(d->serviceName);
}

INatTalker::~INatTalker()
{
    d->clear();
    WSToolUtils::removeTemporaryDir(d->serviceName.toLatin1().constData());

    delete d;
}

/**
 * Try to restore a valid API token; they are good for 24 hours.
 */
bool INatTalker::restoreApiToken(const QString& username,
                                 QList<QNetworkCookie>& cookies,
                                 bool emitSignal)
{
    cookies.clear();

    if (username.isEmpty())
    {
        return false;
    }

    d->store->setGroupKey(d->serviceName + username);
    d->apiToken        = d->store->value(d->keyToken);
    d->apiTokenExpires = d->store->value(d->keyExpires,
                                         QString::number(0)).toInt();
    QString cookiesStr = d->store->value(d->keyCookies);

    if (!cookiesStr.isEmpty())
    {
        QDateTime now(QDateTime::currentDateTime());

        for (auto str : cookiesStr.split(QLatin1Char(COOKIE_SEPARATOR)))
        {
            QList<QNetworkCookie> lst(QNetworkCookie::parseCookies(str.toUtf8()));

            Q_ASSERT(lst.count() == 1);

            for (auto cookie : lst)
            {
                if (INatBrowserDlg::filterCookie(cookie, true, now))
                {
                    cookies << cookie;
                }
            }
        }
    }

    bool valid = emitSignal && apiTokenExpiresIn() > 0;

    if (valid)
    {
        userInfo(cookies);
    }

    return valid;
}

bool INatTalker::stillUploading() const
{
    return d->pendingRequests.count();
}

void INatTalker::unLink()
{
    d->clear();
}

void INatTalker::removeUserName(const QString& userName)
{
    if (userName.startsWith(d->serviceName))
    {
        d->settings->beginGroup(userName);
        d->settings->remove(QString());
        d->settings->endGroup();
    }
}

int INatTalker::apiTokenExpiresIn() const
{
    if (d->apiToken.isEmpty())
    {
        return -1;
    }

    uint time = (uint)(QDateTime::currentMSecsSinceEpoch() / 1000);

    return ((d->apiTokenExpires <= time) ? -1 : (d->apiTokenExpires - time));
}

/**
 * We received an api token from a web brower login.
 */
void INatTalker::slotApiToken(const QString& apiToken,
                              const QList<QNetworkCookie>& cookies)
{
    d->apiToken = apiToken;

    if (apiToken.isEmpty())
    {
        emit signalLinkingFailed(QLatin1String("no api token"));
        return;
    }
    else
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "API token received; "
                                            "querying user info.";
        d->apiTokenExpires = (uint)(QDateTime::currentMSecsSinceEpoch() / 1000 +
                                    INAT_API_TOKEN_EXPIRATION);
        userInfo(cookies);
    }
}

/**
 * Get login, name, and icon of authorized user.
 */
class UserRequest : public Request
{
public:

    UserRequest(const QList<QNetworkCookie>& cookies)
        : m_cookies(cookies)
    {
    }

    void parseResponse(INatTalker& talker,
                       const QByteArray& data) const override
    {
        QJsonObject json = parseJsonResponse(data);

        if (json.contains(RESULTS) && (json[RESULTS].toArray().count() == 1))
        {
            QJsonObject result = json[RESULTS].toArray()[0].toObject();
            QString username(result[LOGIN].toString());
            emit talker.signalLinkingSucceeded(username,
                                               result[NAME].toString(),
                                               QUrl(result[ICON].toString()));
            talker.d->store->setGroupKey(talker.d->serviceName + username);

            // save api token

            talker.d->store->setValue(talker.d->keyToken, talker.d->apiToken);

            // save api token expiration

            talker.d->store->setValue(talker.d->keyExpires, QString::number(talker.d->apiTokenExpires));
            // save cookies

            QDateTime now(QDateTime::currentDateTime());
            QByteArray saveCookies;

            for (auto cookie : m_cookies)
            {
                if (!INatBrowserDlg::filterCookie(cookie, true, now))
                {
                    continue;
                }

                if (!saveCookies.isEmpty())
                {
                    saveCookies += COOKIE_SEPARATOR;
                }

                saveCookies += cookie.toRawForm();
            }

            talker.d->store->setValue(talker.d->keyCookies,
                                      QString::fromUtf8(saveCookies));
        }
        else
        {
            emit talker.signalLinkingFailed(QLatin1String("user-info request "
                                            "failed"));
        }

        if (talker.m_authProgressDlg)
        {
            talker.m_authProgressDlg->setValue(2);
            talker.m_authProgressDlg->hide();
        }

        emit talker.signalBusy(false);
    }

private:

    QList<QNetworkCookie> m_cookies;
};

void INatTalker::userInfo(const QList<QNetworkCookie>& cookies)
{
    if (d->apiToken.isEmpty())
    {
        return;
    }

    emit signalBusy(true);

    if (m_authProgressDlg)
    {
        m_authProgressDlg->
            setLabelText(QLatin1String("<font color=\"#74ac00\">") +
                         i18n("iNaturalist") + QLatin1String("</font> ") +
                         i18n("Login"));
        m_authProgressDlg->setMaximum(2);
        m_authProgressDlg->setValue(1);
        m_authProgressDlg->show();
    }

    QUrl url(d->apiUrl + QLatin1String("users/me"));
    QNetworkRequest netRequest(url);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                         QLatin1String(O2_MIME_TYPE_JSON));
    netRequest.setRawHeader("Authorization", d->apiToken.toLatin1());
    d->pendingRequests.insert(d->netMngr->get(netRequest),
                              new UserRequest(cookies));
}

/**
 * Load a URL and return result as QByteArry; used for images (icons).
 */
class LoadUrlRequest : public Request
{
public:

    explicit LoadUrlRequest(const QUrl& url)
        : m_url(url)
    {
    }

    void reportError(INatTalker& talker, int error,
                     const QString& errorString) const override
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Url" << m_url << "error:"
                                         << errorString;
        Request::reportError(talker, error, errorString);
    }

    void parseResponse(INatTalker& talker,
                       const QByteArray& data) const override
    {
        talker.d->cachedLoadUrls.insert(m_url, data);
        emit talker.signalLoadUrlSucceeded(m_url, data);
    }

private:

    QUrl m_url;
};

void INatTalker::loadUrl(const QUrl& imgUrl)
{
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Requesting url" << imgUrl.url();

    if (imgUrl.isEmpty() || imgUrl.isLocalFile() || imgUrl.isRelative())
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Ignoring; NOT loading url"
                                         << imgUrl;
        return;
    }

    if (d->cachedLoadUrls.contains(imgUrl))
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Url" << imgUrl << "found in cache";
        emit signalLoadUrlSucceeded(imgUrl, d->cachedLoadUrls.value(imgUrl));

        return;
    }

    QNetworkRequest netRequest(imgUrl);
    d->pendingRequests.insert(d->netMngr->get(netRequest),
                              new LoadUrlRequest(imgUrl));
}

/**
 *  taxon auto-completion
 */
class AutoCompletionRequest : public Request
{
public:

    explicit AutoCompletionRequest(const QString& name)
        : m_partialName(name)
    {
    }

    void parseResponse(INatTalker& talker,
                       const QByteArray& data) const override
    {
        QJsonObject json = parseJsonResponse(data);

        if (json.contains(RESULTS))
        {
            QJsonArray results = json[RESULTS].toArray();
            QList<Taxon> taxa;

            for (auto result : results)
            {
                taxa << parseTaxon(result.toObject());
            }

            QPair<QString, QList<Taxon> > result(m_partialName, taxa);
            talker.d->cachedAutoCompletions.insert(m_partialName, result);

            emit talker.signalTaxonAutoCompletions(result);
        }
    }

private:

    QString m_partialName;
};

void INatTalker::taxonAutoCompletions(const QString& partialName)
{
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Requesting taxon auto-completions for"
                                     <<  partialName;

    if (d->cachedAutoCompletions.contains(partialName))
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Taxon auto-completions for"
                                         <<  partialName << "found in cache.";
        emit signalTaxonAutoCompletions(d->cachedAutoCompletions.
                                        value(partialName));
        return;
    }

    QUrl url(d->apiUrl + QLatin1String("taxa/autocomplete"));
    QUrlQuery query;
    query.addQueryItem(QLatin1String("q"), partialName);
    query.addQueryItem(QLatin1String("is_active"), QLatin1String("true"));
    query.addQueryItem(QLatin1String("per_page"), QString::number(12));
    query.addQueryItem(QLatin1String("locale"), locale.name());
    url.setQuery(query.query());

    QNetworkRequest netRequest(url);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                         QLatin1String(O2_MIME_TYPE_JSON));

    d->pendingRequests.insert(d->netMngr->get(netRequest),
                              new AutoCompletionRequest(partialName));
}

/**
 * get nearby places
 */
class NearbyPlacesRequest : public Request
{

public:

    NearbyPlacesRequest(double latitude, double longitude, const QString& query)
        : m_latitude(latitude),
          m_longitude(longitude),
          m_query(query)
    {
    }

    void parseResponse(INatTalker& talker,
                       const QByteArray& data) const override
    {
        QJsonObject json = parseJsonResponse(data);

        if (json.contains(RESULTS))
        {
            static const QString BBOX_AREA = QLatin1String("bbox_area");
            QJsonObject results            = json[RESULTS].toObject();
            QList<Place> places;

            for (auto key : results.keys())
            {
                for (auto placeValue : results.value(key).toArray())
                {
                    QJsonObject place = placeValue.toObject();
                    places.push_front(Place(place[NAME].toString(),
                                            place[BBOX_AREA].toDouble()));
                }
            }

            std::sort(places.begin(), places.end());
            QStringList placesStrList;

            for (auto place : places)
            {
                placesStrList << place.m_name;
            }

            talker.d->cachedNearbyPlaces.insert(m_query, placesStrList);
            emit talker.signalNearbyPlaces(placesStrList);
        }
    }

private:

    struct Place
    {
        QString m_name;
        double  m_bboxArea;

        Place()
            : m_bboxArea(0.0)
        {
        }

        Place(const QString& n, double ba)
            : m_name(n),
              m_bboxArea(ba)
        {
        }

        bool operator <(const Place& other) const
        {
            return (m_bboxArea < other.m_bboxArea);
        }
    };

private:

    double  m_latitude;
    double  m_longitude;
    QString m_query;
};

void INatTalker::nearbyPlaces(double latitude, double longitude)
{
    QUrl url(d->apiUrl + QLatin1String("places/nearby"));

    QString lat = QString::number(latitude,  'f', GEOLOCATION_PRECISION);
    QString lng = QString::number(longitude, 'f', GEOLOCATION_PRECISION);
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Requesting nearby places for lat"
                                     << lat << "lon" << lng;
    QUrlQuery query;
    query.addQueryItem(QLatin1String("nelat"), lat);
    query.addQueryItem(QLatin1String("nelng"), lng);
    query.addQueryItem(QLatin1String("swlat"), lat);
    query.addQueryItem(QLatin1String("swlng"), lng);
    query.addQueryItem(QLatin1String("per_page"), QString::number(100));
    url.setQuery(query.query());

    if (d->cachedNearbyPlaces.contains(query.query()))
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Nearby places for lat" << lat
                                         << "lon" << lng << "found in cache.";

        emit signalNearbyPlaces(d->cachedNearbyPlaces.value(query.query()));

        return;
    }

    QNetworkRequest netRequest(url);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                         QLatin1String(O2_MIME_TYPE_JSON));

    d->pendingRequests.insert(d->netMngr->get(netRequest),
                              new NearbyPlacesRequest(latitude, longitude, query.query()));
}

/**
 * Find the closest observation; used as a sanity check for identifications:
 * when the closest known observation is hundreds of kilometers away, we
 * have likely misidentified the organism in our photo.
 */
class NearbyObservationRequest : public Request
{

public:

    NearbyObservationRequest(uint taxon, double latitude, double longitude,
                             double km, const QString& query)
        : m_taxon    (taxon),
          m_latitude (latitude),
          m_longitude(longitude),
          m_radiusKm (km),
          m_query    (query)
    {
    }

    void parseResponse(INatTalker& talker,
                       const QByteArray& data) const override
    {
        QJsonObject json = parseJsonResponse(data);

        if (json.contains(TOTAL_RESULTS))
        {
            static const double MAX_DISTANGE_KM = EARTH_RADIUS_KM * M_PI;
            int total_results                   = json[TOTAL_RESULTS].toInt();

            if (total_results == 0)
            {
                if (m_radiusKm < MAX_DISTANGE_KM)
                {
                    // Try again, double the radius.

                    talker.closestObservation(m_taxon, m_latitude, m_longitude,
                                              2 * m_radiusKm, m_query);
                }
                else
                {
                    // The entire Earth searched, no observation found.

                    talker.d->cachedNearbyObservations.insert(m_query,
                            INatTalker::NearbyObservation());

                    emit talker.signalNearbyObservation(INatTalker::
                                                        NearbyObservation());
                }
            }
            else
            {
                INatTalker::NearbyObservation closestObscured(
                    -1, 0.0, 0.0, MAX_DISTANGE_KM * 1000.0,
                    true, m_taxon, m_latitude, m_longitude);

                INatTalker::NearbyObservation closestOpen(
                    -1, 0.0, 0.0, MAX_DISTANGE_KM * 1000.0,
                    false, m_taxon, m_latitude, m_longitude);

                QJsonArray results = json[RESULTS].toArray();

                for (auto resultValue : results)
                {
                    QJsonObject result = resultValue.toObject();

                    if (!result.contains(GEOJSON))
                    {
                        continue;
                    }

                    int observationId     = result[ID].toInt();
                    QJsonArray coordinates(result[GEOJSON].toObject()
                                           [COORDINATES].toArray());
                    double latitude       = coordinates[1].toDouble();
                    double longitude      = coordinates[0].toDouble();
                    bool obscured         = result[OBSCURED].toBool();
                    double distanceMeters = distanceBetween(m_latitude,
                                                            m_longitude,
                                                            latitude,
                                                            longitude);

                    if (obscured)
                    {
                        if (distanceMeters < closestObscured.m_distanceMeters)
                        {
                            closestObscured.updateObservation(observationId,
                                                              latitude,
                                                              longitude,
                                                              distanceMeters);
                        }
                    }
                    else if (distanceMeters < closestOpen.m_distanceMeters)
                    {
                        closestOpen.updateObservation(observationId,
                                                      latitude, longitude,
                                                      distanceMeters);
                    }
                }

                double newDistanceMeters = closestOpen.isValid() ? closestOpen.m_distanceMeters
                                                                 : closestObscured.m_distanceMeters;

                if ((results.count() < total_results) && (newDistanceMeters > 10.0))
                {
                    // There are additional observations that we have not
                    // downloaded; request closer ones.

                    talker.closestObservation(m_taxon, m_latitude, m_longitude,
                                              newDistanceMeters / 999.0,
                                              m_query);
                }
                else
                {
                    talker.d->cachedNearbyObservations.insert(m_query,
                            closestOpen.isValid() ? closestOpen
                                                  : closestObscured);

                    emit talker.signalNearbyObservation(closestOpen.isValid()
                                                        ? closestOpen
                                                        : closestObscured);
                }
            }
        }
    }

private:

    uint    m_taxon;
    double  m_latitude;
    double  m_longitude;
    double  m_radiusKm;
    QString m_query;
};

void INatTalker::closestObservation(uint taxon, double latitude,
                                    double longitude, double radiusKm,
                                    const QString& origQuery)
{
    qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Requesting closest observation of"
                                     << taxon << "to" << latitude << longitude
                                     << "with radius" << radiusKm << "km.";
    QUrl url(d->apiUrl + QLatin1String("observations"));

    QUrlQuery query;
    query.addQueryItem(QLatin1String("geo"), QLatin1String("true"));
    query.addQueryItem(QLatin1String("taxon_id"), QString::number(taxon));
    query.addQueryItem(QLatin1String("lat"), QString::number(latitude, 'f',
                       GEOLOCATION_PRECISION));
    query.addQueryItem(QLatin1String("lng"), QString::number(longitude, 'f',
                       GEOLOCATION_PRECISION));
    query.addQueryItem(QLatin1String("radius"), QString::number(radiusKm, 'f',
                       RADIUS_PRECISION));
    query.addQueryItem(QLatin1String("quality_grade"),
                       QLatin1String("research"));
    query.addQueryItem(QLatin1String("locale"), locale.name());
    query.addQueryItem(QLatin1String("per_page"), QString::number(100));
    url.setQuery(query.query());

    if (d->cachedNearbyObservations.contains(query.query()))
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Closest observation of"
                                         << taxon << "at" << latitude
                                         << longitude << "with radius"
                                         << radiusKm << "km found in cache.";

        emit signalNearbyObservation(d->cachedNearbyObservations.value(
                                         query.query()));
        return;
    }

    QNetworkRequest netRequest(url);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                         QLatin1String(O2_MIME_TYPE_JSON));

    d->pendingRequests.insert(d->netMngr->get(netRequest),
                              new NearbyObservationRequest(taxon, latitude, longitude, radiusKm,
                                                           origQuery.isEmpty() ? query.query()
                                                                               : origQuery));
}


/**
 *  get taxon suggestions for an image
 */
class ComputerVisionRequest : public Request
{

public:

    ComputerVisionRequest(const QString& imgPath, const QString& tmpFile)
        : m_imagePath(imgPath), m_tmpFilePath(tmpFile)
    {
    }

    ~ComputerVisionRequest()
    {
        if (!m_tmpFilePath.isEmpty() && QFile::exists(m_tmpFilePath))
        {
            QFile::remove(m_tmpFilePath);
        }
    }

    void parseScore(QJsonObject json, QList<ComputerVisionScore>& scores) const
    {
        static const QString FREQUENCY_SCORE = QLatin1String("frequency_score");
        static const QString VISION_SCORE    = QLatin1String("vision_score");
        static const QString COMBINED_SCORE  = QLatin1String("combined_score");

        double frequency_score = 0.0;
        double vision_score    = 0.0;
        double combined_score  = 0.0;
        Taxon  taxon;

        if (json.contains(FREQUENCY_SCORE))
        {
            frequency_score = json[FREQUENCY_SCORE].toDouble();
        }

        if (json.contains(VISION_SCORE))
        {
            vision_score = json[VISION_SCORE].toDouble();
        }

        if (json.contains(COMBINED_SCORE))
        {
            combined_score = json[COMBINED_SCORE].toDouble();
        }

        if (json.contains(TAXON))
        {
            taxon = parseTaxon(json[TAXON].toObject());
        }

        scores << ComputerVisionScore(frequency_score, vision_score,
                                      combined_score, taxon);
    }

    void parseResponse(INatTalker& talker,
                       const QByteArray& data) const override
    {
        static const QString COMMON_ANCESTOR = QLatin1String("common_ancestor");

        QJsonObject json = parseJsonResponse(data);
        QList<ComputerVisionScore> scores;

        if (json.contains(COMMON_ANCESTOR))
        {
            parseScore(json[COMMON_ANCESTOR].toObject(), scores);
        }

        if (json.contains(RESULTS))
        {
            for (auto result : json[RESULTS].toArray())
            {
                parseScore(result.toObject(), scores);
            }
        }

        ImageScores result(m_imagePath, scores);
        talker.d->cachedImageScores.insert(m_imagePath, result);
        emit talker.signalComputerVisionResults(result);
    }

private:

    QString m_imagePath;
    QString m_tmpFilePath;
};

void INatTalker::computerVision(const QUrl& localImage)
{
    if (localImage.isEmpty() || (apiTokenExpiresIn() <= 0))
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Computer-vision API not called:"
                                         << (localImage.isEmpty() ? "No image."
                                                                  : "Not logged in.");
        return;
    }

    enum
    {
        HEIGHT = 299,
        WIDTH  = 299
    };

    QString path = localImage.path();

    if (d->cachedImageScores.contains(path))
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Vision identification for"
                                         << localImage.path()
                                         << "found in cache.";
        emit signalComputerVisionResults(d->cachedImageScores.value(path));

        return;
    }

    QImage image = PreviewLoadThread::loadHighQualitySynchronously(path).
                   copyQImage();

    if (image.isNull())
    {
        image.load(path);
    }

    path  = tmpFileName(path);
    image = image.scaled(WIDTH, HEIGHT);
    image.save(path, "JPEG");

    QList<Parameter> parameters;

    DItemInfo info(d->iface->itemInfo(localImage));

    if (info.hasGeolocationInfo())
    {
        static const QString lat = QLatin1String("lat");
        parameters << Parameter(lat, QString::number(info.latitude(), 'f',
                                GEOLOCATION_PRECISION));
        static const QString lng = QLatin1String("lng");
        parameters << Parameter(lng, QString::number(info.longitude(), 'f',
                                GEOLOCATION_PRECISION));
    }

    QDateTime dateTime = info.dateTime();

    if (dateTime.isValid())
    {
        static const QString observed_on = QLatin1String("observed_on");
        parameters << Parameter(observed_on,
                                dateTime.date().toString(Qt::ISODate));
    }

    parameters << Parameter(QLatin1String("locale"), locale.name());

    QHttpMultiPart* const multiPart = getMultiPart(parameters, QLatin1String("image"), path);

    QUrl url(d->apiUrl + QLatin1String("computervision/score_image"));
    QNetworkRequest netRequest(url);
    netRequest.setRawHeader("Authorization", d->apiToken.toLatin1());

    QNetworkReply* const reply = d->netMngr->post(netRequest, multiPart);
    multiPart->setParent(reply);
    d->pendingRequests.insert(reply, new ComputerVisionRequest(localImage.path(), path));
}

QString INatTalker::tmpFileName(const QString& path)
{
    QString suffix(QLatin1String(""));

    for ( ; ; )
    {
        QString tmpFn = WSToolUtils::makeTemporaryDir(d->serviceName.toLatin1().constData()).
            filePath(QFileInfo(path).baseName().trimmed() + suffix + QLatin1String(".jpg"));

        if (!QFile::exists(tmpFn))
        {
            return tmpFn;
        }

        suffix += QLatin1String("_");
    }
}

class CreateObservationRequest : public Request
{
public:

    CreateObservationRequest(const INatTalker::PhotoUploadRequest& req)
        : m_uploadRequest(req)
    {
    }

    void parseResponse(INatTalker& talker,
                       const QByteArray& data) const override
    {
        QJsonObject json = parseJsonResponse(data);

        if (json.contains(ID))
        {
            INatTalker::PhotoUploadRequest request(m_uploadRequest);
            request.m_observationId = json[ID].toInt();

            emit talker.signalObservationCreated(request);
        }
    }

private:

    INatTalker::PhotoUploadRequest m_uploadRequest;
};

void INatTalker::createObservation(const QByteArray& parameters,
                                   const PhotoUploadRequest& photoUpload)
{
    QUrl url(d->apiUrl + QLatin1String("observations"));

    QNetworkRequest netRequest(url);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                         QLatin1String(O2_MIME_TYPE_JSON));
    netRequest.setRawHeader("Authorization", d->apiToken.toLatin1());

    PhotoUploadRequest upload(photoUpload);
    upload.m_apiKey = d->apiToken;
    d->pendingRequests.insert(d->netMngr->post(netRequest, parameters),
                              new CreateObservationRequest(upload));
}

class UploadPhotoRequest : public Request
{
public:

    UploadPhotoRequest(const INatTalker::PhotoUploadRequest& req,
                       const QString& tmpImg)
        : m_request (req),
          m_tmpImage(tmpImg)
    {
    }

    ~UploadPhotoRequest()
    {
        if (!m_tmpImage.isEmpty() && QFile::exists(m_tmpImage))
        {
            QFile::remove(m_tmpImage);
        }
    }

    void parseResponse(INatTalker& talker,
                       const QByteArray& data) const override
    {
        static const QString PHOTO_ID = QLatin1String("photo_id");
        QJsonObject json              = parseJsonResponse(data);

        if (json.contains(PHOTO_ID))
        {
            INatTalker::PhotoUploadResult result(m_request, json[ID].toInt(),
                                                 json[PHOTO_ID].toInt());
            emit talker.signalPhotoUploaded(result);
        }
    }

private:

    INatTalker::PhotoUploadRequest m_request;
    QString                        m_tmpImage;
};

void INatTalker::uploadNextPhoto(const PhotoUploadRequest& request)
{
    QList<Parameter> parameters;
    parameters << Parameter(QLatin1String("observation_photo[observation_id]"),
                            QString::number(request.m_observationId));
    QString tmpImage;
    QString path = request.m_images.front().path();

    if (request.m_rescale)
    {
        QImage image = PreviewLoadThread::loadHighQualitySynchronously(path).
                       copyQImage();

        if (image.isNull())
        {
            image.load(path);
        }

        if (!image.isNull())
        {
            tmpImage = tmpFileName(path);

            if ((image.width()  > request.m_maxDim) ||
                (image.height() > request.m_maxDim))
            {
                image = image.scaled(request.m_maxDim, request.m_maxDim,
                                     Qt::KeepAspectRatio,
                                     Qt::SmoothTransformation);
            }

            image.save(tmpImage, "JPEG", request.m_quality);
            path = tmpImage;
        }
    }

    QHttpMultiPart* multiPart = getMultiPart(parameters, QLatin1String("file"),
                                path);
    QUrl url(d->apiUrl + QLatin1String("observation_photos"));
    QNetworkRequest netRequest(url);
    netRequest.setRawHeader("Authorization", request.m_apiKey.toLatin1());
    QNetworkReply* const reply = d->netMngr->post(netRequest, multiPart);
    multiPart->setParent(reply);
    d->pendingRequests.insert(reply, new UploadPhotoRequest(request, tmpImage));
}


class DeleteObservationRequest : public Request
{
public:

    DeleteObservationRequest(int id)
        : m_observationId(id)
    {
    }

    void parseResponse(INatTalker& talker, const QByteArray&) const override
    {
        emit talker.signalObservationDeleted(m_observationId);
    }

private:

    int m_observationId;
};

/**
 * Delete an observation; called when canceling uploads.
 */
void INatTalker::deleteObservation(int id, const QString& apiKey)
{
    QUrl url(d->apiUrl + QLatin1String("observations/") + QString::number(id));
    QNetworkRequest netRequest(url);
    netRequest.setHeader(QNetworkRequest::ContentTypeHeader,
                         QLatin1String(O2_MIME_TYPE_JSON));
    netRequest.setRawHeader("Authorization", apiKey.toLatin1());
    d->pendingRequests.insert(d->netMngr->deleteResource(netRequest),
                              new DeleteObservationRequest(id));
}

void INatTalker::cancel()
{
    if (m_authProgressDlg && !m_authProgressDlg->isHidden())
    {
        m_authProgressDlg->hide();
    }

    d->clear();
    emit signalBusy(false);
}

void INatTalker::slotFinished(QNetworkReply* reply)
{
    // ignore unexpected response

    if (!d->pendingRequests.contains(reply))
    {
        qCDebug(DIGIKAM_WEBSERVICES_LOG) << "Ignoring unexpected NetworkReply.";
        reply->deleteLater();
        return;
    }

    Request* const request = d->pendingRequests.take(reply);

    if (reply->error() == QNetworkReply::NoError)
    {
        request->parseResponse(*this, reply->readAll());
    }
    else
    {
        request->reportError(*this, reply->error(), reply->errorString());
    }

    delete request;
    reply->deleteLater();
}

} // namespace DigikamGenericINatPlugin
