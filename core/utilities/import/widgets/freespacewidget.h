/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2007-08-31
 * Description : a widget to display free space for a mount-point.
 *
 * SPDX-FileCopyrightText: 2007-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FREE_SPACE_WIDGET_H
#define DIGIKAM_FREE_SPACE_WIDGET_H

// Qt includes

#include <QString>
#include <QWidget>

namespace Digikam
{

class FreeSpaceWidget : public QWidget
{
    Q_OBJECT

public:

    enum FreeSpaceMode
    {
        AlbumLibrary = 0,
        UMSCamera,
        GPhotoCamera
    };

public:

    explicit FreeSpaceWidget(QWidget* const parent, int width);
    ~FreeSpaceWidget()                                override;

    void          setEstimatedDSizeKb(unsigned long dSize);
    unsigned long estimatedDSizeKb()           const;

    bool          isValid()                    const;
    int           percentUsed()                const;
    unsigned long kBSize()                     const;
    unsigned long kBUsed()                     const;
    unsigned long kBAvail()                    const;
    unsigned long kBAvail(const QString& path) const;

    void          refresh();

    void setMode(FreeSpaceMode mode);
    void setPath(const QString& path);
    void setPaths(const QStringList& paths);
    void addInformation(unsigned long kBSize,
                        unsigned long kBUsed,
                        unsigned long kBAvail,
                        const QString& mountPoint);

protected:

#if (QT_VERSION > QT_VERSION_CHECK(5, 99, 0))
    void enterEvent(QEnterEvent*)                     override;
#else
    void enterEvent(QEvent*)                          override;
#endif

    void leaveEvent(QEvent*)                          override;
    void paintEvent(QPaintEvent*)                     override;
    void updateToolTip();

private Q_SLOTS:

    void slotTimeout();

private:

    class Private;
    Private* const d;
};

} // namespace Digikam

#endif // DIGIKAM_FREE_SPACE_WIDGET_H
