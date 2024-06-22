/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : A convenience class to train faces
 *
 * SPDX-FileCopyrightText:      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText:      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

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
