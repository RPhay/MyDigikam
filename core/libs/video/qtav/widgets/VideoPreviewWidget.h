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

#ifndef QTAV_WIDGETS_VIDEO_PREVIEW_WIDGET_H
#define QTAV_WIDGETS_VIDEO_PREVIEW_WIDGET_H

// Qt includes

#include <QWidget>

// Local includes

#include "QtAVWidgets_Global.h"

namespace QtAV
{

class VideoFrame;
class VideoOutput;
class VideoFrameExtractor;

class DIGIKAM_EXPORT VideoPreviewWidget : public QWidget
{
    Q_OBJECT

public:

    explicit VideoPreviewWidget(QWidget* const parent = nullptr);

    void setTimestamp(qint64 msec);
    qint64 timestamp()          const;
    void preview();
    void setFile(const QString& file);
    QString file()              const;

    // default is false

    void setKeepAspectRatio(bool value = true)  { m_keep_ar = value;     }
    bool isKeepAspectRatio()    const           { return m_keep_ar;      }

    /// AutoDisplayFrame -- default is true. if true, new frames from underlying extractor
    /// will update display widget automatically.

    bool isAutoDisplayFrame()   const           { return m_auto_display; }

    /// If false, new frames (or frame errors) won't automatically update widget
    /// (caller must ensure to call displayFrame()/displayFrame(frame) for this if false).
    /// set to false only if you want to do your own frame caching magic with preview frames.

    void setAutoDisplayFrame(bool b = true);

    /// these were previously private but made public to allow calling code to cache
    /// some preview frames and directly display frames to this class

public Q_SLOTS:

    void displayFrame(const QtAV::VideoFrame& frame); // parameter VideoFrame
    void displayNoFrame();

Q_SIGNALS:

    void timestampChanged();
    void fileChanged();

    /// emitted on real decode error -- in that case displayNoFrame() will be automatically called

    void gotError(const QString&);

    /// usually emitted when a new request for a frame came in and current
    /// request was aborted. displayNoFrame() will be automatically called

    void gotAbort(const QString&);

    /// useful if calling code is interested in keeping stats on good versus bad frame counts,
    /// or if it wants to cache preview frames. Keeping counts helps caller decide if
    /// preview is working reliably or not for the designated file.
    /// parameter frame will always have: frame.isValid() == true, and will be
    /// already-scaled and in the right format to fit in the preview widget

    void gotFrame(const QtAV::VideoFrame& frame);

protected:

    virtual void resizeEvent(QResizeEvent*);

private:

    bool                 m_keep_ar      = false;
    bool                 m_auto_display = false;
    QString              m_file;
    VideoFrameExtractor* m_extractor    = nullptr;
    VideoOutput*         m_out          = nullptr;
};

} // namespace QtAV

#endif // QTAV_WIDGETS_VIDEO_PREVIEW_WIDGET_H
