/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-02-06
 * Description : image editor printing interface.
 *
 * SPDX-FileCopyrightText: 2009      by Angelo Naselli <anaselli at linux dot it>
 * SPDX-FileCopyrightText: 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_PRINT_HELPER_H
#define DIGIKAM_PRINT_HELPER_H

// Local includes

#include "dimg.h"

class QWidget;

using namespace Digikam;

namespace DigikamEditorPrintToolPlugin
{

class PrintHelper
{

public:

    explicit PrintHelper(QWidget* const parent);
    ~PrintHelper();

    void print(DImg&);

private:

    // Disable
    PrintHelper(const PrintHelper&)            = delete;
    PrintHelper& operator=(const PrintHelper&) = delete;

private:

    class Private;
    Private* const d;
};

} // namespace DigikamEditorPrintToolPlugin

#endif // DIGIKAM_PRINT_HELPER_H
