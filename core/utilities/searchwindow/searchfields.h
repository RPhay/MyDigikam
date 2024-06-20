/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-01-20
 * Description : User interface for searches
 *
 * SPDX-FileCopyrightText: 2008-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2011-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QObject>
#include <QWidget>
#include <QMap>

// Local includes

#include "coredbsearchxml.h"
#include "searchutilities.h"
#include "visibilitycontroller.h"

class QLabel;
class QCheckBox;
class QGridLayout;
class QSpinBox;
class QDoubleSpinBox;
class QTimeEdit;
class QTreeView;
class QComboBox;
class QLineEdit;

namespace Digikam
{

class DAdjustableLabel;
class AbstractAlbumTreeViewSelectComboBox;
class SearchFieldGroup;
class SqueezedComboBox;
class DDateEdit;
class ChoiceSearchModel;
class ChoiceSearchComboBox;
class RatingComboBox;
class PickLabelFilter;
class ColorLabelFilter;

class SearchField : public QObject,
                    public VisibilityObject
{
    Q_OBJECT

public:

    enum WidgetRectType
    {
        LabelAndValueWidgetRects,
        ValueWidgetRectsOnly
    };

public:

    static SearchField* createField(const QString& fieldName, SearchFieldGroup* const parent);

    explicit SearchField(QObject* const parent);

    void setup(QGridLayout* const layout, int row = -1);
    void setFieldName(const QString& fieldName);
    void setCategoryLabelVisible(bool visible);
    void setCategoryLabelVisibleFromPreviousField(SearchField* const previousField);

    QList<QRect> widgetRects(WidgetRectType = ValueWidgetRectsOnly) const;

    virtual void setText(const QString& label, const QString& detailLabel);

    virtual bool supportsField(const QString& fieldName);
    virtual void read(SearchXmlCachingReader& reader) = 0;
    virtual void write(SearchXmlWriter& writer)       = 0;
    virtual void reset()                              = 0;

    void setVisible(bool visible) override;
    bool isVisible()              override;

Q_SIGNALS:

    void signalVisibilityChanged();

protected Q_SLOTS:

    void clearButtonClicked();

protected:

    void setValidValueState(bool valueIsValid);

    virtual void setupValueWidgets(QGridLayout* layout, int row, int column) = 0;
    virtual void setupLabels(QGridLayout* layout, int line);

    virtual void setValueWidgetsVisible(bool visible)                        = 0;
    virtual QList<QRect> valueWidgetRects()                            const = 0;

protected:

    QString              m_name;

    QLabel*              m_label                    = nullptr;
    QLabel*              m_detailLabel              = nullptr;

    AnimatedClearButton* m_clearButton              = nullptr;

    bool                 m_categoryLabelVisible     = true;
    bool                 m_valueIsValid             = false;

private:

    // Disable
    SearchField(const SearchField&)            = delete;
    SearchField& operator=(const SearchField&) = delete;
};

//-----------------------------------------------------------------------------

class SearchFieldText : public SearchField
{
    Q_OBJECT

public:

    explicit SearchFieldText(QObject* const parent);

    void setupValueWidgets(QGridLayout* layout, int row, int column)    override;
    void read(SearchXmlCachingReader& reader)                           override;
    void write(SearchXmlWriter& writer)                                 override;
    void setValueWidgetsVisible(bool visible)                           override;
    void reset()                                                        override;
    QList<QRect> valueWidgetRects()                               const override;

protected Q_SLOTS:

    void valueChanged(const QString& text);

protected:

    QLineEdit* m_edit = nullptr;
};

//-----------------------------------------------------------------------------

class SearchFieldKeyword : public SearchFieldText
{
    Q_OBJECT

public:

    explicit SearchFieldKeyword(QObject* const parent);

    void read(SearchXmlCachingReader& reader) override;
    void write(SearchXmlWriter& writer)       override;
};

//-----------------------------------------------------------------------------

class SearchFieldRangeInt : public SearchField
{
    Q_OBJECT

public:

    explicit SearchFieldRangeInt(QObject* const parent);

