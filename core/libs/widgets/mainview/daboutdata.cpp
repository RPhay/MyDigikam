/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-07-30
 * Description : digiKam about data.
 *
 * SPDX-FileCopyrightText: 2008-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "daboutdata.h"

// Qt includes

#include <QIcon>
#include <QAction>

// KDE includes

#include <klocalizedstring.h>
#include <kaboutdata.h>

// Local includes

#include "dxmlguiwindow.h"

namespace Digikam
{

DAboutData::DAboutData(DXmlGuiWindow* const parent)
    : QObject(parent)
{
}

const QString DAboutData::digiKamSloganFormated()
{
    return i18nc("This is the slogan formatted string displayed in splashscreen. "
                 "Please translate using short words else the slogan can be truncated.",
                 "<qt><font color=\"white\">"
                 "<b>Professional</b> Photo <b>Management</b> with the Power of <b>Open Source</b>"
                 "</font><font color=\"gray\"><br/>"
                 "<i>%1</i>"
                 "</font></qt>",
                 digiKamFamily()
                );
}

const QString DAboutData::digiKamSlogan()
{
    return i18n("Professional Photo Management with the Power of Open Source");
}

const QString DAboutData::digiKamFamily()
{
    return i18n("A KDE Family Project");
}

const QString DAboutData::copyright()
{
    return i18n("(c) 2001-2025, digiKam developers team");
}

const QUrl DAboutData::webProjectUrl()
{
    return QUrl(QLatin1String("https://www.digikam.org"));
}

void DAboutData::authorsRegistration(KAboutData& aboutData)
{
    // -- Core team --------------------------------------------------------------

    aboutData.addAuthor ( QLatin1String("Caulier Gilles"),
                          ki18n("Coordinator, Developer, and Mentoring").toString(),
                          QLatin1String("caulier dot gilles at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/gilles-caulier/")
                        );

    aboutData.addAuthor ( QLatin1String("Maik Qualmann"),
                          ki18n("Developer and Mentoring").toString(),
                          QLatin1String("metzpinguin at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/maik-qualmann-2b266717b/")
                        );

    aboutData.addAuthor ( QLatin1String("Mohamed Anwer"),                                       // krazy:exclude=spelling
                          ki18n("Developer and Mentoring").toString(),
                          QLatin1String("mohammed dot ahmed dot anwer at gmail dot com"),       // krazy:exclude=spelling
                          QLatin1String("https://www.linkedin.com/in/mohamedanwer/")
                        );

    aboutData.addAuthor ( QLatin1String("Michael G. Hansen"),
                          ki18n("Developer and Mentoring").toString(),
                          QLatin1String("mike at mghansen dot de"),
                          QLatin1String("http://www.mghansen.de")                               // krazy:exclude=insecurenet
                        );

    aboutData.addAuthor ( QLatin1String("Teemu Rytilahti"),
                          ki18n("Developer").toString(),
                          QLatin1String("tpr at iki dot fi"),
                          QLatin1String("https://www.linkedin.com/in/teemurytilahti/")
                        );

    aboutData.addAuthor ( QLatin1String("Thanh Trung Dinh"),
                          ki18n("Mentoring, Port web-service tools to OAuth, factoring web service tools, "
                                "and port faces recognition engine to OpenCV neural network").toString(),
                          QLatin1String("dinhthanhtrung1996 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/thanhtrungdinh/")
                        );

    aboutData.addAuthor ( QLatin1String("Michael Miller"),
                          ki18n("SFace and YuNet DNN models integrationin face workflow. "
                                "Face Management and auto-tag AI improvements. "
                                "AI red eyes detection").toString(),
                          QLatin1String("michael underscore miller at msn dot com"),
                          QLatin1String("https://www.linkedin.com/in/michaelamiller01/")
                        );

    // -- Contributors -----------------------------------------------------------

    aboutData.addAuthor ( QLatin1String("Matthias Welwarsky"),
                          ki18n("Developer").toString(),
                          QLatin1String("matze at welwarsky dot de"),
                          QLatin1String("https://www.linkedin.com/in/matthiaswelwarsky/")
                        );

    aboutData.addAuthor ( QLatin1String("Julien Narboux"),
                          ki18n("Developer").toString(),
                          QLatin1String("Julien at narboux dot fr"),
                          QLatin1String("https://www.linkedin.com/in/julien-narboux-17566610/")
                        );

    aboutData.addAuthor ( QLatin1String("Mario Frank"),
                          ki18n("Advanced Searches Tool Improvements").toString(),
                          QLatin1String("mario.frank@uni-potsdam.de")
                        );

    aboutData.addAuthor ( QLatin1String("Nicolas Lécureuil"),
                          ki18n("Releases Manager").toString(),
                          QLatin1String("neoclust dot kde at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/nicolaslecureuil/")
                        );

    // -- Students ---------------------------------------------------------------

    aboutData.addCredit ( QLatin1String("Anjani Kumar"),
                          ki18n("Port to Qt6,").toString(),
                          QLatin1String("anjanik012 at gmail dot com "),
                          QLatin1String("https://www.linkedin.com/in/anjanik012/")
                        );

    aboutData.addCredit ( QLatin1String("Phuoc Khanh LE"),
                          ki18n("Rewrite Image Quality Sorter algorithms and Photo Focus Information Extraction, "
                                "Aesthetic Detection to classify images using deep learning model, "
                                "Slideshow and Presentation tools improvement").toString(),
                          QLatin1String("phuockhanhnk94 at gmail dot com "),
                          QLatin1String("https://www.linkedin.com/in/phuoc-khanh-le-476448169/")
                        );

    aboutData.addCredit ( QLatin1String("Quốc Hưng Trần"),
                          ki18n("MJPEGStream plugin, OCR text Converter plugin, auto-tags assignment engine").toString(),
                          QLatin1String("quochungtran1999 at gmail dot com "),
                          QLatin1String("https://www.linkedin.com/in/qu%E1%BB%91c-h%C6%B0ng-tr%E1%BA%A7n-92a504163/")
                        );

    aboutData.addCredit ( QLatin1String("Minh Nghĩa Duong"),
                          ki18n("Clustering support and improvement for Face Engine, "
                                "Port Slideshow tool to plugins interface").toString(),
                          QLatin1String("minhnghiaduong997 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/minh-nghia-duong-2b5bbb15a/")
                        );

    aboutData.addCredit ( QLatin1String("Kartik Ramesh"),
                          ki18n("Face management workflow improvements").toString(),
                          QLatin1String("kartikx2000 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/kartikxramesh/")
                        );

    aboutData.addCredit ( QLatin1String("Ahmed Fathi"),
                          ki18n("UPNP/DLNA export tool, and Healing clone tool for image editor").toString(),
                          QLatin1String("ahmed dot fathi dot abdelmageed at gmail dot com"),
                          QLatin1String("https://ahmedfathishabanblog.wordpress.com/")
                        );

    aboutData.addCredit ( QLatin1String("Veaceslav Munteanu"),
                          ki18n("Tags Manager").toString(),
                          QLatin1String("veaceslav dot munteanu90 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/veaceslav-munteanu-4370a063/")
                        );

    aboutData.addCredit ( QLatin1String("Tarek Talaat"),
                          ki18n("New OneDrive, Pinterrest, and Box export tools").toString(),
                          QLatin1String("tarektalaat93 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/tarek-talaat-9bb5b1a6/")
                        );

    aboutData.addCredit ( QLatin1String("Yingjie Liu"),
                          ki18n("Face-engine improvements and manual icon-view sort").toString(),
                          QLatin1String("yingjiewudi at gmail dot com"),
                          QLatin1String("https://yjwudi.github.io")
                        );

    aboutData.addCredit ( QLatin1String("Yiou Wang"),
                          ki18n("Model/View Port of Image Editor Canvas").toString(),
                          QLatin1String("geow812 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/yiouwang/")
                        );

    aboutData.addCredit ( QLatin1String("Gowtham Ashok"),
                          ki18n("Image Quality Sorter").toString(),
                          QLatin1String("gwty93 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/gowtham-ashok/")
                        );

    aboutData.addCredit ( QLatin1String("Aditya Bhatt"),
                          ki18n("Face Detection").toString(),
                          QLatin1String("aditya at bhatts dot org"),
                          QLatin1String("https://www.linkedin.com/in/adityabhatt/")
                        );

    aboutData.addCredit ( QLatin1String("Martin Klapetek"),
                          ki18n("Non-destructive image editing").toString(),
                          QLatin1String("martin dot klapetek at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/mck182/")
                        );

    aboutData.addCredit ( QLatin1String("Gabriel Voicu"),
                          ki18n("Reverse Geo-Coding").toString(),
                          QLatin1String("ping dot gabi at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/gabriel-voicu-958b8327/")
                        );

    aboutData.addCredit ( QLatin1String("Mahesh Hegde"),
                          ki18n("Face Recognition").toString(),
                          QLatin1String("maheshmhegade at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/maheshmhegade/")
                        );

    aboutData.addCredit ( QLatin1String("Pankaj Kumar"),
                          ki18n("Multi-core Support in Batch Queue Manager and Mentoring").toString(),
                          QLatin1String("me at panks dot me"),
                          QLatin1String("https://www.linkedin.com/in/panks42/")
                        );

    aboutData.addCredit ( QLatin1String("Smit Mehta"),
                          ki18n("UPnP / DLNA Export tool and Mentoring").toString(),
                          QLatin1String("smit dot tmeh at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/smit-mehta-45b82640/")
                        );

    aboutData.addCredit ( QLatin1String("Islam Wazery"),
                          ki18n("Model/View port of Import Tool and Mentoring").toString(),
                          QLatin1String("wazery at ubuntu dot com"),
                          QLatin1String("https://www.linkedin.com/in/wazery/")
                        );

    aboutData.addCredit ( QLatin1String("Abhinav Badola"),
                          ki18n("Video Metadata Support and Mentoring").toString(),
                          QLatin1String("mail dot abu dot to at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/abhinav-badola-86085424/")
                        );

    aboutData.addCredit ( QLatin1String("Benjamin Girault"),
                          ki18n("Panorama Tool and Mentoring").toString(),
                          QLatin1String("benjamin dot girault at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/benjamingirault/")
                        );

    aboutData.addCredit ( QLatin1String("Victor Dodon"),
                          ki18n("XML based GUI port of tools").toString(),
                          QLatin1String("dodonvictor at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/dodonvictor/")
                        );

    aboutData.addCredit ( QLatin1String("Sayantan Datta"),
                          ki18n("Auto Noise Reduction").toString(),
                          QLatin1String("sayantan dot knz at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/stndta/")
                        );

    // -- Former contributors ----------------------------------------------------

    aboutData.addAuthor ( QLatin1String("Ananta Palani"),
                          ki18n("Windows Port and Release Manager").toString(),
                          QLatin1String("anantapalani at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/anantapalani/")
                        );

    aboutData.addAuthor ( QLatin1String("Andi Clemens"),
                          ki18n("Developer").toString(),
                          QLatin1String("andi dot clemens at gmail dot com")
                        );

    aboutData.addAuthor ( QLatin1String("Patrick Spendrin"),
                          ki18n("Developer and Windows port").toString(),
                          QLatin1String("patrick_spendrin at gmx dot de"),
                          QLatin1String("https://www.linkedin.com/in/patrickspendrin/")
                        );

    aboutData.addCredit ( QLatin1String("Francesco Riosa"),
                          ki18n("LCMS2 library port").toString(),
                          QLatin1String("francesco plus kde at pnpitalia dot it"),
                          QLatin1String("https://www.linkedin.com/in/vivogentoo/")
                        );

    aboutData.addCredit ( QLatin1String("Johannes Wienke"),
                          ki18n("Developer").toString(),
                          QLatin1String("languitar at semipol dot de"),
                          QLatin1String("https://www.facebook.com/languitar")
                        );

    aboutData.addAuthor ( QLatin1String("Julien Pontabry"),
                          ki18n("Developer").toString(),
                          QLatin1String("julien dot pontabry at ulp dot u-strasbg dot fr"),
                          QLatin1String("https://www.linkedin.com/in/julien-pontabry-b784a247/")
                        );

    aboutData.addAuthor ( QLatin1String("Arnd Baecker"),
                          ki18n("Developer").toString(),
                          QLatin1String("arnd dot baecker at web dot de")
                        );

    aboutData.addAuthor ( QLatin1String("Francisco J. Cruz"),
                          ki18n("Color Management").toString(),
                          QLatin1String("fj dot cruz at supercable dot es")
                        );

    aboutData.addCredit ( QLatin1String("Pieter Edelman"),
                          ki18n("Developer").toString(),
                          QLatin1String("p dot edelman at gmx dot net"),
                          QLatin1String("https://www.facebook.com/pieter.edelman")
                        );

    aboutData.addCredit ( QLatin1String("Holger Foerster"),
                          ki18n("MySQL interface").toString(),
                          QLatin1String("hamsi2k at freenet dot de")
                        );

    aboutData.addCredit ( QLatin1String("Risto Saukonpaa"),
                          ki18n("Design, icons, logo, banner, mockup, beta tester").toString(),
                          QLatin1String("paristo at gmail dot com")
                        );

    aboutData.addCredit ( QLatin1String("Mikolaj Machowski"),
                          ki18n("Bug reports and patches").toString(),
                          QLatin1String("mikmach at wp dot pl"),
                          QLatin1String("https://www.facebook.com/mikolaj.machowski")
                        );

    aboutData.addCredit ( QLatin1String("Achim Bohnet"),
                          ki18n("Bug reports and patches").toString(),
                          QLatin1String("ach at mpe dot mpg dot de"),
                          QLatin1String("https://www.facebook.com/achim.bohnet")
                        );

    aboutData.addCredit ( QLatin1String("Luka Renko"),
                          ki18n("Developer").toString(),
                          QLatin1String("lure at kubuntu dot org"),
                          QLatin1String("https://www.facebook.com/luka.renko")
                        );

    aboutData.addCredit ( QLatin1String("Angelo Naselli"),
                          ki18n("Developer").toString(),
                          QLatin1String("a dot naselli at libero dot it"),
                          QLatin1String("https://www.linkedin.com/in/angelo-naselli-11199028/")
                        );

    aboutData.addCredit ( QLatin1String("Fabien Salvi"),
                          ki18n("Webmaster").toString(),
                          QLatin1String("fabien dot ubuntu at gmail dot com")
                        );

    aboutData.addCredit ( QLatin1String("Todd Shoemaker"),
                          ki18n("Developer").toString(),
                          QLatin1String("todd at theshoemakers dot net")
                        );

    aboutData.addCredit ( QLatin1String("Gerhard Kulzer"),
                          ki18n("Handbook writer, alpha tester, webmaster").toString(),
                          QLatin1String("gerhard at kulzer dot net"),
                          QLatin1String("https://www.linkedin.com/in/gerhard-kulzer-8931301/")
                        );

    aboutData.addCredit ( QLatin1String("Oliver Doerr"),
                          ki18n("Beta tester").toString(),
                          QLatin1String("oliver at doerr-privat dot de")
                        );

    aboutData.addCredit ( QLatin1String("Charles Bouveyron"),
                          ki18n("Beta tester").toString(),
                          QLatin1String("c dot bouveyron at tuxfamily dot org")
                        );

    aboutData.addCredit ( QLatin1String("Richard Taylor"),
                          ki18n("Feedback and patches. Handbook writer").toString(),
                          QLatin1String("rjt-digicam at thegrindstone dot me dot uk")
                        );

    aboutData.addCredit ( QLatin1String("Hans Karlsson"),
                          ki18n("digiKam website banner and application icons").toString(),
                          QLatin1String("karlsson dot h at home dot se")
                        );

    aboutData.addCredit ( QLatin1String("Aaron Seigo"),
                          ki18n("Various usability fixes and general application polishing").toString(),
                          QLatin1String("aseigo at kde dot org"),
                          QLatin1String("https://www.linkedin.com/in/aaronseigo/")
                        );

    aboutData.addCredit ( QLatin1String("Yves Chaufour"),
                          ki18n("digiKam website, Feedback").toString(),
                          QLatin1String("yves dot chaufour at wanadoo dot fr")
                        );

    aboutData.addCredit ( QLatin1String("Tung Nguyen"),
                          ki18n("Bug reports, feedback and icons").toString(),
                          QLatin1String("ntung at free dot fr")
                        );

    // -- Former Members ---------------------------------------------------------

    aboutData.addAuthor ( QLatin1String("Renchi Raju"),
                          ki18n("Developer (2001-2005)").toString(),
                          QLatin1String("renchi dot raju at gmail dot com"),
                          QLatin1String("https://www.facebook.com/renchi.raju")
                        );

    aboutData.addAuthor ( QLatin1String("Joern Ahrens"),
                          ki18n("Developer (2004-2005)").toString(),
                          QLatin1String("kde at jokele dot de"),
                          QLatin1String("http://www.jokele.de/")        // krazy:exclude=insecurenet
                        );

    aboutData.addAuthor ( QLatin1String("Tom Albers"),
                          ki18n("Developer (2004-2005)").toString(),
                          QLatin1String("tomalbers at kde dot nl"),
                          QLatin1String("https://www.linkedin.com/in/tom-a-676a58157/")
                        );

    aboutData.addAuthor ( QLatin1String("Ralf Holzer"),
                          ki18n("Developer (2004)").toString(),
                          QLatin1String("kde at ralfhoelzer dot com")
                        );

    aboutData.addAuthor ( QLatin1String("Marcel Wiesweg"),
                          ki18n("Developer and Mentoring (2005-2012)").toString(),
                          QLatin1String("marcel dot wiesweg at gmx dot de"),
                          QLatin1String("https://www.facebook.com/marcel.wiesweg")
                        );

}

} // namespace Digikam

#include "moc_daboutdata.cpp"
