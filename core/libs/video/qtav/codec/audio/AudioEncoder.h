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

#ifndef QTAV_AUDIO_ENCODER_H
#define QTAV_AUDIO_ENCODER_H

// Qt includes

#include <QStringList>

// Local includes

#include "AVEncoder.h"
#include "AudioFrame.h"

namespace QtAV
{

typedef int AudioEncoderId;

class AudioEncoderPrivate;

class DIGIKAM_EXPORT AudioEncoder : public AVEncoder
{
    Q_OBJECT
    Q_PROPERTY(QtAV::AudioFormat audioFormat READ audioFormat WRITE setAudioFormat NOTIFY audioFormatChanged)
    Q_DISABLE_COPY(AudioEncoder)
    DPTR_DECLARE_PRIVATE(AudioEncoder)

public:

    static QStringList supportedCodecs();
    static AudioEncoder* create(AudioEncoderId id);

    /*!
     * \brief create
     * create an encoder from registered names
     * \param name can be "FFmpeg". FFmpeg encoder will be created for empty name
     * \return 0 if not registered
     */
    static AudioEncoder* create(const char* name = "FFmpeg");

    virtual AudioEncoderId id() const = 0;

    QString name() const override; // name from factory

    /*!
     * \brief encode
     * encode a audio frame to a Packet
     * \param frame pass an invalid frame to get delayed frames
     * \return
     */
    virtual bool encode(const AudioFrame& frame = AudioFrame()) = 0;

    /// output parameters

    /*!
     * \brief audioFormat
     * If not set or set to an invalid format, a supported format will be used
     * and audioFormat() will be that format after open()
     * \return
     * TODO: check supported formats
     */
    const AudioFormat& audioFormat() const;
    void setAudioFormat(const AudioFormat& format);

    int frameSize() const;

Q_SIGNALS:

    void audioFormatChanged();

public:

    template<class C>
    static bool Register(AudioEncoderId id, const char* name)
    {
        return Register(id, create<C>, name);
    }

    /*!
     * \brief next
     * \param id nullptr to get the first id address
     * \return address of id or nullptr if not found/end
     */
    static AudioEncoderId* next(AudioEncoderId* id = nullptr);
    static const char* name(AudioEncoderId id);
    static AudioEncoderId id(const char* name);

private:

    template<class C> static AudioEncoder* create()
    {
        return new C();
    }

    typedef AudioEncoder* (*AudioEncoderCreator)();

    static bool Register(AudioEncoderId id, AudioEncoderCreator, const char* name);

protected:

    AudioEncoder(AudioEncoderPrivate& d);

private:

    AudioEncoder();
    AudioEncoder(QObject*);
};

} // namespace QtAV

#endif // QTAV_AUDIO_ENCODER_H
