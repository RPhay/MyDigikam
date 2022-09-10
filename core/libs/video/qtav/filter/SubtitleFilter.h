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

#ifndef QTAV_SUBTITLE_FILTER_H
#define QTAV_SUBTITLE_FILTER_H

// Local includes

#include "Filter.h"
#include "Subtitle.h"

// final class

namespace QtAV
{

class AVPlayerCore;
class SubtitleFilterPrivate;

/*!
 * \brief The SubtitleFilter class
 * draw text and image subtitles
 * Subtitle load priority: user specified file (setFile(...)) > auto load external (autoLoad()
 * must be true) > embedded subtitle
 */
class DIGIKAM_EXPORT SubtitleFilter : public VideoFilter,
                                      public SubtitleAPIProxy
{
    Q_OBJECT
    DPTR_DECLARE_PRIVATE(SubtitleFilter)
    Q_PROPERTY(QByteArray codec READ codec WRITE setCodec NOTIFY codecChanged)
    Q_PROPERTY(QStringList engines READ engines WRITE setEngines NOTIFY enginesChanged)
    Q_PROPERTY(QString engine READ engine NOTIFY engineChanged)
    Q_PROPERTY(bool fuzzyMatch READ fuzzyMatch WRITE setFuzzyMatch NOTIFY fuzzyMatchChanged)

    Q_PROPERTY(QStringList dirs READ dirs WRITE setDirs NOTIFY dirsChanged)
    Q_PROPERTY(QStringList suffixes READ suffixes WRITE setSuffixes NOTIFY suffixesChanged)
    Q_PROPERTY(QStringList supportedSuffixes READ supportedSuffixes NOTIFY supportedSuffixesChanged)
    Q_PROPERTY(bool canRender READ canRender NOTIFY canRenderChanged)
    Q_PROPERTY(qreal delay READ delay WRITE setDelay NOTIFY delayChanged)

    Q_PROPERTY(bool autoLoad READ autoLoad WRITE setAutoLoad NOTIFY autoLoadChanged)
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_PROPERTY(QRectF rect READ rect WRITE setRect NOTIFY rectChanged)
    Q_PROPERTY(QFont font READ font WRITE setFont NOTIFY fontChanged)
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

    // font properties for libass engine

    Q_PROPERTY(QString fontFile READ fontFile WRITE setFontFile NOTIFY fontFileChanged)
    Q_PROPERTY(QString fontsDir READ fontsDir WRITE setFontsDir NOTIFY fontsDirChanged)
    Q_PROPERTY(bool fontFileForced READ isFontFileForced WRITE setFontFileForced NOTIFY fontFileForcedChanged)

public:

    explicit SubtitleFilter(QObject* const parent = nullptr);

    void setPlayer(AVPlayerCore* player);

    bool isSupported(VideoFilterContext::Type ct) const override
    {
        return ((ct == VideoFilterContext::QtPainter) || (ct == VideoFilterContext::X11));
    }

    /*!
     * \brief setFile
     * Load user selected subtitle.
     * The subtitle will not change unless you manually setFile(QString()).
     */
    void setFile(const QString& file);
    QString file()                  const;

    /*!
     * \brief autoLoad
     * Auto find and load a suitable external subtitle if file() is not empty.
     */
    bool autoLoad()                 const;

    // < 1 means normalized. not valid means the whole target rect.
    // default is (0, 0, 1, 0.9) and align bottom

    void setRect(const QRectF& r);
    QRectF rect()                   const;

    void setFont(const QFont& f);
    QFont font()                    const;

    void setColor(const QColor& c);
    QColor color()                  const;

    QString subtitleText(qreal t)   const;

public Q_SLOTS:

    // TODO: enable changed & autoload=> load

    void setAutoLoad(bool value);

Q_SIGNALS:

    void rectChanged();
    void fontChanged();
    void colorChanged();
    void autoLoadChanged(bool value);

    void fileChanged();
    void canRenderChanged();
    void loaded(const QString& path);

    void codecChanged();
    void enginesChanged();
    void fuzzyMatchChanged();
    void contentChanged();

    //void fileNameChanged();

    void dirsChanged();
    void suffixesChanged();
    void supportedSuffixesChanged();
    void engineChanged();
    void delayChanged();
    void fontFileChanged();
    void fontsDirChanged();
    void fontFileForcedChanged();

protected:

    void process(Statistics* statistics, VideoFrame* frame) override;
};

} // namespace QtAV

#endif // QTAV_SUBTITLE_FILTER_H
