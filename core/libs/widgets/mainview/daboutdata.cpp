/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-07-30
 * Description : digiKam about data.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
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

DAboutData::~DAboutData()
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
    return i18n("(c) 2001-2021, digiKam developers team");
}

const QUrl DAboutData::webProjectUrl()
{
    return QUrl(QLatin1String("https://www.digikam.org"));
}

void DAboutData::authorsRegistration(KAboutData& aboutData)
{
    // -- Core team --------------------------------------------------------------

    aboutData.addAuthor ( ki18n("Caulier Gilles").toString(),
                          ki18n("Coordinator, Developer, and Mentoring").toString(),
                          QLatin1String("caulier dot gilles at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/gilles-caulier/")
                        );

    aboutData.addAuthor ( ki18n("Maik Qualmann").toString(),
                          ki18n("Developer and Mentoring").toString(),
                          QLatin1String("metzpinguin at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/maik-qualmann-2b266717b/")
                        );

    aboutData.addAuthor ( ki18n("Marcel Wiesweg").toString(),
                          ki18n("Developer and Mentoring").toString(),
                          QLatin1String("marcel dot wiesweg at gmx dot de"),
                          QLatin1String("https://www.facebook.com/marcel.wiesweg")
                        );

    aboutData.addAuthor ( ki18n("Mohamed Anwer").toString(),                                    // krazy:exclude=spelling
                          ki18n("Developer and Mentoring").toString(),
                          QLatin1String("mohammed dot ahmed dot anwer at gmail dot com"),       // krazy:exclude=spelling
                          QLatin1String("https://www.linkedin.com/in/mohamedanwer/")
                        );

    aboutData.addAuthor ( ki18n("Michael G. Hansen").toString(),
                          ki18n("Developer and Mentoring").toString(),
                          QLatin1String("mike at mghansen dot de"),
                          QLatin1String("http://www.mghansen.de")           // krazy:exclude=insecurenet
                        );

    aboutData.addAuthor ( ki18n("Teemu Rytilahti").toString(),
                          ki18n("Developer").toString(),
                          QLatin1String("tpr at iki dot fi"),
                          QLatin1String("https://www.linkedin.com/in/teemurytilahti/")
                        );

    aboutData.addAuthor ( ki18n("Thanh Trung Dinh").toString(),
                          ki18n("Mentoring, Port web-service tools to OAuth, factoring web service tools, "
                                "and port faces recognition engine to OpenCV neural network").toString(),
                          QLatin1String("dinhthanhtrung1996 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/thanhtrungdinh/")
                        );

    // -- Contributors -----------------------------------------------------------

    aboutData.addAuthor ( ki18n("Matthias Welwarsky").toString(),
                          ki18n("Developer").toString(),
                          QLatin1String("matze at welwarsky dot de"),
                          QLatin1String("https://www.linkedin.com/in/matthiaswelwarsky/")
                        );

    aboutData.addAuthor ( ki18n("Julien Narboux").toString(),
                          ki18n("Developer").toString(),
                          QLatin1String("Julien at narboux dot fr"),
                          QLatin1String("https://www.linkedin.com/in/julien-narboux-17566610/")
                        );

    aboutData.addAuthor ( ki18n("Mario Frank").toString(),
                          ki18n("Advanced Searches Tool Improvements").toString(),
                          QLatin1String("mario.frank@uni-potsdam.de")
                        );

    aboutData.addAuthor ( ki18n("Nicolas Lécureuil").toString(),
                          ki18n("Releases Manager").toString(),
                          QLatin1String("neoclust dot kde at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/nicolaslecureuil/")
                        );

    // -- Students ---------------------------------------------------------------


    aboutData.addCredit ( ki18n("Minh Nghĩa Duong").toString(),
                          ki18n("Clustering support and improvement for Face Engine, "
                                "Port Slideshow tool to plugins interface").toString(),
                          QLatin1String("minhnghiaduong997 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/minh-nghia-duong-2b5bbb15a/")
                        );

    aboutData.addCredit ( ki18n("Kartik Ramesh").toString(),
                          ki18n("Face management workflow improvements").toString(),
                          QLatin1String("kartikx2000 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/kartikxramesh/")
                        );

    aboutData.addCredit ( ki18n("Ahmed Fathi").toString(),
                          ki18n("UPNP/DLNA export tool, and Healing clone tool for image editor").toString(),
                          QLatin1String("ahmed dot fathi dot abdelmageed at gmail dot com"),
                          QLatin1String("https://ahmedfathishabanblog.wordpress.com/")
                        );

    aboutData.addCredit ( ki18n("Veaceslav Munteanu").toString(),
                          ki18n("Tags Manager").toString(),
                          QLatin1String("veaceslav dot munteanu90 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/veaceslav-munteanu-4370a063/")
                        );

    aboutData.addCredit ( ki18n("Tarek Talaat").toString(),
                          ki18n("New OneDrive, Pinterrest, and Box export tools").toString(),
                          QLatin1String("tarektalaat93 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/tarek-talaat-9bb5b1a6/")
                        );

    aboutData.addCredit ( ki18n("Yingjie Liu").toString(),
                          ki18n("Face-engine improvements and manual icon-view sort").toString(),
                          QLatin1String("yingjiewudi at gmail dot com"),
                          QLatin1String("https://yjwudi.github.io")
                        );

    aboutData.addCredit ( ki18n("Yiou Wang").toString(),
                          ki18n("Model/View Port of Image Editor Canvas").toString(),
                          QLatin1String("geow812 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/yiouwang/")
                        );

    aboutData.addCredit ( ki18n("Gowtham Ashok").toString(),
                          ki18n("Image Quality Sorter").toString(),
                          QLatin1String("gwty93 at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/gowtham-ashok/")
                        );

    aboutData.addCredit ( ki18n("Aditya Bhatt").toString(),
                          ki18n("Face Detection").toString(),
                          QLatin1String("aditya at bhatts dot org"),
                          QLatin1String("https://www.linkedin.com/in/adityabhatt/")
                        );

    aboutData.addCredit ( ki18n("Martin Klapetek").toString(),
                          ki18n("Non-destructive image editing").toString(),
                          QLatin1String("martin dot klapetek at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/mck182/")
                        );

    aboutData.addCredit ( ki18n("Gabriel Voicu").toString(),
                          ki18n("Reverse Geo-Coding").toString(),
                          QLatin1String("ping dot gabi at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/gabriel-voicu-958b8327/")
                        );

    aboutData.addCredit ( ki18n("Mahesh Hegde").toString(),
                          ki18n("Face Recognition").toString(),
                          QLatin1String("maheshmhegade at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/maheshmhegade/")
                        );

    aboutData.addCredit ( ki18n("Pankaj Kumar").toString(),
                          ki18n("Multi-core Support in Batch Queue Manager and Mentoring").toString(),
                          QLatin1String("me at panks dot me"),
                          QLatin1String("https://www.linkedin.com/in/panks42/")
                        );

    aboutData.addCredit ( ki18n("Smit Mehta").toString(),
                          ki18n("UPnP / DLNA Export tool and Mentoring").toString(),
                          QLatin1String("smit dot tmeh at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/smit-mehta-45b82640/")
                        );

    aboutData.addCredit ( ki18n("Islam Wazery").toString(),
                          ki18n("Model/View port of Import Tool and Mentoring").toString(),
                          QLatin1String("wazery at ubuntu dot com"),
                          QLatin1String("https://www.linkedin.com/in/wazery/")
                        );

    aboutData.addCredit ( ki18n("Abhinav Badola").toString(),
                          ki18n("Video Metadata Support and Mentoring").toString(),
                          QLatin1String("mail dot abu dot to at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/abhinav-badola-86085424/")
                        );

    aboutData.addCredit ( ki18n("Benjamin Girault").toString(),
                          ki18n("Panorama Tool and Mentoring").toString(),
                          QLatin1String("benjamin dot girault at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/benjamingirault/")
                        );

    aboutData.addCredit ( ki18n("Victor Dodon").toString(),
                          ki18n("XML based GUI port of tools").toString(),
                          QLatin1String("dodonvictor at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/dodonvictor/")
                        );

    aboutData.addCredit ( ki18n("Sayantan Datta").toString(),
                          ki18n("Auto Noise Reduction").toString(),
                          QLatin1String("sayantan dot knz at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/stndta/")
                        );

    // -- Former contributors ----------------------------------------------------

    aboutData.addAuthor ( ki18n("Ananta Palani").toString(),
                          ki18n("Windows Port and Release Manager").toString(),
                          QLatin1String("anantapalani at gmail dot com"),
                          QLatin1String("https://www.linkedin.com/in/anantapalani/")
                        );

    aboutData.addAuthor ( ki18n("Andi Clemens").toString(),
                          ki18n("Developer").toString(),
                          QLatin1String("andi dot clemens at gmail dot com")
                        );

    aboutData.addAuthor ( ki18n("Patrick Spendrin").toString(),
                          ki18n("Developer and Windows port").toString(),
                          QLatin1String("patrick_spendrin at gmx dot de"),
                          QLatin1String("https://www.linkedin.com/in/patrickspendrin/")
                        );

    aboutData.addCredit ( ki18n("Francesco Riosa").toString(),
                          ki18n("LCMS2 library port").toString(),
                          QLatin1String("francesco plus kde at pnpitalia dot it"),
                          QLatin1String("https://www.linkedin.com/in/vivogentoo/")
                        );

    aboutData.addCredit ( ki18n("Johannes Wienke").toString(),
                          ki18n("Developer").toString(),
                          QLatin1String("languitar at semipol dot de"),
                          QLatin1String("https://www.facebook.com/languitar")
                        );

    aboutData.addAuthor ( ki18n("Julien Pontabry").toString(),
                          ki18n("Developer").toString(),
                          QLatin1String("julien dot pontabry at ulp dot u-strasbg dot fr"),
                          QLatin1String("https://www.linkedin.com/in/julien-pontabry-b784a247/")
                        );

    aboutData.addAuthor ( ki18n("Arnd Baecker").toString(),
                          ki18n("Developer").toString(),
                          QLatin1String("arnd dot baecker at web dot de")
                        );

    aboutData.addAuthor ( ki18n("Francisco J. Cruz").toString(),
                          ki18n("Color Management").toString(),
                          QLatin1String("fj dot cruz at supercable dot es")
                        );

    aboutData.addCredit ( ki18n("Pieter Edelman").toString(),
                          ki18n("Developer").toString(),
                          QLatin1String("p dot edelman at gmx dot net"),
                          QLatin1String("https://www.facebook.com/pieter.edelman")
                        );

    aboutData.addCredit ( ki18n("Holger Foerster").toString(),
                          ki18n("MySQL interface").toString(),
                          QLatin1String("hamsi2k at freenet dot de")
                        );

    aboutData.addCredit ( ki18n("Risto Saukonpaa").toString(),
                          ki18n("Design, icons, logo, banner, mockup, beta tester").toString(),
                          QLatin1String("paristo at gmail dot com")
                        );

    aboutData.addCredit ( ki18n("Mikolaj Machowski").toString(),
                          ki18n("Bug reports and patches").toString(),
                          QLatin1String("mikmach at wp dot pl"),
                          QLatin1String("https://www.facebook.com/mikolaj.machowski")
                        );

    aboutData.addCredit ( ki18n("Achim Bohnet").toString(),
                          ki18n("Bug reports and patches").toString(),
                          QLatin1String("ach at mpe dot mpg dot de"),
                          QLatin1String("https://www.facebook.com/achim.bohnet")
                        );

    aboutData.addCredit ( ki18n("Luka Renko").toString(),
                          ki18n("Developer").toString(),
                          QLatin1String("lure at kubuntu dot org"),
                          QLatin1String("https://www.facebook.com/luka.renko")
                        );

    aboutData.addCredit ( ki18n("Angelo Naselli").toString(),
                          ki18n("Developer").toString(),
                          QLatin1String("a dot naselli at libero dot it"),
                          QLatin1String("https://www.linkedin.com/in/angelo-naselli-11199028/")
                        );

    aboutData.addCredit ( ki18n("Fabien Salvi").toString(),
                          ki18n("Webmaster").toString(),
                          QLatin1String("fabien dot ubuntu at gmail dot com")
                        );

    aboutData.addCredit ( ki18n("Todd Shoemaker").toString(),
                          ki18n("Developer").toString(),
                          QLatin1String("todd at theshoemakers dot net")
                        );

    aboutData.addCredit ( ki18n("Gerhard Kulzer").toString(),
                          ki18n("Handbook writer, alpha tester, webmaster").toString(),
                          QLatin1String("gerhard at kulzer dot net"),
                          QLatin1String("https://www.linkedin.com/in/gerhard-kulzer-8931301/")
                        );

    aboutData.addCredit ( ki18n("Oliver Doerr").toString(),
                          ki18n("Beta tester").toString(),
                          QLatin1String("oliver at doerr-privat dot de")
                        );

    aboutData.addCredit ( ki18n("Charles Bouveyron").toString(),
                          ki18n("Beta tester").toString(),
                          QLatin1String("c dot bouveyron at tuxfamily dot org")
                        );

    aboutData.addCredit ( ki18n("Richard Taylor").toString(),
                          ki18n("Feedback and patches. Handbook writer").toString(),
                          QLatin1String("rjt-digicam at thegrindstone dot me dot uk")
                        );

    aboutData.addCredit ( ki18n("Hans Karlsson").toString(),
                          ki18n("digiKam website banner and application icons").toString(),
                          QLatin1String("karlsson dot h at home dot se")
                        );

    aboutData.addCredit ( ki18n("Aaron Seigo").toString(),
                          ki18n("Various usability fixes and general application polishing").toString(),
                          QLatin1String("aseigo at kde dot org"),
                          QLatin1String("https://www.linkedin.com/in/aaronseigo/")
                        );

    aboutData.addCredit ( ki18n("Yves Chaufour").toString(),
                          ki18n("digiKam website, Feedback").toString(),
                          QLatin1String("yves dot chaufour at wanadoo dot fr")
                        );

    aboutData.addCredit ( ki18n("Tung Nguyen").toString(),
                          ki18n("Bug reports, feedback and icons").toString(),
                          QLatin1String("ntung at free dot fr")
                        );

    // -- Former Members ---------------------------------------------------------

    aboutData.addAuthor ( ki18n("Renchi Raju").toString(),
                          ki18n("Developer (2001-2005)").toString(),
                          QLatin1String("renchi dot raju at gmail dot com"),
                          QLatin1String("https://www.facebook.com/renchi.raju")
                        );

    aboutData.addAuthor ( ki18n("Joern Ahrens").toString(),
                          ki18n("Developer (2004-2005)").toString(),
                          QLatin1String("kde at jokele dot de"),
                          QLatin1String("http://www.jokele.de/")        // krazy:exclude=insecurenet
                        );

    aboutData.addAuthor ( ki18n("Tom Albers").toString(),
                          ki18n("Developer (2004-2005)").toString(),
                          QLatin1String("tomalbers at kde dot nl"),
                          QLatin1String("https://www.linkedin.com/in/tom-a-676a58157/")
                        );

    aboutData.addAuthor ( ki18n("Ralf Holzer").toString(),
                          ki18n("Developer (2004)").toString(),
                          QLatin1String("kde at ralfhoelzer dot com")
                        );
}

} // namespace Digikam
