/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-23
 * Description : C++ library interface to Perl exiftool application script
 *
 * Copyright (C) 2013-2019 by Phil Harvey <philharvey66 at gmail dot com>
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

#ifndef __EXIFTOOL_H__
#define __EXIFTOOL_H__

#include "TagInfo.h"
#include "ExifToolPipe.h"

#define NOW     0
#define NEVER   1e9

#define SUMMARY_DIRECTORIES_SCANNED     "directories scanned"
#define SUMMARY_DIRECTORIES_CREATED     "directories created"
#define SUMMARY_FILES_FAILED_CONDITION  "files failed condition"
#define SUMMARY_IMAGE_FILES_CREATED     "image files created"
#define SUMMARY_IMAGE_FILES_UPDATED     "image files updated"
#define SUMMARY_IMAGE_FILES_UNCHANGED   "image files unchanged"
#define SUMMARY_IMAGE_FILES_MOVED       "image files moved"
#define SUMMARY_IMAGE_FILES_COPIED      "image files copied"
#define SUMMARY_FILE_UPDATE_ERRORS      "files weren't updated due to errors"
#define SUMMARY_FILE_CREATE_ERRORS      "files weren't created due to errors"
#define SUMMARY_IMAGE_FILES_READ        "image files read"
#define SUMMARY_IMAGE_FILE_ERRORS       "files could not be read"
#define SUMMARY_OUTPUT_FILES_CREATED    "output files created"
#define SUMMARY_OUTPUT_FILES_APPENDED   "output files appended"
#define SUMMARY_HARD_LINKS_CREATED      "hard links created"
#define SUMMARY_HARD_LINK_ERRORS        "hard links could not be created"
#define SUMMARY_SYMBOLIC_LINKS_CREATED  "symbolic links created"
#define SUMMARY_SYMBOLIC_LINK_ERRORS    "symbolic links could not be created"

class ExifTool
{
public:

    ExifTool(const char* exec = NULL, const char* arg1 = NULL);
    virtual ~ExifTool();

    TagInfo *ImageInfo(const char *file, const char *opts=NULL, double timeout=NEVER);

    int     ExtractInfo(const char *file, const char *opts=NULL);
    TagInfo *GetInfo(int cmdNum=0, double timeout=NEVER);

    int     SetNewValue(const char *tag=NULL, const char *value=NULL, int len=-1);
    int     WriteInfo(const char *file, const char *opts=NULL, TagInfo *info=NULL);

    int     Command(const char *cmd=NULL);
    int     Complete(double timeout=NEVER);

    int     IsRunning();
    int     LastComplete()  { return mLastComplete; }
    int     LastCommand()   { return mCmdNum; } // (undocumented)
    void    SetLastComplete(int lastComplete) { mLastComplete = lastComplete; }
    void    SetWaitTime(int waitTime) { mWaitTime = waitTime; }

    char *  GetOutput()     { return mLastComplete > 0 ? mStdout.GetString() : NULL; }
    int     GetOutputLen()  { return mLastComplete > 0 ? mStdout.GetStringLen() : 0; }
    char *  GetError()      { return mLastComplete > 0 ? mStderr.GetString() : NULL; }
    int     GetErrorLen()   { return mLastComplete > 0 ? mStderr.GetStringLen() : 0; } // (undocumented)

    int     GetSummary(const char *msg);

    // flags to allow some ExifTool features to be disabled
    // (must be set before creating ExifTool object)
    static int  sNoSigPipe;     // set to disable SIGPIPE handler
    static int  sNoWatchdog;    // set to disable watchdog process

private:

    ExifToolPipe  mStdout;      // buffer for exiftool stdout read pipe
    ExifToolPipe  mStderr;      // buffer for exiftool stderr read pipe
    int           mTo;          // write pipe for exiftool stdin
    int           mPid;         // exiftool application process ID
    int           mWatchdog;    // watchdog process ID
    TagInfo     * mWriteInfo;   // tag information to write
    char        * mCmdQueue;    // queued command arguments (NULL if nothing queued)
    int           mCmdQueueLen; // length of data in command queue
    int           mCmdQueueSize;// size of command queue
    int           mLastComplete;// result of last Complete() call
    int           mCmdNum;      // last command number
    int           mWaitTime;    // time to wait (microsec) if no commands in queue
};

#endif // __EXIFTOOL_H__
