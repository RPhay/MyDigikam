/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-08-19
 * Description : Image Quality setup page
 *
 * SPDX-FileCopyrightText: 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_SETUP_IMAGE_QUALITY_SORTER_H
#define DIGIKAM_SETUP_IMAGE_QUALITY_SORTER_H

// Qt includes

#include <QScrollArea>

// Locals includes

#include "imagequalitycontainer.h"

namespace Digikam
{

class SetupImageQualitySorter : public QScrollArea
{
    Q_OBJECT

public:

    explicit SetupImageQualitySorter(QWidget* const parent = nullptr);
    ~SetupImageQualitySorter()                              override;

    void applySettings();
    ImageQualityContainer getImageQualityContainer() const;

private:

    void readSettings();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SETUP_IMAGE_QUALITY_SORTER_H
