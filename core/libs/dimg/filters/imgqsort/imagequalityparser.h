/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 25/08/2013
 * Description : Image Quality Parser
 *
 * Copyright (C) 2013-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2013-2014 by Gowtham Ashok <gwty93 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_IMAGE_QUALITY_PARSER_H
#define DIGIKAM_IMAGE_QUALITY_PARSER_H

// Local includes

#include "dimg.h"
#include "digikam_export.h"
#include "digikam_globals.h"
#include "imagequalitycontainer.h"
#include "digikam_opencv.h"

namespace Digikam
{

class DIGIKAM_EXPORT ImageQualityParser : public QObject
{
    Q_OBJECT

public:

    /**
     * Standard constructor with picklabel container to fill at end of analyze.
     */
    explicit ImageQualityParser(const DImg& image,
                                const ImageQualityContainer& settings,
                                PickLabel* const label);
    ~ImageQualityParser();

    /**
     * Perform quality estimation and fill Pick Label value accordingly.
     */
    void startAnalyse();
    void cancelAnalyse();

private:

    // Disable
    ImageQualityParser(const ImageQualityParser&)            = delete;
    ImageQualityParser& operator=(const ImageQualityParser&) = delete;
    explicit ImageQualityParser(QObject*);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_IMAGE_QUALITY_PARSER_H
