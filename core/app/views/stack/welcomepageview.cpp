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
#include <QPushButton>
#include <QButtonGroup>
#include <QApplication>
#include <QStandardPaths>
#include <QScrollArea>
#include <QGridLayout>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QStackedWidget>
#include <QStylePainter>
#include <QStyleOptionTab>

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

class Q_DECL_HIDDEN TitleEffect : public QGraphicsDropShadowEffect
{
    Q_OBJECT

public:

    TitleEffect(QWidget* const parent = nullptr)
        : QGraphicsDropShadowEffect(parent)
    {
        setColor(Qt::white);
        setBlurRadius(35);
        setOffset(0, 0);
    }
};

// ---

class Q_DECL_HIDDEN GradientWidget : public QWidget
{
    Q_OBJECT

public:

    explicit GradientWidget(QWidget* const parent = nullptr)
        : QWidget (parent)
    {
        setContentsMargins(0, 0, 0, 0);
    }

protected:

    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        QLinearGradient gradient(0, 0, 0, height());
        gradient.setColorAt(0,    QColor(qRgb(0x01, 0x08, 0x0F)));
        gradient.setColorAt(0.55, QColor(qRgb(0x22, 0x3c, 0x54)));
        painter.fillRect(rect(), gradient);
    }
};

// ---

class Q_DECL_HIDDEN InvertedGradientWidget : public QWidget
{
    Q_OBJECT

public:

    explicit InvertedGradientWidget(QWidget* const parent = nullptr)
        : QWidget(parent)
    {
        setContentsMargins(0, 0, 0, 0);
    }

protected:

    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        QLinearGradient gradient(0, 0, 0, height());
        gradient.setColorAt(0,    QColor(qRgb(0x22, 0x3c, 0x54)));
        gradient.setColorAt(0.55, QColor(qRgb(0x01, 0x08, 0x0F)));
        painter.fillRect(rect(), gradient);
    }
};

// ---

class Q_DECL_HIDDEN ResizableBackgroundWidget : public QWidget
{
    Q_OBJECT

public:

    explicit ResizableBackgroundWidget(QWidget* const parent = nullptr)
        : QWidget(parent)
    {
        setContentsMargins(0, 0, 0, 0);
    }

protected:

    void resizeEvent(QResizeEvent* event) override
    {
        QWidget::resizeEvent(event);
        update();
    }

    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);
        painter.fillRect(rect(), QColor(qRgb(0x22, 0x3c, 0x54)));

        QPixmap scaledPixmap = m_backgroundPixmap.scaledToWidth(width(), Qt::SmoothTransformation);
        painter.drawPixmap(0, 0, scaledPixmap);
    }

private:

    QPixmap m_backgroundPixmap = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                        QLatin1String("digikam/data/body-background.webp"));
};

// ---

