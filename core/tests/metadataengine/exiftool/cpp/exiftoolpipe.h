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

    void  Init(int fd,
               int pid,
               int initialSize);

    int   Read();                 ///< Read exiftool response
    void  Free();                 ///< Free buffer memory
    void  Flush();                ///< Remove previous response from buffer
    char* GetString()    const;
    int   GetStringLen() const;

private:

    int   mFile;                  ///< Read file descriptor
    char* mBuff;                  ///< Buffer pointer
    int   mSize;                  ///< Buffer size
    int   mLen;                   ///< Length of data in buffer
    int   mPos;                   ///< Current read position in buffer
    int   mSearchPos;             ///< Current search position in buffer
    int   mBlockSize;             ///< Initial buffer size
    char* mString;                ///< Returned string from last Read()
    int   mStringLen;             ///< Length of returned string
    int   mPid;                   ///< Process id for other side of the pipe
};

} // namespace Digikam

#endif // DIGIKAM_EXIF_TOOL_PIPE_H__
