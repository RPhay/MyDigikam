/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-11-25
 * Description : a bar used to search a string - version based on database models
 *
 * Copyright (C) 2007-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_SEARCH_TEXT_BAR_DB_H
#define DIGIKAM_SEARCH_TEXT_BAR_DB_H

// Local includes

#include "digikam_export.h"
#include "searchtextbar.h"

namespace Digikam
{

class AbstractAlbumModel;
class AlbumFilterModel;

/**
 * A text input for searching entries with visual feedback.
 * Can be used on Database Models.
 *
 * @author Gilles Caulier
 */
class DIGIKAM_GUI_EXPORT SearchTextBarDb : public SearchTextBar
{
    Q_OBJECT

public:

    explicit SearchTextBarDb(QWidget* const parent,
                             const QString& name,
                             const QString& msg=QString());
    ~SearchTextBarDb() override;

    /**
     * If the given model is != null, the model is used to populate the
     * completion for this text field.
     *
     * @param model to fill from or null for manual mode
     * @param uniqueIdRole a role for which the model will return a unique integer for each entry
     * @param displayRole the role to retrieve the text for completion, default is Qt::DisplayRole.
     */
    void setModel(QAbstractItemModel* model, int uniqueIdRole, int displayRole = Qt::DisplayRole);

    /**
     * Sets the album model this text bar shall use to invoke filtering on and
     * reading the result for highlighting from.
     *
     * @param model album model to use for filtering. <code>null</code>
     *              means there is no model to use and external
     *              connections need to be created with
     *              signalSearchTextSettings and slotSearchResult
     */
    void setModel(AbstractAlbumModel* const model);

    /**
     * Sets the filter model this text bar shall use to invoke filtering on and
     * reading the result for highlighting from.
     *
     * @param filterModel filter model to use for filtering. <code>null</code>
     *                    means there is no model to use and external
     *                    connections need to be created with
     *                    signalSearchTextSettings and slotSearchResult
     */
    void setFilterModel(AlbumFilterModel* const filterModel);

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_SEARCH_TEXT_BAR_DB_H
