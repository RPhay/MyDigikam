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

#ifndef QTAV_AUDIO_RESAMPLER_H
#define QTAV_AUDIO_RESAMPLER_H

// Local includes

#include "QtAV_Global.h"

namespace QtAV
{

typedef int AudioResamplerId;

class AudioFormat;
class AudioResamplerPrivate;

class DIGIKAM_EXPORT AudioResampler // export is required for users who want add their own subclass outside QtAV
{
    DPTR_DECLARE_PRIVATE(AudioResampler)

public:

    virtual ~AudioResampler();

    // if QtAV is static linked (ios for example), components may be not
    // automatically registered. Add registerAll() to workaround

    static void registerAll();

    template<class C>
    static bool Register(AudioResamplerId id, const char* name)
    {
        return Register(id, create<C>, name);
    }

    static AudioResampler* create(AudioResamplerId id);

    /*!
     * \brief create
     * Create resampler from name
     * \param name can be "FFmpeg", "Libav"
     */
    static AudioResampler* create(const char* name);

    /*!
     * \brief next
     * \param id nullptr to get the first id address
     * \return address of id or nullptr if not found/end
     */
    static AudioResamplerId* next(AudioResamplerId* id = nullptr);
    static const char* name(AudioResamplerId id);
    static AudioResamplerId id(const char* name);

    QByteArray outData() const;

    /* check whether the parameters are supported. If not, you should use ff*/

    /*!
     * \brief prepare
     * Check whether the parameters are supported and setup the resampler
     * setIn/OutXXX will call prepare() if format is changed
     */
    virtual bool prepare();
    virtual bool convert(const quint8** data);

    // speed: >0, default is 1

    void setSpeed(qreal speed); // out_sample_rate = out_sample_rate/speed
    qreal speed() const;

    void setInAudioFormat(const AudioFormat& format);
    AudioFormat& inAudioFormat();
    const AudioFormat& inAudioFormat() const;

    void setOutAudioFormat(const AudioFormat& format);
    AudioFormat& outAudioFormat();
    const AudioFormat& outAudioFormat() const;

    // decoded frame's samples/channel

    void setInSampesPerChannel(int samples);

    // > 0 valid after resample done

    int outSamplesPerChannel() const;

    // channel count can be computed by av_get_channel_layout_nb_channels(chl)

    void setInSampleRate(int isr);
    void setOutSampleRate(int osr);         // default is in

    // TODO: enum

    void setInSampleFormat(int isf);        // FFmpeg sample format
    void setOutSampleFormat(int osf);       // FFmpeg sample format. set by user. default is in

    // TODO: enum. layout will be set to the default layout of the channels if not defined

    void setInChannelLayout(qint64 icl);
    void setOutChannelLayout(qint64 ocl);   // default is in
    void setInChannels(int channels);
    void setOutChannels(int channels);

    // Are getter functions required?

private:

    AudioResampler();

    template<class C>
    static AudioResampler* create()
    {
        return new C();
    }

    typedef AudioResampler* (*AudioResamplerCreator)();

    static bool Register(AudioResamplerId id, AudioResamplerCreator, const char* name);

protected:

    explicit AudioResampler(AudioResamplerPrivate& d);
    DPTR_DECLARE(AudioResampler)
};

extern DIGIKAM_EXPORT AudioResamplerId AudioResamplerId_FF;
extern DIGIKAM_EXPORT AudioResamplerId AudioResamplerId_Libav;

} // namespace QtAV

#endif // QTAV_AUDIO_RESAMPLER_H
