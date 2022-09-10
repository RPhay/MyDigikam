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

#ifndef QTAV_VIDEO_CAPTURE_H
#define QTAV_VIDEO_CAPTURE_H

// Qt includes

#include <QObject>
#include <QImage>

// Local includes

#include "QtAV_Global.h"
#include "VideoFrame.h"

namespace QtAV
{

// One capture per thread or all in one thread?

class DIGIKAM_EXPORT VideoCapture : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool async READ isAsync WRITE setAsync NOTIFY asyncChanged)
    Q_PROPERTY(bool autoSave READ autoSave WRITE setAutoSave NOTIFY autoSaveChanged)
    Q_PROPERTY(bool originalFormat READ isOriginalFormat WRITE setOriginalFormat NOTIFY originalFormatChanged)
    Q_PROPERTY(QString saveFormat READ saveFormat WRITE setSaveFormat NOTIFY saveFormatChanged)
    Q_PROPERTY(int quality READ quality WRITE setQuality NOTIFY qualityChanged)
    Q_PROPERTY(QString captureName READ captureName WRITE setCaptureName NOTIFY captureNameChanged)
    Q_PROPERTY(QString captureDir READ captureDir WRITE setCaptureDir NOTIFY captureDirChanged)

public:

    explicit VideoCapture(QObject* const parent = nullptr);

    // TODO: if async is true, the cloned hw frame shares the same interop object with original frame,
    // so interop obj may do 2 map() at the same time. It's not safe

    void setAsync(bool value = true);
    bool isAsync() const;

    /*!
     * \brief setAutoSave
     *  If auto save is true, then the captured video frame will be saved as a file when frame is available.
     *  Otherwise, signal ready() and finished() will be emitted without doing other things.
     */
    void setAutoSave(bool value = true);
    bool autoSave() const;

    /*!
     * \brief setOriginalFormat
     *  Save the original frame, can be YUV, NV12 etc. No format converting. default is false
     *  The file name suffix is video frame's format name in lower case, e.g. yuv420p, nv12.
     *  quality property will not take effect.
     */
    void setOriginalFormat(bool value = true);
    bool isOriginalFormat() const;

    /*!
     * \brief setFormat
     *  Set saved format. can be "PNG", "jpg" etc. Not be used if save raw frame data.
     * \param format image format string like "png", "jpg"
     */
    void setSaveFormat(const QString& format);
    QString saveFormat() const;

    /*!
     * \brief setQuality
     *  Set saved image quality. Not be used if save original frame data.
     * \param value 0-100, larger is better quality. -1: default quality
     */
    void setQuality(int value);
    int quality() const;

    /*!
     * \brief name
     * suffix is auto add
     * empty name: filename_timestamp.format(suffix is videoframe.format.name() if save as raw data)
     * If autoSave() is true, saved file name will add a timestamp string.
     */
    void setCaptureName(const QString& value);
    QString captureName() const;

    void setCaptureDir(const QString& value);
    QString captureDir() const;

public Q_SLOTS:

    void capture();

Q_SIGNALS:

    void requested();

    /*use it to popup a dialog for selecting dir, name etc. TODO: block avthread if not async*/
    /*!
     * \brief frameAvailable
     * Emitted when requested frame is available.
     */
    void frameAvailable(const QtAV::VideoFrame& frame);

    /*!
     * \brief imageCaptured
     * Emitted when captured video frame is converted to a QImage.
     * \param image
     */
    void imageCaptured(const QImage& image); //TODO: emit only if not original format is set?
    void failed();

    /*!
     * \brief saved
     * Only for autoSave is true. Emitted when captured frame is saved.
     * \param path the saved captured frame path.
     */
    void saved(const QString& path);

    void asyncChanged();
    void autoSaveChanged();
    void originalFormatChanged();
    void saveFormatChanged();
    void qualityChanged();
    void captureNameChanged();
    void captureDirChanged();

private Q_SLOTS:

    void handleAppQuit();

private:

    void setVideoFrame(const VideoFrame& frame);

    // It's called by VideoThread after imediatly setVideoFrame(). Will emit ready()

    void start();

    friend class CaptureTask;
    friend class VideoThread;

private:

    bool            async;
    bool            auto_save;
    bool            original_fmt;

    // TODO: use blocking queue? If not, the parameters will change when thre previous is not finished
    // or use a capture event that wrapper all these parameters

    int             qual;
    QImage::Format  qfmt;
    QString         fmt;
    QString         name;
    QString         dir;
    VideoFrame      frame;
};

} // namespace QtAV

#endif // QTAV_VIDEO_CAPTURE_H
