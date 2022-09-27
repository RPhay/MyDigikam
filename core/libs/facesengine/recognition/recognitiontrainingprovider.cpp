/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2010-06-16
 * Description : A convenience class to train faces
 *
 * SPDX-FileCopyrightText:      2010 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText:      2010 by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * SPDX-FileCopyrightText: 2010-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "recognitiontrainingprovider.h"

namespace Digikam
{

RecognitionTrainingProvider::RecognitionTrainingProvider(const Identity& identity,
                                                         const QList<QImage*>& newImages)
    : m_identity(identity)
{
    m_toTrain.setImages(newImages);
}

RecognitionTrainingProvider::~RecognitionTrainingProvider()
{
}

ImageListProvider* RecognitionTrainingProvider::newImages(const Identity& id)
{
    if (m_identity == id)
    {
        m_toTrain.reset();

        return &m_toTrain;
    }

    return &m_empty;
}

ImageListProvider* RecognitionTrainingProvider::images(const Identity&)
{
    return &m_empty;
}

} // namespace Digikam
