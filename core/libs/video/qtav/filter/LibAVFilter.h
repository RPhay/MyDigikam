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

#ifndef QTAV_LIBAV_FILTER_H
#define QTAV_LIBAV_FILTER_H

// Qt includes

#include <QObject>

// Local includes

#include "Filter.h"

namespace QtAV
{

class DIGIKAM_EXPORT LibAVFilter
{
public:

    /*!
     * \brief The Status enum
     * Status of filter graph.
     * If setOptions() is called, the status is NotConfigured, and will to configure when processing
     * a new frame.
     * Filter graph will be reconfigured if options, incoming video frame format or size changed
     */
    enum Status
    {
        NotConfigured,
        ConfigureFailed,
        ConfigureOk
    };

public:

    static QString filterDescription(const QString& filterName);
    static QStringList videoFilters();
    static QStringList audioFilters();

    LibAVFilter();
    virtual ~LibAVFilter();

    /*!
     * \brief setOptions
     * Set new option. Filter graph will be setup if receives a frame if options changed.
     * \param options option string for libavfilter. libav and ffmpeg are different
     */
    void setOptions(const QString& options);
    QString options() const;

    Status status() const;

protected:

    virtual QString sourceArguments() const = 0;
    bool pushVideoFrame(Frame* frame, bool changed);
    bool pushAudioFrame(Frame* frame, bool changed);
    void* pullFrameHolder();
    static QStringList registeredFilters(int type); // filters whose input/output type matches

    virtual void emitOptionsChanged() = 0;

protected:

    class Private;
    Private* priv;

private:

    // Disable

    LibAVFilter(const LibAVFilter&)            = delete;
    LibAVFilter& operator=(const LibAVFilter&) = delete;
};

// ---------------------------------------------------------------------------------

class DIGIKAM_EXPORT LibAVFilterVideo : public VideoFilter,
                                        public LibAVFilter
{
    Q_OBJECT
    Q_PROPERTY(QString options READ options WRITE setOptions NOTIFY optionsChanged)
    Q_PROPERTY(QStringList filters READ filters)

public:

    explicit LibAVFilterVideo(QObject* const parent = nullptr);

    bool isSupported(VideoFilterContext::Type t) const override
    {
        return (t == VideoFilterContext::None);
    }

    QStringList filters() const; // the same as LibAVFilter::videoFilters

Q_SIGNALS:

    void optionsChanged();

protected:

    void process(Statistics* statistics, VideoFrame* frame) override;
    QString sourceArguments() const override;

private:

    void emitOptionsChanged() override;
};

// ---------------------------------------------------------------------------------

class DIGIKAM_EXPORT LibAVFilterAudio : public AudioFilter,
                                        public LibAVFilter
{
    Q_OBJECT
    Q_PROPERTY(QString options READ options WRITE setOptions NOTIFY optionsChanged)
    Q_PROPERTY(QStringList filters READ filters)

public:

    explicit LibAVFilterAudio(QObject* const parent = nullptr);

    QStringList filters() const; // the same as LibAVFilter::audioFilters

Q_SIGNALS:

    void optionsChanged();

protected:

    void process(Statistics* statistics, AudioFrame* frame) override;
    QString sourceArguments()                         const override;

private:

    void emitOptionsChanged() override;
};

} // namespace QtAV

#endif // QTAV_LIBAV_FILTER_H
