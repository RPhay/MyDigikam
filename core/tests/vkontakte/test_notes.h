/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-02-19
 * Description : Unit tests for VKontakte web service
 *
 * Copyright (C) 2011-2015 by Alexander Potashev <aspotashev at gmail dot com>
 * Copyright (C) 2011-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef DIGIKAM_VKONTAKTE_TEST_NOTES_H
#define DIGIKAM_VKONTAKTE_TEST_NOTES_H

// Qt includes

#include <QObject>
#include <QVector>

// Local includes

#include "vktestbase.h"

/**
 * What is tested here:
 *   class NotesListJob
 *   class AllNotesListJob
 *   class NoteAddJob - tested in initTestCase()
 *   class NoteJob
 */
class TestNotes : public VkTestBase
{
    Q_OBJECT

public:

    TestNotes();

private Q_SLOTS:

    void initTestCase();
    void testNotesListJob();
    void testAllNotesListJob();
    void testNoteJob();

private:

    QVector<int> m_noteIds;
};

#endif // DIGIKAM_VKONTAKTE_TEST_NOTES_H
