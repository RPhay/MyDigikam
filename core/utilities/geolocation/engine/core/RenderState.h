/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2023-05-15
 * Description : geolocation engine based on Marble.
 *
 * SPDX-FileCopyrightText: 2007-2022 Marble Team
 * SPDX-FileCopyrightText: 2023-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * ============================================================ */

#pragma once

#include "digikam_export.h"
#include "MarbleGlobal.h"

#include <QDebug>

namespace Marble
{

class DIGIKAM_EXPORT RenderState
{
public:

    explicit RenderState(const QString& name = QString(), RenderStatus status = Complete);
    RenderState(const RenderState& other);
    RenderState& operator=(const RenderState& other);
    ~RenderState();

    RenderStatus status() const;
    QString name() const;
    int children() const;
    RenderState childAt(int index) const;
    void addChild(const RenderState& child);
    operator QString() const;

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Marble
