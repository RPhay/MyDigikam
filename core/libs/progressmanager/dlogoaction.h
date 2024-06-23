/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-27-08
 * Description : a tool bar action object to display animated logo
 *
 * SPDX-FileCopyrightText: 2007-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QWidgetAction>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT DLogoAction : public QWidgetAction
{
    Q_OBJECT

public:

    explicit DLogoAction(QObject* const parent, bool alignOnright = true);
    ~DLogoAction()                          override;

    void start();
    void stop();
    bool running()                    const;

protected:

    QWidget* createWidget(QWidget* parent) override;
    void     deleteWidget(QWidget* widget) override;

private Q_SLOTS:

    void slotProgressTimerDone();

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
