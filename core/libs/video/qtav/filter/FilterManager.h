/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2012-10-31
 * Description : QtAV: Multimedia framework based on Qt and FFmpeg
 *
 * SPDX-FileCopyrightText: 2012-2022 Wang Bin <wbsecg1 at gmail dot com>
 * SPDX-FileCopyrightText:      2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef QTAV_FILTER_MANAGER_H
#define QTAV_FILTER_MANAGER_H

// Qt includes

#include <QList>

// Local includes

#include "Filter.h"         // signal qobj parameter
#include "QtAV_Global.h"

namespace QtAV
{

class AVOutput;
class AVPlayerCore;
class FilterManagerPrivate;

class FilterManager
{
    DPTR_DECLARE_PRIVATE(FilterManager)         // cppcheck-suppress unusedPrivateFunction
    Q_DISABLE_COPY(FilterManager)

public:

    static FilterManager& instance();

    /*!
     * \brief registerFilter
     * record the filter in manager
     * target.installXXXFilter/filter.installTo(target) must call registerXXXFilter
     */
    bool registerFilter(Filter* filter, AVOutput* output, int pos = 0x7FFFFFFF);
    QList<Filter*> outputFilters(AVOutput* output) const;
    bool registerAudioFilter(Filter* filter, AVPlayerCore* player, int pos = 0x7FFFFFFF);
    QList<Filter*> audioFilters(AVPlayerCore* player) const;
    bool registerVideoFilter(Filter* filter, AVPlayerCore* player, int pos = 0x7FFFFFFF);
    QList<Filter*> videoFilters(AVPlayerCore* player) const;
    bool unregisterAudioFilter(Filter* filter, AVPlayerCore* player);
    bool unregisterVideoFilter(Filter* filter, AVPlayerCore* player);
    bool unregisterFilter(Filter* filter, AVOutput* output);

    // unregister and call target.uninstall

    bool uninstallFilter(Filter* filter);                           // called by filter.uninstall
    bool uninstallAudioFilter(Filter* filter, AVPlayerCore* player);
    bool uninstallVideoFilter(Filter* filter, AVPlayerCore* player);
    bool uninstallFilter(Filter* filter, AVOutput* output);

private:

    // return bool is for AVPlayerCore.installAudio/VideoFilter compatibility

    bool insert(Filter* filter, QList<Filter*>& filters, int pos);

    FilterManager();
    ~FilterManager();

    DPTR_DECLARE(FilterManager)
};

} // namespace QtAV

#endif // QTAV_FILTER_MANAGER_H
