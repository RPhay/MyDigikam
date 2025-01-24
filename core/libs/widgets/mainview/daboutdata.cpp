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
                 "<b>Professional</b> Photo <b>Management</b><br/>with the Power of <b>Open Source</b>"
                 "</font></qt>"
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
                          i18n("Coordinator, Developer, and Mentoring"),
                          QLatin1String("caulier dot gilles at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/gilles-caulier/")
                        );

    aboutData.addAuthor ( QLatin1String("Maik Qualmann"),
                          i18n("Developer and Mentoring"),
                          QLatin1String("metzpinguin at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/maik-qualmann-2b266717b/")
                        );

    aboutData.addAuthor ( QLatin1String("Michael Miller"),
                          i18n("SFace and YuNet DNN models integration\n"
                               "in face workflow.\n"
                               "Face Management and auto-tag AI improvements.\n"
                               "AI red eyes detection"),
                          QLatin1String("michael underscore miller at msn dot com"),
                          QLatin1String("https://www.linkedin.com/in/michaelamiller01/")
                        );

    aboutData.addAuthor ( QLatin1String("Mohamed Anwer"),                                       // krazy:exclude=spelling
                          i18n("Developer and Mentoring"),
                          QLatin1String("mohammed dot ahmed dot anwer at gmail dot com"),       // krazy:exclude=spelling
                          QLatin1String("https://www.linkedin.com/in/mohamedanwer/")
                        );

    aboutData.addAuthor ( QLatin1String("Michael G. Hansen"),
                          i18n("Developer and Mentoring"),
                          QLatin1String("mike at mghansen dot de"),
                          QLatin1String("http://www.mghansen.de")                               // krazy:exclude=insecurenet
                        );

    aboutData.addAuthor ( QLatin1String("Teemu Rytilahti"),
                          i18n("Developer"),
                          QLatin1String("tpr at iki dot fi"),
                          QLatin1String("https://www.linkedin.com/in/teemurytilahti/")
                        );

    aboutData.addAuthor ( QLatin1String("Thanh Trung Dinh"),
                          i18n("Mentoring, Port web-service tools to OAuth,\n"
                               "factoring web service tools,\n"
                               "and port faces recognition engine\n"
                               "to OpenCV neural network"),
                          QLatin1String("dinhthanhtrung1996 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/thanhtrungdinh/")
                        );


    // -- Contributors -----------------------------------------------------------

    aboutData.addAuthor ( QLatin1String("Matthias Welwarsky"),
                          i18n("Developer"),
                          QLatin1String("matze at welwarsky dot de"),
                          QLatin1String("https://www.linkedin.com/in/matthiaswelwarsky/")
                        );

    aboutData.addAuthor ( QLatin1String("Julien Narboux"),
                          i18n("Developer"),
                          QLatin1String("Julien at narboux dot fr"),
                          QLatin1String("https://www.linkedin.com/in/julien-narboux-17566610/")
                        );

    aboutData.addAuthor ( QLatin1String("Mario Frank"),
                          i18n("Advanced Searches Tool Improvements"),
                          QLatin1String("mario.frank@uni-potsdam.de")
                        );

    aboutData.addAuthor ( QLatin1String("Nicolas Lécureuil"),
                          i18n("Releases Manager"),
                          QLatin1String("neoclust dot kde at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/nicolaslecureuil/")
                        );

    // -- Students ---------------------------------------------------------------

    aboutData.addCredit ( QLatin1String("Anjani Kumar"),
                          i18n("Port to Qt6,"),
                          QLatin1String("anjanik012 at gmail dot com "),
                          QLatin1String("https://www.linkedin.com/in/anjanik012/")
                        );

    aboutData.addCredit ( QLatin1String("Phuoc Khanh LE"),
                          i18n("Rewrite Image Quality Sorter algorithms\n"
                               "and Photo Focus Information Extraction.\n"
                               "Aesthetic Detection to classify images\n"
                               "using deep learning model.\n"
                               "Slideshow and Presentation tools improvement"),
                          QLatin1String("phuockhanhnk94 at gmail dot com "),
                          QLatin1String("https://www.linkedin.com/in/phuoc-khanh-le-476448169/")
                        );

    aboutData.addCredit ( QLatin1String("Quốc Hưng Trần"),
                          i18n("MJPEGStream plugin,\n"
                               "OCR text Converter plugin,\n"
                               "auto-tags assignment engine"),
                          QLatin1String("quochungtran1999 at gmail dot com "),
                          QLatin1String("https://www.linkedin.com/in/qu%E1%BB%91c-h%C6%B0ng-tr%E1%BA%A7n-92a504163/")
                        );

    aboutData.addCredit ( QLatin1String("Minh Nghĩa Duong"),
                          i18n("Face workflow Clustering support,\n"
                               "improvement for Face Engine,\n"
                               "Port Slideshow tool to plugins interface"),
                          QLatin1String("minhnghiaduong997 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/minh-nghia-duong-2b5bbb15a/")
                        );

    aboutData.addCredit ( QLatin1String("Kartik Ramesh"),
                          i18n("Face management workflow improvements"),
                          QLatin1String("kartikx2000 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/kartikxramesh/")
                        );

    aboutData.addCredit ( QLatin1String("Ahmed Fathi"),
                          i18n("UPNP/DLNA export tool\nand Healing clone tool for image editor"),
                          QLatin1String("ahmed dot fathi dot abdelmageed at gmail dot com"),
                          QLatin1String("https://ahmedfathishabanblog.wordpress.com/")
                        );

    aboutData.addCredit ( QLatin1String("Veaceslav Munteanu"),
                          i18n("Tags Manager"),
                          QLatin1String("veaceslav dot munteanu90 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/veaceslav-munteanu-4370a063/")
                        );

    aboutData.addCredit ( QLatin1String("Tarek Talaat"),
                          i18n("New OneDrive, Pinterrest, and Box export tools"),
                          QLatin1String("tarektalaat93 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/tarek-talaat-9bb5b1a6/")
                        );

    aboutData.addCredit ( QLatin1String("Yingjie Liu"),
                          i18n("Face-engine improvements\nand manual icon-view sort"),
                          QLatin1String("yingjiewudi at gmail dot com"),
                          QLatin1String("https://yjwudi.github.io")
                        );

    aboutData.addCredit ( QLatin1String("Yiou Wang"),
                          i18n("Model/View Port of Image Editor Canvas"),
                          QLatin1String("geow812 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/yiouwang/")
                        );

    aboutData.addCredit ( QLatin1String("Gowtham Ashok"),
                          i18n("Image Quality Sorter"),
                          QLatin1String("gwty93 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/gowtham-ashok/")
                        );

    aboutData.addCredit ( QLatin1String("Aditya Bhatt"),
                          i18n("Face Detection"),
                          QLatin1String("aditya at bhatts dot org"),
                          QLatin1String("https://www.linkedin.com/in/adityabhatt/")
                        );

    aboutData.addCredit ( QLatin1String("Martin Klapetek"),
                          i18n("Non-destructive image editing"),
                          QLatin1String("martin dot klapetek at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/mck182/")
                        );

    aboutData.addCredit ( QLatin1String("Gabriel Voicu"),
                          i18n("Reverse Geo-Coding"),
                          QLatin1String("ping dot gabi at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/gabriel-voicu-958b8327/")
                        );

    aboutData.addCredit ( QLatin1String("Mahesh Hegde"),
                          i18n("Face Recognition"),
                          QLatin1String("maheshmhegade at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/maheshmhegade/")
                        );

    aboutData.addCredit ( QLatin1String("Pankaj Kumar"),
                          i18n("Multi-core Support in Batch Queue Manager and Mentoring"),
                          QLatin1String("me at panks dot me"),
                          QLatin1String("https://www.linkedin.com/in/panks42/")
                        );

    aboutData.addCredit ( QLatin1String("Smit Mehta"),
                          i18n("UPnP/DLNA Export tool and Mentoring"),
                          QLatin1String("smit dot tmeh at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/smit-mehta-45b82640/")
                        );

    aboutData.addCredit ( QLatin1String("Islam Wazery"),
                          i18n("Model/View port of Import Tool and Mentoring"),
                          QLatin1String("wazery at ubuntu dot com"),
                          QLatin1String("https://www.linkedin.com/in/wazery/")
                        );

    aboutData.addCredit ( QLatin1String("Abhinav Badola"),
                          i18n("Video Metadata Support and Mentoring"),
                          QLatin1String("mail dot abu dot to at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/abhinav-badola-86085424/")
                        );

    aboutData.addCredit ( QLatin1String("Benjamin Girault"),
                          i18n("Panorama Tool and Mentoring"),
                          QLatin1String("benjamin dot girault at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/benjamingirault/")
                        );

    aboutData.addCredit ( QLatin1String("Victor Dodon"),
                          i18n("XML based GUI port of tools"),
                          QLatin1String("dodonvictor at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/dodonvictor/")
                        );

    aboutData.addCredit ( QLatin1String("Sayantan Datta"),
                          i18n("Auto Noise Reduction"),
                          QLatin1String("sayantan dot knz at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/stndta/")
                        );

    // -- Former contributors ----------------------------------------------------

    aboutData.addAuthor ( QLatin1String("Ananta Palani"),
                          i18n("Windows Port and Release Manager"),
                          QLatin1String("anantapalani at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/anantapalani/")
                        );

    aboutData.addAuthor ( QLatin1String("Andi Clemens"),
                          i18n("Developer"),
                          QLatin1String("andi dot clemens at gmail dot com")
                        );

    aboutData.addAuthor ( QLatin1String("Patrick Spendrin"),
                          i18n("Developer and Windows port"),
                          QLatin1String("patrick_spendrin at gmx dot de"),
                          QLatin1String("https://www.linkedin.com/in/patrickspendrin/")
                        );

    aboutData.addCredit ( QLatin1String("Francesco Riosa"),
                          i18n("LCMS2 library port"),
                          QLatin1String("francesco plus kde at pnpitalia dot it"),
                          QLatin1String("https://www.linkedin.com/in/vivogentoo/")
                        );

    aboutData.addCredit ( QLatin1String("Johannes Wienke"),
                          i18n("Developer"),
                          QLatin1String("languitar at semipol dot de"),
                          QLatin1String("https://www.facebook.com/languitar")
                        );

    aboutData.addAuthor ( QLatin1String("Julien Pontabry"),
                          i18n("Developer"),
                          QLatin1String("julien dot pontabry at ulp dot u-strasbg dot fr"),
                          QLatin1String("https://www.linkedin.com/in/julien-pontabry-b784a247/")
                        );

    aboutData.addAuthor ( QLatin1String("Arnd Baecker"),
                          i18n("Developer"),
                          QLatin1String("arnd dot baecker at web dot de")
                        );

    aboutData.addAuthor ( QLatin1String("Francisco J. Cruz"),
                          i18n("Color Management"),
                          QLatin1String("fj dot cruz at supercable dot es")
                        );

    aboutData.addCredit ( QLatin1String("Pieter Edelman"),
                          i18n("Developer"),
                          QLatin1String("p dot edelman at gmx dot net"),
                          QLatin1String("https://www.facebook.com/pieter.edelman")
                        );

    aboutData.addCredit ( QLatin1String("Holger Foerster"),
                          i18n("MySQL interface"),
                          QLatin1String("hamsi2k at freenet dot de")
                        );

    aboutData.addCredit ( QLatin1String("Risto Saukonpaa"),
                          i18n("Design, icons, logo, banner, mockup, beta tester"),
                          QLatin1String("paristo at gmail dot com")
                        );

    aboutData.addCredit ( QLatin1String("Mikolaj Machowski"),
                          i18n("Bug reports and patches"),
                          QLatin1String("mikmach at wp dot pl"),
                          QLatin1String("https://www.facebook.com/mikolaj.machowski")
                        );

    aboutData.addCredit ( QLatin1String("Achim Bohnet"),
                          i18n("Bug reports and patches"),
                          QLatin1String("ach at mpe dot mpg dot de"),
                          QLatin1String("https://www.facebook.com/achim.bohnet")
                        );

    aboutData.addCredit ( QLatin1String("Luka Renko"),
                          i18n("Developer"),
                          QLatin1String("lure at kubuntu dot org"),
                          QLatin1String("https://www.facebook.com/luka.renko")
                        );

    aboutData.addCredit ( QLatin1String("Angelo Naselli"),
                          i18n("Developer"),
                          QLatin1String("a dot naselli at libero dot it"),
                          QLatin1String("https://www.linkedin.com/in/angelo-naselli-11199028/")
                        );

    aboutData.addCredit ( QLatin1String("Fabien Salvi"),
                          i18n("Webmaster"),
                          QLatin1String("fabien dot ubuntu at gmail dot com")
                        );

    aboutData.addCredit ( QLatin1String("Todd Shoemaker"),
                          i18n("Developer"),
                          QLatin1String("todd at theshoemakers dot net")
                        );

    aboutData.addCredit ( QLatin1String("Gerhard Kulzer"),
                          i18n("Handbook writer, alpha tester, webmaster"),
                          QLatin1String("gerhard at kulzer dot net"),
                          QLatin1String("https://www.linkedin.com/in/gerhard-kulzer-8931301/")
                        );

    aboutData.addCredit ( QLatin1String("Oliver Doerr"),
                          i18n("Beta tester"),
                          QLatin1String("oliver at doerr-privat dot de")
                        );

    aboutData.addCredit ( QLatin1String("Charles Bouveyron"),
                          i18n("Beta tester"),
                          QLatin1String("c dot bouveyron at tuxfamily dot org")
                        );

    aboutData.addCredit ( QLatin1String("Richard Taylor"),
                          i18n("Feedback and patches. Handbook writer"),
                          QLatin1String("rjt-digicam at thegrindstone dot me dot uk")
                        );

    aboutData.addCredit ( QLatin1String("Hans Karlsson"),
                          i18n("digiKam website banner and application icons"),
                          QLatin1String("karlsson dot h at home dot se")
                        );

    aboutData.addCredit ( QLatin1String("Aaron Seigo"),
                          i18n("Various usability fixes\nand general application polishing"),
                          QLatin1String("aseigo at kde dot org"),
                          QLatin1String("https://www.linkedin.com/in/aaronseigo/")
                        );

    aboutData.addCredit ( QLatin1String("Yves Chaufour"),
                          i18n("digiKam website, Feedback"),
                          QLatin1String("yves dot chaufour at wanadoo dot fr")
                        );

    aboutData.addCredit ( QLatin1String("Tung Nguyen"),
                          i18n("Bug reports, feedback and icons"),
                          QLatin1String("ntung at free dot fr")
                        );

    // -- Former Members ---------------------------------------------------------

    aboutData.addAuthor ( QLatin1String("Renchi Raju"),
                          i18n("Developer (2001-2005)"),
                          QLatin1String("renchi dot raju at gmail dot com"),
                          QLatin1String("https://www.facebook.com/renchi.raju")
                        );

    aboutData.addAuthor ( QLatin1String("Joern Ahrens"),
                          i18n("Developer (2004-2005)"),
                          QLatin1String("kde at jokele dot de"),
                          QLatin1String("http://www.jokele.de/")        // krazy:exclude=insecurenet
                        );

    aboutData.addAuthor ( QLatin1String("Tom Albers"),
                          i18n("Developer (2004-2005)"),
                          QLatin1String("tomalbers at kde dot nl"),
                          QLatin1String("https://www.linkedin.com/in/tom-a-676a58157/")
                        );

    aboutData.addAuthor ( QLatin1String("Ralf Holzer"),
                          i18n("Developer (2004)"),
                          QLatin1String("kde at ralfhoelzer dot com")
                        );

    aboutData.addAuthor ( QLatin1String("Marcel Wiesweg"),
                          i18n("Developer and Mentoring (2005-2012)"),
                          QLatin1String("marcel dot wiesweg at gmx dot de"),
                          QLatin1String("https://www.facebook.com/marcel.wiesweg")
                        );
}

} // namespace Digikam

#include "moc_daboutdata.cpp"
