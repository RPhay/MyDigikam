/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-12-20
 * Description : a widget to display a welcome page
 *               on root album.
 *
 * SPDX-FileCopyrightText: 2006-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2015      by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "welcomepageview.h"

// Qt includes

#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <QApplication>
#include <QStandardPaths>

#include <QtWebEngineWidgetsVersion>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_version.h"
#include "daboutdata.h"
#include "thememanager.h"
#include "webbrowserdlg.h"

namespace Digikam
{

WelcomePageViewPage::WelcomePageViewPage(QObject* const parent)
    : QWebEnginePage(parent)
{
}

bool WelcomePageViewPage::acceptNavigationRequest(const QUrl& url, QWebEnginePage::NavigationType type, bool)
{
    if (type == QWebEnginePage::NavigationTypeLinkClicked)
    {
        Q_EMIT linkClicked(url);

        return false;
    }

    return true;
}

// ----------------------------------------------------------------------------

WelcomePageView::WelcomePageView(QWidget* const parent)
    : QWebEngineView(parent)
{
    setFocusPolicy(Qt::WheelFocus);

    setContextMenuPolicy(Qt::NoContextMenu);
    setContentsMargins(QMargins());

    WelcomePageViewPage* const wpage = new WelcomePageViewPage(this);
    setPage(wpage);

    settings()->setAttribute(QWebEngineSettings::WebGLEnabled, false);
    settings()->setAttribute(QWebEngineSettings::Accelerated2dCanvasEnabled, false);

    // ------------------------------------------------------------

    connect(wpage, SIGNAL(linkClicked(QUrl)),
            this, SLOT(slotUrlOpen(QUrl)));

    connect(ThemeManager::instance(), SIGNAL(signalThemeChanged()),
            this, SLOT(slotThemeChanged()));

    QTimer::singleShot(0, this, SLOT(slotThemeChanged()));
}

void WelcomePageView::slotUrlOpen(const QUrl& url)
{
    WebBrowserDlg* const browser = new WebBrowserDlg(url, qApp->activeWindow());
    browser->show();
}

QStringList WelcomePageView::featuresTabContent() const
{
    QStringList newFeatures;
    newFeatures << i18n("More than <a href=\"https://bugs.kde.org/buglist.cgi?f1=cf_versionfixedin&limit=0&o1=equals&order=bug_id&product=digikam&v1=%1\">350 user-reports</a> closed on bugzilla.", QLatin1String(digikam_version_short));
    newFeatures << i18n("Internal QtAVPlayer has been updated to the latest upstream 2026-01-24 which brings with it a variety of bug fixes and performance improvements (Qt5 only).");
    newFeatures << i18n("Add support of customizable date format to show everywhere in the program.");
    newFeatures << i18n("Color label names are now customizable in the Labels tab from left sidebar by double clicking on text. Label strings size are limited to 32 characters.");
    newFeatures << i18n("Add support of Album Icon-View sort items by Pick and Color Labels.");
    newFeatures << i18n("Add support of Import Icon-View sort items by Pick and Color Labels.");
    newFeatures << i18n("Add support of Album Icon-View sort items by Orientation.");
    newFeatures << i18n("Add support of Album Icon-View sort items by GPS location.");
    newFeatures << i18n("Add support of Album Icon-View sort items by File Format.");
    newFeatures << i18n("Add support of Import Icon-View sort items by File Format.");
    newFeatures << i18n("Add regular expression wildcard support for ignored directories settings.");
    newFeatures << i18n("Add new button to copy all properties as text in clipboard.");
    newFeatures << i18n("Add new button to open current item is file manager.");
    newFeatures << i18n("If no icon-view item selected, show current album statistics.");
    newFeatures << i18n("Show items from an open group highlighted for a better usability with Icon-View.");
    newFeatures << i18n("Usability improvements of the grouped icon-view items overlay.");
    newFeatures << i18n("Thumbbar improvements to support grouped items visibility as with Icon-View.");
    newFeatures << i18n("Split filters view into 3 separated tabs for better usabilty: Properties, Tags, and People.");
    newFeatures << i18n("Load and save Trash-view column states between sessions.");
    newFeatures << i18n("Extend Image and Video preview toolbar overlay with Labels editor.");
    newFeatures << i18n("Add new Preview toolbar buttons: exposure indicators, color management, OSD, magnifier.");
    newFeatures << i18n("Add context-menu to the video player.");
    newFeatures << i18n("Add load progress indicator on the status bar (Icon-view and LightTable).");
    newFeatures << i18n("Add trackpad gesture events support to zoom the contents, pan the canvas, and change to previous/next items.");
    newFeatures << i18n("Add a magnifier glass tool to zoom and explore a small area of the preview canvas. Tool Size and zoom factor are customizable.");
    newFeatures << i18n("Add under and over exposure indicators for the preview canvas.");
    newFeatures << i18n("Railway over the canvas is displayed as an overlay when the scrollbar are visible.");
    newFeatures << i18n("Clicking on GPS icon overlay open the Geolocation tab from the left sidebar.");
    newFeatures << i18n("Add customizable columns to show Grouped and Versioned properties.");
    newFeatures << i18n("New Advanced Rename modifier Date Format.");
    newFeatures << i18n("Add support to merge existing template entries assigned in items with a new template.");
    newFeatures << i18n("Add new Slideshow option to customize the color background instead pure black in case of images transparency usage.");
    newFeatures << i18n("Add new Presentation option to customize the color background instead pure black in case of images transparency usage.");
    newFeatures << i18n("Complete rewrite the File-Tranfert tool GUI for a better usability.");
    newFeatures << i18n("Complete rewrite the File-Copy tool GUI for a better usability.");
    newFeatures << i18n("Add marching-ants effect to the canvas selection rubber band.");
    newFeatures << i18n("Add pick color histogram indicators for the Curves, Levels, White Balance, and Black & White Sepia tools.");
    newFeatures << i18n("Huge web site update about contents and design eg. screenshots, description, history, features, support, download, etc.");
    newFeatures << i18n("Update internal RAW engine to last Libraw snapshot 2026-01-24.");
    newFeatures << i18n("New RAW camera supported:\n"
                        "Canon EOS R1, EOS R5 Mark II, EOS R5 C, EOS R6 Mark II, EOS R8, EOS R50, EOS R100, EOS Ra ; "
                        "Fujifilm X-T50, GFX 100S II, GFX100-II, X-T5, X-S20, X-H2, X-H2S ; "
                        "Hasselblad  CFV-50c, CFV-100c, X2D-100c ; "
                        "Leica Q3 43, D-Lux8, SL3, Q3, M11 Monochrom ; "
                        "Nikon (standard compression only): Z6-III, Z f, Z30, Z8 ; "
                        "Olympus/OM System OM-1 Mark II, TG-7, OM-5 ; "
                        "Panasonic GH7, S9, DC-G9 II, DC-ZS200D / ZS220D, DC-TZ200D / TZ202D / TZ220D, DC-S5-II, DC-GH6 ; "
                        "Pentax KF, K III Monochrome ; "
                        "Sony ZV-E10M2, UMC-R10C, A9-III, ILX-LR1, A7C-II, A7CR, ILCE-6700,  ZV-1M2, ZV-E1, ILCE-7RM5 (A7R-V), ILME-FX30, A1 ; "
                        "Multiple DJI and Skydio drones ; "
                        "Multiple smartphones with DNG format recorded."
                       );

    // Add new features here...

    newFeatures << i18n("...and much more.");

    QString featureItems;

    for (int i = 0 ; i < newFeatures.count() ; ++i)
    {
        featureItems += i18n("<li>%1</li><br>", newFeatures.at(i));
    }

    QString tabHeader  = i18n("New Features");
    QString tabContent =
        i18n("<h3>%1</h3><ul>%2</ul>",
             i18n("Some of the new features in this release of digiKam include (compared to digiKam 8):"),
             featureItems);

    return QStringList() << tabHeader << tabContent;
}

QStringList WelcomePageView::aboutTabContent() const
{
    QString tabHeader  = i18n("About");
    QString tabContent =
        i18n("<h3>%1</h3><h3>%2</h3><ul>%3</ul>",
             i18n("digiKam is an open source photo management program designed to import, organize, enhance, search and export your digital images to and from your computer."),
             i18n("Currently, you are in the Album view mode of digiKam. Albums are the places where your files are stored, and are identical to the folders on your hard disk."),
             i18n("<li>%1</li><li>%2</li>",
                  i18n("digiKam has many powerful features which are described in the <a href=\"https://docs.digikam.org/en/index.html\">documentation</a>"),
                  i18n("The <a href=\"https://www.digikam.org\">digiKam homepage</a> provides information about new versions of digiKam.")));

    return QStringList() << tabHeader << tabContent;
}

QByteArray WelcomePageView::fileToString(const QString& aFileName) const
{
    QByteArray   result;
    QFileInfo    info(aFileName);
    unsigned int readLen;
    unsigned int len = info.size();
    QFile        file(aFileName);

    if (
        aFileName.isEmpty() ||
        (len == 0)          ||
        !info.exists()      ||
        info.isDir()        ||
        !info.isReadable() ||
        !file.open(QIODevice::Unbuffered|QIODevice::ReadOnly)
       )
    {
        return QByteArray();
    }

    result.resize(len + 2);
    readLen = file.read(result.data(), len);
    file.close();

    if (result[len-1] != '\n')
    {
        result[len++] = '\n';
        ++readLen;
    }

    result[len] = '\0';

    if (readLen < len)
    {
        return QByteArray();
    }

    return result;
}

void WelcomePageView::slotThemeChanged()
{
    QString appTitle         = i18n("digiKam");
    QString slogan           = DAboutData::digiKamSlogan();
    QString locationHtml     = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QLatin1String("digikam/about/main.html"));
    QString content          = QString::fromUtf8(fileToString(locationHtml));

    content                  = content.arg((layoutDirection() == Qt::RightToLeft) ? QLatin1String("rtl") : QLatin1String("ltr")) // For RTL issue. See bug #376438
                                      .arg(appTitle)
                                      .arg(slogan)
                                      .arg(i18n("Welcome to digiKam %1", QLatin1String(digikam_version)))
                                      .arg(featuresTabContent().value(0))
                                      .arg(aboutTabContent().value(0))
                                      .arg(i18n("Background Image Credits"))
                                      .arg(featuresTabContent().value(1))
                                      .arg(aboutTabContent().value(1))
                                      .arg(i18n("Author:"))
                                      .arg(i18n("Location:"))
                                      .arg(i18n("Date:"))
                                      .arg(i18n("Camera:"))
                                      .arg(i18n("Lens:"))
    ;

    //qCDebug(DIGIKAM_GENERAL_LOG) << content;

    setHtml(content, QUrl::fromLocalFile(locationHtml));
}

} // namespace Digikam

#include "moc_welcomepageview.cpp"
