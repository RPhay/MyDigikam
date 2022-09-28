/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2003-03-09
 * Description : Captions, Tags, and Rating properties editor - private container
 *
 * SPDX-FileCopyrightText: 2003-2005 by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2003-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2006-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2009-2011 by Andi Clemens <andi dot clemens at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2011 by Johannes Wienke <languitar at semipol dot de>
 * SPDX-FileCopyrightText: 2015      by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itemdescedittab_p.h"

namespace Digikam
{

ItemDescEditTab::Private::Private()
    : modified                    (false),
      ignoreItemAttributesWatch   (false),
      ignoreTagChanges            (false),
      togglingSearchSettings      (false),
      recentTagsBtn               (nullptr),
      assignedTagsBtn             (nullptr),
      revertBtn                   (nullptr),
      openTagMngr                 (nullptr),
      moreMenu                    (nullptr),
      applyBtn                    (nullptr),
      moreButton                  (nullptr),
      applyToAllVersionsButton    (nullptr),
      lastSelectedWidget          (nullptr),
      titleEdit                   (nullptr),
      captionsEdit                (nullptr),
      dateTimeEdit                (nullptr),
      tabWidget                   (nullptr),
      tagsSearchBar               (nullptr),
      newTagEdit                  (nullptr),
      tagCheckView                (nullptr),
      templateSelector            (nullptr),
      templateViewer              (nullptr),
      ratingWidget                (nullptr),
      colorLabelSelector          (nullptr),
      pickLabelSelector           (nullptr),
      tagModel                    (nullptr),
      metadataChangeTimer         (nullptr)
{
}

} // namespace Digikam
