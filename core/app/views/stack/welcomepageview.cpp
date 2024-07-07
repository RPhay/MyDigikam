/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2006-12-20
 * Description : a widget to display a welcome page
 *               on root album.
 *
 * SPDX-FileCopyrightText: 2006-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
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
    newFeatures << i18n("More than 1500 user-reports closed on bugzilla.");
    newFeatures << i18n("Port code to Qt6 framework (code still compatible with Qt5 API).");
    newFeatures << i18n("Windows Installer is now compiled natively with Microsoft VCPKG tool-chain and use Qt6 and KF6 frameworks.");
    newFeatures << i18n("Add JPEG-XL, WEBP, and AVIF export settings everywhere.");
    newFeatures << i18n("Add convert to lossless JPEG-XL, WEBP, and AVIF while importing from camera.");
    newFeatures << i18n("Add JPEG-XL; WEBP, and AVIF converters in Batch Queue Manager.");
    newFeatures << i18n("Versionning: add support of JPEG-XL, HEIF, WEBP, and AVIF as lossless image format to store versionned images in editor.");
    newFeatures << i18n("Usability: add hamburger menu to tool-bar if main menu is hidden.");
    newFeatures << i18n("Metadata: add new option to write metadata to files with ExifTool backend.");
    newFeatures << i18n("Metadata: optional write metadata operations to DNG and RAW files are now always delegated to ExifTool Backend.");
    newFeatures << i18n("Metadata: optimize layout of Metadata Edit plugin for a better usability.");
    newFeatures << i18n("Metadata: add new options to save and load configuration profiles in Advanced Metadata Settings Panel.");
    newFeatures << i18n("Advanced Rename: add increment file counter to sequence numbering from with option [c] or [c,step].");
    newFeatures << i18n("Advanced Rename: add random text sequence numbering with option [r].");
    newFeatures << i18n("Advanced Rename: add configurable first character for the unique modifier with {unique:n,c}.");
    newFeatures << i18n("Advanced Search: add search for items by number of face regions.");
    newFeatures << i18n("Advanced Search: add search for items without face regions.");
    newFeatures << i18n("Collections: add support for cross-platform network paths.");
    newFeatures << i18n("Database: add support for SQLite WAL (Write-Ahead-Logging) mode.");
    newFeatures << i18n("Add spell-checking support in caption text widget using KF5::Sonnet component.");
    newFeatures << i18n("Add Spellcheck config panel in Settings/Miscellaneous for digiKam and Showfoto.");
    newFeatures << i18n("Add online translations support to Alternative Language text editor widget.");
    newFeatures << i18n("Add Localize config panel in Settings/Miscellaneous for digiKam and Showfoto.");
    newFeatures << i18n("Add settings to list preferred alternative/translated languages in text edit widget.");
    newFeatures << i18n("Allow to use Global or Customized Image Quality Sorting settings everywhere.");
    newFeatures << i18n("Add Icon-View settings to customize Labels visibility over thumbnails.");
    newFeatures << i18n("Add support for float16 (half float) images in the TIFF Loader.");
    newFeatures << i18n("Add new maintenance tool to perform <a href=\"https://community.kde.org/GSoc/2022/StatusReports/PhuocKhanhLe\">Aesthetic Detection to classify images using deep learning model</a>.");
    newFeatures << i18n("Add new tool <a href=\"https://community.kde.org/GSoC/2022/StatusReports/QuocHungTran\">to perform OCR over scanned text</a>.");
    newFeatures << i18n("Queue Manager: add new tool to assign pick label automatically by parsing image quality.");
    newFeatures << i18n("Queue Manager: add new tool to assign rating, pick, and color labels in batch.");
    newFeatures << i18n("Queue Manager: add new tool to assign titles and captions in batch.");
    newFeatures << i18n("Queue Manager: add new tool to run G'MIC filters in batch.");
    newFeatures << i18n("Documentation: <a href=\"https://docs.digikam.org/en/index.html\">Rewrite handbook</a> from DocBook to Sphinx/ReStructuredText framework.");
    newFeatures << i18n("Add new maintenance and Queue manager tools to assign tag automatically to images using deep learning neural network engine."
                        "<a href=\"https://community.kde.org/GSoc/2023/StatusReports/QuocHungTran#Add_Automatic_Tags_Assignment_Tools_and_Improve_Face_Recognition_Engine_for_digiKam\">See this GoSC-2023 report for details.</a>");
    newFeatures << i18n("Update G'MIC-Qt tool to last version 3.4.0 and finalize MacOS port and improve stability of plugin.");
    newFeatures << i18n("Update Adobe DNG SDK to last 1.7.1 with JPEG-XL compression support.");
    newFeatures << i18n("Update internal RAW engine to last Libraw snapshot 2024-06-16.");
    newFeatures << i18n("New RAW features: "
                        "Canon EOS R6 Mark II, EOS R8, EOS R50, EOS R100, EOS Ra."
                        "Fujifilm GFX100-II, X-T5, X-S20, X-H2, X-H2S."
                        "Hasselblad X2D-100c."
                        "Leica Q3, M11 Monochrom."
                        "Nikon Z30, Z8."
                        "OM Digital OM-5."
                        "Panasonic DC-G9 II, DC-ZS200D / ZS220D, DC-TZ200D / TZ202D / TZ220D, DC-S5-II, DC-GH6."
                        "Sony A7C-II, A7CR, ILCE-6700, ZV-1M2, ZV-E1, ILCE-7RM5 (A7R-V), ILME-FX30, DSC-HX95, A1."
                        "Multiple DJI and Skydio drones."
                        "Multiple smartphones with DNG format recorded."
                       );

    // Add new features here...

    newFeatures << i18n("...and much more.");

    QString featureItems;

    for (int i = 0 ; i < newFeatures.count() ; ++i)
    {
        featureItems += i18n("<li>%1</li>\n", newFeatures.at(i));
    }

    QString tabHeader  = i18n("New Features");
    QString tabContent =
        i18n("<h3>%1</h3><ul>%2</ul>",
             i18n("Some of the new features in this release of digiKam include (compared to digiKam 7):"),
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
