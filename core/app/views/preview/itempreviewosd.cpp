/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2025-12-25
 * Description : On Screen Display preview overlay.
 *
 * SPDX-FileCopyrightText: 2014-2026 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "itempreviewosd.h"

// Qt includes

#include <QTextDocument>
#include <QApplication>
#include <QDateTime>
#include <QPixmap>
#include <QLocale>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "itempropertiestab.h"
#include "albummanager.h"
#include "photoinfocontainer.h"

namespace Digikam
{

class Q_DECL_HIDDEN ItemPreviewOsd::Private
{
public:

    Private() = default;

public:

    const int  maxStringLen         = 80;

    ItemInfo   info;

    bool       printName            = true;
    bool       printTags            = true;
    bool       printTitle           = true;
    bool       printComment         = true;
    bool       printMakeModel       = true;
    bool       printLensModel       = true;
    bool       printExpoSensitivity = true;
    bool       printApertureFocal   = true;
    bool       printDate            = true;
};

ItemPreviewOsd::ItemPreviewOsd(QWidget* const parent)
    : QWidget(parent),
      d      (new Private)
{
}

ItemPreviewOsd::~ItemPreviewOsd()
{
    delete d;
}

void ItemPreviewOsd::setItemInfo(const ItemInfo& inf)
{
    d->info = inf;

    update();
}

void ItemPreviewOsd::paintEvent(QPaintEvent*)
{
    QPainter p(this);

    QString str;

    PhotoInfoContainer photoInfo = d->info.photoInfoContainer();
    QString comment              = d->info.comment();
    QString title                = d->info.title();
    QStringList tags             = AlbumManager::instance()->tagNames(d->info.tagIds());
    int offset                   = 0;

    // Display tag names.

    if (d->printTags)
    {
        printTags(p, offset, tags);
    }

    // Display Titles.

    if (d->printTitle)
    {
        str.clear();

        if (!title.isEmpty())
        {
            str += title;
            printInfoText(p, offset, str);
        }
    }

    // Display Comments.

    if (d->printComment)
    {
        str = comment;
        printComments(p, offset, str);
    }

    // Display Make and Model.

    if (d->printMakeModel)
    {
        str.clear();

        QString make  = photoInfo.make;
        QString model = photoInfo.model;

        if (!make.isEmpty())
        {
            ItemPropertiesTab::shortenedMakeInfo(make);
            str = make;
        }

        if (!model.isEmpty())
        {
            if (!make.isEmpty())
            {
                str += QLatin1String(" / ");
            }

            ItemPropertiesTab::shortenedModelInfo(model);
            str += model;
        }

        printInfoText(p, offset, str);
    }

    // Display Lens model.

    if (d->printLensModel)
    {
        str.clear();

        QString lens = photoInfo.lens;

        if (!lens.isEmpty())
        {
            str = lens;
            printInfoText(p, offset, str);
        }
    }

    // Display Exposure and Sensitivity.

    if (d->printExpoSensitivity)
    {
        str.clear();

        QString exposureTime = photoInfo.exposureTime;
        QString sensitivity  = photoInfo.sensitivity;

        if (!exposureTime.isEmpty())
        {
            str = exposureTime;
        }

        if (!sensitivity.isEmpty())
        {
            if (!exposureTime.isEmpty())
            {
                str += QLatin1String(" / ");
            }

            str += i18n("%1 ISO", sensitivity);
        }

        printInfoText(p, offset, str);
    }

    // Display Aperture and Focal.

    if (d->printApertureFocal)
    {
        str.clear();

        QString aperture        = photoInfo.aperture;
        QString focalLength     = photoInfo.focalLength;
        QString focalLength35mm = photoInfo.focalLength35mm;

        if (!aperture.isEmpty())
        {
            str = aperture;
        }

        if (focalLength35mm.isEmpty())
        {
            if (!focalLength.isEmpty())
            {
                if (!aperture.isEmpty())
                {
                    str += QLatin1String(" / ");
                }

                str += focalLength;
            }
        }
        else
        {
            if (!aperture.isEmpty())
            {
                str += QLatin1String(" / ");
            }

            if (!focalLength.isEmpty())
            {
                str += QString::fromUtf8("%1 (%2)").arg(focalLength).arg(focalLength35mm);
            }
            else
            {
                str += QString::fromUtf8("%1").arg(focalLength35mm);
            }
        }

        printInfoText(p, offset, str);
    }

    // Display Creation Date.

    if (d->printDate)
    {
        QDateTime dateTime = d->info.dateTime();

        if (dateTime.isValid())
        {
            str = QLocale().toString(dateTime, QLocale::ShortFormat);
            printInfoText(p, offset, str);
        }
    }

    // Display image File Name.

    if (d->printName)
    {
        printInfoText(p, offset, d->info.name());
    }
}

void ItemPreviewOsd::printInfoText(QPainter& p, int& offset, const QString& str, const QColor& pcol)
{
    if (!str.isEmpty())
    {
        offset += QFontMetrics(p.font()).lineSpacing();
        p.setPen(Qt::black);

        for (int x = -1 ; x <= 1 ; ++x)
        {
            for (int y = offset + 1 ; y >= offset - 1 ; --y)
            {
                p.drawText(x, p.window().height() - y, str);
            }
        }

        p.setPen(pcol);
        p.drawText(0, p.window().height() - offset, str);
    }
}

void ItemPreviewOsd::printComments(QPainter& p, int& offset, const QString& comments)
{
    QStringList commentsByLines;

    uint commentsIndex = 0;     // Comments QString index

    while (commentsIndex < (uint)comments.length())
    {
        QString newLine;
        bool breakLine = false; // End Of Line found
        uint currIndex;         // Comments QString current index

        // Check minimal lines dimension

        uint commentsLinesLengthLocal = d->maxStringLen;

        for (currIndex = commentsIndex ;
             (currIndex < (uint)comments.length()) && !breakLine ; ++currIndex)
        {
            if ((comments.at(currIndex) == QLatin1Char('\n')) || comments.at(currIndex).isSpace())
            {
                breakLine = true;
            }
        }

        if (commentsLinesLengthLocal <= (currIndex - commentsIndex))
        {
            commentsLinesLengthLocal = (currIndex - commentsIndex);
        }

        breakLine = false;

        for (currIndex = commentsIndex ;
             (currIndex <= (commentsIndex + commentsLinesLengthLocal)) &&
             (currIndex < (uint)comments.length()) && !breakLine ;
             ++currIndex)
        {
            breakLine = (comments.at(currIndex) == QLatin1Char('\n')) ? true : false;

            if (breakLine)
            {
                newLine.append(QLatin1Char(' '));
            }
            else
            {
                newLine.append(comments.at(currIndex));
            }
        }

        commentsIndex = currIndex; // The line is ended

        if (commentsIndex != (uint)comments.length())
        {
            while (!newLine.endsWith(QLatin1Char(' ')))
            {
                newLine.truncate(newLine.length() - 1);

                if (commentsIndex > 0)
                {
                    --commentsIndex;
                }
            }
        }

        commentsByLines.prepend(newLine.trimmed());
    }

    for (int i = 0 ; i < (int)commentsByLines.count() ; ++i)
    {
        printInfoText(p, offset, commentsByLines.at(i));
    }
}

void ItemPreviewOsd::printTags(QPainter& p, int& offset, QStringList& tags)
{
    tags.removeDuplicates();
    tags.sort();

    QString str = tags.join(QLatin1String(", "));

    if (!str.isEmpty())
    {
        printInfoText(p, offset, str, qApp->palette().color(QPalette::Link).name());
    }
}

} // namespace Digikam

#include "moc_itempreviewosd.cpp"
