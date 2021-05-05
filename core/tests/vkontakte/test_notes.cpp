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

#include "test_notes.h"

// KDE includes

#include <qtest_kde.h>

// Local includes

#include "vkontakte_noteaddjob.h"
#include "vkontakte_noteslistjob.h"
#include "vkontakte_allnoteslistjob.h"
#include "vkontakte_notejob.h"

#define NOTE1_NAME "__note for unit testing of digiKam #1"
#define NOTE2_NAME "__note for unit testing of digiKam #2"

using namespace Vkontakte;

TestNotes::TestNotes()
    : VkTestBase()
{
}

void TestNotes::initTestCase()
{
    authenticate(AppPermissions::Notes);

    // Create notes for testing
    QList<QString> noteNames;
    noteNames << NOTE1_NAME;
    noteNames << NOTE2_NAME;

    foreach (const QString& name, noteNames)
    {
        NoteAddJob* const job = new NoteAddJob(accessToken(), name, QString::fromUtf8("Text for %1").arg(name));
        job->exec();
        QVERIFY(!job->error());

        m_noteIds.append(job->nid());
    }
}

void TestNotes::testNotesListJob()
{
    NotesListJob* const job = new NotesListJob(accessToken(), 0, 0, 100);
    job->exec();
    QVERIFY(!job->error());

    QList<NoteInfoPtr> list = job->list();

    // We created two notes and did not remove them,
    // so at least two notes should exist at the moment.
    QVERIFY(list.size() >= 2);
}

void TestNotes::testAllNotesListJob()
{
    AllNotesListJob* const job = new AllNotesListJob(accessToken(), 0);
    job->exec();
    QVERIFY(!job->error());

    QList<NoteInfoPtr> list = job->list();

    // We created two notes and did not remove them,
    // so at least two notes should exist at the moment.
    QVERIFY(list.size() >= 2);
}

void TestNotes::testNoteJob()
{
    int noteId = m_noteIds[0];

    NoteJob* const job = new NoteJob(accessToken(), noteId);
    job->exec();
    QVERIFY(!job->error());

    NoteInfoPtr note = job->noteInfo();
    QCOMPARE(note->title(), QString(NOTE1_NAME));
}

QTEST_KDEMAIN(TestNotes, GUI)
