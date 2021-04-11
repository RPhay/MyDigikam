/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2013-11-23
 * Description : Tag information object
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

#include "exiftooltaginfo.h"

TagInfo::TagInfo()
    : name      (NULL),
      desc      (NULL),
      id        (NULL),
      value     (NULL),
      valueLen  (0),
      num       (NULL),
      numLen    (0),
      copyNum   (0),
      next      (NULL)
{
    group[0] = group[1] = group[2] = NULL;
}

/// delete entire linked list of TagInfo objects
TagInfo::~TagInfo()
{
    // delete our members

    delete [] group[0];
    delete [] group[1];
    delete [] group[2];
    delete [] name;
    delete [] desc;
    delete [] id;

    if (num != value)
    {
        delete [] num;   // delete numerical value if unique
    }

    delete [] value;

    // delete remaining elements of linked list

    while (next)
    {
        TagInfo* info = next;

        // remove next entry from the list, then delete it

        next          = info->next;
        info->next    = (TagInfo *)NULL;

        delete info;
    }
}

