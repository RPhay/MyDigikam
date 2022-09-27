/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-02-06
 * Description : Config panel for generic dplugins.
 *
 * SPDX-FileCopyrightText: 2018-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DPLUGIN_SETUP_H
#define DIGIKAM_DPLUGIN_SETUP_H

// Qt includes

#include <QWidget>

// Local includes

#include "searchtextbar.h"
#include "digikam_export.h"
#include "dpluginaction.h"

class QTreeWidgetItem;

namespace Digikam
{

class DPluginConfView;

class DIGIKAM_EXPORT DPluginSetup : public QWidget
{
    Q_OBJECT

public:

    explicit DPluginSetup(QWidget* const parent = nullptr);
    ~DPluginSetup() override;

    void setPluginConfView(DPluginConfView* const view);
    void applySettings();

private Q_SLOTS:

    void slotSearchTextChanged(const SearchTextSettings& settings);
    void slotSetFilter(const QString& filter, Qt::CaseSensitivity cs);
    void slotCheckAll();
    void slotClearList();
    void slotItemClicked();
    void slotSearchResult(int found);
    void slotAboutPlugin(QTreeWidgetItem*, int);

private:

    void updateInfo();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_DPLUGIN_SETUP_H
