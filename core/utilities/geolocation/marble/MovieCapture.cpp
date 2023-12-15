// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Illya Kovalevskyy <illya.kovalevskyy@gmail.com>
// SPDX-FileCopyrightText: 2014 Dennis Nienhüser <nienhueser@kde.org>
//

#include "MovieCapture.h"
#include "MarbleWidget.h"

#include <QProcess>
#include <QMessageBox>
#include <QTimer>
#include <QElapsedTimer>
#include <QFile>

#include <klocalizedstring.h>

#include "digikam_debug.h"

namespace Marble
{

class MovieCapturePrivate
{
public:
    explicit MovieCapturePrivate(MarbleWidget *widget) :
        marbleWidget(widget), method(MovieCapture::TimeDriven)
    {}

    /**
     * @brief This gets called when user doesn't have avconv/ffmpeg installed
     */
    void missingToolsWarning() {
        QMessageBox::warning(marbleWidget,
                             i18n("Missing encoding tools"),
                             i18n("Marble requires additional software in order to "
                                         "create movies. Please get %1 ", QString::fromUtf8("<a href=\"https://libav.org/"
                                               "download.html\">avconv</a>")),
                             QMessageBox::Ok);
    }

    QTimer frameTimer;
    MarbleWidget *marbleWidget;
    QString encoderExec;
    QString destinationFile;
    QProcess process;
    MovieCapture::SnapshotMethod method;
    int fps;
};

MovieCapture::MovieCapture(MarbleWidget *widget, QObject *parent) :
    QObject(parent),
    d_ptr(new MovieCapturePrivate(widget))
{
    Q_D(MovieCapture);
    if( d->method == MovieCapture::TimeDriven ){
        d->frameTimer.setInterval(1000/30); // fps = 30 (default)
        connect(&d->frameTimer, SIGNAL(timeout()), this, SLOT(recordFrame()));
    }
    d->fps = 30;
    MovieFormat avi( QString::fromUtf8("avi"), i18n( "AVI (mpeg4)" ), QString::fromUtf8("avi") );
    MovieFormat flv( QString::fromUtf8("flv"), i18n( "FLV" ), QString::fromUtf8("flv") );
    MovieFormat mkv( QString::fromUtf8("matroska"), i18n( "Matroska (h264)" ), QString::fromUtf8("mkv") );
    MovieFormat mp4( QString::fromUtf8("mp4"), i18n( "MPEG-4" ), QString::fromUtf8("mp4") );
    MovieFormat vob( QString::fromUtf8("vob"), i18n( "MPEG-2 PS (VOB)" ), QString::fromUtf8("vob") );
    MovieFormat ogg( QString::fromUtf8("ogg"), i18n( "OGG" ), QString::fromUtf8("ogg") );
    MovieFormat swf( QString::fromUtf8("swf"), i18n( "SWF" ), QString::fromUtf8("swf") );
    m_supportedFormats << avi << flv << mkv << mp4 << vob << ogg << swf;
}

MovieCapture::~MovieCapture()
{
    delete d_ptr;
}

void MovieCapture::setFps(int fps)
{
    Q_D(MovieCapture);
    if( d->method == MovieCapture::TimeDriven ){
        d->frameTimer.setInterval(1000/fps);
    }
    d->fps = fps;
}

void MovieCapture::setFilename(const QString &path)
{
    Q_D(MovieCapture);
    d->destinationFile = path;
}

void MovieCapture::setSnapshotMethod(MovieCapture::SnapshotMethod method)
{
    Q_D(MovieCapture);
    d->method = method;
}

int MovieCapture::fps() const
{
    Q_D(const MovieCapture);
    return d->fps;
}

QString MovieCapture::destination() const
{
    Q_D(const MovieCapture);
    return d->destinationFile;
}

QVector<MovieFormat> MovieCapture::availableFormats()
{
    Q_D(MovieCapture);
    static QVector<MovieFormat> availableFormats;
    if ( availableFormats.isEmpty() && checkToolsAvailability() ) {
        QProcess encoder(this);
        for ( const MovieFormat &format: m_supportedFormats ) {
            QString type = format.type();
            QStringList args;
            args << QString::fromUtf8("-h") << QLatin1String("muxer=") + type;
            encoder.start( d->encoderExec, args );
            encoder.waitForFinished();
            QString output = QString::fromUtf8(encoder.readAll());
            bool isFormatAvailable = !output.contains(QLatin1String("Unknown format"));
            if( isFormatAvailable ) {
                availableFormats << format;
            }
        }
    }
    return availableFormats;
}

MovieCapture::SnapshotMethod MovieCapture::snapshotMethod() const
{
    Q_D(const MovieCapture);
    return d->method;
}

bool MovieCapture::checkToolsAvailability()
{
    Q_D(MovieCapture);
    static bool toolsAvailable = false;
    if (toolsAvailable == false) {
        QProcess encoder(this);
        encoder.start(QString::fromUtf8("avconv"), QStringList() << QString::fromUtf8("-version"));
        encoder.waitForFinished();
        if ( !encoder.readAll().isEmpty() ) { // avconv have output when it's here
            d->encoderExec = QString::fromUtf8("avconv");
            toolsAvailable = true;
        } else {
            encoder.start(QString::fromUtf8("ffmpeg"), QStringList() << QString::fromUtf8( "-version"));
            encoder.waitForFinished();
            if ( !encoder.readAll().isEmpty() ) {
                d->encoderExec = QString::fromUtf8("ffmpeg");
                toolsAvailable = true;
            }
        }
    }
    return toolsAvailable;
}

void MovieCapture::recordFrame()
{
    Q_D(MovieCapture);
    QImage const screenshot = d->marbleWidget->mapScreenShot().toImage().convertToFormat(QImage::Format_RGB888);
    if (d->process.state() == QProcess::NotRunning) {
        QStringList const arguments = QStringList()
                << QString::fromUtf8("-y")
                << QString::fromUtf8("-r") << QString::number(fps())
                << QString::fromUtf8("-f") << QString::fromUtf8("rawvideo")
                << QString::fromUtf8("-pix_fmt") << QString::fromUtf8("rgb24")
                << QString::fromUtf8("-s") << QString::fromUtf8("%1x%2").arg( screenshot.width() ).arg( screenshot.height() )
                << QString::fromUtf8("-i") << QString::fromUtf8("pipe:")
                << QString::fromUtf8("-b") << QString::fromUtf8("2000k")
                << d->destinationFile;
        d->process.start( d->encoderExec, arguments );
        connect(&d->process, SIGNAL(finished(int)), this, SLOT(processWrittenMovie(int)));
    }
    d->process.write( (char*) screenshot.bits(), screenshot.sizeInBytes() );
    for (int i=0; i<30 && d->process.bytesToWrite()>0; ++i) {
        QElapsedTimer t;
        int then = d->process.bytesToWrite();
        t.start();
        d->process.waitForBytesWritten( 100 );
        int span = t.elapsed();
        int now = d->process.bytesToWrite();
        int bytesWritten = then - now;
        double rate = ( bytesWritten * 1000.0 ) / ( qMax(1, span) * 1024 );
        Q_EMIT rateCalculated( rate );
    }
}

bool MovieCapture::startRecording()
{
    Q_D(MovieCapture);

    if( !checkToolsAvailability() ) {
        d->missingToolsWarning();
        return false;
    }

    if( d->method == MovieCapture::TimeDriven ){
        d->frameTimer.start();
    }
    recordFrame();
    return true;
}

void MovieCapture::stopRecording()
{
    Q_D(MovieCapture);

    d->frameTimer.stop();
    d->process.closeWriteChannel();
}

void MovieCapture::cancelRecording()
{
    Q_D(MovieCapture);

    d->frameTimer.stop();
    d->process.close();
    QFile::remove( d->destinationFile );
}

void MovieCapture::processWrittenMovie(int exitCode)
{
    if (exitCode != 0) {
        qCDebug(DIGIKAM_MARBLE_LOG) << "[*] avconv finished with" << exitCode;
        Q_EMIT errorOccured();
    }
}

} // namespace Marble

#include "moc_MovieCapture.cpp"
