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

#ifndef AV_PLAYER_QOPTIONS_H
#define AV_PLAYER_QOPTIONS_H

// Qt includes

#include <QVariant>
#include <QMap>
#include <QList>

/*
 command line: some_cmd -short1 value --long1 value --long2=value -short_novalue --long_novalue...
 C++:
 QOptions get_global_options() {
    static QOptions ops = QOptions().addDescription("...")
                    .add("common option group")
                    ("long1,short1", default1, "description1")
                    ("--long2", default2, "description2")
                    ("short_novalue", "description3")
                    ("--long_novalue", "description4")
                    ;
    return ops;
 }
 QOptions options = get_global_options();
 options.add("special option group")(....)...;
 options.parse(argc, argv);
 int v1 = options.value("short1").toInt();
 */

namespace AVPlayer
{

class QOption
{
public:

    // TODO: MultiToken -name value1 -name value2 ...

    enum Type
    {
        NoToken,
        SingleToken,
        MultiToken
    };

public:

    QOption();
    explicit QOption(const char* name, const QVariant& defaultValue,
                     Type type, const QString& description);
    explicit QOption(const char* name, Type type, const QString& description);

    QString shortName()                 const;
    QString longName()                  const;
    QString formatName()                const;
    QString description()               const;
    QVariant value()                    const;
    void setValue(const QVariant& value);
    bool isSet()                        const;
    bool isValid()                      const;

    void setType(QOption::Type type);
    QOption::Type type()                const;

    QString help()                      const;
    void print()                        const;
    bool operator <(const QOption& o)   const;

private:

    /*!
     * \brief setName
     * short/long name format:
     * "--long", "-short", "short"
     * "long,short", "--long,short", "--long,-short", "long,-short"
     * "short,--long", "-short,long", "-short,--long"
     * \param name
     */
    void setName(const QString& name);

    QOption::Type mType;
    QString       mShortName;
    QString       mLongName;
    QString       mDescription;
    QVariant      mDefaultValue;
    QVariant      mValue;
};

class QOptions
{
public:

    // e.g. application information, copyright etc.

    QOptions();
    ~QOptions();

    /*!
     * \brief parse
     * \param argc
     * \param argv
     * \return false if invalid option found
     */
    bool parse(int argc, const char* const* argv);
    QOptions& add(const QString& group_description);
    QOptions& addDescription(const QString& description);

    QOptions& operator()(const char* name,
                         const QString& description = QString());
    QOptions& operator()(const char* name, QOption::Type type,
                         const QString& description = QString());
    QOptions& operator()(const char* name, const QVariant& defaultValue,
                         const QString& description);
    QOptions& operator()(const char* name, const QVariant& defaultValue,
                         QOption::Type type, const QString& description = QString());

    QOption option(const QString& name)         const;
    QVariant value(const QString& name)         const;
    QVariant operator [](const QString& name)   const;

    QString help()                              const;
    void print()                                const;

private:

    QString mDescription,               mCurrentDescription;
    QList<QOption>                      mOptions;
    QMap<QOption, QString /* group */ > mOptionGroupMap;
};

} // namespace AVPlayer

#endif // AV_PLAYER_QOPTIONS_H
