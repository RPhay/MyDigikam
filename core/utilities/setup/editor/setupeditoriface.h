/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-08-03
 * Description : setup Image Editor interface.
 *
 * SPDX-FileCopyrightText: 2004-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_SETUP_EDITOR_IFACE_H
#define DIGIKAM_SETUP_EDITOR_IFACE_H

// Qt includes

#include <QScrollArea>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_EXPORT SetupEditorIface : public QScrollArea
{
    Q_OBJECT

public:

    explicit SetupEditorIface(QWidget* const parent = nullptr);
    ~SetupEditorIface() override;

    void applySettings();

private Q_SLOTS:

    void slotThemeBackgroundColor(bool);
    void slotExpoSettingsChanged();
    void slotShowOverExpoHistogramGuide(double);
    void slotShowUnderExpoHistogramGuide(double);

private:

    void readSettings();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SETUP_EDITOR_IFACE_H
