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

#include "FilterManager.h"

// Qt includes

#include <QMap>

// Local includes

#include "AVPlayerCore.h"
#include "Filter.h"
#include "AVOutput.h"
#include "digikam_debug.h"

namespace QtAV
{

class Q_DECL_HIDDEN FilterManagerPrivate : public DPtrPrivate<FilterManager>
{
public:

    FilterManagerPrivate()
    {
    }

    ~FilterManagerPrivate()
    {
    }

    QList<Filter*>                        pending_release_filters;
    QHash<AVOutput*, QList<Filter*> >     filter_out_map;
    QHash<AVPlayerCore*, QList<Filter*> > afilter_player_map;
    QHash<AVPlayerCore*, QList<Filter*> > vfilter_player_map;

private:

    Q_DISABLE_COPY(FilterManagerPrivate);
};

FilterManager::FilterManager()
{
}

FilterManager::~FilterManager()
{
}

FilterManager& FilterManager::instance()
{
    static FilterManager sMgr;

    return sMgr;
}

bool FilterManager::insert(Filter* filter, QList<Filter*>& filters, int pos)
{
    int p = pos;

    if (p < 0)
        p += filters.size();

    if (p < 0)
        p = 0;

    if (p > filters.size())
        p = filters.size();

    const int index = filters.indexOf(filter);

    // already installed at desired position

    if (p == index)
        return false;

    if (p >= 0)
        filters.removeAt(p);

    filters.insert(p, filter);

    return true;
}

bool FilterManager::registerFilter(Filter* filter, AVOutput* output, int pos)
{
    DPTR_D(FilterManager);
    d.pending_release_filters.removeAll(filter);    // erase?
    QList<Filter*>& fs = d.filter_out_map[output];

    return insert(filter, fs, pos);
}

QList<Filter*> FilterManager::outputFilters(AVOutput* output) const
{
    DPTR_D(const FilterManager);

    return d.filter_out_map.value(output);
}

bool FilterManager::registerAudioFilter(Filter* filter, AVPlayerCore* player, int pos)
{
    DPTR_D(FilterManager);

    d.pending_release_filters.removeAll(filter);    // erase?
    QList<Filter*>& fs = d.afilter_player_map[player];

    return insert(filter, fs, pos);
}

QList<Filter*> FilterManager::audioFilters(AVPlayerCore* player) const
{
    DPTR_D(const FilterManager);

    return d.afilter_player_map.value(player);
}

bool FilterManager::registerVideoFilter(Filter* filter, AVPlayerCore* player, int pos)
{
    DPTR_D(FilterManager);

    d.pending_release_filters.removeAll(filter);    // erase?
    QList<Filter*>& fs = d.vfilter_player_map[player];

    return insert(filter, fs, pos);
}

QList<Filter*> FilterManager::videoFilters(AVPlayerCore* player) const
{
    DPTR_D(const FilterManager);

    return d.vfilter_player_map.value(player);
}

// called by AVOutput/AVPlayerCore.uninstall imediatly

bool FilterManager::unregisterAudioFilter(Filter* filter, AVPlayerCore* player)
{
    DPTR_D(FilterManager);

    QList<Filter*>& fs = d.afilter_player_map[player];
    bool ret           = false;

    if (fs.removeAll(filter) > 0)
    {
        ret = true;
    }

    if (fs.isEmpty())
        d.afilter_player_map.remove(player);

    return ret;
}

bool FilterManager::unregisterVideoFilter(Filter* filter, AVPlayerCore* player)
{
    DPTR_D(FilterManager);

    QList<Filter*>& fs = d.vfilter_player_map[player];
    bool ret           = false;

    if (fs.removeAll(filter) > 0)
    {
        ret = true;
    }

    if (fs.isEmpty())
        d.vfilter_player_map.remove(player);

    return ret;
}

bool FilterManager::unregisterFilter(Filter* filter, AVOutput* output)
{
    DPTR_D(FilterManager);

    QList<Filter*>& fs = d.filter_out_map[output];
    bool ret           = (fs.removeAll(filter) > 0);

    if (fs.isEmpty())
        d.filter_out_map.remove(output);

    return ret;
}

bool FilterManager::uninstallFilter(Filter* filter)
{
    DPTR_D(FilterManager);
    QHash<AVPlayerCore*, QList<Filter*> > map1(d.vfilter_player_map); // NB: copy it for iteration because called code may modify map -- which caused crashes
    QHash<AVPlayerCore*, QList<Filter*> >::iterator it = map1.begin();

    while (it != map1.end())
    {
        if (uninstallVideoFilter(filter, it.key()))
            return true;

        ++it;
    }

    QHash<AVPlayerCore*, QList<Filter*> > map2(d.afilter_player_map); // copy to avoid crashes when called-code modifies map
    it = map2.begin();

    while (it != map2.end())
    {
        if (uninstallAudioFilter(filter, it.key()))
            return true;

        ++it;
    }

    QHash<AVOutput*, QList<Filter*> > map3(d.filter_out_map);    // copy to avoid crashes
    QHash<AVOutput*, QList<Filter*> >::iterator it2 = map3.begin();

    while (it2 != map3.end())
    {
        if (uninstallFilter(filter, it2.key()))
            return true;

        ++it2;
    }

    return false;
}

bool FilterManager::uninstallAudioFilter(Filter* filter, AVPlayerCore* player)
{
    if (unregisterAudioFilter(filter, player))
        return player->uninstallFilter(reinterpret_cast<AudioFilter*>(filter));

    return false;
}

bool FilterManager::uninstallVideoFilter(Filter* filter, AVPlayerCore* player)
{
    if (unregisterVideoFilter(filter, player))
        return player->uninstallFilter(reinterpret_cast<VideoFilter*>(filter));

    return false;
}

bool FilterManager::uninstallFilter(Filter* filter, AVOutput* output)
{
    if (unregisterFilter(filter, output))
        return output->uninstallFilter(filter);

    return false;
}

} // namespace QtAV