WelcomePageView::WelcomePageView(QWidget* const parent)
    : QWidget(parent)
{
    QScrollArea* const sv = new QScrollArea(this);
    sv->setFrameStyle(QFrame::NoFrame);
    sv->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    sv->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    sv->setWidgetResizable(true);

    QWidget* const plain = new QWidget(sv->viewport());
    plain->setMinimumWidth(sv->width());
    plain->setContentsMargins(0, 0, 0, 0);

    // ---

    ResizableBackgroundWidget* const background = new ResizableBackgroundWidget(plain);
    GradientWidget* const gradHeader            = new GradientWidget(plain);

    // ---

    QWidget* const headerWidget = new QWidget(plain);
    QLabel* const logo          = new QLabel(headerWidget);
    logo->setPixmap(QPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                   QLatin1String("digikam/data/logo-digikam.webp")))
                    .scaled(96, 96, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    QLabel* const title         = new QLabel(QLatin1String("<qt><h1>digiKam</h1></qt>"), headerWidget);
    title->setObjectName(QLatin1String("app-name"));
    title->setStyleSheet(QLatin1String("#app-name {"
                                       "   color: rgba(255, 255, 255, 0.67);"
                                       "   margin-top: 11px;"
                                       "}"));
    title->setGraphicsEffect(new TitleEffect);

    QWidget* const titleWidget           = new QWidget(plain);

    QLabel* const smallTitle             = new QLabel(DAboutData::digiKamSlogan(), titleWidget);
    smallTitle->setAlignment(Qt::AlignCenter);
    smallTitle->setObjectName(QLatin1String("small-title"));
    smallTitle->setStyleSheet(QLatin1String("#small-title {"
                                            "   color: rgba(255, 255, 255, 0.67);"
                                            "   font-size: 14px;"
                                            "}"));
    smallTitle->setGraphicsEffect(new TitleEffect);

    QLabel* const bigTitle               = new QLabel(i18n("Welcome to digiKam %1",
                                                      QLatin1String(digikam_version)), titleWidget);
    bigTitle->setAlignment(Qt::AlignCenter);
    bigTitle->setObjectName(QLatin1String("big-title"));
    smallTitle->setStyleSheet(QLatin1String("#big-title {"
                                            "   color: rgba(255, 255, 255, 0.67);"
                                            "   font-size: 24px;"
                                            "   font-weight: bold;"
                                            "}"));
    bigTitle->setGraphicsEffect(new TitleEffect);

    QVBoxLayout* const titleLayout       = new QVBoxLayout(titleWidget);
    titleLayout->setContentsMargins(0, 0, 0, 20);
    titleLayout->setSpacing(0);
    titleLayout->addWidget(smallTitle);
    titleLayout->addWidget(bigTitle);

    QHBoxLayout* const headerLayout      = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(10, 10, 10, 10);
    headerLayout->setSpacing(0);
    headerLayout->addWidget(logo);
    headerLayout->addStretch(10);
    headerLayout->addWidget(titleWidget, Qt::AlignCenter);
    headerLayout->addStretch(10);
    headerLayout->addWidget(title);

    // ---

    InvertedGradientWidget* const gradFooter = new InvertedGradientWidget(plain);
    QWidget* const footerWidget              = new QWidget(plain);
    footerWidget->setContentsMargins(0, 0, 0, 0);
    footerWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);

    // ---

    QWidget* const tabButtonsWidget      = new QWidget(plain);

    QPushButton* const newFeaturesButton = new QPushButton(i18n("New Features"), tabButtonsWidget);
    QPushButton* const aboutButton       = new QPushButton(i18n("About"), tabButtonsWidget);
    QPushButton* const creditsButton     = new QPushButton(i18n("Background Image Credits"), tabButtonsWidget);

    QString buttonStyle                  = QLatin1String("QPushButton { background-color: rgba(220, 220, 220, 128); border: none; border-radius: 8px; padding: 8px; }"
                                                         "QPushButton:checked { background-color: rgba(150, 150, 150, 200); }");
    newFeaturesButton->setStyleSheet(buttonStyle);
    newFeaturesButton->setAutoDefault(false);
    newFeaturesButton->setShortcutEnabled(false);
    newFeaturesButton->setFocusPolicy(Qt::NoFocus);
    newFeaturesButton->setShortcut(QKeySequence());
    aboutButton->setStyleSheet(buttonStyle);
    aboutButton->setAutoDefault(false);
    aboutButton->setShortcutEnabled(false);
    aboutButton->setFocusPolicy(Qt::NoFocus);
    aboutButton->setShortcut(QKeySequence());
    creditsButton->setStyleSheet(buttonStyle);
    creditsButton->setAutoDefault(false);
    creditsButton->setShortcutEnabled(false);
    creditsButton->setFocusPolicy(Qt::NoFocus);
    creditsButton->setShortcut(QKeySequence());

    // ---

    QHBoxLayout* const tabButtonsLayout  = new QHBoxLayout(tabButtonsWidget);
    tabButtonsLayout->setContentsMargins(0, 0, 0, 0);
    tabButtonsLayout->setSpacing(10);
    tabButtonsLayout->addStretch();
    tabButtonsLayout->addWidget(newFeaturesButton);
    tabButtonsLayout->addWidget(aboutButton);
    tabButtonsLayout->addWidget(creditsButton);
    tabButtonsLayout->addStretch();

    // ---

    QButtonGroup* const tabButtonGroup   = new QButtonGroup(plain);
    tabButtonGroup->addButton(newFeaturesButton);
    tabButtonGroup->addButton(aboutButton);
    tabButtonGroup->addButton(creditsButton);
    newFeaturesButton->setCheckable(true);
    aboutButton->setCheckable(true);
    creditsButton->setCheckable(true);

    // ---

    QStackedWidget* const stackedWidget  = new QStackedWidget(plain);
    stackedWidget->setContentsMargins(0, 0, 0, 0);
    QString labelsStyle                  = QLatin1String("background: rgba(240, 240, 240, 128); "
                                                         "color: black; border-radius: 15px; "
                                                         "padding: 10px;");

    QWidget* const newFeaturesTab        = new QWidget(stackedWidget);
    QLabel* const newFeatures            = new QLabel(featuresTabContent(), stackedWidget);
    newFeatures->setStyleSheet(labelsStyle);
    newFeatures->setOpenExternalLinks(true);
    newFeatures->setWordWrap(true);
    newFeatures->setMargin(10);
    QVBoxLayout* const vlay1             = new QVBoxLayout(newFeaturesTab);
    vlay1->addWidget(newFeatures);
    vlay1->addStretch();
    vlay1->setContentsMargins(80, 10, 80, 10);
    stackedWidget->addWidget(newFeaturesTab);

    QWidget* const aboutTab              = new QWidget(stackedWidget);
    QLabel* const about                  = new QLabel(aboutTabContent(), stackedWidget);
    about->setStyleSheet(labelsStyle);
    about->setOpenExternalLinks(true);
    about->setWordWrap(true);
    about->setMargin(10);
    QVBoxLayout* const vlay2             = new QVBoxLayout(aboutTab);
    vlay2->addWidget(about);
    vlay2->addStretch();
    vlay2->setContentsMargins(80, 10, 80, 10);
    stackedWidget->addWidget(aboutTab);

    QWidget* const creditsTab            = new QWidget(stackedWidget);
    QLabel* const credits                = new QLabel(creditsTabContent(), stackedWidget);
    credits->setStyleSheet(labelsStyle);
    credits->setOpenExternalLinks(true);
    credits->setWordWrap(true);
    credits->setMargin(10);
    QVBoxLayout* const vlay3             = new QVBoxLayout(creditsTab);
    vlay3->addWidget(credits);
    vlay3->addStretch();
    vlay3->setContentsMargins(80, 10, 80, 10);
    stackedWidget->addWidget(creditsTab);

    // ---

    connect(newFeaturesButton, &QPushButton::clicked,
            this, [stackedWidget, newFeaturesTab]()
        {
            stackedWidget->setCurrentWidget(newFeaturesTab);
        }
    );

    connect(aboutButton, &QPushButton::clicked,
            this, [stackedWidget, aboutTab]()
        {
            stackedWidget->setCurrentWidget(aboutTab);
        }
    );

    connect(creditsButton, &QPushButton::clicked,
            this, [stackedWidget, creditsTab]()
        {
            stackedWidget->setCurrentWidget(creditsTab);
        }
    );

    aboutButton->click();

    // ---

    QGridLayout* const grid = new QGridLayout(plain);
    grid->addWidget(gradHeader,       0, 0, 1, 3);
    grid->addWidget(headerWidget,     0, 0, 1, 3);
    grid->addWidget(background,       1, 0, 2, 3);
    grid->addWidget(tabButtonsWidget, 1, 0, 1, 3, Qt::AlignCenter);
    grid->addWidget(gradFooter,       3, 0, 1, 3);
    grid->addWidget(footerWidget,     3, 0, 1, 3);
    grid->addWidget(stackedWidget,    2, 0, 2, 3);
    grid->setContentsMargins(0, 0, 0, 0);
    grid->setSpacing(0);
    grid->setColumnStretch(1, 10);
    grid->setRowStretch(2, 10);
    grid->setRowStretch(3, 10);

    // ---

    sv->setWidget(plain);

    QVBoxLayout* const mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(sv);
}

