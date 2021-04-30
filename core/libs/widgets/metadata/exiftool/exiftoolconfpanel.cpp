/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-29
 * Description : ExifTool configuration panel.
 *
 * Copyright (C) 2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "exiftoolconfpanel.h"

// Qt includes

#include <QGridLayout>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QLabel>
#include <QStringList>
#include <QGroupBox>
#include <QApplication>
#include <QStyle>
#include <QHeaderView>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "exiftoolbinary.h"
#include "exiftoolparser.h"
#include "dbinarysearch.h"
#include "metaenginesettings.h"

namespace Digikam
{

class Q_DECL_HIDDEN ExifToolConfPanel::Private
{
public:

    explicit Private()
      : exifToolBinWidget(nullptr),
        searchBar        (nullptr),
        exifToolFormats  (nullptr)
    {
    }

public:

    DBinarySearch* exifToolBinWidget;
    ExifToolBinary exifToolBin;
    SearchTextBar* searchBar;

    QTreeWidget*   exifToolFormats;
};

// --------------------------------------------------------

ExifToolConfPanel::ExifToolConfPanel(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
    QGridLayout* const grid        = new QGridLayout;
    QLabel* const exifToolBinLabel = new QLabel(i18nc("@info",
                                                "ExifTool is an open-source software program for reading, writing, "
                                                "and manipulating multimedia files. It is platform independent "
                                                "available as a command-line application. ExifTool is commonly "
                                                "incorporated into different types of digital workflows and supports "
                                                "many types of metadata including Exif, IPTC, XMP, JFIF, GeoTIFF, ICC Profile, "
                                                "Photoshop IRB, as well as the manufacturer-specific metadata formats of "
                                                "many digital cameras.\n\n"
                                                "Here you can configure location where ExifTool binary is located. "
                                                "Application will try to find this binary automatically if they are "
                                                "already installed on your computer."),
                                                this);
    exifToolBinLabel->setWordWrap(true);

    d->exifToolBinWidget         = new DBinarySearch(this);
    d->exifToolBinWidget->addBinary(d->exifToolBin);

    QGroupBox* const exifToolBox = new QGroupBox(i18n("Supported File Formats"), this);
    d->exifToolFormats           = new QTreeWidget(exifToolBox);
    d->exifToolFormats->setSortingEnabled(true);
    d->exifToolFormats->sortByColumn(0, Qt::AscendingOrder);
    d->exifToolFormats->setSelectionMode(QAbstractItemView::SingleSelection);
    d->exifToolFormats->setAllColumnsShowFocus(true);
    d->exifToolFormats->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->exifToolFormats->setColumnCount(4);
    d->exifToolFormats->setHeaderHidden(false);
    d->exifToolFormats->setHeaderLabels(QStringList() << i18n("Extension") << i18n("Read") << i18n("Write") << i18n("Description"));

    d->searchBar                 = new SearchTextBar(this, QLatin1String("ExifToolFormatsSearchBar"));

    QVBoxLayout* const vlay      = new QVBoxLayout(exifToolBox);
    vlay->addWidget(d->exifToolFormats);
    vlay->addWidget(d->searchBar);

    grid->addWidget(exifToolBinLabel,     0, 0, 1, 2);
    grid->addWidget(d->exifToolBinWidget, 1, 0, 1, 2);
    grid->addWidget(exifToolBox,          2, 0, 1, 2);
    grid->setRowStretch(2, 10);
    setLayout(grid);

    // ---

    connect(d->searchBar, SIGNAL(signalSearchTextSettings(SearchTextSettings)),
            this, SLOT(slotSearchTextChanged(SearchTextSettings)));

    connect(d->exifToolBinWidget, SIGNAL(signalBinariesFound(bool)),
            this, SLOT(slotExifToolBinaryFound(bool)));

    foreach (const QString& path, MetaEngineSettings::instance()->settings().defaultExifToolSearchPaths())
    {
        d->exifToolBinWidget->addDirectory(path);
    }

    d->exifToolBinWidget->allBinariesFound();
}

ExifToolConfPanel::~ExifToolConfPanel()
{
    delete d;
}

QString ExifToolConfPanel::exifToolDirectory() const
{
    return d->exifToolBin.directory();
}

void ExifToolConfPanel::setExifToolDirectory(const QString& dir)
{
    d->exifToolBin.setup(dir);
}

void ExifToolConfPanel::slotExifToolBinaryFound(bool found)
{
    if (!found)
    {
        return;
    }

    d->exifToolFormats->clear();
    ExifToolParser parser;
    parser.setExifToolProgram(exifToolDirectory());
    ExifToolParser::ExifToolData parsed;
    QStringList read;
    QStringList write;

    if (parser.readableFormats())
    {
        parsed = parser.currentData();
        read   = parsed.find(QLatin1String("READ_FORMATS")).value()[0].toStringList();
    }

    if (parser.writableFormats())
    {
        parsed = parser.currentData();
        write  = parsed.find(QLatin1String("WRITE_FORMATS")).value()[0].toStringList();
    }

    foreach (const QString& frm, read)
    {
        new QTreeWidgetItem(d->exifToolFormats, QStringList() << frm
                                                              << i18n("yes")
                                                              << (write.contains(frm) ? i18n("yes") : i18n("no"))
                                                              << formatDescription(frm));
    }
}

void ExifToolConfPanel::slotSearchTextChanged(const SearchTextSettings& settings)
{
    bool query     = false;
    int  results   = 0;
    QString search = settings.text.toLower();

    QTreeWidgetItemIterator it(d->exifToolFormats);

    while (*it)
    {
        QTreeWidgetItem* const item  = *it;

        if (item->text(0).toLower().contains(search, settings.caseSensitive) ||
            item->text(3).toLower().contains(search, settings.caseSensitive))
        {
            ++results;
            query = true;
            item->setHidden(false);
        }
        else
        {
            item->setHidden(true);
        }

        ++it;
    }

    d->searchBar->slotSearchResult(query);
}

QString ExifToolConfPanel::formatDescription(const QString& frm)
{
    // See description database here: https://github.com/exiftool/exiftool/blob/master/lib/Image/ExifTool.pm#L212

    if (frm == QLatin1String("3FR"))         return i18n("Hasselblad RAW");

    if ((frm == QLatin1String("3G2")) ||
        (frm == QLatin1String("3GP2")))      return i18n("3rd Gen. Partnership Project 2 a/v");

    if ((frm == QLatin1String("3GP")) ||
        (frm == QLatin1String("3GPP")))      return i18n("3rd Gen. Partnership Project a/v");

    if (frm == QLatin1String("A"))           return i18n("Unix static library code Archive");

    if (frm == QLatin1String("AA"))          return i18n("Audible Audiobook");

    if (frm == QLatin1String("AAE"))         return i18n("Apple edit information");

    if (frm == QLatin1String("AAX"))         return i18n("Audible Enhanced Audiobook");

    if (frm == QLatin1String("ACR"))         return i18n("American College of Radiology ACR-NEMA");

    if ((frm == QLatin1String("AFM"))  ||
        (frm == QLatin1String("ACFM")) ||
        (frm == QLatin1String("AMFM")))      return i18n("Adobe Composite / Multiple Master Font Metrics");

    if ((frm == QLatin1String("AI")) ||
        (frm == QLatin1String("AIT")))       return i18n("Adobe Illustrator");

    if ((frm == QLatin1String("AIFF")) ||
        (frm == QLatin1String("AIF"))  ||
        (frm == QLatin1String("AIFC")))      return i18n("Audio Interchange File Format");

    if (frm == QLatin1String("ALIAS"))       return i18n("MacOS file alias");

    if (frm == QLatin1String("APE"))         return i18n("Monkey's Audio");

    if (frm == QLatin1String("APNG"))        return i18n("Animated Portable Network Graphics");

    if (frm == QLatin1String("ARQ"))         return i18n("Sony Alpha Pixel-Shift RAW");

    if (frm == QLatin1String("ARW"))         return i18n("Sony Alpha RAW");

    if (frm == QLatin1String("ASF"))         return i18n("Microsoft Advanced Systems Format");

    if (frm == QLatin1String("AVC"))         return i18n("Advanced Video Connection");

    if (frm == QLatin1String("AVI"))         return i18n("Audio Video Interleaved");

    if (frm == QLatin1String("AVIF"))        return i18n("AV1 Image File Format");

    if (frm == QLatin1String("BMP"))         return i18n("Windows BitMaP");

    if (frm == QLatin1String("BPG"))         return i18n("Better Portable Graphics");

    if (frm == QLatin1String("BTF"))         return i18n("BigTIFF");

    if (frm == QLatin1String("BZ2"))         return i18n("BZIP2 archive");

    if (frm == QLatin1String("CHM"))         return i18n("Microsoft Compiled HTML format");

    if ((frm == QLatin1String("COS")) ||
        (frm == QLatin1String("NEWER")))     return i18n("Capture One Settings");

    if (frm == QLatin1String("CR2"))         return i18n("Canon RAW 2");

    if (frm == QLatin1String("CR3"))         return i18n("Canon RAW 3");

    if (frm == QLatin1String("CRM"))         return i18n("Canon RAW Movie");

    if ((frm == QLatin1String("CRW")) ||
        (frm == QLatin1String("CIFF")))      return i18n("Canon RAW Camera Image File Format");

    if (frm == QLatin1String("CS1"))         return i18n("Sinar CaptureShop 1-shot RAW");

    if (frm == QLatin1String("CSV"))         return i18n("Comma-Separated Values");

    if (frm == QLatin1String("CZI"))         return i18n("Zeiss Integrated Software RAW");

    if ((frm == QLatin1String("DCM")) ||
        (frm == QLatin1String("DC3")) ||
        (frm == QLatin1String("DIC")) ||
        (frm == QLatin1String("DICM")))      return i18n("DICOM - Digital Imaging and Communications in Medicine");

    if (frm == QLatin1String("DIB"))         return i18n("Device Independent Bitmap");

    if (frm == QLatin1String("DCX"))         return i18n("Multi-page PC Paintbrush");

    if (frm == QLatin1String("DCP"))         return i18n("DNG Camera Profile");

    if (frm == QLatin1String("DCR"))         return i18n("Kodak Digital Camera RAW");

    if (frm == QLatin1String("DEX"))         return i18n("Dalvik Executable format");

    if (frm == QLatin1String("DFONT"))       return i18n("Macintosh Data Fork Font");

    if (frm == QLatin1String("DIVX"))        return i18n("DivX media format");

    if ((frm == QLatin1String("DJVU")) ||
        (frm == QLatin1String("DJV")))       return i18n("DjVu image");

    if (frm == QLatin1String("DNG"))         return i18n("Digital Negative");

    if ((frm == QLatin1String("DOC")) ||
        (frm == QLatin1String("DOT")))       return i18n("Microsoft Word Document / Template");

    if ((frm == QLatin1String("DOCX")) ||
        (frm == QLatin1String("DOCM")))      return i18n("Office Open XML Document");

    if ((frm == QLatin1String("DOTX")) ||
        (frm == QLatin1String("DOTM")))      return i18n("Office Open XML Document Template");

    if (frm == QLatin1String("DPX"))         return i18n("Digital Picture Exchange");

    if (frm == QLatin1String("DR4"))         return i18n("Canon DPP version 4 Recipe");

    if ((frm == QLatin1String("DSS")) ||
        (frm == QLatin1String("DS2")))       return i18n("Digital Speech Standard");

    if (frm == QLatin1String("DYLIB"))       return i18n("MacOS Mach-O executable and library files");

    if (frm == QLatin1String("DV"))          return i18n("Digital Video");

    if (frm == QLatin1String("DVB"))         return i18n("Digital Video Broadcasting");

    if (frm == QLatin1String("DVR-MS"))      return i18n("Microsoft Digital Video Recording");

    if (frm == QLatin1String("DWF"))         return i18n("Autodesk drawing (Design Web Format)");

    if (frm == QLatin1String("DWG"))         return i18n("AutoCAD Drawing");

    if (frm == QLatin1String("EIP"))         return i18n("Capture One Enhanced Image Package");

    if ((frm == QLatin1String("EPS"))  ||
        (frm == QLatin1String("EPS2")) ||
        (frm == QLatin1String("EPS3")) ||
        (frm == QLatin1String("EPSF")) ||
        (frm == QLatin1String("PS"))   ||
        (frm == QLatin1String("PS2"))  ||
        (frm == QLatin1String("PS3")))       return i18n("Encapsulated PostScript Format");

    if (frm == QLatin1String("EPUB"))        return i18n("Electronic Publication");

    if (frm == QLatin1String("ERF"))         return i18n("Epson RAW Format");

    if ((frm == QLatin1String("EXE")) ||
        (frm == QLatin1String("DLL")))       return i18n("DOS/Windows executable and library files");

    if (frm == QLatin1String("EXIF"))        return i18n("Exchangeable Image File Format metadata");

    if (frm == QLatin1String("EXR"))         return i18n("Open EXR");

    if (frm == QLatin1String("EXV"))         return i18n("Exiv2 metadata file");

    if ((frm == QLatin1String("F4A")) ||
        (frm == QLatin1String("F4B")) ||
        (frm == QLatin1String("F4P")) ||
        (frm == QLatin1String("F4V")))       return i18n("Adobe Flash Player 9+ Audio / Video");

    if (frm == QLatin1String("FFF"))         return i18n("Hasselblad Flexible File Format");

    if (frm == QLatin1String("FFF"))         return i18n("FLIR Systems thermal image File Format");

    if ((frm == QLatin1String("FIT")) ||
        (frm == QLatin1String("FITS")))      return i18n("Flexible Image Transport System");

    if (frm == QLatin1String("FLA"))         return i18n("Macromedia / Adobe Flash project");

    if (frm == QLatin1String("FLAC"))        return i18n("Free Lossless Audio Codec");

    if (frm == QLatin1String("FLIF"))        return i18n("Free Lossless Image Format");

    if (frm == QLatin1String("FLIR"))        return i18n("FLIR File Format");

    if (frm == QLatin1String("FLV"))         return i18n("Flash Video");

    if (frm == QLatin1String("FPF"))         return i18n("FLIR Public image Format");

    if (frm == QLatin1String("FPX"))         return i18n("FlashPix image");

    if (frm == QLatin1String("GIF"))         return i18n("Compuserve Graphics Interchange Format");

    if (frm == QLatin1String("GPR"))         return i18n("GoPro RAW");

    if ((frm == QLatin1String("GZ")) ||
        (frm == QLatin1String("GZIP")))      return i18n("GNU ZIP compressed archive");

    if ((frm == QLatin1String("HDP")) ||
        (frm == QLatin1String("WDP")) ||
        (frm == QLatin1String("JXR")))       return i18n("Windows HD Photo / Media Photo / JPEG XR");

    if (frm == QLatin1String("HDR"))         return i18n("Radiance RGBE High Dynamic-Range");

    if ((frm == QLatin1String("HEIC")) ||
        (frm == QLatin1String("HEIF")) ||
        (frm == QLatin1String("HIF")))       return i18n("High Efficiency Image Format");

    if ((frm == QLatin1String("HTML")) ||
        (frm == QLatin1String("HTM"))  ||
        (frm == QLatin1String("XHTML")))     return i18n("Extensible HyperText Markup Language");

    if ((frm == QLatin1String("ICC")) ||
        (frm == QLatin1String("ICM")))       return i18n("International Color Consortium color profile");

    if ((frm == QLatin1String("ICS")) ||
        (frm == QLatin1String("ICAL")))      return i18n("iCalendar Schedule");

    if (frm == QLatin1String("IDML"))        return i18n("Adobe InDesign Markup Language");

    if (frm == QLatin1String("IIQ"))         return i18n("Phase One Intelligent Image Quality");

    if ((frm == QLatin1String("IND"))  ||
        (frm == QLatin1String("INDD")) ||
        (frm == QLatin1String("INDT")))      return i18n("Adobe InDesign Document / Template");

    if (frm == QLatin1String("INSP"))        return i18n("Insta360 Picture");

    if (frm == QLatin1String("INSV"))        return i18n("Insta360 Video");

    if (frm == QLatin1String("INX"))         return i18n("Adobe InDesign Interchange");

    if (frm == QLatin1String("ISO"))         return i18n("ISO 9660 disk image");

    if (frm == QLatin1String("ITC"))         return i18n("iTunes Cover Flow artwork");

    if ((frm == QLatin1String("J2C")) ||
        (frm == QLatin1String("J2K")) ||
        (frm == QLatin1String("JPC")))       return i18n("JPEG 2000 codestream");

    if ((frm == QLatin1String("JP2")) ||
        (frm == QLatin1String("JPF")) ||
        (frm == QLatin1String("JPM")) ||
        (frm == QLatin1String("JPX")))       return i18n("JPEG 2000 image");

    if ((frm == QLatin1String("JPEG")) ||
        (frm == QLatin1String("JPG"))  ||
        (frm == QLatin1String("JPE")))       return i18n("Joint Photographic Experts Group image");

    if (frm == QLatin1String("JSON"))        return i18n("JavaScript Object Notation");

    if (frm == QLatin1String("JXL"))         return i18n("JPEG XL");

    if (frm == QLatin1String("K25"))         return i18n("Kodak DC25 RAW");

    if (frm == QLatin1String("KDC"))         return i18n("Kodak Digital Camera RAW");

    if ((frm == QLatin1String("KEY")) ||
        (frm == QLatin1String("KTH")))       return i18n("Apple iWork '09 Keynote presentation / Theme");

    if (frm == QLatin1String("LA"))          return i18n("Lossless Audio");

    if ((frm == QLatin1String("LFP")) ||
        (frm == QLatin1String("LFR")))       return i18n("Lytro Light Field Picture");

    if (frm == QLatin1String("LNK"))         return i18n("Microsoft Shell Link");

    if (frm == QLatin1String("LRV"))         return i18n("Low-Resolution Video");

    if (frm == QLatin1String("LRI"))         return i18n("Light RAW");

    if ((frm == QLatin1String("M2TS")) ||
        (frm == QLatin1String("MTS"))  ||
        (frm == QLatin1String("M2T"))  ||
        (frm == QLatin1String("TS")))        return i18n("MPEG-2 Transport Stream");

    if ((frm == QLatin1String("M4A")) ||
        (frm == QLatin1String("M4B")) ||
        (frm == QLatin1String("M4P")) ||
        (frm == QLatin1String("M4V")))       return i18n("MPEG-4 Audio/Video");

    if (frm == QLatin1String("MACOS"))       return i18n("MacOS sidecar file");

    if (frm == QLatin1String("MAX"))         return i18n("3D Studio MAX");

    if (frm == QLatin1String("MEF"))         return i18n("Mamiya RAW Electronic Format");

    if (frm == QLatin1String("MIE"))         return i18n("Meta Information Encapsulation");

    if ((frm == QLatin1String("MIFF")) ||
        (frm == QLatin1String("MIF")))       return i18n("Magick Image File Format");

    if ((frm == QLatin1String("MKA")) ||
        (frm == QLatin1String("MKV")) ||
        (frm == QLatin1String("MKS")))       return i18n("Matroska Audio/Video/Subtitle");

    if ((frm == QLatin1String("MOBI")) ||
        (frm == QLatin1String("AZW"))  ||
        (frm == QLatin1String("AZW3")))      return i18n("Mobipocket electronic book");

    if (frm == QLatin1String("MODD"))        return i18n("Sony Picture Motion metadata");

    if (frm == QLatin1String("MOI"))         return i18n("MOD Information file");

    if (frm == QLatin1String("MOS"))         return i18n("Creo Leaf Mosaic");

    if ((frm == QLatin1String("MOV")) ||
        (frm == QLatin1String("QT")))        return i18n("Apple QuickTime Movie");

    if (frm == QLatin1String("MP3"))         return i18n("MPEG-1 layer 3 audio");

    if (frm == QLatin1String("MP4"))         return i18n("Motion Picture Experts Group version 4");

    if (frm == QLatin1String("MPC"))         return i18n("Musepack Audio");

    if ((frm == QLatin1String("MPEG")) ||
        (frm == QLatin1String("MPG"))  ||
        (frm == QLatin1String("M2V")))       return i18n("Motion Picture Experts Group version 1 or 2");

    if (frm == QLatin1String("MPO"))         return i18n("Extended Multi-Picture format");

    if (frm == QLatin1String("MQV"))         return i18n("Sony Mobile QuickTime Video");

    if (frm == QLatin1String("MRW"))         return i18n("Minolta RAW");

    if (frm == QLatin1String("MRC"))         return i18n("Medical Research Council");

    if (frm == QLatin1String("MXF"))         return i18n("Material Exchange Format");

    if (frm == QLatin1String("NEF"))         return i18n("Nikon (RAW) Electronic Format");

    if (frm == QLatin1String("NMBTEMPLATE")) return i18n("Apple iWork '09 Numbers Template");

    if (frm == QLatin1String("NRW"))         return i18n("Nikon RAW");

    if (frm == QLatin1String("NUMBERS"))     return i18n("Apple iWork '09 Numbers spreadsheet");

    if (frm == QLatin1String("O"))           return i18n("Unix compiled code Object");

    if ((frm == QLatin1String("ODB")) ||
        (frm == QLatin1String("ODC")) ||
        (frm == QLatin1String("ODF")) ||
        (frm == QLatin1String("ODG")))       return i18n("Open Document");

    if ((frm == QLatin1String("ODI")) ||
        (frm == QLatin1String("ODP")) ||
        (frm == QLatin1String("ODS")) ||
        (frm == QLatin1String("ODT")))       return i18n("Open Document Ressources");

    if (frm == QLatin1String("OFR"))         return i18n("OptimFROG audio");

    if ((frm == QLatin1String("OGG")) ||
        (frm == QLatin1String("OGV")))       return i18n("Ogg bitstream container");

    if (frm == QLatin1String("ONP"))         return i18n("ON1 Presets");

    if (frm == QLatin1String("OPUS"))        return i18n("Ogg Opus audio");

    if ((frm == QLatin1String("ORF")) ||
        (frm == QLatin1String("ORI")))       return i18n("Olympus RAW Format");

    if (frm == QLatin1String("OTF"))         return i18n("Open Type Font");

    if (frm == QLatin1String("PAC"))         return i18n("Lossless Predictive Audio Compression");

    if (frm == QLatin1String("PAGES"))       return i18n("Apple iWork '09 Pages document");

    if (frm == QLatin1String("PCD"))         return i18n("Kodak Photo CD Image Pac");

    if (frm == QLatin1String("PCX"))         return i18n("PC Paintbrush");

    if ((frm == QLatin1String("PDB")) ||
        (frm == QLatin1String("PRC")))       return i18n("Palm Database");

    if (frm == QLatin1String("PDF"))         return i18n("Adobe Portable Document Format");

    if (frm == QLatin1String("PEF"))         return i18n("Pentax (RAW) Electronic Format");

    if ((frm == QLatin1String("PFA")) ||
        (frm == QLatin1String("PFB")))       return i18n("PostScript Font ASCII/Binary");

    if (frm == QLatin1String("PFM"))         return i18n("Printer Font Metrics");

    if (frm == QLatin1String("PGF"))         return i18n("Progressive Graphics File");

    if ((frm == QLatin1String("PICT")) ||
        (frm == QLatin1String("PCT")))       return i18n("Apple Picture file");

    if ((frm == QLatin1String("PHP"))   ||
        (frm == QLatin1String("PHP3"))  ||
        (frm == QLatin1String("PHP4"))  ||
        (frm == QLatin1String("PHP5"))  ||
        (frm == QLatin1String("PHTML")) ||
        (frm == QLatin1String("PHPS")))      return i18n("PHP Hypertext Preprocessor");

    if (frm == QLatin1String("PLIST"))       return i18n("Apple Property List");

    if (frm == QLatin1String("PMP"))         return i18n("Sony DSC-F1 Cyber-Shot image");

    if ((frm == QLatin1String("PNG")) ||
        (frm == QLatin1String("JNG")) ||
        (frm == QLatin1String("MNG")))       return i18n("Portable JPEG Multiple-image Network Graphics");

    if ((frm == QLatin1String("PPM")) ||
        (frm == QLatin1String("PBM")) ||
        (frm == QLatin1String("PGM")))       return i18n("Portable Pixel Bit Gray Map");

    if ((frm == QLatin1String("PPT")) ||
        (frm == QLatin1String("PPS")) ||
        (frm == QLatin1String("POT")))       return i18n("PowerPoint Presentation / Slideshow / Template");

    if ((frm == QLatin1String("POTX")) ||
        (frm == QLatin1String("POTM")))      return i18n("Office Open XML Presentation Template");

    if ((frm == QLatin1String("PPAX")) ||
        (frm == QLatin1String("PPAM")))      return i18n("Office Open XML Presentation Addin");

    if ((frm == QLatin1String("PPSX")) ||
        (frm == QLatin1String("PPSM")))      return i18n("Office Open XML Presentation Slideshow");

    if ((frm == QLatin1String("PPTX")) ||
        (frm == QLatin1String("PPTM")))      return i18n("Office Open XML Presentation");

    if ((frm == QLatin1String("PSD")) ||
        (frm == QLatin1String("PSB")) ||
        (frm == QLatin1String("PSDT")))      return i18n("PhotoShop Document / Large Document / Template");

    if ((frm == QLatin1String("PSP"))      ||
        (frm == QLatin1String("TUB"))      ||
        (frm == QLatin1String("PSPFRAME")) ||
        (frm == QLatin1String("PSPSHAPE")) ||
        (frm == QLatin1String("PSPTUBE"))  ||
        (frm == QLatin1String("PSPIMAGE")))  return i18n("Paint Shop Pro");

    if ((frm == QLatin1String("QTIF")) ||
        (frm == QLatin1String("QTI"))  ||
        (frm == QLatin1String("QIF")))       return i18n("QuickTime Image File");

    if (frm == QLatin1String("R3D"))         return i18n("Redcode RAW video");

    if (frm == QLatin1String("RA"))          return i18n("Real Audio");

    if (frm == QLatin1String("RAF"))         return i18n("FujiFilm RAW Format");

    if ((frm == QLatin1String("RAM")) ||
        (frm == QLatin1String("RPM")))       return i18n("Real Audio / Plug-in Metafile");

    if (frm == QLatin1String("RAR"))         return i18n("RAR Archive");

    if (frm == QLatin1String("RAW"))         return i18n("Kyocera Contax N Digital RAW");

    if (frm == QLatin1String("RAW"))         return i18n("Panasonic RAW");

    if ((frm == QLatin1String("RIFF")) ||
        (frm == QLatin1String("RIF")))       return i18n("Resource Interchange File Format");

    if ((frm == QLatin1String("RM")) ||
        (frm == QLatin1String("RV")) ||
        (frm == QLatin1String("RMVB")))      return i18n("Real Media / Video");

    if (frm == QLatin1String("RSRC"))        return i18n("Mac OS Resource");

    if (frm == QLatin1String("RTF"))         return i18n("Rich Text Format");

    if (frm == QLatin1String("RW2"))         return i18n("Panasonic RAW 2");

    if (frm == QLatin1String("RWL"))         return i18n("Leica RAW");

    if (frm == QLatin1String("RWZ"))         return i18n("Rawzor compressed image");

    if (frm == QLatin1String("SEQ"))         return i18n("FLIR Systems image Sequence");

    if (frm == QLatin1String("SKETCH"))      return i18n("Sketch design file");

    if (frm == QLatin1String("SO"))          return i18n("Unix ELF executable and Shared Object files");

    if (frm == QLatin1String("SR2"))         return i18n("Sony RAW 2");

    if (frm == QLatin1String("SRF"))         return i18n("Sony RAW Format");

    if (frm == QLatin1String("SRW"))         return i18n("Samsung RAW format");

    if (frm == QLatin1String("SVG"))         return i18n("Scalable Vector Graphics");

    if (frm == QLatin1String("SWF"))         return i18n("Shockwave Flash");

    if (frm == QLatin1String("TAR"))         return i18n("TAR archive");

    if (frm == QLatin1String("THM"))         return i18n("Thumbnail image");

    if (frm == QLatin1String("THMX"))        return i18n("Office Open XML Theme");

    if ((frm == QLatin1String("TIFF")) ||
        (frm == QLatin1String("TIF")))       return i18n("Tagged Image File Format");

    if ((frm == QLatin1String("TTF")) ||
        (frm == QLatin1String("TTC")))       return i18n("True Type Font/Collection");

    if (frm == QLatin1String("TORRENT"))     return i18n("BitTorrent description file");

    if (frm == QLatin1String("TXT"))         return i18n("Text files");

    if ((frm == QLatin1String("VCF")) ||
        (frm == QLatin1String("VCARD")))     return i18n("Virtual Card");

    if (frm == QLatin1String("VOB"))         return i18n("Video Object");

    if (frm == QLatin1String("VRD"))         return i18n("Canon DPP Recipe Data");

    if (frm == QLatin1String("VSD"))         return i18n("Microsoft Visio Drawing");

    if (frm == QLatin1String("WAV"))         return i18n("Windows digital audio WAVeform");

    if (frm == QLatin1String("WEBM"))        return i18n("Google Web Movie");

    if (frm == QLatin1String("WEBP"))        return i18n("Google Web Picture");

    if ((frm == QLatin1String("WMA")) ||
        (frm == QLatin1String("WMV")))       return i18n("Windows Media Audio / Video");

    if (frm == QLatin1String("WMF"))         return i18n("Windows Metafile Format");

    if ((frm == QLatin1String("WOFF")) ||
        (frm == QLatin1String("WOFF2")))     return i18n("Web Open Font Format");

    if (frm == QLatin1String("WTV"))         return i18n("Windows recorded TV show");

    if (frm == QLatin1String("WV"))          return i18n("WavePack lossless audio");

    if (frm == QLatin1String("X3F"))         return i18n("Sigma / Foveon RAW");

    if (frm == QLatin1String("XCF"))         return i18n("GIMP native image format");

    if (frm == QLatin1String("XLA"))         return i18n("Microsoft Excel Add-in");

    if (frm == QLatin1String("XLAM"))        return i18n("Office Open XML Spreadsheet Add-in Macro-enabled");

    if ((frm == QLatin1String("XLS")) ||
        (frm == QLatin1String("XLT")))       return i18n("Microsoft Excel Spreadsheet / Template");

    if ((frm == QLatin1String("XLSX")) ||
        (frm == QLatin1String("XLSM")) ||
        (frm == QLatin1String("XLSB")))      return i18n("Office Open XML Spreadsheet");

    if ((frm == QLatin1String("XLTX")) ||
        (frm == QLatin1String("XLTM")))      return i18n("Office Open XML Spreadsheet Template");

    if (frm == QLatin1String("XMP"))         return i18n("Extensible Metadata Platform sidecar file");

    if (frm == QLatin1String("ZIP"))         return i18n("ZIP archive");

    return QString();
}

} // namespace Digikam
