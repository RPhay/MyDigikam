/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2009-06-29
 * Description : metadata template viewer.
 *
 * SPDX-FileCopyrightText: 2009-2023 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_TEMPLATE_VIEWER_H
#define DIGIKAM_TEMPLATE_VIEWER_H

// Qt includes

#include <QString>

// Local includes

#include "digikam_export.h"
#include "dexpanderbox.h"

namespace Digikam
{

class Template;

class DIGIKAM_GUI_EXPORT TemplateViewer : public DExpanderBox
{
    Q_OBJECT

public:

    explicit TemplateViewer(QWidget* const parent = nullptr);
    ~TemplateViewer() override;

    void setTemplate(const Template& t);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_TEMPLATE_VIEWER_H
