/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-07-21
 * Description : MJPEG Stream Server configuration dialog - Private container
 *
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2021 by Quoc Hưng Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "mjpegstreamdlg_p.h"

namespace DigikamGenericMjpegStreamPlugin
{

MjpegStreamDlg::Private::Private()
    : dirty               (false),
      mngr                (MjpegServerMngr::instance()),
      srvButton           (nullptr),
      srvStatus           (nullptr),
      srvPreview          (nullptr),
      progress            (nullptr),
      aStats              (nullptr),
      separator           (nullptr),
      iStats              (nullptr),
      startOnStartup      (nullptr),
      spacing             (0),
      albumSupport        (false),
      albumSelector       (nullptr),
      listView            (nullptr),
      buttons             (nullptr),
      streamSettings      (nullptr),
      srvPort             (nullptr),
      delay               (nullptr),
      rate                (nullptr),
      quality             (nullptr),
      streamLoop          (nullptr),
      typeVal             (nullptr),
      effVal              (nullptr),
      effPreview          (nullptr),
      transVal            (nullptr),
      transPreview        (nullptr),
      tabView             (nullptr),
      showName            (nullptr),
      showDate            (nullptr),
      showApertureFocal   (nullptr),
      showExpoSensitivity (nullptr),
      showMakeModel       (nullptr),
      showLensModel       (nullptr),
      showComment         (nullptr),
      showTitle           (nullptr),
      showTags            (nullptr),
      showRating          (nullptr),
      showCapIfNoTitle    (nullptr),
      osdFont             (nullptr)
{
}

MjpegStreamDlg::Private::~Private()
{
}

} // namespace DigikamGenericMjpegStreamPlugin
