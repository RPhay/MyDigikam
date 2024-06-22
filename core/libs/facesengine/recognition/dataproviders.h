/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2013-05-18
 * Description : Wrapper class for face recognition
 *
 * SPDX-FileCopyrightText:      2013 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2014-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QImage>
#include <QList>

// Local includes

#include "digikam_export.h"
#include "identity.h"

namespace Digikam
{

/**
 * This class provides access to a list of unspecified entities,
 * where for each entry a QImage can be provided.
 * Only forward iteration is required.
 */
class DIGIKAM_GUI_EXPORT ImageListProvider
{
public:

    ImageListProvider()                           = default;
    virtual ~ImageListProvider()                  = default;

    virtual int            size() const           = 0;
    virtual bool           atEnd() const          = 0;
    virtual void           proceed(int steps = 1) = 0;
    virtual QImage*        image()                = 0;
    virtual QList<QImage*> images()               = 0;
    virtual void setImages(const QList<QImage*>&) = 0;

private:

    Q_DISABLE_COPY(ImageListProvider)
};

// ----------------------------------------------------------------------------------------

/**
 * A wrapper implementation for ImageListProvider if you have a QList of QImages
 */
class DIGIKAM_GUI_EXPORT QListImageListProvider : public ImageListProvider
{
public:

    QListImageListProvider();
    ~QListImageListProvider()             override;

    void reset();

public:

    int            size()  const          override;
    bool           atEnd() const          override;
    void           proceed(int steps = 1) override;
    QImage*        image()                override;
    QList<QImage*> images()               override;
    void setImages(const QList<QImage*>&) override;

public:

    QList<QImage*>                 list;
    QList<QImage*>::const_iterator it;

private:

    Q_DISABLE_COPY(QListImageListProvider)
};

// ----------------------------------------------------------------------------------------

class DIGIKAM_GUI_EXPORT EmptyImageListProvider : public ImageListProvider
{
public:

    EmptyImageListProvider()              = default;
    ~EmptyImageListProvider()             = default;

    int     size()  const                 override;
    bool    atEnd() const                 override;
    void    proceed(int steps = 1)        override;
    QImage* image()                       override;
    QList<QImage*> images()               override;
    void setImages(const QList<QImage*>&) override;

private:

    Q_DISABLE_COPY(EmptyImageListProvider)
};

// ----------------------------------------------------------------------------------------

/**
 * A TrainingDataProvider provides a call-back interface
 * for the training process to retrieve the necessary information.
 * It is not specified, but depends on the backend which of the methods
 * in which order and for which identities will be called.
 */
class DIGIKAM_GUI_EXPORT TrainingDataProvider
{
public:

    TrainingDataProvider()                                         = default;
    virtual ~TrainingDataProvider()                                = default;

    /**
     * Provides those images for the given identity that have not yet been
     * supplied for training.
     * Ownership of the returned object stays with the TrainingDataProvider.
     */
    virtual ImageListProvider* newImages(const Identity& identity) = 0;

    /**
     * Provides all images known for the given identity.
     * Ownership of the returned object stays with the TrainingDataProvider.
     */
    virtual ImageListProvider* images(const Identity& identity)    = 0;

private:

    Q_DISABLE_COPY(TrainingDataProvider)
};

} // namespace Digikam
