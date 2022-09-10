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

#include "Filter_p.h"

// Local includes

#include "QtAV_Statistics.h"
#include "AVOutput.h"
#include "AVPlayerCore.h"
#include "FilterManager.h"
#include "digikam_debug.h"

/*
 * 1. parent == target (QObject):
 * in ~target(), remove the filter but not delete it (parent not null now).
 * in ~QObject, filter is deleted as a child
 *-----------------------------------------------
 * 2. parent != target.parent:
 * if delete filter first, filter must notify FilterManager (uninstall in dtor here) to uninstall to avoid target to access it (in ~target())
 * if delete target first, target remove the filter but not delete it (parent not null now).
 */

namespace QtAV
{

Filter::Filter(FilterPrivate& d, QObject* const parent)
    : QObject  (parent),
      DPTR_INIT(&d)
{
    if (parent)
        setOwnedByTarget(false);
}

Filter::~Filter()
{
    uninstall();
}

void Filter::setEnabled(bool enabled)
{
    DPTR_D(Filter);

    if (d.enabled == enabled)
        return;

    d.enabled = enabled;

    Q_EMIT enabledChanged(enabled);
}

bool Filter::isEnabled() const
{
    DPTR_D(const Filter); // cppcheck-suppress constVariable

    return d.enabled;
}

void Filter::setOwnedByTarget(bool value)
{
    d_func().owned_by_target = value;
}

bool Filter::isOwnedByTarget() const
{
    return d_func().owned_by_target;
}

bool Filter::uninstall()
{
    return FilterManager::instance().uninstallFilter(this); // TODO: target
}

AudioFilter::AudioFilter(QObject* const parent)
    : Filter(*new AudioFilterPrivate(), parent)
{
}

AudioFilter::AudioFilter(AudioFilterPrivate& d, QObject* const parent)
    : Filter(d, parent)
{
}

/*
 * TODO: move to AVPlayerCore.cpp to reduce dependency?
 */

bool AudioFilter::installTo(AVPlayerCore* player)
{
    return player->installFilter(this);
}

void AudioFilter::apply(Statistics* statistics, AudioFrame* frame)
{
    process(statistics, frame);
}

VideoFilter::VideoFilter(QObject* const parent)
    : Filter(*new VideoFilterPrivate(), parent)
{
}

VideoFilter::VideoFilter(VideoFilterPrivate &d, QObject* const parent)
    : Filter(d, parent)
{
}

VideoFilterContext* VideoFilter::context()
{
    DPTR_D(VideoFilter);

    if (!d.context)
    {
        // Fake. only to store some parameters at the beginnig.
        // It will be destroyed and set to a new instance if context type mismatch in prepareContext, with old parameters

        d.context = VideoFilterContext::create(VideoFilterContext::QtPainter);
    }

    return d.context;
}

bool VideoFilter::isSupported(VideoFilterContext::Type ct) const
{
    // TODO: return false

    return VideoFilterContext::None == ct;
}

bool VideoFilter::installTo(AVPlayerCore* player)
{
    return player->installFilter(this);
}

/* TODO: move to AVOutput.cpp to reduce dependency?*/

/*
 * filter.installTo(target,...) calls target.installFilter(filter)
 * If filter is already registered in FilterManager, then return false
 * Otherwise, call FilterManager.register(filter) and target.filters.push_back(filter), return true
 * NOTE: the installed filter will be deleted by the target if filter is owned by target AND it's parent (QObject) is null.
 */
bool VideoFilter::installTo(AVOutput* output)
{
    return output->installFilter(this);
}

bool VideoFilter::prepareContext(VideoFilterContext*& ctx, Statistics* statistics, VideoFrame* frame)
{
    DPTR_D(VideoFilter);

    if (!ctx || !isSupported(ctx->type()))
    {
/*
        qCDebug(DIGIKAM_QTAV_LOG).noquote()
            << QString::asprintf("no context: %p, or context type %d is not supported",
                ctx, ctx? ctx->type() : 0);
*/
        return isSupported(VideoFilterContext::None);
    }

    if (!d.context || d.context->type() != ctx->type())
    {
        // each filter has it's own context instance, but share the common parameters

        VideoFilterContext* const c = VideoFilterContext::create(ctx->type());

        if (d.context)
        {
            c->pen          = d.context->pen;
            c->brush        = d.context->brush;
            c->clip_path    = d.context->clip_path;
            c->rect         = d.context->rect;
            c->transform    = d.context->transform;
            c->font         = d.context->font;
            c->opacity      = d.context->opacity;
            c->paint_device = d.context->paint_device;
        }

        if (d.context)
        {
            delete d.context;
        }

        d.context = c;
    }

    d.context->video_width  = statistics->video_only.width;
    d.context->video_height = statistics->video_only.height;
    ctx->video_width        = statistics->video_only.width;
    ctx->video_height       = statistics->video_only.height;

    // share common data

    d.context->shareFrom(ctx);
    d.context->initializeOnFrame(frame);
    ctx->shareFrom(d.context);

    return true;
}

void VideoFilter::apply(Statistics* statistics, VideoFrame* frame)
{
    process(statistics, frame);
}

} // namespace QtAV
