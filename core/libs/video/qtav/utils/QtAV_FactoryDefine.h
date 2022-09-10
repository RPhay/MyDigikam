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

#ifndef QTAV_FACTORY_DEFINE_H
#define QTAV_FACTORY_DEFINE_H

// C++ includes

#include <cstdio>
#include <string>
#include <vector>

/*!
 * A: Suppose we need a factory for class MyClass. We can query a derived class
 * of MyClass from factory by an id type MyClassId.
 * To create the factory, just 2 steps
 * 1. In MyClass.h:
 *      #include "QtAV_FactoryDefine.h"
 *      FACTORY_DECLARE(MyClass)
 * 2. In MyClass.cpp:
 *      #include "QtAV_factory.h"
 *      FACTORY_DEFINE(MyClass)
 *
 * To create and register a new subclass MyClassSubA with it's id
 * 0. In MyClassTypes.h (Usually just include this is enough to use the factory. And MyClassXXX.{h,cpp} can NOT include this file),
 *    MyClassSubA's ID:
 *      extern DIGIKAM_EXPORT MyClassId MyClassId_SubA;
 *    In MyClassTypes.cpp, define the id value:
 *      MyClassId MyClassId_SubA = some_value;
 *  We define the id in MyClassTypes.cpp because MyClassSubA may not be compiled(e.g. platform dependent features), but the id must be defined.
 * 1. create a source file MyClassSubA.cpp and implement the required members
 * 2. In MyClassSubA.cpp, add the following lines
 *      #include "QtAV_prepost.h" //for PRE_FUNC_ADD()
 *      //we don't want to depend on MyClassTypes.h, so extern
 *      extern MyClassId MyClassId_SubA;
 *      FACTORY_REGISTER_ID_AUTO(MyClass, SubA, "SubA's name")
 *      void RegisterMyClassSubA_Man()
 *      {
 *          FACTORY_REGISTER_ID_MAN(MyClass, SubA, "SubA's name")
 *      }
 *
 * 3. In MyClass.cpp, add register function into MyClass_RegisterAll();
 *      extern void RegisterMyClassSubA_Man();
 *      void MyClass_RegisterAll()
 *      {
 *          RegisterMyClassSubA_Man(); //add this line
 *      }
 *
 *******************************************************************************************************
 * B: If MyClass and it's factory already exist in the library and you want to add a new subclass without
 * changing the library,
 *  just create MyClassSubA.cpp with the content:
 *
 *      #include "MyClass.h"
 *      #include "QtAV_prepost.h" //for PRE_FUNC_ADD()
 *      MyClassId MyClassId_SubA = some_value; //it can be used somewhere else as "extern"
 *      FACTORY_REGISTER_ID_AUTO(MyClass, SubA, "SubA's name")
 *      void RegisterMyClassSubA_Man() //call it when you need as "extern"
 *      {
 *          FACTORY_REGISTER_ID_MAN(MyClass, SubA, "SubA's name")
 *      }
 *
 *      class MyClassSubA : public MyClass
 *      {
 *          ...
 *      };
 */

/**
 * This should be in header
 */
#define FACTORY_REGISTER(BASE, _ID, NAME) FACTORY_REGISTER_ID_TYPE(BASE, BASE##Id_##_ID, BASE##_ID, NAME)

#define FACTORY_REGISTER_ID_TYPE(BASE, ID, TYPE, NAME)      \
    FACTORY_REGISTER_ID_TYPE_AUTO(BASE, ID, TYPE, NAME)     \
    void Register##TYPE##_Man() {                           \
        FACTORY_REGISTER_ID_TYPE_MAN(BASE, ID, TYPE, NAME); \
    }

#define FACTORY_REGISTER_ID_AUTO(BASE, _ID, NAME) \
    FACTORY_REGISTER_ID_TYPE_AUTO(BASE, BASE##Id_##_ID, BASE##_ID, NAME)

#define FACTORY_REGISTER_ID_MAN(BASE, _ID, NAME) \
    FACTORY_REGISTER_ID_TYPE_MAN(BASE, BASE##Id_##_ID, BASE##_ID, NAME)

#define FACTORY_REGISTER_ID_TYPE_MAN(BASE, ID, TYPE, NAME)  \
    BASE##Factory::register_<TYPE>(ID);                     \
    BASE##Factory::registerIdName(ID, NAME);

/*
 * FIXME: __init_##TYPE (only if static) and xxx_Man() has the same content, and are both defined, construtor functions will not be called for gcc5.
 * maybe also happens for ios
 * Remove xxx_Man() is also a workaround
 */
#define FACTORY_REGISTER_ID_TYPE_AUTO(BASE, ID, TYPE, NAME) \
    static int __init_##TYPE() {                            \
        FACTORY_REGISTER_ID_TYPE_MAN(BASE, ID, TYPE, NAME)  \
        return 0;                                           \
    }                                                       \
    PRE_FUNC_ADD(__init_##TYPE)

/**
 * This should be in header
 */
#define FACTORY_DECLARE(T) FACTORY_DECLARE_ID(T, T##Id)

#define FACTORY_DECLARE_ID(T, ID)                                                       \
    class DIGIKAM_EXPORT T##Factory                                                        \
    {                                                                                   \
    public:                                                                             \
        typedef T* (*T##Creator)();                                                     \
        static T* create(const ID& id);                                                 \
        template<class C>                                                               \
        static bool register_(const ID& id) { return registerCreator(id, create<C>); }  \
        static bool registerCreator(const ID&, const T##Creator&);                      \
        static bool registerIdName(const ID& id, const std::string& name);              \
        static bool unregisterCreator(const ID& id);                                    \
        static ID id(const std::string& name, bool caseSensitive = true);               \
        static std::string name(const ID &id);                                          \
        static std::vector<ID> registeredIds();                                         \
        static std::vector<std::string> registeredNames();                              \
        static size_t count();                                                          \
        static T* getRandom();                                                          \
    private:                                                                            \
        template<class C> static T* create() { return new C(); }                        \
    };

/**
 * This should be in cpp
 */
#define FACTORY_DEFINE(T) FACTORY_DEFINE_ID(T, T##Id)

#define FACTORY_DEFINE_ID(T, ID)                                                                                                                      \
    class T##FactoryBridge : public Factory<ID, T, T##FactoryBridge> {};                                                                              \
    T* T##Factory::create(const ID& id) { return T##FactoryBridge::Instance().create(id); }                                                           \
    bool T##Factory::registerCreator(const ID& id, const T##Creator& callback) { return T##FactoryBridge::Instance().registerCreator(id, callback); } \
    bool T##Factory::registerIdName(const ID& id, const std::string& name) { return T##FactoryBridge::Instance().registerIdName(id, name); }          \
    bool T##Factory::unregisterCreator(const ID& id) { return T##FactoryBridge::Instance().unregisterCreator(id); }                                   \
    ID T##Factory::id(const std::string& name, bool caseSensitive) { return T##FactoryBridge::Instance().id(name, caseSensitive); }                   \
    std::string T##Factory::name(const ID &id) { return T##FactoryBridge::Instance().name(id); }                                                      \
    std::vector<ID> T##Factory::registeredIds() { return T##FactoryBridge::Instance().registeredIds(); }                                              \
    std::vector<std::string> T##Factory::registeredNames() { return T##FactoryBridge::Instance().registeredNames(); }                                 \
    size_t T##Factory::count() { return T##FactoryBridge::Instance().count(); }                                                                       \
    T* T##Factory::getRandom() { fflush(0);return T##FactoryBridge::Instance().getRandom(); }

#endif // QTAV_FACTORY_DEFINE_H
