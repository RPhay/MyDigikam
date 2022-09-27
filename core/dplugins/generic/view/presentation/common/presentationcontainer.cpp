/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-10-02
 * Description : a presentation tool.
 *
 * SPDX-FileCopyrightText: 2008-2009 by Valerio Fuoglio <valerio dot fuoglio at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

// Local includes

#include "presentationcontainer.h"

namespace DigikamGenericPresentationPlugin
{

PresentationContainer::PresentationContainer()
    : delayMsMaxValue               (0),
      delayMsMinValue               (0),
      delayMsLineStep               (0),
      urlList                       (QList<QUrl>()),
      mainPage                      (nullptr),
      captionPage                   (nullptr),

#ifdef HAVE_MEDIAPLAYER

      soundtrackPage                (nullptr),

#endif

      advancedPage                  (nullptr),

      opengl                        (false),
      openGlFullScale               (false),
      delay                         (0),
      printFileName                 (false),
      printProgress                 (false),
      printFileComments             (false),
      loop                          (false),
      shuffle                       (false),
      offAutoDelay                  (false),
      commentsFontColor             (0),
      commentsBgColor               (0),
      commentsDrawOutline           (false),
      bgOpacity                     (10),
      commentsLinesLength           (0),
      captionFont                   (nullptr),

      soundtrackLoop                (false),
      soundtrackPlay                (false),
      soundtrackRememberPlaylist    (false),
      soundtrackPlayListNeedsUpdate (false),

      useMilliseconds               (false),
      enableMouseWheel              (false),
      enableCache                   (false),
      kbDisableFadeInOut            (false),
      kbDisableCrossFade            (false),
      kbEnableSameSpeed             (false),
      cacheSize                     (0),

      iface                         (nullptr)
{
}

PresentationContainer::~PresentationContainer()
{
    delete captionFont;
}

} // namespace DigikamGenericPresentationPlugin
