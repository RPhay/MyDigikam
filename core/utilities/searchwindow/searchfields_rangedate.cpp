/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-01-20
 * Description : User interface for searches
 *
 * Copyright (C) 2008-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2011-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "searchfields_p.h"

namespace Digikam
{

SearchFieldRangeDate::SearchFieldRangeDate(QObject* const parent, Type type)
    : SearchField     (parent),
      m_firstTimeEdit (nullptr),
      m_firstDateEdit (nullptr),
      m_secondTimeEdit(nullptr),
      m_secondDateEdit(nullptr),
      m_type          (type)
{
    m_betweenLabel = new QLabel;
}

void SearchFieldRangeDate::setupValueWidgets(QGridLayout* layout, int row, int column)
{
/*
    QHBoxLayout *hbox = new QHBoxLayout;
    layout->addLayout(hbox, row, column, 1, 3);
*/
    m_firstDateEdit  = new DDateEdit;
    m_secondDateEdit = new DDateEdit;

    if (m_type == DateOnly)
    {
        layout->addWidget(m_firstDateEdit,  row, column);
        layout->addWidget(m_betweenLabel,   row, column + 1, Qt::AlignHCenter);
        layout->addWidget(m_secondDateEdit, row, column + 2);
    }
    else
    {
        QHBoxLayout* const hbox1 = new QHBoxLayout;
        QHBoxLayout* const hbox2 = new QHBoxLayout;

        m_firstTimeEdit  = new QTimeEdit;
        m_secondTimeEdit = new QTimeEdit;

        hbox1->addWidget(m_firstDateEdit);
        hbox1->addWidget(m_firstTimeEdit);
        hbox2->addWidget(m_secondDateEdit);
        hbox2->addWidget(m_secondTimeEdit);

        layout->addLayout(hbox1,          row, column);
        layout->addWidget(m_betweenLabel, row, column + 1, Qt::AlignHCenter);
        layout->addLayout(hbox2,          row, column + 2);
    }

    connect(m_firstDateEdit, SIGNAL(dateChanged(QDate)),
            this, SLOT(valueChanged()));

    connect(m_secondDateEdit, SIGNAL(dateChanged(QDate)),
            this, SLOT(valueChanged()));
}

void SearchFieldRangeDate::setBetweenText(const QString& between)
{
    m_betweenLabel->setText(between);
}

void SearchFieldRangeDate::read(SearchXmlCachingReader& reader)
{
    SearchXml::Relation relation = reader.fieldRelation();

    if ((relation == SearchXml::Interval) || (relation == SearchXml::IntervalOpen))
    {
        QList<QDateTime> dates = reader.valueToDateTimeList();

        if (dates.size() != 2)
        {
            return;
        }

        if (m_type == DateTime)
        {
            m_firstDateEdit->setDate(dates.first().date());
            m_firstTimeEdit->setTime(dates.first().time());
            m_secondDateEdit->setDate(dates.last().date());
            m_secondTimeEdit->setTime(dates.last().time());
        }
        else
        {
            if (relation == SearchXml::Interval)
            {
                dates.last() = dates.last().addDays(-1);
            }

            m_firstDateEdit->setDate(dates.first().date());
            m_secondDateEdit->setDate(dates.last().date());
        }
    }
    else
    {
        QDateTime dt = reader.valueToDateTime();

        if (m_type == DateTime)
        {
            if      (relation == SearchXml::Equal)
            {
                m_firstDateEdit->setDate(dt.date());
                m_firstTimeEdit->setTime(dt.time());
                m_secondDateEdit->setDate(dt.date());
                m_secondTimeEdit->setTime(dt.time());
            }
            else if (relation == SearchXml::GreaterThanOrEqual || relation == SearchXml::GreaterThan)
            {
                m_firstDateEdit->setDate(dt.date());
                m_firstTimeEdit->setTime(dt.time());
            }

            {
                m_secondDateEdit->setDate(dt.date());
                m_secondTimeEdit->setTime(dt.time());
            }
        }
        else
        {
            // In DateOnly mode, we always assume dealing with the beginning of the day, QTime(0,0,0).
            // In the UI, we show the date only (including the whole day).
            // The difference between ...Than and ...ThanOrEqual is only one second, ignored.

            if      (relation == SearchXml::Equal)
            {
                m_firstDateEdit->setDate(dt.date());
                m_secondDateEdit->setDate(dt.date());
            }
            else if (relation == SearchXml::GreaterThanOrEqual || relation == SearchXml::GreaterThan)
            {
                m_firstDateEdit->setDate(dt.date());
            }
            else if (relation == SearchXml::LessThanOrEqual || relation == SearchXml::LessThan)
            {
                dt = dt.addDays(-1);
                m_secondDateEdit->setDate(dt.date());
            }
        }
    }

    valueChanged();
}

void SearchFieldRangeDate::write(SearchXmlWriter& writer)
{
    if (m_firstDateEdit->date().isValid() && m_secondDateEdit->date().isValid())
    {
        QDateTime firstDate = startOfDay(m_firstDateEdit->date());

        if (m_type == DateTime)
        {
            firstDate.setTime(m_firstTimeEdit->time());
        }

        QDateTime secondDate = startOfDay(m_secondDateEdit->date());

        if (m_type == DateTime)
        {
            secondDate.setTime(m_secondTimeEdit->time());
        }

        if (firstDate == secondDate)
        {
            writer.writeField(m_name, SearchXml::Equal);
            writer.writeValue(firstDate);
            writer.finishField();
        }
        else
        {
            if (m_type == DateOnly)
            {
                secondDate = secondDate.addDays(1);
            }

            writer.writeField(m_name, SearchXml::Interval);
            writer.writeValue(QList<QDateTime>() << firstDate << secondDate);
            writer.finishField();
        }
    }
    else
    {
        QDate date = m_firstDateEdit->date();

        if (date.isValid())
        {
            writer.writeField(m_name, SearchXml::GreaterThanOrEqual);
            QDateTime dt = startOfDay(date);

            if (m_type == DateTime)
            {
                dt.setTime(m_firstTimeEdit->time());
            }

            writer.writeValue(dt);
            writer.finishField();
        }

        date = m_secondDateEdit->date();

        if (date.isValid())
        {
            writer.writeField(m_name, SearchXml::LessThan);
            QDateTime dt = startOfDay(date);

            if (m_type == DateTime)
            {
                dt.setTime(m_secondTimeEdit->time());
            }
            else
            {
                dt = dt.addDays(1);    // include whole day
            }

            writer.writeValue(dt);
            writer.finishField();
        }
    }
}

void SearchFieldRangeDate::reset()
{
    m_firstDateEdit->setDate(QDate());

    if (m_type == DateTime)
    {
        m_firstTimeEdit->setTime(QTime(0, 0, 0, 0));
    }

    m_secondDateEdit->setDate(QDate());

    if (m_type == DateTime)
    {
        m_secondTimeEdit->setTime(QTime(0, 0, 0, 0));
    }

    valueChanged();
}

void SearchFieldRangeDate::setBoundary(const QDateTime& min, const QDateTime& max)
{
    // something here?

    Q_UNUSED(min);
    Q_UNUSED(max);
}

void SearchFieldRangeDate::setValueWidgetsVisible(bool visible)
{
    m_firstDateEdit->setVisible(visible);

    if (m_firstTimeEdit)
    {
        m_firstTimeEdit->setVisible(visible);
    }

    m_secondDateEdit->setVisible(visible);

    if (m_secondTimeEdit)
    {
        m_secondTimeEdit->setVisible(visible);
    }

    m_betweenLabel->setVisible(visible);
}

QList<QRect> SearchFieldRangeDate::valueWidgetRects() const
{
    QList<QRect> rects;
    rects << m_firstDateEdit->geometry();

    if (m_firstTimeEdit)
    {
        rects << m_firstTimeEdit->geometry();
    }

    rects << m_secondDateEdit->geometry();

    if (m_secondTimeEdit)
    {
        rects << m_secondTimeEdit->geometry();
    }

    return rects;
}

void SearchFieldRangeDate::valueChanged()
{
    setValidValueState(m_firstDateEdit->date().isValid() || m_secondDateEdit->date().isValid());
}

} // namespace Digikam
