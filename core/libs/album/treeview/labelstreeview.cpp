/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2014-05-17
 * Description : Album Labels Tree View.
 *
 * SPDX-FileCopyrightText: 2014-2015 by Mohamed_Anwer <m_dot_anwer at gmx dot com>
 * SPDX-FileCopyrightText: 2014-2025 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "labelstreeview.h"

// QT includes

#include <QApplication>
#include <QPainter>
#include <QValidator>
#include <QStyledItemDelegate>
#include <QUrl>

// KDE includes

#include <kconfiggroup.h>
#include <ksharedconfig.h>
#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "coredbsearchxml.h"
#include "searchtabheader.h"
#include "thememanager.h"
#include "albummanager.h"
#include "albumtreeview.h"
#include "itemlister.h"
#include "coredbaccess.h"
#include "coredb.h"
#include "colorlabelfilter.h"
#include "picklabelfilter.h"
#include "tagscache.h"
#include "applicationsettings.h"
#include "dnotificationwrapper.h"
#include "digikamapp.h"
#include "tagsactionmngr.h"
#include "ratingwidget.h"
#include "dbjobsmanager.h"

namespace Digikam
{

class Q_DECL_HIDDEN ColorLabelValidator : public QValidator
{
public:

    ColorLabelValidator(int maxLength, QObject* const parent = nullptr)
        : QValidator (parent),
          m_maxLength(maxLength)
    {
    }

    QValidator::State validate(QString& input, int& /*pos*/) const override
    {
        if (input.length() <= m_maxLength)
        {
            return QValidator::Acceptable;
        }

        return QValidator::Invalid;
    }

private:

    int m_maxLength = 32;
};

// ---

class Q_DECL_HIDDEN ColorLabelDelegate : public QStyledItemDelegate
{
    Q_OBJECT

public:

    ColorLabelDelegate(int maxLength, QObject *parent = nullptr)
        : QStyledItemDelegate(parent), m_maxLength(maxLength) {}

    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override {
        QLineEdit *editor = new QLineEdit(parent);
        editor->setValidator(new ColorLabelValidator(m_maxLength, editor));
        return editor;
    }

    void setEditorData(QWidget *editor, const QModelIndex &index) const override {
        QString text = index.model()->data(index, Qt::EditRole).toString();
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
        lineEdit->setText(text);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override {
        QLineEdit *lineEdit = qobject_cast<QLineEdit*>(editor);
        model->setData(index, lineEdit->text(), Qt::EditRole);
    }

private:

    int m_maxLength = 32;
};

// ---

class Q_DECL_HIDDEN LabelsTreeView::Private
{
public:

    explicit Private(QTreeWidget* const w)
        : itemIterator(w)
    {
    }

public:

    QFont                      regularFont;
    QSize                      iconSize;

    QTreeWidgetItem*           ratings                      = nullptr;
    QTreeWidgetItem*           picks                        = nullptr;
    QTreeWidgetItem*           colors                       = nullptr;

    QTreeWidgetItemIterator    itemIterator;

    bool                       isCheckableTreeView          = false;
    bool                       isLoadingState               = false;
    int                        iconSizeFromSetting          = 0;

    QHash<Labels, QList<int> > selectedLabels;

public:

