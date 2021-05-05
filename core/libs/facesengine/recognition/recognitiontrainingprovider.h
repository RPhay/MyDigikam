/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : A convenience class to train faces
 *
 * Copyright (C)      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C)      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#ifndef DIGIKAM_RECOGNITION_TRAINING_PROVIDER_H
#define DIGIKAM_RECOGNITION_TRAINING_PROVIDER_H

// Qt includes

#include <QList>
#include <QImage>

// Local includes

#include "dataproviders.h"
#include "identity.h"

namespace Digikam
{

/**
 * A simple QImage training data container used by
 * RecognitionDatabase::train(Identity, QImage, QString)
 */
class RecognitionTrainingProvider : public TrainingDataProvider
{
public:

    explicit RecognitionTrainingProvider(const Identity& identity,
                                         const QList<QImage*>& newImages);
    ~RecognitionTrainingProvider()                   override;

    ImageListProvider* newImages(const Identity& id) override;

    ImageListProvider* images(const Identity&)       override;

public:

    Identity               m_identity;
    QListImageListProvider m_toTrain;
    QListImageListProvider m_empty;

private:

    // Disable
    RecognitionTrainingProvider(const RecognitionTrainingProvider&)            = delete;
    RecognitionTrainingProvider& operator=(const RecognitionTrainingProvider&) = delete;
};

} // namespace Digikam

#endif // DIGIKAM_RECOGNITION_TRAINING_PROVIDER_H
