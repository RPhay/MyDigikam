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

#ifndef QTAV_AV_DECODER_H
#define QTAV_AV_DECODER_H

// Qt includes

#include <QVariant>
#include <QObject>

// Local includes

#include "AVError.h"

namespace QtAV
{

class Packet;
class AVDecoderPrivate;

class DIGIKAM_EXPORT AVDecoder : public QObject
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(AVDecoder)

public:

    virtual ~AVDecoder();
    virtual QString name() const;
    virtual QString description() const;

    /*!
     * default is open FFmpeg codec context
     * codec config must be done before open
     * NOTE: open() and close() are not thread safe. You'd better call them in the same thread.
     */
    virtual bool open();
    virtual bool close();
    bool isOpen() const;
    virtual void flush();
    void setCodecContext(void* codecCtx); // protected
    void* codecContext() const;

    /**
     * not available if AVCodecContext == 0
     */
    bool isAvailable() const;

    virtual bool decode(const Packet& packet) = 0;
    int undecodedSize() const; // TODO: remove. always decode whole input data completely

    // avcodec_open2

    /*!
     * \brief setOptions
     * 1. If has key "avcodec", it's value (suboption, a hash or map) will be used to set AVCodecContext use av_opt_set and av_dict_set. A value of hash type is ignored.
     * we can ignore the flags used in av_dict_xxx because we can use hash api.
     * empty value does nothing to current context if it is open, but will clear AVDictionary in the next open.
     * AVDictionary is used in avcodec_open2() and will not change unless user call setOptions().
     * 2. Set QObject properties for AVDecoder. Use AVDecoder::name() or lower case as a key to set properties. If key not found, assume key is "avcodec"
     * 3. If no ket AVDecoder::name() found in the option, set key-value pairs as QObject property-value pairs.
     * \param dict
     * example:
     *  "avcodec": {"vismv":"pf"}, "vaapi":{"display":"DRM"}, "copyMode": "ZeroCopy"
     *  means set avcodec context option vismv=>pf, VA-API display (qt property) to DRM when using VA-API, set copyMode (GPU decoders) property to ZeroCopy
     */
    void setOptions(const QVariantHash& dict);
    QVariantHash options() const;

Q_SIGNALS:

    void error(const QtAV::AVError& e); // explictly use QtAV::AVError in connection
    void descriptionChanged();
    void codecNameChanged();

protected:

    AVDecoder(AVDecoderPrivate& d);
    DPTR_DECLARE(AVDecoder)

    /**
     * force a codec. only used by avcodec sw decoders. TODO: move to public? profile set?
     */
    void setCodecName(const QString& name);
    QString codecName() const;

private:

    Q_DISABLE_COPY(AVDecoder)
    AVDecoder();                        // base class, not direct create. only final class has is enough
    AVDecoder(QObject*);
};

} // namespace QtAV

#endif // QTAV_QAV_DECODER_H
