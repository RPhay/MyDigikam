/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2002-12-09
 * Description : a tool to print images
 *
 * SPDX-FileCopyrightText: 2002-2003 by Todd Shoemaker <todd at theshoemakers dot net>
 * SPDX-FileCopyrightText: 2007-2012 by Angelo Naselli <anaselli at linux dot it>
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_ADV_PRINT_PHOTO_H
#define DIGIKAM_ADV_PRINT_PHOTO_H

// Qt includes

#include <QRect>
#include <QFont>
#include <QColor>
#include <QUrl>
#include <QPointer>
#include <QIcon>
#include <QList>
#include <QSize>
#include <QTransform>

// Local includes

#include "dinfointerface.h"
#include "dimg.h"
#include "advprintsettings.h"

using namespace Digikam;

namespace DigikamGenericPrintCreatorPlugin
{

class AdvPrintPhotoSize
{
public:

    explicit AdvPrintPhotoSize();
    AdvPrintPhotoSize(const AdvPrintPhotoSize& other);
    ~AdvPrintPhotoSize();

public:

    QString       m_label;
    int           m_dpi;
    bool          m_autoRotate;
    QList<QRect*> m_layouts;     ///< first element is page size
    QIcon         m_icon;
};

// -----------------------------------------------------------

class AdvPrintAdditionalInfo
{
public:

    explicit AdvPrintAdditionalInfo();
    AdvPrintAdditionalInfo(const AdvPrintAdditionalInfo& other);
    ~AdvPrintAdditionalInfo();

public:

    int    m_unit;
    int    m_printPosition;
    int    m_scaleMode;
    bool   m_keepRatio;
    bool   m_autoRotate;
    double m_printWidth;
    double m_printHeight;
    bool   m_enlargeSmallerImages;
};

// -----------------------------------------------------------

class AdvPrintCaptionInfo
{

public:

    explicit AdvPrintCaptionInfo();

    /// Copy constructor to get old photo info.
    AdvPrintCaptionInfo(const AdvPrintCaptionInfo& other);
    ~AdvPrintCaptionInfo();

public:

    AdvPrintSettings::CaptionType m_captionType;
    QFont                         m_captionFont;
    QColor                        m_captionColor;
    int                           m_captionSize;
    QString                       m_captionText;
};

// -----------------------------------------------------------

class AdvPrintPhoto
{

public:

    explicit AdvPrintPhoto(int thumbnailSize, DInfoInterface* const iface);
    AdvPrintPhoto(const AdvPrintPhoto& other);
    ~AdvPrintPhoto();

    DImg&  thumbnail();
    DImg   loadPhoto();
    int    width();
    int    height();
    QSize& size();

    QTransform updateCropRegion(int woutlay, int houtlay, bool autoRotate);

    double scaleWidth(double unitToInches);
    double scaleHeight(double unitToInches);

public:

    /// Url of original image file.
    QUrl                    m_url;

    /// Thumbnail size in pixels.
    int                     m_thumbnailSize;

    /// Region to crop while print from original image.
    QRect                   m_cropRegion;

    /// To get first copy quickly.
    bool                    m_first;

    /// Number of copies while printing stage.
    int                     m_copies;

    /// Rotation angle in degrees.
    int                     m_rotation;

    AdvPrintAdditionalInfo* m_pAddInfo;
    AdvPrintCaptionInfo*    m_pAdvPrintCaptionInfo;
    DInfoInterface*         m_iface;

private:

    void loadInCache();

private:

    DImg*                   m_thumbnail;
    QSize*                  m_size;
};

} // Namespace Digikam

#endif // DIGIKAM_ADV_PRINT_PHOTO_H
