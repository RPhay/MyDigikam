/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : image quality sorter maintenance tool
 *
 * SPDX-FileCopyrightText: 2013-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
 * SPDX-FileCopyrightText: 2021-2022 by Phuoc Khanh Le <phuockhanhnk94 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QObject>

// Local includes

#include "album.h"
#include "maintenancetool.h"
#include "imagequalitycontainer.h"

class QImage;

namespace Digikam
{

class ItemInfo;

class ImageQualitySorter : public MaintenanceTool
{
    Q_OBJECT

public:

    enum QualityScanMode
    {
        AllItems = 0,        ///< Clean all Pick Labels assignments and re-scan all items.
        NonAssignedItems     ///< Scan only items with no Pick Labels assigned.
    };

public:

    /**
     * Constructor using AlbumList as argument. If list is empty, whole Albums collection is processed.
     */
    explicit ImageQualitySorter(QualityScanMode mode,
                                const AlbumList& list=AlbumList(),
                                const ImageQualityContainer& quality = ImageQualityContainer(),
                                ProgressItem* const parent = nullptr);
    ~ImageQualitySorter() override;

    void setUseMultiCoreCPU(bool b) override;

private:

    void processOne();

private Q_SLOTS:

    void slotStart()                override;
    void slotCancel()               override;
    void slotAdvance(const ItemInfo&, const QImage&);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
