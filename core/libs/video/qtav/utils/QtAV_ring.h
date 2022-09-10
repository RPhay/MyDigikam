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

#ifndef QTAV_RING_H
#define QTAV_RING_H

// C++ includes

#include <cassert>
#include <vector>

namespace QtAV
{

template<typename T, typename C>
class ring_api
{
public:

    ring_api()
        : m_0(0),
          m_1(0),
          m_s(0)
    {
    }

    virtual ~ring_api()
    {
    }

    void push_back(const T& t);
    void pop_front();

    T& front()
    {
        return m_data[m_0];
    }

    const T& front() const
    {
        return m_data[m_0];
    }

    T& back()
    {
        return m_data[m_1];
    }

    const T& back() const
    {
        return m_data[m_1];
    }

    virtual size_t capacity() const = 0;

    size_t size() const
    {
        return m_s;
    }

    bool empty() const
    {
        return (size() == 0);
    }

    // need at() []?

    const T& at(size_t i) const
    {
        assert(i < m_s);

        return m_data[index(m_0 + i)];
    }

    const T& operator[](size_t i) const
    {
        return at(i);
    }

    T& operator[](size_t i)
    {
        assert(i < m_s);

        return m_data[index(m_0 + i)];
    }

protected:

    size_t index(size_t i) const
    {
        // i always [0,capacity())

        return ((i < capacity()) ? i : i - capacity());
    }

    size_t m_0;
    size_t m_1;
    size_t m_s;
    C      m_data;
};

template<typename T>
class ring : public ring_api<T, std::vector<T> >
{
    using ring_api<T, std::vector<T> >::m_data; // why need this?

public:

    explicit ring(size_t capacity)
        : ring_api<T, std::vector<T> >()
    {
        m_data.reserve(capacity);
        m_data.resize(capacity);
    }

    size_t capacity() const override
    {
        return m_data.size();
    }
};

template<typename T, int N>
class static_ring : public ring_api<T, T[N]>
{
    using ring_api<T, T[N]>::m_data; // why need this?

public:

    static_ring()
        : ring_api<T, T[N]>()
    {
    }

    size_t capacity() const
    {
        return N;
    }
};

template<typename T, typename C>
void ring_api<T,C>::push_back(const T& t)
{
    if      (m_s == capacity())
    {
        m_data[m_0] = t;
        m_0         = index(++m_0);
        m_1         = index(++m_1);
    }
    else if (empty())
    {
        m_s         = 1;
        m_0         = 0;
        m_1         = 0;
        m_data[m_0] = t;
    }
    else
    {
        m_data[index(m_0 + m_s)] = t;
        ++m_1;
        ++m_s;
    }
}

template<typename T, typename C>
void ring_api<T,C>::pop_front()
{
    assert(!empty());

    if (empty())
    {
        return;
    }

    m_data[m_0] = T(); // erase the old data
    m_0         = index(++m_0);
    --m_s;
}

} // namespace QtAV

#endif // QTAV_RING_H
