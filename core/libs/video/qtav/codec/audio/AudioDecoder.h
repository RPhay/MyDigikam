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

#ifndef QTAV_AUDIO_DECODER_H
#define QTAV_AUDIO_DECODER_H

// Qt includes

#include <QStringList>

// Local includes

#include "AVDecoder.h"
#include "AudioFrame.h"

// TODO: decoder.in/outAudioFormat()?

namespace QtAV
{

typedef int AudioDecoderId;

// built-in decoders

extern DIGIKAM_EXPORT AudioDecoderId AudioDecoderId_FFmpeg;

class AudioResampler;
class AudioDecoderPrivate;

class DIGIKAM_EXPORT AudioDecoder : public AVDecoder
{
    Q_OBJECT
    Q_DISABLE_COPY(AudioDecoder)
    DPTR_DECLARE_PRIVATE(AudioDecoder)

public:

    static QStringList supportedCodecs();
    static AudioDecoder* create(AudioDecoderId id);

    /*!
     * \brief create
     * create a decoder from registered name. FFmpeg decoder will be created for empty name
     * \param name can be "FFmpeg"
     * \return 0 if not registered
     */
    static AudioDecoder* create(const char* name = "FFmpeg");
    virtual AudioDecoderId id() const = 0;
    QString name()              const override;     // name from factory
    virtual QByteArray data()   const;              // decoded data
    virtual AudioFrame frame()        = 0;
    AudioResampler* resampler();                    // TODO: remove. can not share the same resampler for multiple frames

public:

    template<class C>
    static bool Register(AudioDecoderId id, const char* name)
    {
        return Register(id, create<C>, name);
    }

    /*!
     * \brief next
     * \param id nullptr to get the first id address
     * \return address of id or nullptr if not found/end
     */
    static AudioDecoderId* next(AudioDecoderId* id = nullptr);
    static const char* name(AudioDecoderId id);
    static AudioDecoderId id(const char* name);

private:

    // if QtAV is static linked (ios for example), components may be not automatically registered.
    // Add registerAll() to workaround.

    static void registerAll();

    template<class C>
    static AudioDecoder* create()
    {
        return new C();
    }

    typedef AudioDecoder* (*AudioDecoderCreator)();

    static bool Register(AudioDecoderId id, AudioDecoderCreator, const char* name);

protected:

    AudioDecoder(AudioDecoderPrivate& d);

private:

    AudioDecoder();
    AudioDecoder(QObject*);
};

} // namespace QtAV

#endif // QTAV_AUDIO_DECODER_H
