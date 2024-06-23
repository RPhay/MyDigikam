/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-04-30
 * Description : Graphics View items for DImg
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QGraphicsItem>
#include <QObject>

// Local includes

#include "digikam_export.h"
#include "graphicsdimgitem.h"

namespace Digikam
{

class DImg;
class ICCSettingsContainer;
class LoadingDescription;
class PreviewSettings;

class DIGIKAM_EXPORT DImgPreviewItem : public GraphicsDImgItem
{
    Q_OBJECT

public:

    enum State
    {
        NoImage,
        Loading,
        ImageLoaded,
        ImageLoadingFailed
    };

public:

    explicit DImgPreviewItem(QGraphicsItem* const parent = nullptr);
    ~DImgPreviewItem()                override;

    void setDisplayingWidget(QWidget* const widget);
    void setPreviewSettings(const PreviewSettings& settings);

    QString path()              const;
    void    setPath(const QString& path, bool rePreview = false);

    State state()               const;
    bool  isLoaded()            const;
    void  reload();

    void setPreloadPaths(const QStringList& pathsToPreload);

    QString userLoadingHint()   const override;

Q_SIGNALS:

    void stateChanged(int state);
    void loaded();
    void loadingFailed();

private Q_SLOTS:

    void slotGotImagePreview(const LoadingDescription& loadingDescription, const DImg& image);
    void preloadNext();
    void slotFileChanged(const QString& path);
    void iccSettingsChanged(const ICCSettingsContainer& current, const ICCSettingsContainer& previous);

private:

    // Disable
    DImgPreviewItem(QObject*);

    class DImgPreviewItemPrivate;
    Q_DECLARE_PRIVATE(DImgPreviewItem)

protected:

    explicit DImgPreviewItem(DImgPreviewItemPrivate& dd, QGraphicsItem* const parent = nullptr);
};

} // namespace Digikam
