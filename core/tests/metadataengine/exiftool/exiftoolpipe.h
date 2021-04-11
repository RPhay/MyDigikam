/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-23
 * Description : Piped output from exiftool application
 *
 * Copyright (C) 2013-2019 by Phil Harvey <philharvey66 at gmail dot com>
 * Copyright (C) 2020-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_EXIF_TOOL_PIPE_H
#define DIGIKAM_EXIF_TOOL_PIPE_H

namespace Digikam
{

class ExifToolPipe
{
public:

    ExifToolPipe();
    ~ExifToolPipe();

    void    Init(int fd, int pid, int initialSize);
    int     Read();     ///< read exiftool response
    void    Free();     ///< free buffer memory
    void    Flush();    ///< remove previous response from buffer
    char*   GetString()    { return (mString && mString[0]) ? mString : (char*)0; }
    int     GetStringLen() { return mStringLen; }

private:

    int     mFile;      ///< read file descriptor
    char*   mBuff;      ///< buffer pointer
    int     mSize;      ///< buffer size
    int     mLen;       ///< length of data in buffer
    int     mPos;       ///< current read position in buffer
    int     mSearchPos; ///< current search position in buffer
    int     mBlockSize; ///< initial buffer size
    char*   mString;    ///< returned string from last Read()
    int     mStringLen; ///< length of returned string
    int     mPid;       ///< process id for other side of the pipe
};

} // namespace Digikam

#endif // DIGIKAM_EXIF_TOOL_PIPE_H__
