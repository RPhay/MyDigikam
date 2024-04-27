/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-12-01
 * Description : Widget for displaying HTML in the backends - QtWebKit version
 *
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Michael G. Hansen <mike at mghansen dot de>
 * SPDX-FileCopyrightText: 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "htmlwidget_qwebkit.h"

// Qt includes

#include <QTimer>
#include <QResizeEvent>
#include <QWebSettings>
#include <qwebframe.h>

// Local includes

#include "digikam_debug.h"

namespace Digikam
{

class Q_DECL_HIDDEN HTMLWidget::Private
{
public:

    Private() = default;

    QWidget*       parent           = nullptr;
    bool           isReady          = false;

    bool           selectionStatus  = false;
    GeoCoordinates firstSelectionPoint;
    GeoCoordinates intermediateSelectionPoint;
    QPoint         firstSelectionScreenPoint;
    QPoint         intermediateSelectionScreenPoint;
};

HTMLWidget::HTMLWidget(QWidget* const parent)
    : QWebView(parent),
      d       (new Private)
{
    d->parent = parent;
    setAcceptDrops(false);
    setFocusPolicy(Qt::WheelFocus);
    setRenderHint(QPainter::TextAntialiasing);
    page()->setLinkDelegationPolicy(QWebPage::DelegateAllLinks);

    settings()->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
    settings()->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
    settings()->setAttribute(QWebSettings::JavascriptCanOpenWindows, true);
    settings()->setAttribute(QWebSettings::LocalStorageEnabled, true);

    d->parent->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    connect(this, SIGNAL(loadProgress(int)),
            this, SLOT(progress(int)));

    connect(this, SIGNAL(loadFinished(bool)),
            this, SLOT(slotHTMLCompleted(bool)));

    connect(this, SIGNAL(statusBarMessage(QString)),
            this, SLOT(slotScanForJSMessages(QString)));

    if (d->parent)
    {
        d->parent->installEventFilter(this);
    }
}

HTMLWidget::~HTMLWidget()
{
    delete d;
}

void HTMLWidget::progress(int progress)
{
    qCDebug(DIGIKAM_GEOIFACE_LOG) << "Maps Loading Progress: " << progress << "%";
}

void HTMLWidget::slotHTMLCompleted(bool ok)
{
    qCDebug(DIGIKAM_GEOIFACE_LOG) << "Map Loading Completed: " << ok;
    d->isReady = ok;

    Q_EMIT signalJavaScriptReady();
}

void HTMLWidget::mousePressEvent(QMouseEvent* e)
{
    slotScanForJSMessages(QLatin1String("(event)"));
    QWebView::mousePressEvent(e);
}

void HTMLWidget::mouseReleaseEvent(QMouseEvent* e)
{
    if (s->currentMouseMode == MouseModeRegionSelection)
    {
        if (!d->firstSelectionPoint.hasCoordinates())
        {
            runScript2Coordinates(QString::fromLatin1("kgeomapPixelToLatLng(%1, %2);")
                                  .arg(e->x())
                                  .arg(e->y()),
                                  &d->firstSelectionPoint);

            d->firstSelectionScreenPoint = QPoint(e->x(), e->y());
        }
        else
        {
            runScript2Coordinates(QString::fromLatin1("kgeomapPixelToLatLng(%1, %2);")
                                  .arg(e->x())
                                  .arg(e->y()),
                                  &d->intermediateSelectionPoint);

            d->intermediateSelectionScreenPoint = QPoint(e->x(), e->y());

            qreal lonWest, latNorth, lonEast, latSouth;

            if (d->firstSelectionScreenPoint.x() < d->intermediateSelectionScreenPoint.x())
            {
                lonWest  = d->firstSelectionPoint.lon();
                lonEast  = d->intermediateSelectionPoint.lon();
            }
            else
            {
                lonEast  = d->firstSelectionPoint.lon();
                lonWest  = d->intermediateSelectionPoint.lon();
            }

            if (d->firstSelectionScreenPoint.y() < d->intermediateSelectionScreenPoint.y())
            {
                latNorth = d->firstSelectionPoint.lat();
                latSouth = d->intermediateSelectionPoint.lat();
            }
            else
            {
                latNorth = d->intermediateSelectionPoint.lat();
                latSouth = d->firstSelectionPoint.lat();
            }

            runScript(QLatin1String("kgeomapRemoveTemporarySelectionRectangle();"));
            runScript(QString::fromLatin1("kgeomapSetSelectionRectangle(%1, %2, %3, %4);")
                     .arg(lonWest)
                     .arg(latNorth)
                     .arg(lonEast)
                     .arg(latSouth));

            const GeoCoordinates::Pair selectionCoordinates(
                    GeoCoordinates(latNorth, lonWest),
                    GeoCoordinates(latSouth, lonEast));

            d->firstSelectionPoint.clear();
            d->intermediateSelectionPoint.clear();

            Q_EMIT selectionHasBeenMade(selectionCoordinates);
        }
    }

    slotScanForJSMessages(QLatin1String("(event)"));
    QWebView::mouseReleaseEvent(e);
}

void HTMLWidget::mouseMoveEvent(QMouseEvent *e)
{
    if (
        (s->currentMouseMode == MouseModeRegionSelection) &&
        d->firstSelectionPoint.hasCoordinates()
       )
    {
        runScript2Coordinates(QString::fromLatin1("kgeomapPixelToLatLng(%1, %2);")
                              .arg(e->x())
                              .arg(e->y()),
                              &d->intermediateSelectionPoint);

        d->intermediateSelectionScreenPoint = QPoint(e->x(), e->y());

        qCDebug(DIGIKAM_GEOIFACE_LOG) << d->firstSelectionScreenPoint << QLatin1Char(' ')
                                      << d->intermediateSelectionScreenPoint;

        qreal lonWest, latNorth, lonEast, latSouth;

        if (d->firstSelectionScreenPoint.x() < d->intermediateSelectionScreenPoint.x())
        {
            lonWest  = d->firstSelectionPoint.lon();
            lonEast  = d->intermediateSelectionPoint.lon();
        }
        else
        {
            lonEast  = d->firstSelectionPoint.lon();
            lonWest  = d->intermediateSelectionPoint.lon();
        }

        if (d->firstSelectionScreenPoint.y() < d->intermediateSelectionScreenPoint.y())
        {
            latNorth = d->firstSelectionPoint.lat();
            latSouth = d->intermediateSelectionPoint.lat();
        }
        else
        {
            latNorth = d->intermediateSelectionPoint.lat();
            latSouth = d->firstSelectionPoint.lat();
        }

        runScript(QString::fromLatin1("kgeomapSetTemporarySelectionRectangle(%1, %2, %3, %4);")
                  .arg(lonWest)
                  .arg(latNorth)
                  .arg(lonEast)
                  .arg(latSouth));
    }

    QWebView::mouseMoveEvent(e);
}

void HTMLWidget::slotScanForJSMessages(const QString& message)
{
    if (message != QLatin1String("(event)"))
    {
        return;
    }

    //qCDebug(DIGIKAM_GEOIFACE_LOG) << message;

    const QString eventBufferString = runScript(QLatin1String("kgeomapReadEventStrings();")).toString();

    if (eventBufferString.isEmpty())
    {
        return;
    }

    const QStringList events = eventBufferString.split(QLatin1Char('|'));

    Q_EMIT signalHTMLEvents(events);
}

/**
 * @brief Wrapper around executeScript to catch more errors
 */
QVariant HTMLWidget::runScript(const QString& scriptCode, bool /*async*/)
{
    GEOIFACE_ASSERT(d->isReady);

    if (!d->isReady)
    {
        return QVariant();
    }

    //qCDebug(DIGIKAM_GEOIFACE_LOG) << scriptCode;

    return page()->mainFrame()->evaluateJavaScript(scriptCode);
}

/**
 * @brief Execute a script which returns coordinates and parse these
 */
bool HTMLWidget::runScript2Coordinates(const QString& scriptCode, GeoCoordinates* const coordinates)
{
    const QVariant scriptResult = runScript(scriptCode);

    return GeoIfaceHelperParseLatLonString(scriptResult.toString(), coordinates);
}

bool HTMLWidget::eventFilter(QObject* object, QEvent* event)
{
    if (d->parent && (object == d->parent))
    {
        if (event->type() == QEvent::Resize)
        {
            QResizeEvent* const resizeEvent = dynamic_cast<QResizeEvent*>(event);

            if (resizeEvent)
            {
                resize(resizeEvent->size());
            }
        }
    }

    return false;
}

void HTMLWidget::setSelectionRectangle(const GeoCoordinates::Pair& searchCoordinates)
{
    if (!searchCoordinates.first.hasCoordinates())
    {
        runScript(QLatin1String("kgeomapRemoveSelectionRectangle();"));
        return;
    }

    qreal West  = searchCoordinates.first.lon();
    qreal North = searchCoordinates.first.lat();
    qreal East  = searchCoordinates.second.lon();
    qreal South = searchCoordinates.second.lat();

    runScript(QString::fromLatin1("kgeomapSetSelectionRectangle(%1, %2, %3, %4);")
              .arg(West).arg(North).arg(East).arg(South));
}

void HTMLWidget::removeSelectionRectangle()
{
    runScript(QLatin1String("kgeomapRemoveSelectionRectangle();"));
}

void HTMLWidget::mouseModeChanged(const GeoMouseModes mouseMode)
{
    const bool inSelectionMode = (mouseMode == MouseModeRegionSelection);

    if (inSelectionMode)
    {
        d->firstSelectionPoint.clear();
        d->intermediateSelectionPoint.clear();
        runScript(QString::fromLatin1("kgeomapSelectionModeStatus(%1);").arg(inSelectionMode));
    }
    else
    {
        runScript(QString::fromLatin1("kgeomapSelectionModeStatus(%1);").arg(inSelectionMode));
    }
}

void HTMLWidget::centerOn(const qreal west, const qreal north,
                          const qreal east, const qreal south,
                          const bool useSaneZoomLevel)
{
/*
    qCDebug(DIGIKAM_GEOIFACE_LOG) << "West:" << west
                                  << " North:" << north
                                  << " East:" << east
                                  << " South:" << south;
*/
    runScript(QString::fromLatin1("kgeomapSetMapBoundaries(%1, %2, %3, %4, %5);")
              .arg(west)
              .arg(north)
              .arg(east)
              .arg(south)
              .arg(useSaneZoomLevel ? 1 : 0));
}

void HTMLWidget::setSharedGeoIfaceObject(GeoIfaceSharedData* const sharedData)
{
    s = sharedData;
}

} // namespace Digikam

#include "moc_htmlwidget_qwebkit.cpp"