QString WelcomePageView::featuresTabContent() const
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
    newFeatures << i18n("Add new button to open current item in file manager.");
    newFeatures << i18n("If no icon-view item selected, show current album statistics.");
    newFeatures << i18n("Show items from an open group highlighted for a better usability with Icon-View.");
    newFeatures << i18n("Usability improvements of the grouped icon-view items overlay.");
    newFeatures << i18n("Thumbbar improvements to support grouped items visibility as with Icon-View.");
    newFeatures << i18n("Split filters view into 3 separated tabs for better usability: Properties, Tags, and People.");
    newFeatures << i18n("Load and save Trash-view column states between sessions.");
    newFeatures << i18n("Extend Image and Video preview toolbar overlay with Labels editor.");
    newFeatures << i18n("Add new Preview toolbar buttons: exposure indicators, color management, OSD, magnifier.");
    newFeatures << i18n("Add context-menu to the video player.");
    newFeatures << i18n("Add load progress indicator on the status bar (Icon-view and LightTable).");
    newFeatures << i18n("Add trackpad gesture events support to zoom the contents, pan the canvas, and change to previous/next items.");
    newFeatures << i18n("Add a magnifier glass tool to zoom and explore a small area of the preview canvas. Tool Size and zoom factor are customizable.");
    newFeatures << i18n("Add under and over exposure indicators for the preview canvas.");
    newFeatures << i18n("Railway over the canvas is displayed as an overlay when the scrollbar are visible.");
    newFeatures << i18n("Clicking on GPS icon overlay opens the Geolocation tab from the left sidebar.");
    newFeatures << i18n("Add customizable columns to show Grouped and Versioned properties.");
    newFeatures << i18n("New Advanced Rename modifier Date Format.");
    newFeatures << i18n("Add support to merge existing template entries assigned in items with a new template.");
    newFeatures << i18n("Add new Slideshow option to customize the color background instead pure black in case of images transparency usage.");
    newFeatures << i18n("Add new Presentation option to customize the color background instead pure black in case of images transparency usage.");
    newFeatures << i18n("Complete rewrite the File-Transfert tool GUI for a better usability.");
    newFeatures << i18n("Complete rewrite the File-Copy tool GUI for a better usability.");
    newFeatures << i18n("Add marching-ants effect to the canvas selection rubber band.");
    newFeatures << i18n("Add pick color histogram indicators for the Curves, Levels, White Balance, and Black & White Sepia tools.");
    newFeatures << i18n("Huge web site update about contents and design eg. screenshots, description, history, features, support, download, etc.");
    newFeatures << i18n("New Advanced Search options to find empty properties registered in the database.");
    newFeatures << i18n("Add support for search Creator and Author fields from the Advanced Search tool to be drop down lists populated with the values already presents in the database.");

    newFeatures << i18n("Update internal RAW engine to last Libraw snapshot 2026-01-24.");
    newFeatures << i18n("New RAW camera supported:")
                + appendSubList(QStringList()
                     << i18n("Canon EOS R1, EOS R5 Mark II, EOS R5 C, EOS R6 Mark II, EOS R8, EOS R50, EOS R100, EOS Ra ;")
                     << i18n("Fujifilm X-T50, GFX 100S II, GFX100-II, X-T5, X-S20, X-H2, X-H2S ;")
                     << i18n("Hasselblad  CFV-50c, CFV-100c, X2D-100c ;")
                     << i18n("Leica Q3 43, D-Lux8, SL3, Q3, M11 Monochrom ;")
                     << i18n("Nikon (standard compression only): Z6-III, Z f, Z30, Z8 ;")
                     << i18n("Olympus/OM System OM-1 Mark II, TG-7, OM-5 ;")
                     << i18n("Panasonic GH7, S9, DC-G9 II, DC-ZS200D / ZS220D, DC-TZ200D / TZ202D / TZ220D, DC-S5-II, DC-GH6 ;")
                     << i18n("Pentax KF, K III Monochrome ;")
                     << i18n("Sony ZV-E10M2, UMC-R10C, A9-III, ILX-LR1, A7C-II, A7CR, ILCE-6700,  ZV-1M2, ZV-E1, ILCE-7RM5 (A7R-V), ILME-FX30, A1 ;")
                     << i18n("Multiple DJI and Skydio drones ;")
                     << i18n("Multiple smartphones with DNG format recorded.")
                );

    // Add new features here...

    newFeatures << i18n("...and much more.");

    QString featureItems;

    for (int i = 0 ; i < newFeatures.count() ; ++i)
    {
        featureItems += i18n("<li>%1</li>", newFeatures.at(i));
    }

    QString tabContent = i18n("<h3>Some of the new features in this release of digiKam include (compared to digiKam version 8):</h3><ul>%1</ul>",
                         featureItems);

    return tabContent;
}

