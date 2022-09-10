/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-11-24
 * Description : Batch Tools Factory.
 *
 * SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_BQM_BATCH_TOOLS_FACTORY_H
#define DIGIKAM_BQM_BATCH_TOOLS_FACTORY_H

// Qt includes

#include <QObject>

// Local includes

#include "batchtool.h"
#include "batchtoolutils.h"

namespace Digikam
{

class DInfoInterface;

class BatchToolsFactory : public QObject
{
    Q_OBJECT

public:

    static BatchToolsFactory* instance();

public:

    void           registerTool(BatchTool* const tool);
    BatchTool*     findTool(const QString& name, BatchTool::BatchToolGroup group) const;
    BatchToolsList toolsList()                                                    const;
    DInfoInterface* infoIface()                                                   const;

private:

    // Disable
    BatchToolsFactory();
    ~BatchToolsFactory() override;

    explicit BatchToolsFactory(QObject*) = delete;

private:

    class Private;
    Private* const d;

    friend class BatchToolsFactoryCreator;
};

} // namespace Digikam

#endif // DIGIKAM_BQM_BATCH_TOOLS_FACTORY_H
