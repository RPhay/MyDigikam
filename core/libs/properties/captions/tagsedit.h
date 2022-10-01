/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-07-12
 * Description : tags editor
 *
 * SPDX-FileCopyrightText: 2009-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_TAGS_EDIT_H
#define DIGIKAM_TAGS_EDIT_H

// Qt includes

#include <QWidget>
#include <QScrollArea>

// Local includes

#include "digikam_export.h"

namespace Digikam
{

class DIGIKAM_GUI_EXPORT TagsEdit : public QScrollArea
{
    Q_OBJECT

public:

    explicit TagsEdit(QWidget* const parent);
    ~TagsEdit()                          override;

Q_SIGNALS:

    void signalModified();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_TAGS_EDIT_H
