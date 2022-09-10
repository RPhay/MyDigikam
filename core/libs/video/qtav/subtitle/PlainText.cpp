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

#include "PlainText.h"

// C++ includes

#include <cstdio>
#include <cstring>

// Local includes

#include "QtAV_Global.h"

namespace QtAV
{

namespace PlainText
{

/*
 * from mpv/sub/sd_ass.c
 * ass_to_plaintext() was written by wm4 and he says it can be under LGPL
 */

struct Q_DECL_HIDDEN buf
{
    char* start = nullptr;
    int   size  = 0;
    int   len   = 0;
};

static void append(struct buf* b, char c)
{
    if (b->len < b->size)
    {
        b->start[b->len] = c;
        b->len++;
    }
}

static void ass_to_plaintext(struct buf* b, const char* in)
{
    bool in_tag              = false;
    const char* open_tag_pos = nullptr;
    bool in_drawing          = false;

    while (*in)
    {
        if (in_tag)
        {
            if      (in[0] == '}')
            {
                in += 1;
                in_tag = false;
            }
            else if ((in[0] == '\\') && (in[1] == 'p'))
            {
                in += 2;

                // Skip text between \pN and \p0 tags. A \p without a number
                // is the same as \p0, and leading 0s are also allowed.

                in_drawing = false;

                while ((in[0] >= '0') && (in[0] <= '9'))
                {
                    if (in[0] != '0')
                        in_drawing = true;

                    in += 1;
                }
            }
            else
            {
                in += 1;
            }
        }
        else
        {
            if      ((in[0] == '\\') && ((in[1] == 'N') || (in[1] == 'n')))
            {
                in += 2;
                append(b, '\n');
            }
            else if ((in[0] == '\\') && (in[1] == 'h'))
            {
                in += 2;
                append(b, ' ');
            }
            else if (in[0] == '{')
            {
                open_tag_pos = in;
                in          += 1;
                in_tag       = true;
            }
            else
            {
                if (!in_drawing)
                    append(b, in[0]);

                in += 1;
            }
        }
    }

    // A '{' without a closing '}' is always visible.

    if (in_tag)
    {
        while (*open_tag_pos)
            append(b, *open_tag_pos++);
    }
}

QString fromAss(const char* ass)
{
    char text[512] = { 0 };
    struct buf b;
    b.start        = text;
    b.size         = sizeof(text) - 1;
    b.len          = 0;
    ass_to_plaintext(&b, ass);
    int hour1      = 0;
    int min1       = 0;
    int sec1       = 0;
    int hunsec1    = 0;
    int hour2      = 0;
    int min2       = 0;
    int sec2       = 0;
    int hunsec2    = 0;
    char line[512] = { 0 };
    char* ret      = nullptr;

    // fixme: "\0" maybe not allowed

    if (
        // cppcheck-suppress invalidscanf
        sscanf(b.start,
               "Dialogue: Marked=%*d,%d:%d:%d.%d,%d:%d:%d.%d%[^\r\n]", // &nothing,
                &hour1, &min1, &sec1, &hunsec1,
                &hour2, &min2, &sec2, &hunsec2,
                line) < 9
       )
    {
        if (
            // cppcheck-suppress invalidscanf
            sscanf(b.start,
                   "Dialogue: %*d,%d:%d:%d.%d,%d:%d:%d.%d%[^\r\n]",    // &nothing,
                    &hour1, &min1, &sec1, &hunsec1,
                    &hour2, &min2, &sec2, &hunsec2,
                    line) < 9
           )
        {
            return QString::fromUtf8(b.start); // libass ASS_Event. Text has no Dialogue
        }
    }

    ret = strchr(line, ',');

    if (!ret)
        return QString::fromUtf8(line);

    static const char kDefaultStyle[] = "Default,";

    for (int comma = 0 ; comma < 6 ; ++comma)
    {
        if (!(ret = strchr(++ret, ',')))
        {
            // workaround for ffmpeg decoded srt in ass format: "Dialogue: 0,0:42:29.20,0:42:31.08,Default,Chinese\NEnglish.

            if (!(ret = strstr(line, kDefaultStyle)))
            {
                if (line[0] == ',') //work around for libav-9-
                    return QString::fromUtf8(line+1);

                return QString::fromUtf8(line);
            }
            else
            {
                ret += sizeof(kDefaultStyle) - 1 - 1; // tail \0
            }
        }
    }

    ret++;
    int p = strcspn(b.start, "\r\n");

    if (p == b.len) // not found
        return QString::fromUtf8(ret);

    QString line2 = QString::fromUtf8(b.start + p + 1).trimmed();

    if (line2.isEmpty())
        return QString::fromUtf8(ret);

    return (QString::fromUtf8(ret) + QLatin1Char('\n') + line2);
}

} // namespace PlainText

} // namespace QtAV
