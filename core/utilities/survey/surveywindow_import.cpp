/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-11-22
 * Description : digiKam Survey tool - Import tools
 *
 * SPDX-FileCopyrightText: 2007-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "surveywindow_p.h"

namespace Digikam
{

void SurveyWindow::slotImportedImagefromScanner(const QUrl& url)
{
    ItemInfo info = ScanController::instance()->scannedInfo(url.toLocalFile());
//    loadItemInfos(ItemInfoList(QList<ItemInfo>() << info), info, true);
}

} // namespace Digikam