    void setBetweenText(const QString& text);
    void setNoValueText(const QString& text);
    void setNumberPrefixAndSuffix(const QString& prefix, const QString& suffix);
    void setBoundary(int min, int max, int step = 1);
    void enableFractionMagic(const QString& prefix);

    void setSuggestedValues(const QList<int>& values);
    void setSuggestedInitialValue(int initialValue);
    void setSingleSteps(int smaller, int larger);
    void setInvertStepping(bool invert);

    void setupValueWidgets(QGridLayout* layout, int row, int column)    override;
    void read(SearchXmlCachingReader& reader)                           override;
    void write(SearchXmlWriter& writer)                                 override;
    void reset()                                                        override;
    void setValueWidgetsVisible(bool visible)                           override;
    QList<QRect> valueWidgetRects()                               const override;

protected Q_SLOTS:

    void valueChanged();

protected:

    int                    m_min            = 0;
    int                    m_max            = 100;
    bool                   m_reciprocal     = false;
    CustomStepsIntSpinBox* m_firstBox       = nullptr;
    CustomStepsIntSpinBox* m_secondBox      = nullptr;
    QLabel*                m_betweenLabel   = nullptr;

private:

    // Disable
    SearchFieldRangeInt(const SearchFieldRangeInt&)            = delete;
    SearchFieldRangeInt& operator=(const SearchFieldRangeInt&) = delete;
};

//-----------------------------------------------------------------------------

class SearchFieldRangeDouble : public SearchField
{
    Q_OBJECT

public:

    explicit SearchFieldRangeDouble(QObject* const parent);

    void setBetweenText(const QString& text);
    void setNoValueText(const QString& text);
    void setNumberPrefixAndSuffix(const QString& prefix, const QString& suffix);
    void setBoundary(double min, double max, int decimals, double step);
    void setFactor(double factor);

    void setSuggestedValues(const QList<double>& values);
    void setSuggestedInitialValue(double initialValue);
    void setSingleSteps(double smaller, double larger);
    void setInvertStepping(bool invert);

    void setupValueWidgets(QGridLayout* layout, int row, int column)    override;
    void read(SearchXmlCachingReader& reader)                           override;
    void write(SearchXmlWriter& writer)                                 override;
    void reset()                                                        override;
    void setValueWidgetsVisible(bool visible)                           override;
    QList<QRect> valueWidgetRects()                               const override;

protected Q_SLOTS:

    void valueChanged();

protected:

    double                    m_min          = 0.0;
    double                    m_max          = 100.0;
    double                    m_factor       = 1.0;
    CustomStepsDoubleSpinBox* m_firstBox     = nullptr;
    CustomStepsDoubleSpinBox* m_secondBox    = nullptr;
    QLabel*                   m_betweenLabel = nullptr;

private:

    // Disable
    SearchFieldRangeDouble(const SearchFieldRangeDouble&)            = delete;
    SearchFieldRangeDouble& operator=(const SearchFieldRangeDouble&) = delete;
};

//-----------------------------------------------------------------------------

class SearchFieldRangeDate : public SearchField
{
    Q_OBJECT

public:

    enum Type
    {
        DateOnly,
        DateTime
    };

public:

    SearchFieldRangeDate(QObject* const parent, Type type);

    void setBetweenText(const QString& between);
    void setBoundary(const QDateTime& min, const QDateTime& max);

    void setupValueWidgets(QGridLayout* layout, int row, int column)    override;
    void read(SearchXmlCachingReader& reader)                           override;
    void write(SearchXmlWriter& writer)                                 override;
    void reset()                                                        override;
    void setValueWidgetsVisible(bool visible)                           override;
    QList<QRect> valueWidgetRects()                               const override;

protected Q_SLOTS:

    void valueChanged();

protected:

    QTimeEdit* m_firstTimeEdit  = nullptr;
    DDateEdit* m_firstDateEdit  = nullptr;
    QTimeEdit* m_secondTimeEdit = nullptr;
    DDateEdit* m_secondDateEdit = nullptr;
    QLabel*    m_betweenLabel   = nullptr;

    Type       m_type           = DateOnly;

private:

