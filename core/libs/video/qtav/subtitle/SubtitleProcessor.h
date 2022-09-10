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

#ifndef QTAV_SUBTITLE_PROCESSOR_H
#define QTAV_SUBTITLE_PROCESSOR_H

// Qt includes

#include <QList>
#include <QImage>

// Local includes

#include "QtAV_Global.h"
#include "Subtitle.h"

namespace QtAV
{

typedef QString SubtitleProcessorId;

class DIGIKAM_EXPORT SubtitleProcessor
{
public:

    SubtitleProcessor();
    virtual ~SubtitleProcessor()
    {
    }

    virtual SubtitleProcessorId id()        const = 0;
    virtual QString name()                  const = 0;

    /*!
     * \brief supportedTypes
     * \return a list of supported suffixes. e.g. [ "ass", "ssa", "srt" ]
     * used to find subtitle files with given suffixes
     */
    virtual QStringList supportedTypes()    const = 0;

    /*!
     * \brief process
     * process subtitle from QIODevice.
     * \param dev is open and you don't have to close it
     * \return false if failed or does not supports iodevice,
     * e.g. does not support sequential device
     */
    virtual bool process(QIODevice* dev)          = 0;

    /*!
     * \brief process
     * default behavior is calling process(QFile*)
     * \param path
     * \return false if failed or does not support file
     */
    virtual bool process(const QString& path);

    /*!
     * \brief timestamps
     * call this after process(). SubtitleFrame.text must be set
     * \return
     */
    virtual QList<SubtitleFrame> frames() const = 0;

    virtual bool canRender() const
    {
        return false;
    }

    // return false if not supported

    virtual bool processHeader(const QByteArray& codec, const QByteArray& data)
    {
        Q_UNUSED(codec);
        Q_UNUSED(data);

        return false;
    }

    // return timestamp, insert it to Subtitle's internal linkedlist.
    // can be invalid if only support renderering

    virtual SubtitleFrame processLine(const QByteArray& data, qreal pts = -1, qreal duration = 0) = 0;
    virtual QString getText(qreal pts) const                                                      = 0;

    // default null image

    virtual QImage getImage(qreal pts, QRect* boundingRect = nullptr);
    virtual SubImageSet getSubImages(qreal pts, QRect* boundingRect = nullptr);
    void setFrameSize(int width, int height);
    QSize frameSize() const;
    int frameWidth()  const;
    int frameHeight() const;

    // font properties: libass only now

    virtual void setFontFile(const QString& file)
    {
        Q_UNUSED(file);
    }

    virtual void setFontsDir(const QString& dir)
    {
        Q_UNUSED(dir);
    }

    virtual void setFontFileForced(bool force)
    {
        Q_UNUSED(force);
    }

public:

    static void registerAll();

    template<class C>
    static bool Register(const SubtitleProcessorId& id, const char* name)
    {
        return Register(id, create<C>, name);
    }

    static SubtitleProcessor* create(SubtitleProcessorId id);
    static SubtitleProcessor* create(const char* name = "FFmpeg");

    /*!
     * \brief next
     * \param id nullptr to get the first id address
     * \return address of id or nullptr if not found/end
     */
    static SubtitleProcessorId* next(SubtitleProcessorId* id = nullptr);
    static const char* name(SubtitleProcessorId id);
    static SubtitleProcessorId id(const char* name);

private:

    template<class C> static SubtitleProcessor* create()
    {
        return new C();
    }

    typedef SubtitleProcessor* (*SubtitleProcessorCreator)();

    static bool Register(SubtitleProcessorId id, SubtitleProcessorCreator, const char* name);

protected:

    // default do nothing

    virtual void onFrameSizeChanged(int width, int height);

private:

    int m_width  = 0;
    int m_height = 0;

private:

    Q_DISABLE_COPY(SubtitleProcessor);
};

} // namespace QtAV

#endif // QTAV_SUBTITLE_PROCESSOR_H
