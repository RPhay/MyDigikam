/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-12-10
 * Description : list of memory features from host computer
 *
 * SPDX-FileCopyrightText:      2010 Pino Toscano <pino at kde dot org>
 * SPDX-FileCopyrightText:      2011 Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_DMEMORY_INFO_H
#define DIGIKAM_DMEMORY_INFO_H

// Qt includes

#include <QSharedDataPointer>

// typedef introduced in Qt 5.10
using qsizetype = QIntegerForSizeof<std::size_t>::Signed;

// Local includes

#include "digikam_export.h"

namespace Digikam
{

/**
 * @brief The DMemoryInfo class provides an interface to get memory information (RAM/SWAP).
 *
 * To use the class, simply create an instance.
 * \code
 * DMemoryInfo memInfo;
 *
 * if (!memInfo.isNull())
 * {
 *     ...
 * }
 * \endcode
 *
 */
class DIGIKAM_EXPORT DMemoryInfo
{
public:

    /**
     * @brief DMemoryInfo
     * Constructs a class with a snapshot of the state of the memory. If an error occurs, a null object is returned.
     * @sa isNull.
     */
    DMemoryInfo();
    ~DMemoryInfo();

    /**
     * @brief DMemoryInfo
     * Constructs a copy of the other memoryinfo.
     */
    DMemoryInfo(const DMemoryInfo& other);

    /**
     * @brief operator =
     * Makes a copy of the other memoryinfo and returns a reference to the copy.
     */
    DMemoryInfo &operator=(const DMemoryInfo& other);

    /**
     * @brief operator ==
     * @return @c true if this memoryinfo is equal to the other memoryinfo, otherwise @c false.
     */
    bool operator==(const DMemoryInfo& other) const;

    /**
     * @brief operator !=
     * @return @c true if this memoryinfo is different from the other memoryinfo, otherwise @c false.
     */
    bool operator!=(const DMemoryInfo& other) const;

    /**
     * @brief isNull
     * @return @c true if the class is null, otherwise @c false.
     */
    bool isNull() const;

    /**
     * @brief totalPhysical
     * @return The total system RAM in bytes.
     */
    quint64 totalPhysical() const;

    /**
     * @brief freePhysical
     *
     * The free memory is the amount of free RAM as reported by the operating system.
     * This value is often tainted with caches and buffers used by the operating system, resulting in a low value.
     * @note Don't use this value to determine if you have enough RAM for your data.
     * @return The free RAM reported by OS in bytes.
     * @sa availablePhysical.
     */
    quint64 freePhysical() const;

    /**
     * @brief availablePhysical
     *
     * The available memory is the free RAM without considering caches and buffers allocated by the operating system.
     * @note You should always use this value to check if there is enough RAM for your data.
     * @return The memory available to the processes in bytes.
     * @sa freePhysical.
     */
    quint64 availablePhysical() const;

    /**
     * @brief cached
     * @return The size of RAM used as cache in bytes.
     */
    quint64 cached() const;

    /**
     * @brief buffers
     * @return The size of RAM used as buffers in bytes. This value can be zero.
     */
    quint64 buffers() const;

    /**
     * @brief totalSwapFile
     * @return The size of swap file in bytes.
     * @note On an operating system where the paging file is dynamically allocated, this value can be zero when no memory pages are swapped.
     */
    quint64 totalSwapFile() const;

    /**
     * @brief freeSwapFile
     * @return The free swap size in bytes.
     */
    quint64 freeSwapFile() const;

private:
    /**
     * @brief update Refresh the memory information.
     * @return @c true on success, otherwise @c false.
     */
    bool update();

private:

    class Private;

    QSharedDataPointer<Private> d;
};

} // namespace Digikam

#endif // DIGIKAM_DMEMORY_INFO_H