    // Disable
    SearchFieldRangeDate(const SearchFieldRangeDate&)            = delete;
    SearchFieldRangeDate& operator=(const SearchFieldRangeDate&) = delete;
};

//-----------------------------------------------------------------------------

class SearchFieldRangeTime : public SearchField
{
    Q_OBJECT

public:

    explicit SearchFieldRangeTime(QObject* const parent);

    void setBetweenText(const QString& between);
    void setBoundary(const QTime& min, const QTime& max);

    void setupValueWidgets(QGridLayout* layout, int row, int column)    override;
    void read(SearchXmlCachingReader& reader)                           override;
    void write(SearchXmlWriter& writer)                                 override;
    void reset()                                                        override;
    void setValueWidgetsVisible(bool visible)                           override;
    QList<QRect> valueWidgetRects()                               const override;

protected Q_SLOTS:

    void valueChanged();

protected:

    QTimeEdit* m_firstTimeEdit  = nullptr;
    QTimeEdit* m_secondTimeEdit = nullptr;
    QLabel*    m_betweenLabel   = nullptr;

private:

    // Disable
    SearchFieldRangeTime(const SearchFieldRangeTime&)            = delete;
    SearchFieldRangeTime& operator=(const SearchFieldRangeTime&) = delete;
};

//-----------------------------------------------------------------------------

class SearchFieldChoice : public SearchField
{
    Q_OBJECT

public:

    explicit SearchFieldChoice(QObject* const parent);

    void setChoice(const QMap<int, QString>& map);
    void setChoice(const QStringList& choice);
    void setAnyText(const QString& anyText);

    void setupValueWidgets(QGridLayout* layout, int row, int column)    override;
    void read(SearchXmlCachingReader& reader)                           override;
    void write(SearchXmlWriter& writer)                                 override;
    void reset()                                                        override;
    void setValueWidgetsVisible(bool visible)                           override;
    QList<QRect> valueWidgetRects()                               const override;

protected Q_SLOTS:

    void checkStateChanged();

protected:

    void updateComboText();

protected:

    ChoiceSearchComboBox* m_comboBox    = nullptr;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))

    QMetaType::Type       m_type        = QMetaType::UnknownType;

#else

    QVariant::Type        m_type        = QVariant::Invalid;

#endif

    QString               m_anyText;
    ChoiceSearchModel*    m_model       = nullptr;

private:

    // Disable
    SearchFieldChoice(const SearchFieldChoice&)            = delete;
    SearchFieldChoice& operator=(const SearchFieldChoice&) = delete;
};

//-----------------------------------------------------------------------------

class Album;
class AbstractCheckableAlbumModel;
class AbstractAlbumTreeViewSelectComboBox;
class AlbumTreeViewSelectComboBox;
class TagTreeViewSelectComboBox;
class SqueezedComboBox;

class SearchFieldAlbum : public SearchField
{
    Q_OBJECT

public:

    enum Type
    {
        TypeAlbum,
        TypeTag
    };

    enum Operation
    {
        All,
        OneOf,
        InTree
    };

public:

    SearchFieldAlbum(QObject* const parent, Type type);

    void setupValueWidgets(QGridLayout* layout, int row, int column)    override;
    void read(SearchXmlCachingReader& reader)                           override;
    void write(SearchXmlWriter& writer)                                 override;
    void reset()                                                        override;
    void setValueWidgetsVisible(bool visible)                           override;
    QList<QRect> valueWidgetRects()                               const override;

protected Q_SLOTS:

    void updateState();

protected:

    QWidget*                             m_wrapperBox       = nullptr;
    AlbumTreeViewSelectComboBox*         m_albumComboBox    = nullptr;
    TagTreeViewSelectComboBox*           m_tagComboBox      = nullptr;
    SqueezedComboBox*                    m_operation        = nullptr;
    Type                                 m_type             = TypeAlbum;
    AbstractCheckableAlbumModel*         m_model            = nullptr;
};

//-----------------------------------------------------------------------------

class SearchFieldRating : public SearchField
{
    Q_OBJECT

public:

    explicit SearchFieldRating(QObject* const parent);

    void setBetweenText(const QString& text);