    const QString              configRatingSelectionEntry   = QLatin1String("RatingSelection");
    const QString              configPickSelectionEntry     = QLatin1String("PickSelection");
    const QString              configColorSelectionEntry    = QLatin1String("ColorSelection");
    const QString              configExpansionEntry         = QLatin1String("Expansion");
};

LabelsTreeView::LabelsTreeView(QWidget* const parent, bool setCheckable)
    : QTreeWidget      (parent),
      StateSavingObject(this),
      d                (new Private(this))
{
    d->regularFont         = ApplicationSettings::instance()->getTreeViewFont();
    d->iconSizeFromSetting = ApplicationSettings::instance()->getTreeViewIconSize();
    d->iconSize            = QSize(d->iconSizeFromSetting, d->iconSizeFromSetting);
    d->isCheckableTreeView = setCheckable;

    setHeaderLabel(i18nc("@title", "Labels"));
    setUniformRowHeights(false);

    initTreeView();

    if (d->isCheckableTreeView)
    {
        QTreeWidgetItemIterator it(this);

        while (*it)
        {
            if ((*it)->parent())
            {
                (*it)->setFlags((*it)->flags()|Qt::ItemIsUserCheckable);
                (*it)->setCheckState(0, Qt::Unchecked);
            }

            ++it;
        }
    }
    else
    {
        setSelectionMode(QAbstractItemView::ExtendedSelection);
    }

    // ---

    connect(ApplicationSettings::instance(), SIGNAL(setupChanged()),
            this, SLOT(slotSettingsChanged()));

    connect(this, &LabelsTreeView::signalColorNameChanged,
            TagsActionMngr::defaultManager(), &TagsActionMngr::slotColorNameChanged);

    connect(ThemeManager::instance(), &ThemeManager::signalThemeChanged,
            this, [this]()
        {
            initTreeView();
        }
    );
}

LabelsTreeView::~LabelsTreeView()
{
    delete d;
}

bool LabelsTreeView::isCheckable() const
{
    return d->isCheckableTreeView;
}

bool LabelsTreeView::isLoadingState() const
{
    return d->isLoadingState;
}


QTreeWidgetItem* LabelsTreeView::getOrCreateItem(QTreeWidgetItem* const parent)
{
    QTreeWidgetItem* item = nullptr;

    if (*d->itemIterator)
    {
        item = (*d->itemIterator);
    }
    else
    {
        if (parent)
        {
            item = new QTreeWidgetItem(parent);
        }
        else
        {
            item = new QTreeWidgetItem(this);
        }
    }

    ++d->itemIterator;

    return item;
}

QPixmap LabelsTreeView::goldenStarPixmap(bool fillin) const
{
    QPixmap pixmap = QPixmap(60, 60);
    pixmap.fill(Qt::transparent);

    QPainter p1(&pixmap);
    p1.setRenderHint(QPainter::Antialiasing, true);

    if (fillin)
    {
        QPen pen(palette().color(QPalette::Active, foregroundRole()));
        p1.setBrush(qApp->palette().color(QPalette::Link));
        p1.setPen(pen);
    }
    else
    {
        QPen pen(qApp->palette().color(QPalette::Active, QPalette::ButtonText));
        p1.setPen(pen);
    }

    QTransform transform;
    transform.scale(4, 4);     // 60px/15px (RatingWidget::starPolygon() size is 15*15px)
    p1.setTransform(transform);

    p1.drawPolygon(RatingWidget::starPolygon(), Qt::WindingFill);
    p1.end();

    return pixmap;
}

QPixmap LabelsTreeView::colorRectPixmap(const QColor& color) const
{
    QRect rect(8, 8, 48, 48);
    QPixmap pixmap = QPixmap(60, 60);
    pixmap.fill(Qt::transparent);

    QPainter p1(&pixmap);
    p1.setRenderHint(QPainter::Antialiasing, true);
    p1.setBrush(color);
    p1.setPen(palette().color(QPalette::Active, foregroundRole()));
    p1.drawRect(rect);
    p1.end();

    return pixmap;
}

QHash<LabelsTreeView::Labels, QList<int> > LabelsTreeView::selectedLabels()
{
    QHash<Labels, QList<int> > selectedLabelsHash;
    QList<int> selectedRatings;
    QList<int> selectedPicks;
    QList<int> selectedColors;

    if (d->isCheckableTreeView)
    {
        QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::Checked);

        while (*it)
        {
            QTreeWidgetItem* const item = (*it);

            if      (item->parent() == d->ratings)
            {
                selectedRatings << indexFromItem(item).row();
            }
            else if (item->parent() == d->picks)
            {
                selectedPicks << indexFromItem(item).row();
            }
            else
            {
                selectedColors << indexFromItem(item).row();
            }

            ++it;
        }
    }
    else
    {
        const auto items = selectedItems();

        for (QTreeWidgetItem* const item : items)
        {
            if      (item->parent() == d->ratings)
            {
                selectedRatings << indexFromItem(item).row();
            }
            else if (item->parent() == d->picks)
            {
                selectedPicks << indexFromItem(item).row();
            }
            else
            {
                selectedColors << indexFromItem(item).row();
            }
        }
    }

    selectedLabelsHash[Ratings] = selectedRatings;
    selectedLabelsHash[Picks]   = selectedPicks;
    selectedLabelsHash[Colors]  = selectedColors;

