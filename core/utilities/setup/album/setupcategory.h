/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2004-01-02
 * Description : album category setup tab.
 *
 * SPDX-FileCopyrightText: 2004-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_SETUP_CATEGORY_H
#define DIGIKAM_SETUP_CATEGORY_H

// Qt includes

#include <QScrollArea>

namespace Digikam
{

class SetupCategory : public QScrollArea
{
    Q_OBJECT

public:

    explicit SetupCategory(QWidget* const parent = nullptr);
    ~SetupCategory() override;

    void applySettings();
    void readSettings();

private Q_SLOTS:

    void slotCategorySelectionChanged();
    void slotAddCategory();
    void slotDelCategory();
    void slotRepCategory();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SETUP_CATEGORY_H
