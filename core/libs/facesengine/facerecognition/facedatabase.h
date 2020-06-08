/* ============================================================
 *
 * This file is a part of digiKam
 *
 * Date        : 2019-06-07
 * Description : Database interface for face recognition
 *
 * Copyright (C) 2020 by Nghia Duong <minhnghiaduong997 at gmail dot com>
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

#ifndef FACEDATABASE_H
#define FACEDATABASE_H

#include "identity.h"

using namespace Digikam;

namespace RecognitionTest
{

class FaceDatabase
{
public:
    FaceDatabase();
    ~FaceDatabase();

public:



private:

    class Private;
    Private* d;
};

}

#endif // FACEDATABASE_H
