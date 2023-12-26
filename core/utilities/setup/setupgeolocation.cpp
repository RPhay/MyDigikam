/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-12-24
 * Description : geo-location setup page.
 *
 * SPDX-FileCopyrightText: 2022-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "setupgeolocation.h"

// Qt includes

#include <QApplication>
#include <QVBoxLayout>
#include <QLabel>
#include <QFileInfo>
#include <QFile>
#include <QStandardPaths>
#include <QLineEdit>
#include <QPushButton>
#include <QTabWidget>
#include <QList>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "MarbleConfigView.h"
#include "marblesettings.h"
#include "digikam_debug.h"

using namespace Marble;

namespace Digikam
{

class Q_DECL_HIDDEN SetupGeolocation::Private
{
public:

    Private() = default;

    MarbleConfigView* tab  = nullptr;
    QWidget* gMapView      = nullptr;
    QLineEdit* gMapApi     = nullptr;

public:
/*
    QString htmlFilePath() const
    {
        QString htmlPath = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
        htmlPath        += QLatin1String("/digikam/geoiface/");
        QString htmlFile = htmlPath + d->htmlFileName;

        return htmlFile;
    }
*/
};

SetupGeolocation::SetupGeolocation(QWidget* const parent)
    : QScrollArea(parent),
      d          (new Private)
{
    QWidget* const panel    = new QWidget(viewport());
    setWidget(panel);
    setWidgetResizable(true);

    QVBoxLayout* const vbl = new QVBoxLayout();
    panel->setLayout(vbl);

    d->tab                 = new MarbleConfigView(MarbleSettings::instance()->mainMarbleWidget(), this);

    // ---

    d->gMapView             = new QWidget(this);
    QLabel* const title     = new QLabel(i18n("API Key:"), d->gMapView);
    d->gMapApi              = new QLineEdit(d->gMapView);
    d->gMapApi->setEchoMode(QLineEdit::Normal);

    QVBoxLayout* const vlay = new QVBoxLayout(d->gMapView);
    vlay->addWidget(title);
    vlay->addWidget(d->gMapApi);
    vlay->addStretch();
    d->tab->insertTab(GoogleMaps, d->gMapView, i18n("Google Maps"));

    // ---

    vbl->addWidget(d->tab);

    readSettings();
}

SetupGeolocation::~SetupGeolocation()
{
    delete d;
}

void SetupGeolocation::cancel()
{
    d->tab->cancel();
}

void SetupGeolocation::setActiveTab(GeolocationTab tab)
{
    d->tab->setCurrentIndex(tab);
}

SetupGeolocation::GeolocationTab SetupGeolocation::activeTab() const
{
    return (GeolocationTab)d->tab->currentIndex();
}

void SetupGeolocation::applySettings()
{
    d->tab->applySettings();
/*
    QString key      = d->gMapApi->text().trimmed();
    QString htmlFile = htmlFilePath();

    if (key.isEmpty())
    {
        if (QFileInfo::exists(htmlFile))
        {
            QFile::remove(htmlFile);
        }

        const QUrl htmlUrl = GeoIfaceGlobalObject::instance()->locateDataFile(d->htmlFileName);
        d->htmlWidget->load(htmlUrl);

        return;
    }

    QString jsFile = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                            QLatin1String("digikam/geoiface/backend-googlemaps-js.js"));

    if (!QFileInfo::exists(jsFile))
    {
        return;
    }

    QString htmlText = d->htmlTemplate.arg(key).arg(QUrl::fromLocalFile(jsFile).toString());

    if (!QFileInfo::exists(htmlPath))
    {
        QDir().mkpath(htmlPath);
    }

    QFile writeFile(htmlFile);

    if (writeFile.open(QIODevice::WriteOnly))
    {
        writeFile.write(htmlText.toLatin1());
        d->keyChanged = true;
        writeFile.close();
    }
*/
}

void SetupGeolocation::readSettings()
{
    d->tab->readSettings();

    // Read Google API key
/*
    QString htmlFile = htmlFilePath();
    QString oldKey;

    if (QFileInfo::exists(htmlFile))
    {
        QFile readFile(htmlFile);

        if (readFile.open(QIODevice::ReadOnly))
        {
            QString oldHtml = QString::fromLatin1(readFile.readAll());
            readFile.close();

            if (!oldHtml.isEmpty())
            {
                int firstIdx = oldHtml.indexOf(QLatin1String("key="));

                if (firstIdx != -1)
                {
                    int lastIdx = oldHtml.indexOf(QLatin1String("\"></script>"), firstIdx);

                    if (lastIdx > (firstIdx + 4))
                    {
                        oldKey = oldHtml.mid(firstIdx + 4, lastIdx - firstIdx - 4);
                    }
                }
            }
        }
    }

    d->gMapApi->setText(oldKey);
*/
}

} // namespace Digikam

#include "moc_setupgeolocation.cpp"
