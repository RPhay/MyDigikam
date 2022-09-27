/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2005-02-06
 * Description : undo actions manager for image editor.
 *
 * SPDX-FileCopyrightText: 2005      by Renchi Raju <renchi dot raju at gmail dot com>
 * SPDX-FileCopyrightText: 2005      by Joern Ahrens <joern dot ahrens at kdemail dot net>
 * SPDX-FileCopyrightText: 2006-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_UNDO_ACTION_H
#define DIGIKAM_UNDO_ACTION_H

// Local includes

#include "digikam_export.h"
#include "dimagehistory.h"
#include "dimg.h"
#include "dimgbuiltinfilter.h"

namespace Digikam
{

class EditorCore;

class DIGIKAM_EXPORT UndoMetadataContainer
{
public:

    /**
     * Fill a container from the DImg
     */
    static UndoMetadataContainer fromImage(const DImg& img);

    /**
     * Write this container's values to the DImg
     */
    void toImage(DImg& img) const;

    bool changesIccProfile(const DImg& target) const;

public:

    DImageHistory history;
    IccProfile    profile;
};

// -------------------------------------------------------------------

class DIGIKAM_EXPORT UndoAction
{

public:

    explicit UndoAction(EditorCore* const core);
    virtual ~UndoAction();

    void                  setTitle(const QString& title);
    QString               getTitle()                  const;

    void                  setMetadata(const UndoMetadataContainer&);
    UndoMetadataContainer getMetadata()               const;

    void                  setFileOriginData(const QVariant& data,
                                            const DImageHistory& resolvedInitialHistory);
    bool                  hasFileOriginData()         const;
    QVariant              fileOriginData()            const;
    DImageHistory         fileOriginResolvedHistory() const;

private:

    // Disable
    UndoAction(const UndoAction&)            = delete;
    UndoAction& operator=(const UndoAction&) = delete;

private:

    class Private;
    Private* const d;
};

// --------------------------------------------------------------------

class DIGIKAM_EXPORT UndoActionReversible : public UndoAction
{

public:

    explicit UndoActionReversible(EditorCore* const core,
                                  const DImgBuiltinFilter& reversibleFilter);

    DImgBuiltinFilter getFilter()        const;
    DImgBuiltinFilter getReverseFilter() const;

protected:

    DImgBuiltinFilter m_filter;
};

// --------------------------------------------------------------------

class DIGIKAM_EXPORT UndoActionIrreversible : public UndoAction
{

public:

    explicit UndoActionIrreversible(EditorCore* const core,
                                    const QString& title = QString());
    ~UndoActionIrreversible() override;
};

} // namespace Digikam

#endif // DIGIKAM_UNDOACTION_H
