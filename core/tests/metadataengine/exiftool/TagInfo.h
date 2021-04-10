/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-23
 * Description : Tag information object
 *
 * Copyright (C) 2013-2019 by Phil Harvey, <philharvey66 at gmail dot com>
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

#ifndef __TAGINFO_H__
#define __TAGINFO_H__

struct TagInfo
{
    TagInfo();
    virtual ~TagInfo();

    char    *group[3];  // family 0-2 group names
    char    *name;      // tag name
    char    *desc;      // tag description
    char    *id;        // tag ID
    char    *value;     // converted value
    int     valueLen;   // length of value in bytes (not including null terminator)
    char    *num;       // "numerical" value
    int     numLen;     // length of numerical value
    int     copyNum;    // copy number for this tag name
    TagInfo *next;      // next TagInfo in linked list
};

#endif // __TAGINFO_H__
