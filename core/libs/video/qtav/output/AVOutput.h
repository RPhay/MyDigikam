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

#ifndef QTAV_AV_OUTPUT_H
#define QTAV_AV_OUTPUT_H

// Qt includes

#include <QByteArray>

// Local includes

#include "QtAV_Global.h"

/*!
 * TODO: add api id(), name(), detail()
 */

namespace QtAV
{

class AVDecoder;
class AVOutputPrivate;
class Filter;
class Statistics;
class OutputSet;

class DIGIKAM_EXPORT AVOutput
{
    DPTR_DECLARE_PRIVATE(AVOutput)

public:

    AVOutput();
    virtual ~AVOutput();

    bool isAvailable() const;
/*
    void addSource(AVPlayerCore* player); //call player.addVideoRenderer(this)
    void removeSource(AVPlayerCore* player);
*/
    // Demuxer thread automatically paused because packets will be full
    // only pause the renderering, the thread going on.
    // If all outputs are paused, then pause the thread(OutputSet.tryPause)
    // TODO: what about audio's pause api?

    void pause(bool p); // processEvents when waiting?
    bool isPaused() const;

    QList<Filter*>& filters();

    /*!
     * \brief installFilter
     * Insert a filter at position 'index' of current filter list.
     * If the filter is already installed, it will move to the correct index.
     * \param index A nagative index == size() + index. If index >= size(), append at last
     * \return false if already installed
     */
    bool installFilter(Filter* filter, int index = 0x7fffffff);
    bool uninstallFilter(Filter* filter);

protected:

    explicit AVOutput(AVOutputPrivate& d);

    /**
     * If the pause state is true set by pause(true),
     * then block the thread and wait for pause state changed, i.e. pause(false)
     * and return true. Otherwise, return false immediately.
     */
    bool tryPause();                            // move to OutputSet

    // TODO: we need an active set

    void addOutputSet(OutputSet* set);
    void removeOutputSet(OutputSet* set);
    void attach(OutputSet* set);                // add this to set
    void detach(OutputSet* set = nullptr);      // detatch from (all, if 0) output set(s)

    // for thread safe

    void hanlePendingTasks();

    DPTR_DECLARE(AVOutput)

private:

    // for proxy VideoOutput

    virtual void setStatistics(Statistics* statistics); // called by friend AVPlayerCore
    virtual bool onInstallFilter(Filter* filter, int index);
    virtual bool onUninstallFilter(Filter* filter);

    // only called in handlePaintEvent. But filters may change.
    // so required by proxy to update it's filters

    virtual bool onHanlePendingTasks(); // return true: proxy update filters

    friend class AVPlayerCore;
    friend class OutputSet;
    friend class VideoOutput;
};

} // namespace QtAV

#endif // QTAV_AV_OUTPUT_H
