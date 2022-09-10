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

#ifndef QTAV_SHARED_PTR
#define QTAV_SHARED_PTR

// C++ includes

#include <algorithm>

// Qt includes

#include <QAtomicInt>

/**
 * a simple thread safe shared ptr. QSharedPointer does not provide a way to get how many the ref count is.
 */

namespace impl
{

template <typename T>

class SharedPtrImpl
{

public:

    explicit SharedPtrImpl(T* const ptr = nullptr)
        : m_ptr    (ptr),
          m_counter(1)
    {
    }

    ~SharedPtrImpl()
    {
        delete m_ptr;
    }

    T* operator->() const
    {
        return m_ptr;
    }

    T& operator*() const
    {
        return *m_ptr;
    }

    T* get() const
    {
        return m_ptr;
    }

    void ref()
    {
        m_counter.ref();
    }

    // return false if becomes 0

    bool deref()
    {
        return m_counter.deref();
    }

    int count() const
    {
        return m_counter;
    }

    bool isNull() const
    {
        return !m_ptr;
    }

private:

    T*         m_ptr        = nullptr;
    QAtomicInt m_counter;
};

} // namespace impl

template <typename T>
class SharedPtr
{
public:

    explicit SharedPtr(T* const ptr = nullptr)
        : m_impl(new impl::SharedPtrImpl<T>(ptr))
    {
    }

    template <typename U>
    explicit SharedPtr(U* const ptr = nullptr)
        : m_impl(new impl::SharedPtrImpl<T>(ptr))
    {
    }

    ~SharedPtr()
    {
        if (!m_impl->deref())
            delete m_impl;
    }

    SharedPtr(const SharedPtr& other)
    {
        m_impl = other.m_impl;      // cppcheck-suppress copyCtorPointerCopying
        m_impl->ref();
    }

    template <typename U>
    explicit SharedPtr(const SharedPtr<U>& other)
    {
        m_impl = other.m_impl;
        m_impl->ref();
    }

    SharedPtr& operator=(const SharedPtr& other)
    {
        SharedPtr tmp(other);
        swap(tmp);

        return *this;
    }

    template <typename U>
    SharedPtr<T>& operator=(const SharedPtr<U>& other)
    {
        SharedPtr<T> tmp(other);
        swap(tmp);

        return *this;
    }

    T* operator->() const
    {
        return m_impl->operator->();
    }

    T& operator*() const
    {
        return **m_impl;
    }

    bool operator !() const
    {
        return isNull();
    }

    template <typename U>
    bool operator<(const SharedPtr<U>& other)
    {
        return m_impl->get() < other.get();
    }

    bool isNull() const
    {
        return m_impl->isNull();
    }

    T* get() const
    {
        return m_impl->get();
    }

    int count() const
    {
        return m_impl->count();
    }

    void swap(SharedPtr<T> &other)
    {
        std::swap(m_impl, other.m_impl);
    }

private:

    impl::SharedPtrImpl<T>* m_impl;
};

template <typename T>
void swap(SharedPtr<T>& left, SharedPtr<T>& right)
{
    left.swap(right);
}

#endif // QTAV_SHARED_PTR
