/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2017-05-24
 * Description : video frame effects manager.
 *
 * SPDX-FileCopyrightText: 2017-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_EFFECT_MNGR_H
#define DIGIKAM_EFFECT_MNGR_H

// Qt includes

#include <QMap>
#include <QString>
#include <QImage>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT EffectMngr
{
public:

    /**
     * See KEn Burns effect description: https://en.wikipedia.org/wiki/Ken_Burns_effect
     */
    enum EffectType
    {
        None = 0,        ///< Static camera
        KenBurnsZoomIn,
        KenBurnsZoomOut,
        KenBurnsPanLR,
        KenBurnsPanRL,
        KenBurnsPanTB,
        KenBurnsPanBT,
        Random
    };

public:

    explicit EffectMngr();
    ~EffectMngr();

    void setOutputSize(const QSize& size);
    void setEffect(EffectType eff);
    void setImage(const QImage& img);
    void setFrames(int ifrms);

    QImage currentFrame(int& tmout);

    static QMap<EffectType, QString> effectNames();

private:

    // Disable
    EffectMngr(const EffectMngr&)            = delete;
    EffectMngr& operator=(const EffectMngr&) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_EFFECT_MNGR_H