    return selectedLabelsHash;
}

void LabelsTreeView::doLoadState()
{
    d->isLoadingState                = true;
    KConfigGroup configGroup         = getConfigGroup();
    const QList<int> expansion       = configGroup.readEntry(entryName(d->configExpansionEntry),       QList<int>());
    const QList<int> selectedRatings = configGroup.readEntry(entryName(d->configRatingSelectionEntry), QList<int>());
    const QList<int> selectedPicks   = configGroup.readEntry(entryName(d->configPickSelectionEntry),   QList<int>());
    const QList<int> selectedColors  = configGroup.readEntry(entryName(d->configColorSelectionEntry),  QList<int>());

    d->ratings->setExpanded(true);
    d->picks->setExpanded(true);
    d->colors->setExpanded(true);

    for (int parent : std::as_const(expansion))
    {
        switch (parent)
        {
            case 1:
            {
                d->ratings->setExpanded(false);
                break;
            }

            case 2:
            {
                d->picks->setExpanded(false);
                break;
            }

            case 3:
            {
                d->colors->setExpanded(false);
            }

            default:
            {
                break;
            }
        }
    }

    for (int rating : std::as_const(selectedRatings))
    {
        if (d->isCheckableTreeView)
        {
            d->ratings->child(rating)->setCheckState(0, Qt::Checked);
        }
        else
        {
            d->ratings->child(rating)->setSelected(true);
        }
    }

    for (int pick : std::as_const(selectedPicks))
    {
        if (d->isCheckableTreeView)
        {
            d->picks->child(pick)->setCheckState(0, Qt::Checked);
        }
        else
        {
            d->picks->child(pick)->setSelected(true);
        }
    }

    for (int color : std::as_const(selectedColors))
    {
        if (d->isCheckableTreeView)
        {
            d->colors->child(color)->setCheckState(0, Qt::Checked);
        }
        else
        {
            d->colors->child(color)->setSelected(true);
        }
    }

    d->isLoadingState = false;
}

void LabelsTreeView::doSaveState()
{
    KConfigGroup configGroup = getConfigGroup();
    QList<int> expansion;

    if (!d->ratings->isExpanded())
    {
        expansion << 1;
    }

    if (!d->picks->isExpanded())
    {
        expansion << 2;
    }

    if (!d->colors->isExpanded())
    {
        expansion << 3;
    }

    QHash<Labels, QList<int> > labels = selectedLabels();

    configGroup.writeEntry(entryName(d->configExpansionEntry),       expansion);
    configGroup.writeEntry(entryName(d->configRatingSelectionEntry), labels[Ratings]);
    configGroup.writeEntry(entryName(d->configPickSelectionEntry),   labels[Picks]);
    configGroup.writeEntry(entryName(d->configColorSelectionEntry),  labels[Colors]);
}

void LabelsTreeView::setCurrentAlbum()
{
    Q_EMIT signalSetCurrentAlbum();
}

void LabelsTreeView::initTreeView()
{
    d->itemIterator = QTreeWidgetItemIterator(this);

    setIconSize(QSize(d->iconSizeFromSetting * 5,
                      d->iconSizeFromSetting));

    initRatingsTree();
    initPicksTree();
    initColorsTree();
    expandAll();
    setRootIsDecorated(false);
}

void LabelsTreeView::initRatingsTree()
{
    d->ratings = getOrCreateItem(nullptr);
    d->ratings->setText(0, i18nc("@item: rating tree", "Rating"));
    d->ratings->setFont(0, d->regularFont);
    d->ratings->setFlags(Qt::ItemIsEnabled);

    QTreeWidgetItem* const noRate = getOrCreateItem(d->ratings);
    noRate->setText(0, i18nc("@item: rating tree", "No Rating"));
    noRate->setFont(0, d->regularFont);
    QPixmap pix2(goldenStarPixmap().size());
    pix2.fill(Qt::transparent);
    QPainter p2(&pix2);
    p2.setRenderHint(QPainter::Antialiasing, true);
    p2.setPen(palette().color(QPalette::Active, foregroundRole()));
    p2.drawPixmap(0, 0, goldenStarPixmap(false));
    noRate->setIcon(0, QIcon(pix2));
    noRate->setSizeHint(0, d->iconSize);

    for (int rate = 1 ; rate <= 5 ; ++rate)
    {
        QTreeWidgetItem* const rateWidget = getOrCreateItem(d->ratings);

        QPixmap pix(goldenStarPixmap().width() * rate, goldenStarPixmap().height());
        pix.fill(Qt::transparent);
        QPainter p(&pix);
        int offset = 0;
        p.setRenderHint(QPainter::Antialiasing, true);
        p.setPen(palette().color(QPalette::Active, foregroundRole()));

        for (int i = 0 ; i < rate ; ++i)
        {
            p.drawPixmap(offset, 0, goldenStarPixmap());
            offset += goldenStarPixmap().width();
        }

        rateWidget->setIcon(0, QIcon(pix));
        rateWidget->setSizeHint(0, d->iconSize);
    }
}

void LabelsTreeView::initPicksTree()
{
    d->picks = getOrCreateItem(nullptr);
    d->picks->setText(0, i18nc("@title: pick tree", "Pick"));
    d->picks->setFont(0, d->regularFont);
    d->picks->setFlags(Qt::ItemIsEnabled);

    QStringList pickSetNames;
    pickSetNames << i18nc("@item: pick tree", "No Pick")
                 << i18nc("@item: pick tree", "Rejected Item")
                 << i18nc("@item: pick tree", "Pending Item")
                 << i18nc("@item: pick tree", "Accepted Item");

    QStringList pickSetIcons;
    pickSetIcons << QLatin1String("flag-black")
                 << QLatin1String("flag-red")
                 << QLatin1String("flag-yellow")
                 << QLatin1String("flag-green");

    for (const QString& pick : std::as_const(pickSetNames))
    {
        QTreeWidgetItem* const pickWidgetItem = getOrCreateItem(d->picks);
        pickWidgetItem->setText(0, pick);
        pickWidgetItem->setFont(0, d->regularFont);
        pickWidgetItem->setIcon(0, QIcon(QIcon::fromTheme(pickSetIcons.at(pickSetNames.indexOf(pick))).pixmap(64, 64)));
    }
}

void LabelsTreeView::initColorsTree()
{
    QMap<int, QString> map = TagsActionMngr::defaultManager()->colorLabelNames();
    d->colors              = getOrCreateItem(nullptr);
    d->colors->setText(0, i18nc("@item: color tree", "Color"));
    d->colors->setFont(0, d->regularFont);
    d->colors->setFlags(Qt::ItemIsEnabled);

    ColorLabelDelegate* const delegate = new ColorLabelDelegate(32, this);

    for (int label : map.keys())
    {
        QTreeWidgetItem* const colorWidgetItem = getOrCreateItem(d->colors);
        colorWidgetItem->setData(0, Qt::UserRole, label);
        colorWidgetItem->setFont(0, d->regularFont);
        colorWidgetItem->setText(0, map.value(label));

        setItemDelegateForRow(indexFromItem(colorWidgetItem).row(), delegate);

        if (label == NoColorLabel)
        {
            colorWidgetItem->setIcon(0, QIcon(QIcon::fromTheme(QLatin1String("emblem-unmounted")).pixmap(64, 64)));
        }
        else
        {
            QPixmap colorIcon = colorRectPixmap(QColor(TagsActionMngr::colorSet().value(label)));
            colorWidgetItem->setIcon(0, QIcon(colorIcon));
            colorWidgetItem->setSizeHint(0, d->iconSize);
        }

        if (!d->isCheckableTreeView)
        {
            colorWidgetItem->setFlags(colorWidgetItem->flags() | Qt::ItemIsEditable);
        }
    }

    connect(this, &QTreeWidget::itemChanged,
            [this](QTreeWidgetItem* changedItem, int column)
        {
            if (changedItem && (changedItem->parent() == d->colors) && (column == 0))
            {
                Q_EMIT signalColorNameChanged(changedItem->data(
                                                                0,
                                                                Qt::UserRole).toInt(),
                                                                changedItem->text(0)
                                                               );
            }
        }
    );
}

void LabelsTreeView::slotSettingsChanged()
{
    if (d->iconSizeFromSetting != ApplicationSettings::instance()->getTreeViewIconSize())
    {
        d->iconSizeFromSetting = ApplicationSettings::instance()->getTreeViewIconSize();
        setIconSize(QSize(d->iconSizeFromSetting * 5, d->iconSizeFromSetting));
        d->iconSize            = QSize(d->iconSizeFromSetting, d->iconSizeFromSetting);
        QTreeWidgetItemIterator it(this);

        while (*it)
        {
            (*it)->setSizeHint(0, d->iconSize);
            ++it;
        }
    }

    if (d->regularFont != ApplicationSettings::instance()->getTreeViewFont())
    {
        d->regularFont = ApplicationSettings::instance()->getTreeViewFont();
        QTreeWidgetItemIterator it(this);

        while (*it)
        {
            (*it)->setFont(0, d->regularFont);
            ++it;
        }
    }
}

void LabelsTreeView::restoreSelectionFromHistory(QHash<Labels, QList<int> > neededLabels)
{
    QTreeWidgetItemIterator it(this, QTreeWidgetItemIterator::Selected);

    while (*it)
    {
        (*it)->setSelected(false);
        ++it;
    }

    for (int rateItemIndex : std::as_const(neededLabels[Ratings]))
    {
        d->ratings->child(rateItemIndex)->setSelected(true);
    }

    for (int pickItemIndex : std::as_const(neededLabels[Picks]))
    {
        d->picks->child(pickItemIndex)->setSelected(true);
    }

    for (int colorItemIndex : std::as_const(neededLabels[Colors]))
    {
        d->colors->child(colorItemIndex)->setSelected(true);
    }
}

} // namespace Digikam

#include "moc_labelstreeview.cpp"
#include "labelstreeview.moc"
