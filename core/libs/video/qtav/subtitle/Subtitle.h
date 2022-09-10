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

#ifndef QTAV_SUBTITLE_H
#define QTAV_SUBTITLE_H

// Qt includes

#include <QObject>
#include <QStringList>
#include <QUrl>
#include <QImage>

// Local includes

#include "SubImage.h"

/**
 * NOTE: to avoid read error, subtitle size > 10*1024*1024 will be ignored.
 */

namespace QtAV
{

class DIGIKAM_EXPORT SubtitleFrame
{
public:

    SubtitleFrame()
      : begin(0.0),
        end  (0.0)
    {
    }

    // valide: begin < end

    bool isValid() const                                 { return (begin < end); }
    operator bool() const                                { return isValid();     }
    bool operator !() const                              { return !isValid();    }
    inline bool operator <(const SubtitleFrame& f) const { return (end < f.end); }
    inline bool operator <(qreal t) const                { return (end < t);     }

    qreal   begin;
    qreal   end;
    QString text; ///< plain text. always valid
};

class DIGIKAM_EXPORT Subtitle : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray codec READ codec WRITE setCodec NOTIFY codecChanged)

    // QList<SubtitleProcessorId>

    Q_PROPERTY(QStringList engines READ engines WRITE setEngines NOTIFY enginesChanged)
    Q_PROPERTY(QString engine READ engine NOTIFY engineChanged)
    Q_PROPERTY(bool fuzzyMatch READ fuzzyMatch WRITE setFuzzyMatch NOTIFY fuzzyMatchChanged)
    Q_PROPERTY(QByteArray rawData READ rawData WRITE setRawData NOTIFY rawDataChanged)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(QStringList dirs READ dirs WRITE setDirs NOTIFY dirsChanged)
    Q_PROPERTY(QStringList suffixes READ suffixes WRITE setSuffixes NOTIFY suffixesChanged)
    Q_PROPERTY(QStringList supportedSuffixes READ supportedSuffixes NOTIFY supportedSuffixesChanged)
    Q_PROPERTY(qreal timestamp READ timestamp WRITE setTimestamp)
    Q_PROPERTY(qreal delay READ delay WRITE setDelay NOTIFY delayChanged)
    Q_PROPERTY(QString text READ getText)
    Q_PROPERTY(bool loaded READ isLoaded)
    Q_PROPERTY(bool canRender READ canRender NOTIFY canRenderChanged)

    // font properties for libass engine

    Q_PROPERTY(QString fontFile READ fontFile WRITE setFontFile NOTIFY fontFileChanged)
    Q_PROPERTY(QString fontsDir READ fontsDir WRITE setFontsDir NOTIFY fontsDirChanged)
    Q_PROPERTY(bool fontFileForced READ isFontFileForced WRITE setFontFileForced NOTIFY fontFileForcedChanged)

