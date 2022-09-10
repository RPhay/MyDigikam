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

#ifndef QTAV_SINGLETON_H
#define QTAV_SINGLETON_H

// C++ includes

#include <cstdio>
#include <cstdlib>          // harmattan: atexit
#include <cassert>

#define USE_EXCEPTION 0

#if USE_EXCEPTION
#   include <stdexcept>     // std::string breaks abi
#endif

// Local includes

#include "digikam_debug.h"

#ifdef DEBUG
#   define DBG(fmt, ...)                 \
    qCDebug(DIGIKAM_QTAV_LOG).noquote()  \
        << QString::asprintf(fmt, ##__VA_ARGS__);
#else
#   define DBG(...)
#endif

#define DISABLE_COPY(Class) \
    Class(const Class &);   \
    Class &operator=(const Class &);

/*
 * Used in library, can not be used both in library and outside. so we don't need export it
 */

template <typename T>
class Singleton
{
    DISABLE_COPY(Singleton)

public:

    typedef T ObjectType;
    static T& Instance();

protected:

    Singleton()          {}
    virtual ~Singleton() {}

private:

    static void MakeInstance();
    static void DestroySingleton();

    static T* pInstance_;
    static bool destroyed_;
};

/*
 * if it is used as dll, template will instanced in dll and exe,
 * and pInstance_ are 0 for both
 */

// TODO: use static Singleton<T> inst; return inst;

template<typename T>
T* Singleton<T>::pInstance_; // Why it will be initialized twice? The order?

template<typename T>
bool Singleton<T>::destroyed_;

template<typename T>
T &Singleton<T>::Instance()
{
/*
    DBG("instance = %p\n", pInstance_);
*/
    if (!pInstance_)
    {
        MakeInstance();
    }

    return *pInstance_;
}

template<typename T>
void Singleton<T>::MakeInstance()
{
    if (!pInstance_)
    {
        if (destroyed_)
        {
            destroyed_ = false;

#if USE_EXCEPTION

            throw std::logic_error("Dead Reference Detected");

#else

            DBG("Dead Reference Detected");
            exit(1);

#endif

        }

        pInstance_ = new T();
        DBG("Singleton %p created...\n", pInstance_);
        std::atexit(&DestroySingleton);
    }
}

template<typename T>
void Singleton<T>::DestroySingleton()
{
    DBG("DestroySingleton...\n");
    assert(!destroyed_);
    delete pInstance_;
    pInstance_ = nullptr;
    destroyed_ = true;
}

#endif // QTAV_SINGLETON_H
