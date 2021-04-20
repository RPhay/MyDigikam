/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-04-18
 * Description : ExifTool metadata list view.
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

#include "exiftoolwidget.h"

// Qt includes

#include <QHeaderView>
#include <QFont>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "ditemtooltip.h"

namespace Digikam
{

ExifToolListViewGroup::ExifToolListViewGroup(ExifToolListView* const parent,
                                             const QString& group)
    : QTreeWidgetItem(parent)
{
    setFlags(Qt::ItemIsEnabled);
    setExpanded(true);
    setDisabled(false);
    QFont fn0(font(0));
    fn0.setBold(true);
    fn0.setItalic(false);
    setFont(0, fn0);
    setText(0, group);

    if      (group == QLatin1String("AFCP"))
    {
        setToolTip(0, i18n("AXS File Concatenation Protocol"));
    }
    else if (group == QLatin1String("AIFF"))
    {
        setToolTip(0, i18n("Audio Interchange File Format"));
    }
    else if (group == QLatin1String("APE"))
    {
        setToolTip(0, i18n("Monkey's Audio"));
    }
    else if (group == QLatin1String("APP0"))
    {
        setToolTip(0, i18n("JPEG Meta Information Level 0"));
    }
    else if (group == QLatin1String("APP1"))
    {
        setToolTip(0, i18n("JPEG Meta Information Level 1"));
    }
    else if (group == QLatin1String("APP11"))
    {
        setToolTip(0, i18n("JPEG Meta Information Level 11"));
    }
    else if (group == QLatin1String("APP12"))
    {
        setToolTip(0, i18n("JPEG Meta Information Level 12"));
    }
    else if (group == QLatin1String("APP13"))
    {
        setToolTip(0, i18n("JPEG Meta Information Level 13"));
    }
    else if (group == QLatin1String("APP14"))
    {
        setToolTip(0, i18n("JPEG Meta Information Level 14"));
    }
    else if (group == QLatin1String("APP15"))
    {
        setToolTip(0, i18n("JPEG Meta Information Level 15"));
    }
    else if (group == QLatin1String("APP4"))
    {
        setToolTip(0, i18n("JPEG Meta Information Level 4"));
    }
    else if (group == QLatin1String("APP5"))
    {
        setToolTip(0, i18n("JPEG Meta Information Level 5"));
    }
    else if (group == QLatin1String("APP6"))
    {
        setToolTip(0, i18n("JPEG Meta Information Level 6"));
    }
    else if (group == QLatin1String("APP8"))
    {
        setToolTip(0, i18n("JPEG Meta Information Level 8"));
    }
    else if (group == QLatin1String("ASF"))
    {
        setToolTip(0, i18n("Microsoft Advanced Systems Format"));
    }
    else if (group == QLatin1String("Audible"))
    {
        setToolTip(0, i18n("Audible Audiobook"));
    }
    else if (group == QLatin1String("Canon"))
    {
        setToolTip(0, i18n("Canon Camera Information"));
    }
    else if (group == QLatin1String("CanonVRD"))
    {
        setToolTip(0, i18n("Canon Recipe Data Information"));
    }
    else if (group == QLatin1String("Composite"))
    {
        setToolTip(0, i18n("Derived Convenience Tags"));
    }
    else if (group == QLatin1String("DICOM"))
    {
        setToolTip(0, i18n("Digital Imaging and Communications in Medicine"));
    }
    else if (group == QLatin1String("DNG"))
    {
        setToolTip(0, i18n("Digital Negative"));
    }
    else if (group == QLatin1String("DV"))
    {
        setToolTip(0, i18n("Digital Video"));
    }
    else if (group == QLatin1String("DjVu"))
    {
        setToolTip(0, i18n("DjVu image"));
    }
    else if (group == QLatin1String("Ducky"))
    {
        setToolTip(0, i18n("Photoshop Save for Web"));
    }
    else if (group == QLatin1String("EXE"))
    {
        setToolTip(0, i18n("DOS/Windows executable"));
    }
    else if (group == QLatin1String("EXIF"))
    {
        setToolTip(0, i18n("Exchangeable Image File Format metadata"));
    }
    else if (group == QLatin1String("FITS"))
    {
        setToolTip(0, i18n("Flexible Image Transport System"));
    }
    else if (group == QLatin1String("FLAC"))
    {
        setToolTip(0, i18n("Free Lossless Audio Codec"));
    }
    else if (group == QLatin1String("FLIR"))
    {
        setToolTip(0, i18n("FLIR Thermal Imaging Camera Information"));
    }
    else if (group == QLatin1String("File"))
    {
        setToolTip(0, i18n("File System Information"));
    }
    else if (group == QLatin1String("Flash"))
    {
        setToolTip(0, i18n("Macromedia/Adobe Flash Information"));
    }
    else if (group == QLatin1String("FlashPix"))
    {
        setToolTip(0, i18n("FlashPix Information"));
    }
    else if (group == QLatin1String("Font"))
    {
        setToolTip(0, i18n("Font Name Tags"));
    }
    else if (group == QLatin1String("FotoStation"))
    {
        setToolTip(0, i18n("FotoWare FotoStation Information"));
    }
    else if (group == QLatin1String("GIF"))
    {
        setToolTip(0, i18n("Compuserve Graphics Interchange Format"));
    }
    else if (group == QLatin1String("GIMP"))
    {
        setToolTip(0, i18n("GNU Image Manipulation Program"));
    }
    else if (group == QLatin1String("GeoTiff"))
    {
        setToolTip(0, i18n("Geolocated Tagged Image File Format"));
    }
    else if (group == QLatin1String("GoPro"))
    {
        setToolTip(0, i18n("GoPro Tags"));
    }
    else if (group == QLatin1String("H264"))
    {
        setToolTip(0, i18n("H.264 Video Streams Information"));
    }
    else if (group == QLatin1String("HTML"))
    {
        setToolTip(0, i18n("HyperText Markup Language"));
    }
    else if (group == QLatin1String("ICC Profile"))
    {
        setToolTip(0, i18n("International Color Consortium Color Profile"));
    }
    else if (group == QLatin1String("ID3"))
    {
        setToolTip(0, i18n("ID3 And Lyrics3 information"));
    }
    else if (group == QLatin1String("IPTC"))
    {
        setToolTip(0, i18n("International Press Telecommunications Council"));
    }
    else if (group == QLatin1String("ISO"))
    {
        setToolTip(0, i18n("ISO 9660 Disk Image"));
    }
    else if (group == QLatin1String("ITC"))
    {
        setToolTip(0, i18n("iTunes Cover Flow Artwork"));
    }
    else if (group == QLatin1String("JFIF"))
    {
        setToolTip(0, i18n("JPEG File Interchange Format"));
    }
    else if (group == QLatin1String("JPEG"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("JSON"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Jpeg2000"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("LNK"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Leaf"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Lytro"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("M2TS"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("MIE"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("MIFF"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("MNG"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("MOI"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("MPC"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("MPEG"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("MPF"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("MXF"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("MakerNotes"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Matroska"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Meta"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Ogg"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("OpenEXR"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Opus"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("PDF"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("PICT"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("PLIST"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("PNG"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("PSP"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Palm"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("PanasonicRaw"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Parrot"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("PhotoCD"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("PhotoMechanic"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Photoshop"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("PostScript"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("PrintIM"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("QuickTime"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("RAF"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("RIFF"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("RSRC"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("RTF"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Radiance"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Rawzor"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Real"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Red"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("SVG"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("SigmaRaw"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Sony"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Stim"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Theora"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Torrent"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Trailer"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("VCard"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("Vorbis"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("WTV"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("XML"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("XMP"))
    {
        setToolTip(0, i18n(""));
    }
    else if (group == QLatin1String("ZIP"))
    {
        setToolTip(0, i18n(""));
    }
}

ExifToolListViewGroup::~ExifToolListViewGroup()
{
}

// ---------------------------------------------------------------------------

ExifToolListViewItem::ExifToolListViewItem(ExifToolListViewGroup* const parent,
                                           const QString& name,
                                           const QString& value,
                                           const QString& desc)
    : QTreeWidgetItem(parent)
{
    setDisabled(false);
    setSelected(false);
    setText(0, name);
    setToolTip(0, !desc.isEmpty() ? desc : name);

    QString tagVal = value.simplified();

    if (tagVal.length() > 512)
    {
        tagVal.truncate(512);
        tagVal.append(QLatin1String("..."));
    }

    setText(1, tagVal);

    DToolTipStyleSheet cnt;
    setToolTip(1, QLatin1String("<qt><p>") + cnt.breakString(tagVal) + QLatin1String("</p></qt>"));
}

ExifToolListViewItem::~ExifToolListViewItem()
{
}

// ---------------------------------------------------------------------------

ExifToolListView::ExifToolListView(QWidget* const parent)
    : QTreeWidget(parent)
{
    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setAllColumnsShowFocus(true);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    setColumnCount(2);
    setHeaderHidden(true);
    header()->setSectionResizeMode(QHeaderView::Stretch);

    m_parser = new ExifToolParser(this);
    m_parser->setTranslations(false);
}

ExifToolListView::~ExifToolListView()
{
}

bool ExifToolListView::loadFromUrl(const QUrl& url)
{
    clear();

    if (!url.isValid())
    {
        return true;
    }

    if (!m_parser->load(url.toLocalFile()))
    {
        return false;
    }

    setMetadata(m_parser->currentParsedTags());

    return true;
}

void ExifToolListView::setMetadata(const ExifToolParser::TagsMap& map)
{
    for (ExifToolParser::TagsMap::const_iterator it = map.constBegin() ;
         it != map.constEnd() ; ++it)
    {
        QString grp                   = it.key().section(QLatin1Char('.'), 0, 0)
                                                .replace(QLatin1Char('_'), QLatin1Char(' '));

        if (grp == QLatin1String("ExifTool"))
        {
            continue;
        }

        QString name                  = it.key().section(QLatin1Char('.'), -1);
        QString value                 = it.value()[1].toString();
        QString desc                  = it.value()[3].toString();
        ExifToolListViewGroup* igroup = findGroup(grp);

        if (!igroup)
        {
            igroup = new ExifToolListViewGroup(this, grp);
        }

        new ExifToolListViewItem(igroup, name, value, desc);
    }
}

ExifToolListViewGroup* ExifToolListView::findGroup(const QString& group)
{
    QTreeWidgetItemIterator it(this);

    while (*it)
    {
        ExifToolListViewGroup* const item = dynamic_cast<ExifToolListViewGroup*>(*it);

        if (item && (item->text(0) == group))
        {
            return item;
        }

        ++it;
    }

    return nullptr;
}

// ---------------------------------------------------------------------------

class Q_DECL_HIDDEN ExifToolWidget::Private
{
public:

    enum ViewMode
    {
        MetadataView = 0,
        ErrorView
    };

public:

    explicit Private()
        : metadataView(nullptr),
          errorView   (nullptr)
    {
    }

    ExifToolListView* metadataView;
    QWidget*          errorView;
};

ExifToolWidget::ExifToolWidget(QWidget* const parent)
    : QStackedWidget(parent),
      d             (new Private)
{
    setAttribute(Qt::WA_DeleteOnClose);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    d->metadataView         = new ExifToolListView(this);
    d->errorView            = new QWidget(this);
    QGridLayout* const grid = new QGridLayout(d->errorView);

    QLabel* const errorLbl  = new QLabel(d->errorView);
    errorLbl->setAlignment(Qt::AlignCenter);
    errorLbl->setText(i18n("Cannot load data\nwith ExifTool.\nCheck your configuration."));

    QPushButton* const btn  = new QPushButton(d->errorView);
    btn->setText(i18n("Open Setup Dialog..."));

    connect(btn, SIGNAL(clicked()),
            this, SIGNAL(signalSetupExifTool()));

    grid->addWidget(errorLbl, 1, 1, 1, 1);
    grid->addWidget(btn,      2, 1, 1, 1);
    grid->setColumnStretch(0, 10);
    grid->setColumnStretch(2, 10);
    grid->setRowStretch(0, 10);
    grid->setRowStretch(3, 10);

    insertWidget(Private::MetadataView, d->metadataView);
    insertWidget(Private::ErrorView,    d->errorView);

    setCurrentIndex(Private::MetadataView);
}

ExifToolWidget::~ExifToolWidget()
{
    delete d;
}

void ExifToolWidget::loadFromUrl(const QUrl& url)
{
    bool ret = d->metadataView->loadFromUrl(url);

    if (ret)
    {
        setCurrentIndex(Private::MetadataView);
    }
    else
    {
        setCurrentIndex(Private::ErrorView);
    }
}

} // namespace Digikam
