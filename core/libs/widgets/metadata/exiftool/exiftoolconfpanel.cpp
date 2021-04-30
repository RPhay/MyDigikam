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
        exifToolFormats  (nullptr)
    {
    }

public:

    DBinarySearch* exifToolBinWidget;
    ExifToolBinary exifToolBin;

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

    QVBoxLayout* const vlay      = new QVBoxLayout(exifToolBox);
    vlay->addWidget(d->exifToolFormats);

    grid->addWidget(exifToolBinLabel,     0, 0, 1, 2);
    grid->addWidget(d->exifToolBinWidget, 1, 0, 1, 2);
    grid->addWidget(exifToolBox,          2, 0, 1, 2);
    grid->setRowStretch(2, 10);
    setLayout(grid);

    // ---

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

QString ExifToolConfPanel::formatDescription(const QString& frm)
{
    if (frm == QLatin1String("3FR"))                        return i18n("Hasselblad RAW");

    if ((frm == QLatin1String("3G2")) ||
        (frm == QLatin1String("3GP2")))                     return i18n("3rd Gen. Partnership Project 2 a/v");

    if ((frm == QLatin1String("3GP")) ||
        (frm == QLatin1String("3GPP")))                     return i18n("3rd Gen. Partnership Project a/v");

    if (frm == QLatin1String("A"))                          return i18n("Unix static library code Archive");

    if (frm == QLatin1String("AA"))                         return i18n("Audible Audiobook");

    if (frm == QLatin1String("AAE"))                        return i18n("Apple edit information");

    if (frm == QLatin1String("AAX"))                        return i18n("Audible Enhanced Audiobook");

    if (frm == QLatin1String("ACR"))                        return i18n("American College of Radiology ACR-NEMA");

    if ((frm == QLatin1String("AFM"))  ||
        (frm == QLatin1String("ACFM")) ||
        (frm == QLatin1String("AMFM")))                     return i18n("Adobe Composite / Multiple Master Font Metrics");

    if ((frm == QLatin1String("AI")) ||
        (frm == QLatin1String("AIT")))                      return i18n("Adobe Illustrator");

    if ((frm == QLatin1String("AIFF")) ||
        (frm == QLatin1String("AIF"))  ||
        (frm == QLatin1String("AIFC")))                     return i18n("Audio Interchange File Format");

    if (frm == QLatin1String("APE"))                        return i18n("Monkey's Audio");

    if (frm == QLatin1String("ARQ"))                        return i18n("Sony Alpha Pixel-Shift RAW");

    if (frm == QLatin1String("ARW"))                        return i18n("Sony Alpha RAW");

    if (frm == QLatin1String("ASF"))                        return i18n("Microsoft Advanced Systems Format");

    if (frm == QLatin1String("AVI"))                        return i18n("Audio Video Interleaved");

    if (frm == QLatin1String("AVIF"))                       return i18n("AV1 Image File Format");

    if (frm == QLatin1String("BMP"))                        return i18n("Windows BitMaP");

    if (frm == QLatin1String("DIB"))                        return i18n("Device Independent Bitmap");

    if (frm == QLatin1String("BPG"))                        return i18n("Better Portable Graphics");

    if (frm == QLatin1String("BTF"))                        return i18n("BigTIFF");

    if (frm == QLatin1String("CHM"))                        return i18n("Microsoft Compiled HTML format");

    if (frm == QLatin1String("COS"))                        return i18n("Capture One Settings");

    if (frm == QLatin1String("CR2"))                        return i18n("Canon RAW 2");

    if (frm == QLatin1String("CR3"))                        return i18n("Canon RAW 3");

    if (frm == QLatin1String("CRM"))                        return i18n("Canon RAW Movie");

    if ((frm == QLatin1String("CRW")) ||
        (frm == QLatin1String("CIFF")))                     return i18n("Canon RAW Camera Image File Format");
/*
    if (frm == QLatin1String("CS1                     return i18n("Sinar CaptureShop 1-shot RAW");

    if (frm == QLatin1String("CSV                     return i18n("Comma-Separated Values");

    if (frm == QLatin1String("CZI                     return i18n("Zeiss Integrated Software RAW");

    if (frm == QLatin1String("DCM, DC3, DIC, DICM     return i18n("DICOM - Digital Imaging and Communications in Medicine");

    if (frm == QLatin1String("DCP                     return i18n("DNG Camera Profile");

    if (frm == QLatin1String("DCR                     return i18n("Kodak Digital Camera RAW");

    if (frm == QLatin1String("DFONT                   return i18n("Macintosh Data Fork Font");

    if (frm == QLatin1String("DIVX                    return i18n("DivX media format");

    if (frm == QLatin1String("DJVU, DJV               return i18n("DjVu image");

    if (frm == QLatin1String("DNG                     return i18n("Digital Negative");

    if (frm == QLatin1String("DOC, DOT                return i18n("Microsoft Word Document / Template");

    if (frm == QLatin1String("DOCX, DOCM              return i18n("Office Open XML Document");

    if (frm == QLatin1String("DOTX, DOTM              return i18n("Office Open XML Document Template");

    if (frm == QLatin1String("DPX                     return i18n("Digital Picture Exchange");

    if (frm == QLatin1String("DR4                     return i18n("Canon DPP version 4 Recipe");

    if (frm == QLatin1String("DSS, DS2                return i18n("Digital Speech Standard");

    if (frm == QLatin1String("DYLIB                   return i18n("MacOS Mach-O executable and library files");

    if (frm == QLatin1String("DV                      return i18n("Digital Video");

    if (frm == QLatin1String("DVB                     return i18n("Digital Video Broadcasting");

    if (frm == QLatin1String("DVR-MS                  return i18n("Microsoft Digital Video Recording");

    if (frm == QLatin1String("EIP                     return i18n("Capture One Enhanced Image Package");

    if (frm == QLatin1String("EPS, EPSF, PS           return i18n("[Encapsulated] PostScript Format");

    if (frm == QLatin1String("EPUB                    return i18n("Electronic Publication");

    if (frm == QLatin1String("ERF                     return i18n("Epson RAW Format");

    if (frm == QLatin1String("EXE, DLL                return i18n("DOS/Windows executable and library files");

    if (frm == QLatin1String("EXIF                    return i18n("Exchangeable Image File Format metadata");

    if (frm == QLatin1String("EXR                     return i18n("Open EXR");

    if (frm == QLatin1String("EXV                     return i18n("Exiv2 metadata file");

    if (frm == QLatin1String("F4A, F4B, F4P, F4V      return i18n("Adobe Flash Player 9+ Audio / Video");

    if (frm == QLatin1String("FFF                     return i18n("Hasselblad Flexible File Format");

    if (frm == QLatin1String("FFF                     return i18n("FLIR Systems thermal image File Format");

    if (frm == QLatin1String("FITS                    return i18n("Flexible Image Transport System");

    if (frm == QLatin1String("FLA                     return i18n("Macromedia / Adobe Flash project");

    if (frm == QLatin1String("FLAC                    return i18n("Free Lossless Audio Codec");

    if (frm == QLatin1String("FLIF                    return i18n("Free Lossless Image Format");

    if (frm == QLatin1String("FLV                     return i18n("Flash Video");

    if (frm == QLatin1String("FPF                     return i18n("FLIR Public image Format");

    if (frm == QLatin1String("FPX                     return i18n("FlashPix image");

    if (frm == QLatin1String("GIF                     return i18n("Compuserve Graphics Interchange Format");

    if (frm == QLatin1String("GPR                     return i18n("GoPro RAW");

    if (frm == QLatin1String("GZ, GZIP                return i18n("GNU ZIP compressed archive");

    if (frm == QLatin1String("HDP, WDP, JXR           return i18n("Windows HD Photo / Media Photo / JPEG XR");

    if (frm == QLatin1String("HDR                     return i18n("Radiance RGBE High Dynamic-Range");

    if (frm == QLatin1String("HEIC, HEIF, HIF         return i18n("High Efficiency Image Format");

    if (frm == QLatin1String("HTML, HTM, XHTML        return i18n("Extensible HyperText Markup Language");

    if (frm == QLatin1String("ICC, ICM                return i18n("International Color Consortium color profile");

    if (frm == QLatin1String("ICS, ICAL               return i18n("iCalendar Schedule");

    if (frm == QLatin1String("IDML                    return i18n("Adobe InDesign Markup Language");

    if (frm == QLatin1String("IIQ                     return i18n("Phase One Intelligent Image Quality");

    if (frm == QLatin1String("IND, INDD, INDT         return i18n("Adobe InDesign Document / Template");

    if (frm == QLatin1String("INSP                    return i18n("Insta360 Picture");

    if (frm == QLatin1String("INSV                    return i18n("Insta360 Video");

    if (frm == QLatin1String("INX                     return i18n("Adobe InDesign Interchange");

    if (frm == QLatin1String("ISO                     return i18n("ISO 9660 disk image");

    if (frm == QLatin1String("ITC                     return i18n("iTunes Cover Flow artwork");

    if (frm == QLatin1String("J2C, J2K, JPC           return i18n("JPEG 2000 codestream");

    if (frm == QLatin1String("JP2, JPF, JPM, JPX      return i18n("JPEG 2000 image");

    if (frm == QLatin1String("JPEG, JPG, JPE          return i18n("Joint Photographic Experts Group image");

    if (frm == QLatin1String("JSON                    return i18n("JavaScript Object Notation");

    if (frm == QLatin1String("JXL                     return i18n("JPEG XL");

    if (frm == QLatin1String("K25                     return i18n("Kodak DC25 RAW");

    if (frm == QLatin1String("KDC                     return i18n("Kodak Digital Camera RAW");

    if (frm == QLatin1String("KEY, KTH                return i18n("Apple iWork '09 Keynote presentation / Theme");

    if (frm == QLatin1String("LA                      return i18n("Lossless Audio");

    if (frm == QLatin1String("LFP, LFR                return i18n("Lytro Light Field Picture");

    if (frm == QLatin1String("LNK                     return i18n("Microsoft Shell Link");

    if (frm == QLatin1String("LRV                     return i18n("Low-Resolution Video");

    if (frm == QLatin1String("M2TS, MTS, M2T, TS      return i18n("MPEG-2 Transport Stream");

    if (frm == QLatin1String("M4A, M4B, M4P, M4V      return i18n("MPEG-4 Audio/Video");

    if (frm == QLatin1String("MACOS                   return i18n("MacOS sidecar file");

    if (frm == QLatin1String("MAX                     return i18n("3D Studio MAX");

    if (frm == QLatin1String("MEF                     return i18n("Mamiya RAW Electronic Format");

    if (frm == QLatin1String("MIE                     return i18n("Meta Information Encapsulation");

    if (frm == QLatin1String("MIFF, MIF               return i18n("Magick Image File Format");

    if (frm == QLatin1String("MKA, MKV, MKS           return i18n("Matroska Audio/Video/Subtitle");

    if (frm == QLatin1String("MOBI, AZW, AZW3         return i18n("Mobipocket electronic book");

    if (frm == QLatin1String("MODD                    return i18n("Sony Picture Motion metadata");

    if (frm == QLatin1String("MOI                     return i18n("MOD Information file");

    if (frm == QLatin1String("MOS                     return i18n("Creo Leaf Mosaic");

    if (frm == QLatin1String("MOV, QT                 return i18n("Apple QuickTime Movie");

    if (frm == QLatin1String("MP3                     return i18n("MPEG-1 layer 3 audio");

    if (frm == QLatin1String("MP4                     return i18n("Motion Picture Experts Group version 4");

    if (frm == QLatin1String("MPC                     return i18n("Musepack Audio");

    if (frm == QLatin1String("MPEG, MPG, M2V          return i18n("Motion Picture Experts Group version 1 or 2");

    if (frm == QLatin1String("MPO                     return i18n("Extended Multi-Picture format");

    if (frm == QLatin1String("MQV                     return i18n("Sony Mobile QuickTime Video");

    if (frm == QLatin1String("MRW                     return i18n("Minolta RAW");

    if (frm == QLatin1String("MRC                     return i18n("Medical Research Council");

    if (frm == QLatin1String("MXF                     return i18n("Material Exchange Format");

    if (frm == QLatin1String("NEF                     return i18n("Nikon (RAW) Electronic Format");

    if (frm == QLatin1String("NMBTEMPLATE             return i18n("Apple iWork '09 Numbers Template");

    if (frm == QLatin1String("NRW                     return i18n("Nikon RAW");

    if (frm == QLatin1String("NUMBERS                 return i18n("Apple iWork '09 Numbers spreadsheet");

    if (frm == QLatin1String("O                       return i18n("Unix compiled code Object");

    if (frm == QLatin1String("ODB, ODC, ODF, ODG,     return i18n("Open Document");

    if (frm == QLatin1String("ODI, ODP, ODS, ODT      return i18n("Open Document Ressources");

    if (frm == QLatin1String("OFR                     return i18n("OptimFROG audio");

    if (frm == QLatin1String("OGG, OGV                return i18n("Ogg bitstream container");

    if (frm == QLatin1String("ONP                     return i18n("ON1 Presets");

    if (frm == QLatin1String("OPUS                    return i18n("Ogg Opus audio");

    if (frm == QLatin1String("ORF, ORI                return i18n("Olympus RAW Format");

    if (frm == QLatin1String("OTF                     return i18n("Open Type Font");

    if (frm == QLatin1String("PAC                     return i18n("Lossless Predictive Audio Compression");

    if (frm == QLatin1String("PAGES                   return i18n("Apple iWork '09 Pages document");

    if (frm == QLatin1String("PCD                     return i18n("Kodak Photo CD Image Pac");

    if (frm == QLatin1String("PCX                     return i18n("PC Paintbrush");

    if (frm == QLatin1String("PDB, PRC                return i18n("Palm Database");

    if (frm == QLatin1String("PDF                     return i18n("Adobe Portable Document Format");

    if (frm == QLatin1String("PEF                     return i18n("Pentax (RAW) Electronic Format");

    if (frm == QLatin1String("PFA, PFB                return i18n("PostScript Font ASCII/Binary");

    if (frm == QLatin1String("PFM                     return i18n("Printer Font Metrics");

    if (frm == QLatin1String("PGF                     return i18n("Progressive Graphics File");

    if (frm == QLatin1String("PICT, PCT               return i18n("Apple Picture file");

    if (frm == QLatin1String("PLIST                   return i18n("Apple Property List");

    if (frm == QLatin1String("PMP                     return i18n("Sony DSC-F1 Cyber-Shot image");

    if (frm == QLatin1String("PNG, JNG, MNG           return i18n("Portable JPEG Multiple-image Network Graphics");

    if (frm == QLatin1String("PPM, PBM, PGM           return i18n("Portable Pixel Bit Gray Map");

    if (frm == QLatin1String("PPT, PPS, POT           return i18n("PowerPoint Presentation / Slideshow / Template");

    if (frm == QLatin1String("POTX, POTM              return i18n("Office Open XML Presentation Template");

    if (frm == QLatin1String("PPAX, PPAM              return i18n("Office Open XML Presentation Addin");

    if (frm == QLatin1String("PPSX, PPSM              return i18n("Office Open XML Presentation Slideshow");

    if (frm == QLatin1String("PPTX, PPTM              return i18n("Office Open XML Presentation");

    if (frm == QLatin1String("PSD, PSB, PSDT          return i18n("PhotoShop Document / Large Document / Template");

    if (frm == QLatin1String("PSP, PSPIMAGE           return i18n("Paint Shop Pro");

    if (frm == QLatin1String("QTIF, QTI, QIF          return i18n("QuickTime Image File");

    if (frm == QLatin1String("R3D                     return i18n("Redcode RAW video");

    if (frm == QLatin1String("RA                      return i18n("Real Audio");

    if (frm == QLatin1String("RAF                     return i18n("FujiFilm RAW Format");

    if (frm == QLatin1String("RAM, RPM                return i18n("Real Audio/Plug-in Metafile");

    if (frm == QLatin1String("RAR                     return i18n("RAR Archive");

    if (frm == QLatin1String("RAW                     return i18n("Kyocera Contax N Digital RAW");

    if (frm == QLatin1String("RAW                     return i18n("Panasonic RAW");

    if (frm == QLatin1String("RIFF, RIF               return i18n("Resource Interchange File Format");

    if (frm == QLatin1String("RM, RV, RMVB            return i18n("Real Media/Video");

    if (frm == QLatin1String("RSRC                    return i18n("Mac OS Resource");

    if (frm == QLatin1String("RTF                     return i18n("Rich Text Format");

    if (frm == QLatin1String("RW2                     return i18n("Panasonic RAW 2");

    if (frm == QLatin1String("RWL                     return i18n("Leica RAW");

    if (frm == QLatin1String("RWZ                     return i18n("Rawzor compressed image");

    if (frm == QLatin1String("SEQ                     return i18n("FLIR Systems image Sequence");

    if (frm == QLatin1String("SKETCH                  return i18n("Sketch design file");

    if (frm == QLatin1String("SO                      return i18n("Unix ELF executable and Shared Object files");

    if (frm == QLatin1String("SR2                     return i18n("Sony RAW 2");

    if (frm == QLatin1String("SRF                     return i18n("Sony RAW Format");

    if (frm == QLatin1String("SRW                     return i18n("Samsung RAW format");

    if (frm == QLatin1String("SVG                     return i18n("Scalable Vector Graphics");

    if (frm == QLatin1String("SWF                     return i18n("Shockwave Flash");

    if (frm == QLatin1String("THM                     return i18n("Thumbnail image");

    if (frm == QLatin1String("THMX                    return i18n("Office Open XML Theme");

    if (frm == QLatin1String("TIFF, TIF               return i18n("Tagged Image File Format");

    if (frm == QLatin1String("TTF, TTC                return i18n("True Type Font/Collection");

    if (frm == QLatin1String("TORRENT                 return i18n("BitTorrent description file");

    if (frm == QLatin1String("TXT                     return i18n("Text files");

    if (frm == QLatin1String("VCF, VCARD              return i18n("Virtual Card");

    if (frm == QLatin1String("VOB                     return i18n("Video Object");

    if (frm == QLatin1String("VRD                     return i18n("Canon DPP Recipe Data");

    if (frm == QLatin1String("VSD                     return i18n("Microsoft Visio Drawing");

    if (frm == QLatin1String("WAV                     return i18n("Windows digital audio WAVeform");

    if (frm == QLatin1String("WEBM                    return i18n("Google Web Movie");

    if (frm == QLatin1String("WEBP                    return i18n("Google Web Picture");

    if (frm == QLatin1String("WMA, WMV                return i18n("Windows Media Audio/Video");

    if (frm == QLatin1String("WTV                     return i18n("Windows recorded TV show");

    if (frm == QLatin1String("WV                      return i18n("WavePack lossless audio");

    if (frm == QLatin1String("X3F                     return i18n("Sigma/Foveon RAW");

    if (frm == QLatin1String("XCF                     return i18n("GIMP native image format");

    if (frm == QLatin1String("XLS, XLT                return i18n("Microsoft Excel Spreadsheet/Template");

    if (frm == QLatin1String("XLSX, XLSM, XLSB        return i18n("Office Open XML Spreadsheet");

    if (frm == QLatin1String("XLTX, XLTM              return i18n("Office Open XML Spreadsheet Template");

    if (frm == QLatin1String("XMP                     return i18n("Extensible Metadata Platform sidecar file");

    if (frm == QLatin1String("ZIP                     return i18n("ZIP archive");
*/
    return QString();
}

} // namespace Digikam