QString WelcomePageView::appendSubList(const QStringList& list) const
{
    QString sub;

    for (const QString& str : list)
    {
        sub.append(QLatin1String("<ul>") + str + QLatin1String("</ul>"));
    }

    return sub;
}

QString WelcomePageView::aboutTabContent() const
{
    QString tabContent =
       i18n("<h3>digiKam is an advanced open-source digital photo management application that runs on Linux, Windows, and MacOS. "
            "The application provides a comprehensive set of tools for importing, managing, editing, and sharing photos and raw files. "
            "You can use digiKam’s import capabilities to easily transfer photos, raw files, and videos directly from your camera "
            "and external storage devices (SD cards, USB disks, etc.). The application allows you to configure import settings and rules "
            "that process and organize imported items on-the-fly.</h3>"
            "<h3>With digiKam you can import, organize, enhance, search, export, and more...</h3>"
            "<h3>Currently, you are in the Album view mode of digiKam.</h3>"
            "<h3>Albums are the places where your files are stored, and are identical to the folders on your hard disk.</h3>"
            "<li>digiKam has many powerful features which are described in the <a href=\"https://docs.digikam.org/en/index.html\">documentation</a></li>"
            "<li>The <a href=\"https://www.digikam.org\">digiKam homepage</a> provides information about new versions of digiKam.</li>");

    return tabContent;
}

