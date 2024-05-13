/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-07-11
 * Description : shared libraries list dialog common to digiKam and Showfoto
 *
 * SPDX-FileCopyrightText: 2008-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_LIBS_INFO_DLG_H
#define DIGIKAM_LIBS_INFO_DLG_H

// Qt includes

#include <QMap>
#include <QTreeWidgetItem>

// Local includes

#include "digikam_export.h"
#include "infodlg.h"

namespace cv::ocl
{
    class Device;
}

namespace Digikam
{

class DIGIKAM_EXPORT LibsInfoDlg : public InfoDlg
{
    Q_OBJECT

public:

    explicit LibsInfoDlg(QWidget* const parent);
    ~LibsInfoDlg() = default;

protected:

    QTreeWidgetItem* m_features  = nullptr;
    QTreeWidgetItem* m_libraries = nullptr;

private:

    QString checkTriState(int value)                const;

    QString openCVBytesToStringRepr(size_t value)   const;
    QString openCVGetDeviceTypeString(const cv::ocl::Device& device);
};

} // namespace Digikam

#endif // DIGIKAM_LIBS_INFO_DLG_H