    void setupValueWidgets(QGridLayout* layout, int row, int column)    override;
    void read(SearchXmlCachingReader& reader)                           override;
    void write(SearchXmlWriter& writer)                                 override;
    void reset()                                                        override;
    void setValueWidgetsVisible(bool visible)                           override;
    QList<QRect> valueWidgetRects()                               const override;

protected Q_SLOTS:

    void firstValueChanged();
    void secondValueChanged();

protected:

    RatingComboBox* m_firstBox      = nullptr;
    RatingComboBox* m_secondBox     = nullptr;
    QLabel*         m_betweenLabel  = nullptr;

private:

    // Disable
    SearchFieldRating(const SearchFieldRating&)            = delete;
    SearchFieldRating& operator=(const SearchFieldRating&) = delete;
};

//-----------------------------------------------------------------------------

class SearchFieldComboBox : public SearchField
{
    Q_OBJECT

public:

    explicit SearchFieldComboBox(QObject* const  parent);

    void setupValueWidgets(QGridLayout* layout, int row, int column)    override;
    void write(SearchXmlWriter& writer)                                 override;
    void reset()                                                        override;
    void setValueWidgetsVisible(bool visible)                           override;
    QList<QRect> valueWidgetRects()                               const override;

protected Q_SLOTS:

    void indexChanged(int);

protected:

    QComboBox* m_comboBox = nullptr;
};

//-----------------------------------------------------------------------------

class SearchFieldCheckBox : public SearchField
{
    Q_OBJECT

public:

    explicit SearchFieldCheckBox(QObject* const parent);

    void setLabel(const QString& text);

    void setupValueWidgets(QGridLayout* layout, int row, int column)    override;
    void read(SearchXmlCachingReader& reader)                           override;
    void write(SearchXmlWriter& writer)                                 override;
    void reset()                                                        override;
    void setValueWidgetsVisible(bool visible)                           override;
    QList<QRect> valueWidgetRects()                               const override;

protected Q_SLOTS:

    void slotToggled(bool checked);

protected:

    QCheckBox* m_checkBox = nullptr;
    QString    m_text;
};

//-----------------------------------------------------------------------------

class SearchFieldColorDepth : public SearchFieldComboBox
{
    Q_OBJECT

public:

    explicit SearchFieldColorDepth(QObject* const parent);

    void setupValueWidgets(QGridLayout* layout, int row, int column) override;
    void read(SearchXmlCachingReader& reader)                        override;
};

//-----------------------------------------------------------------------------

class SearchFieldPageOrientation: public SearchFieldComboBox
{
    Q_OBJECT

public:

    explicit SearchFieldPageOrientation(QObject* const parent);

    void setupValueWidgets(QGridLayout* layout, int row, int column) override;
    void read(SearchXmlCachingReader& reader)                        override;
};

//-----------------------------------------------------------------------------

class SearchFieldLabels : public SearchField
{
    Q_OBJECT

public:

    explicit SearchFieldLabels(QObject* const parent);

    void setupValueWidgets(QGridLayout* layout, int row, int column)    override;
    void read(SearchXmlCachingReader& reader)                           override;
    void write(SearchXmlWriter& writer)                                 override;
    void reset()                                                        override;
    void setValueWidgetsVisible(bool visible)                           override;
    QList<QRect> valueWidgetRects()                               const override;

protected Q_SLOTS:

    void updateState();

protected:

    PickLabelFilter*  m_pickLabelFilter  = nullptr;
    ColorLabelFilter* m_colorLabelFilter = nullptr;
};

//-----------------------------------------------------------------------------

class SearchFieldMonthDay : public SearchField
{
    Q_OBJECT

public:

    explicit SearchFieldMonthDay(QObject* const parent);

    void setupValueWidgets(QGridLayout* layout, int row, int column)    override;
    void read(SearchXmlCachingReader& reader)                           override;
    void write(SearchXmlWriter& writer)                                 override;
    void reset()                                                        override;
    void setValueWidgetsVisible(bool visible)                           override;
    QList<QRect> valueWidgetRects()                               const override;

protected Q_SLOTS:

    void slotIndexChanged();

protected:

    QLabel*    m_dayLabel   = nullptr;
    QComboBox* m_monthBox   = nullptr;
    QComboBox* m_dayBox     = nullptr;
};

} // namespace Digikam
