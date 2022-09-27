/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-11-14
 * Description : a presentation tool.
 *
 * SPDX-FileCopyrightText: 2007-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 * SPDX-FileCopyrightText: 2012-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * Parts of this code are based on
 * smoothslidesaver by Carsten Weinhold <carsten dot weinhold at gmx dot de>
 * and slideshowgl by Renchi Raju <renchi dot raju at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_PRESENTATION_KB_PRIVATE_H
#define DIGIKAM_PRESENTATION_KB_PRIVATE_H

#include "presentationkb.h"

// Local includes

#include "digikam_config.h"
#include "digikam_debug.h"
#include "presentationcontainer.h"
#include "kbimageloader.h"
#include "kbeffect.h"

// OpenGL headers is not included automatically with ARM targets

#ifdef Q_PROCESSOR_ARM
#   include <GL/gl.h>
#endif

#ifdef HAVE_MEDIAPLAYER
#   include "presentationaudiowidget.h"
#endif

using namespace Digikam;

namespace DigikamGenericPresentationPlugin
{

class KBImageLoader;
class ScreenProperties;
class PresentationAudioWidget;

class Q_DECL_HIDDEN PresentationKB::Private
{

public:

    explicit Private()
      : deskX               (0),
        deskY               (0),
        deskWidth           (0),
        deskHeight          (0),
        endTexture          (nullptr),
        imageLoadThread     (nullptr),
        mouseMoveTimer      (nullptr),
        timer               (nullptr),
        haveImages          (true),
        effect              (nullptr),
        numKBEffectRepeated (0),
        initialized         (false),
        step                (0.0),
        stepSameSpeed       (0.0),
        endOfShow           (false),
        showingEnd          (false),
        delay               (0),
        disableFadeInOut    (false),
        disableCrossFade    (false),
        enableSameSpeed     (false),
        forceFrameRate      (0),
        sharedData          (nullptr),
        playbackWidget      (nullptr)
    {
        zoomIn   = (QRandomGenerator::global()->bounded(2U) == 0);
        image[0] = nullptr;
        image[1] = nullptr;
    }

    int                      deskX;
    int                      deskY;
    int                      deskWidth;
    int                      deskHeight;

    QOpenGLTexture*          endTexture;
    KBImageLoader*           imageLoadThread;
    QTimer*                  mouseMoveTimer;
    QTimer*                  timer;
    bool                     haveImages;

    KBImage*                 image[2];
    KBEffect*                effect;
    int                      numKBEffectRepeated;
    bool                     zoomIn;
    bool                     initialized;
    float                    step;
    float                    stepSameSpeed;

    bool                     endOfShow;
    bool                     showingEnd;

    int                      delay;
    bool                     disableFadeInOut;
    bool                     disableCrossFade;
    bool                     enableSameSpeed;
    unsigned                 forceFrameRate;

    PresentationContainer*   sharedData;

    PresentationAudioWidget* playbackWidget;
};

} // namespace DigikamGenericPresentationPlugin

#endif // DIGIKAM_PRESENTATION_KB_PRIVATE_H
