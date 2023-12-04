// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "PlaybackSoundCueItem.h"

#include "GeoDataSoundCue.h"

#include <QUrl>

namespace Marble
{
PlaybackSoundCueItem::PlaybackSoundCueItem( const GeoDataSoundCue* soundCue ) :
    m_soundCue( soundCue ),
    m_href( soundCue->href() )
{
}

const GeoDataSoundCue* PlaybackSoundCueItem::soundCue() const
{
    return m_soundCue;
}

double PlaybackSoundCueItem::duration() const
{
    return 0;
}

void PlaybackSoundCueItem::play()
{
}

void PlaybackSoundCueItem::pause()
{
}

void PlaybackSoundCueItem::seek( double progress )
{
    Q_UNUSED( progress )
}

void PlaybackSoundCueItem::stop()
{
}

}

#include "moc_PlaybackSoundCueItem.cpp"