public:

    explicit Subtitle(QObject* const parent = nullptr);
    virtual ~Subtitle();

    /*!
     * \brief setCodec
     * set subtitle encoding that supported by QTextCodec.
     * You have to call load() to manually reload the subtitle with given codec
     * \param value codec name. see QTextCodec.availableCodecs().
     * Empty value means using the default codec in QTextCodec
     * If linked with libchardet(https://github.com/cnangel/libchardet) or can dynamically load it,
     * set value of "AutoDetect" to detect the charset of subtitle
     */
    void setCodec(const QByteArray& value);

    QByteArray codec() const;

    /*!
     * \brief isValid
     * indicate whether the subtitle can be found and processed
     * \return
     */
    bool isLoaded() const;

    /*!
     * \brief setEngines
     * Set subtitle processor engine names, in priority order.
     * When loading a subtitle, use the engines
     * one by one until a usable engine is found.
     * \param value
     */
    void setEngines(const QStringList& value);

    QStringList engines() const;

    /*!
     * \brief engine
     * \return The engine in use for current subtitle
     */
    QString engine() const;
    void setFuzzyMatch(bool value);
    bool fuzzyMatch() const;
    void setRawData(const QByteArray& data);
    QByteArray rawData() const;

    /*!
     * \brief setFileName
     * the given name will be in the 1st place to try to open(if using fuzzy match).
     * then files in suffixes() order
     * or in processor's supported suffixes order
     * \param name
     */
    void setFileName(const QString& name);
    QString fileName() const;

    /*!
     * \brief setDirs
     * Set subtitle search directories. Video's dir will always be added.
     */
    void setDirs(const QStringList& value);
    QStringList dirs() const;

    /*!
     * \brief supportedFormats
     * the suffix names supported by all engines. for example ["ass", "ssa"]
     * \return
     */
    QStringList supportedSuffixes() const;

    /*!
     * \brief setSuffixes
     * default is using SubtitleProcessor. Empty equals default value.
     * But suffixes() will return empty.
     */
    void setSuffixes(const QStringList& value);
    QStringList suffixes() const;

    qreal timestamp() const;

    /*!
     * \brief delay
     * unit: second
     * The subtitle from getText() and getImage() is at the time: timestamp() + delay()
     * \return
     */
    qreal delay() const;
    void setDelay(qreal value);

    /*!
     * \brief canRender
     *whethercurrent processor supports rendering. Check before getImage()
     * \return
     */
    bool canRender() const;

    // call setTimestamp before getText/Image
    // plain text. separated by '\n' if more than 1 text rects found

    QString getText() const;

    /*!
      * \brief getImage
      * Get a subtitle image with given (video) frame size. The result image size usually smaller than
      * given frame size because subtitle are lines of text. The boundingRect indicates the actual
      * image position and size relative to given size.
      * The result image format is QImage::Format_ARGB32
      * \return empty image if no image, or subtitle processor does not support renderering
      */
    QImage getImage(int width, int height, QRect* boundingRect = nullptr);
    SubImageSet getSubImages(int width, int height, QRect* boundingRect = nullptr);

    // used for embedded subtitles.

    /*!
     * \brief processHeader
     * Always called if switch to a new internal subtitle stream. But header data can be empty
     * Used by libass to set style etc.
     */
    bool processHeader(const QByteArray& codec, const QByteArray& data);

    // ffmpeg decodes subtitle lines and call processLine.
    // if AVPacket contains plain text, no decoding is ok

    bool processLine(const QByteArray& data, qreal pts = -1, qreal duration = 0);

    QString fontFile() const;
    void setFontFile(const QString& value);

    /*!
     * \brief fontsDir
     * Not tested for dwrite provider. FontConfig can work.
     */
    QString fontsDir() const;
    void setFontsDir(const QString& value);
    bool isFontFileForced() const;
    void setFontFileForced(bool value);

public Q_SLOTS:

    /*!
     * \brief start
     * start to process the whole subtitle content in a thread
     */
    void load();
    void loadAsync();
    void setTimestamp(qreal t);

Q_SIGNALS:

    // TODO: also add to AVPlayerCore?

    /// empty path if load from raw data

    void loaded(const QString& path = QString());
    void canRenderChanged();
    void codecChanged();
    void enginesChanged();
    void fuzzyMatchChanged();

    /*!
     * \brief contentChanged
     * emitted when text content changed.
     */
    void contentChanged();
    void rawDataChanged();
    void fileNameChanged();
    void dirsChanged();
    void suffixesChanged();
    void supportedSuffixesChanged();
    void engineChanged();
    void delayChanged();
    void fontFileChanged();
    void fontsDirChanged();
    void fontFileForcedChanged();

private:

    void checkCapability();

private:

    class Private;
    Private* d;
};

// internal use

class DIGIKAM_EXPORT SubtitleAPIProxy
{
public:

    explicit SubtitleAPIProxy(QObject* const obj);

    void setSubtitle(Subtitle* const sub);

    // API from Subtitle

    /*!
     * \brief setCodec
     * set subtitle encoding that supported by QTextCodec. subtitle will be reloaded
     * \param value codec name. see QTextCodec.availableCodecs().
     * Empty value means using the default codec in QTextCodec
     */
    void setCodec(const QByteArray& value);
    QByteArray codec()              const;
    bool isLoaded()                 const;
    void setEngines(const QStringList& value);
    QStringList engines()           const;
    QString engine()                const;
    void setFuzzyMatch(bool value);
    bool fuzzyMatch()               const;

    // always use exact file path by setFile(). file name is used internally
/*
    void setFileName(const QString& name);
    QString fileName() const;
*/
    void setDirs(const QStringList& value);
    QStringList dirs()              const;

    QStringList supportedSuffixes() const;
    void setSuffixes(const QStringList& value);
    QStringList suffixes()          const;

    bool canRender() const;                     // TODO: rename to capability()

    qreal delay()                   const;
    void setDelay(qreal value);

    QString fontFile()              const;
    void setFontFile(const QString& value);
    QString fontsDir()              const;
    void setFontsDir(const QString& value);
    bool isFontFileForced()         const;
    void setFontFileForced(bool value);

    // API from PlayerSubtitle

/*
    void setFile(const QString& file);
    QString file() const;
    void setAutoLoad(bool value);
    bool autoLoad() const;
*/

private:

    QObject*  m_obj = nullptr;
    Subtitle* m_s   = nullptr;
};

} // namespace QtAV

#endif // QTAV_SUBTITLE_H
