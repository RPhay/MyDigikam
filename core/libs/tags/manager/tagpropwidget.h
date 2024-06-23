/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 20013-07-03
 * Description : Tag Properties widget to display tag properties
 *               when a tag or multiple tags are selected
 *
 * SPDX-FileCopyrightText: 2013 by Veaceslav Munteanu <veaceslav dot munteanu90 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QWidget>

// Local includes

#include "digikam_config.h"

namespace Digikam
{

class Album;

class TagPropWidget : public QWidget
{
    Q_OBJECT

public:

    enum ItemsEnable
    {
        DisabledAll,
        EnabledAll,
        IconOnly
    };

public:

    explicit TagPropWidget(QWidget* const parent);
    ~TagPropWidget() override;

Q_SIGNALS:

    void signalTitleEditReady();

public Q_SLOTS:

    void slotSelectionChanged(const QList<Album*>& albums);
    void slotFocusTitleEdit();

private Q_SLOTS:

    void slotIconResetClicked();
    void slotIconChanged();
    void slotDataChanged();
    void slotSaveChanges();
    void slotDiscardChanges();
    void slotReturnPressed();

private:

    /**
     * @brief enableItems - enable items based on selection.
     *                      If no item is selected, disable all,
     *                      if one item selected, enable all,
     *                      if multiple selected, enable icon & icon button
     */
    void enableItems(ItemsEnable value);

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace Digikam