QString WelcomePageView::creditsTabContent()  const
{
    QString creditsTab =
        QString::fromUtf8(
                "<table>"
                  "<tr>"
                    "<td width=\"10%\">%1</td>"
                    "<td><h4>%2</h4></td>"
                  "</tr>"
                  "<tr>"
                    "<td width=\"10%\">%3</td>"
                    "<td><h4>%4</h4></td>"
                  "</tr>"
                  "<tr>"
                    "<td width=\"10%\">%5</td>"
                    "<td><h4>%6</h4></td>"
                  "</tr>"
                  "<tr>"
                    "<td width=\"10%\">%7</td>"
                    "<td><h4>%8</h4></td>"
                  "</tr>"
                  "<tr>"
                    "<td width=\"10%\">%9</td>"
                    "<td><h4>%10</h4></td>"
                  "</tr>"
                "</table>"
        ).arg(i18n("Author:"))  .arg(QString::fromUtf8("Andreas K. Huettel"))
         .arg(i18n("Location:")).arg(QString::fromUtf8("<a href=\"https://en.wikipedia.org/wiki/Kjem%C3%A5vatnet\">Kjemåvatnet - Nordland county - Norway.</a>"))
         .arg(i18n("Date:"))    .arg(QString::fromUtf8("August 2020"))
         .arg(i18n("Camera:"))  .arg(QString::fromUtf8("Canon EOS 7D Mark II"))
         .arg(i18n("Lens:"))    .arg(QString::fromUtf8("Sigma 18-250mm f/3.5-6.3 DC OS HSM"))
    ;

    return creditsTab;
}

} // namespace Digikam

#include "welcomepageview.moc"

#include "moc_welcomepageview.cpp"
