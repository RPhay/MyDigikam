/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-11-13
 * Description : a tool to blend bracketed images.
 *
 * SPDX-FileCopyrightText: 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2015      by Benjamin Girault <benjamin dot girault at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_EXPO_BLENDING_PREPROCESS_PAGE_H
#define DIGIKAM_EXPO_BLENDING_PREPROCESS_PAGE_H

// Local includes

#include "dwizardpage.h"
#include "expoblendingactions.h"

using namespace Digikam;

namespace DigikamGenericExpoBlendingPlugin
{

class ExpoBlendingManager;

class ExpoBlendingPreProcessPage : public DWizardPage
{
    Q_OBJECT

public:

    explicit ExpoBlendingPreProcessPage(ExpoBlendingManager* const mngr, QWizard* const dlg);
    ~ExpoBlendingPreProcessPage() override;

    void process();
    void cancel();

Q_SIGNALS:

    void signalPreProcessed(const ExpoBlendingItemUrlsMap&);

private Q_SLOTS:

    void slotProgressTimerDone();
    void slotExpoBlendingAction(const DigikamGenericExpoBlendingPlugin::ExpoBlendingActionData&);

private:

    void resetTitle();

private:

    class Private;
    Private* const d;
};

} // namespace DigikamGenericExpoBlendingPlugin

#endif // DIGIKAM_EXPO_BLENDING_PREPROCESS_PAGE_H
