/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-05-12
 * Description : A widget to apply Reverse Geocoding
 *
 * SPDX-FileCopyrightText: 2010 by Michael G. Hansen <mike at mghansen dot de>
 * SPDX-FileCopyrightText: 2010 by Gabriel Voicu <ping dot gabi at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "rgwidget.h"

// Qt includes

#include <QCheckBox>
#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QMap>
#include <QPointer>
#include <QPushButton>
#include <QTreeView>
#include <QVBoxLayout>
#include <QMenu>
#include <QUrl>
#include <QInputDialog>
#include <QAction>
#include <QComboBox>
#include <QApplication>
#include <QMessageBox>

// KDE includes

#include <kconfiggroup.h>
#include <klocalizedstring.h>

// local includes

#include "geoifacetypes.h"
#include "dlayoutbox.h"
#include "gpsundocommand.h"
#include "gpsitemmodel.h"
#include "gpsitemcontainer.h"
#include "backend-geonames-rg.h"
#include "backend-osm-rg.h"
#include "backend-geonamesUS-rg.h"
#include "parsetagstring.h"
#include "rgtagmodel.h"
#include "simpletreemodel.h"
#include "dmessagebox.h"
#include "dexpanderbox.h"
#include "dmetadata.h"
#include "digikam_debug.h"

#ifdef GPSSYNC_MODELTEST
#   include <modeltest.h>
#endif // GPSSYNC_MODELTEST

namespace Digikam
{

/**
 * @class RGWidget
 *
 * @brief The RGWidget class represents the main widget for reverse geocoding.
 */

class Q_DECL_HIDDEN RGWidget::Private
{
public:

    explicit Private()
        : currentlyAskingCancelQuestion     (false),
          hideOptions                       (true),
          UIEnabled                         (true),
          label                             (nullptr),
          imageModel                        (nullptr),
          selectionModel                    (nullptr),
          buttonRGSelected                  (nullptr),
          undoCommand                       (nullptr),
          serviceComboBox                   (nullptr),
          languageEdit                      (nullptr),
          currentBackend                    (nullptr),
          requestedRGCount                  (0),
          receivedRGCount                   (0),
          buttonHideOptions                 (nullptr),
          tagsLoc                           (nullptr),
          metaLoc                           (nullptr),
          UGridContainer                    (nullptr),
          LGridContainer                    (nullptr),
          serviceLabel                      (nullptr),
          languageLabel                     (nullptr),
          separator                         (nullptr),
          tagModel                          (nullptr),
          tagTreeView                       (nullptr),
          tagSelectionModel                 (nullptr),
          actionAddCountry                  (nullptr),
          actionAddState                    (nullptr),
          actionAddStateDistrict            (nullptr),
          actionAddCounty                   (nullptr),
          actionAddCity                     (nullptr),
          actionAddCityDistrict             (nullptr),
          actionAddSuburb                   (nullptr),
          actionAddTown                     (nullptr),
          actionAddVillage                  (nullptr),
          actionAddHamlet                   (nullptr),
          actionAddStreet                   (nullptr),
          actionAddHouseNumber              (nullptr),
          actionAddPlace                    (nullptr),
          actionAddLAU2                     (nullptr),
          actionAddLAU1                     (nullptr),
          actionAddCustomizedSpacer         (nullptr),
          actionRemoveTag                   (nullptr),
          actionRemoveAllSpacers            (nullptr),
          actionAddAllAddressElementsToTag  (nullptr)
    {
    }

    bool                   currentlyAskingCancelQuestion;
    bool                   hideOptions;
    bool                   UIEnabled;
    QLabel*                label;
    GPSItemModel*          imageModel;
    QItemSelectionModel*   selectionModel;
    QPushButton*           buttonRGSelected;

    GPSUndoCommand*        undoCommand;
    QModelIndex            currentTagTreeIndex;

    QComboBox*             serviceComboBox;
    QComboBox*             languageEdit;
    QList<RGInfo>          photoList;
    QList<RGBackend*>      backendRGList;
    RGBackend*             currentBackend;
    int                    requestedRGCount;
    int                    receivedRGCount;
    QPushButton*           buttonHideOptions;
    QCheckBox*             tagsLoc;
    QCheckBox*             metaLoc;
    QWidget*               UGridContainer;
    QWidget*               LGridContainer;
    QLabel*                serviceLabel;
    QLabel*                languageLabel;
    DLineWidget*           separator;

    RGTagModel*            tagModel;
    QTreeView*             tagTreeView;

    QItemSelectionModel*   tagSelectionModel;
    QAction*               actionAddCountry;
    QAction*               actionAddState;
    QAction*               actionAddStateDistrict;
    QAction*               actionAddCounty;
    QAction*               actionAddCity;
    QAction*               actionAddCityDistrict;
    QAction*               actionAddSuburb;
    QAction*               actionAddTown;
    QAction*               actionAddVillage;
    QAction*               actionAddHamlet;
    QAction*               actionAddStreet;
    QAction*               actionAddHouseNumber;
    QAction*               actionAddPlace;
    QAction*               actionAddLAU2;
    QAction*               actionAddLAU1;
    QAction*               actionAddCustomizedSpacer;
    QAction*               actionRemoveTag;
    QAction*               actionRemoveAllSpacers;
    QAction*               actionAddAllAddressElementsToTag;

    QMap<QString, QString> countryCodes;
};

/**
 * Constructor
 * @param imageModel image model
 * @param selectionModel image selection model
 * @param parent The parent object
 */
RGWidget::RGWidget(GPSItemModel* const imageModel, QItemSelectionModel* const selectionModel,
                   QAbstractItemModel* externTagModel, QWidget* const parent)
    : QWidget(parent),
      d      (new Private())
{
    d->imageModel     = imageModel;
    d->selectionModel = selectionModel;

    // we need to have a main layout and add QVBoxLayout

    QVBoxLayout* const vBoxLayout = new QVBoxLayout(this);
    d->UGridContainer             = new QWidget(this);

    vBoxLayout->addWidget(d->UGridContainer);
    d->tagTreeView                = new QTreeView(this);
    d->tagTreeView->setHeaderHidden(true);
    vBoxLayout->addWidget(d->tagTreeView);

    Q_ASSERT(d->tagTreeView != nullptr);

    if (!externTagModel)
    {
        externTagModel = new SimpleTreeModel(1, this);
    }

    d->tagModel = new RGTagModel(externTagModel, this);
    d->tagTreeView->setModel(d->tagModel);

#ifdef GPSSYNC_MODELTEST

    new ModelTest(externTagModel, d->tagTreeView);
    new ModelTest(d->tagModel, d->tagTreeView);

#endif // GPSSYNC_MODELTEST

    d->tagSelectionModel         = new QItemSelectionModel(d->tagModel);
    d->tagTreeView->setSelectionModel(d->tagSelectionModel);

    d->actionAddCountry          = new QAction(i18n("Add country tag"), this);
    d->actionAddCountry->setData(QLatin1String("{Country}"));
    d->actionAddState            = new QAction(i18n("Add state tag"), this);
    d->actionAddState->setData(QLatin1String("{State}"));
    d->actionAddStateDistrict    = new QAction(i18n("Add state district tag"), this);
    d->actionAddStateDistrict->setData(QLatin1String("{State district}"));
    d->actionAddCounty           = new QAction(i18n("Add county tag"), this);
    d->actionAddCounty->setData(QLatin1String("{County}"));
    d->actionAddCity             = new QAction(i18n("Add city tag"), this);
    d->actionAddCity->setData(QLatin1String("{City}"));
    d->actionAddCityDistrict     = new QAction(i18n("Add city district tag"), this);
    d->actionAddCityDistrict->setData(QLatin1String("{City district}"));
    d->actionAddSuburb           = new QAction(i18n("Add suburb tag"), this);
    d->actionAddSuburb->setData(QLatin1String("{Suburb}"));
    d->actionAddTown             = new QAction(i18n("Add town tag"), this);
    d->actionAddTown->setData(QLatin1String("{Town}"));
    d->actionAddVillage          = new QAction(i18n("Add village tag"), this);
    d->actionAddVillage->setData(QLatin1String("{Village}"));
    d->actionAddHamlet           = new QAction(i18n("Add hamlet tag"), this);
    d->actionAddHamlet->setData(QLatin1String("{Hamlet}"));
    d->actionAddStreet           = new QAction(i18n("Add street"), this);
    d->actionAddStreet->setData(QLatin1String("{Street}"));
    d->actionAddHouseNumber      = new QAction(i18n("Add house number tag"), this);
    d->actionAddHouseNumber->setData(QLatin1String("{House number}"));
    d->actionAddPlace            = new QAction(i18n("Add place"), this);
    d->actionAddPlace->setData(QLatin1String("{Place}"));
    d->actionAddLAU2             = new QAction(i18n("Add Local Administrative Area 2"), this);
    d->actionAddLAU2->setData(QLatin1String("{LAU2}"));
    d->actionAddLAU1             = new QAction(i18n("Add Local Administrative Area 1"), this);
    d->actionAddLAU1->setData(QLatin1String("{LAU1}"));
    d->actionAddCustomizedSpacer = new QAction(i18n("Add new tag"), this);
    d->actionRemoveTag           = new QAction(i18n("Remove selected tag"), this);
    d->actionRemoveAllSpacers    = new QAction(i18n("Remove all control tags below this tag"), this);
    d->actionRemoveAllSpacers->setData(QLatin1String("Remove all spacers"));

    d->actionAddAllAddressElementsToTag = new QAction(i18n("Add all address elements"), this);
    QGridLayout* const gridLayout       = new QGridLayout(d->UGridContainer);

    d->languageLabel = new QLabel(i18n("Select language:"), d->UGridContainer);
    d->languageEdit  = new QComboBox(d->UGridContainer);

    DMetadata::CountryCodeMap map = DMetadata::countryCodeMap();

    for (DMetadata::CountryCodeMap::Iterator it = map.begin() ; it != map.end() ; ++it)
    {
        d->languageEdit->addItem(QString::fromUtf8("%1 - %2").arg(it.key()).arg(it.value()), it.key().toLower());
    }

    d->serviceLabel    = new QLabel(i18n("Select service:"), d->UGridContainer);
    d->serviceComboBox = new QComboBox(d->UGridContainer);

    d->serviceComboBox->addItem(i18n("Open Street Map"));
    d->serviceComboBox->addItem(i18n("Geonames.org place name (non-US)"));
    d->serviceComboBox->addItem(i18n("Geonames.org full address (US only)"));

    gridLayout->addWidget(d->serviceLabel,    0, 0, 1, 2);
    gridLayout->addWidget(d->serviceComboBox, 1, 0, 1, 2);
    gridLayout->addWidget(d->languageLabel,   2, 0, 1, 1);
    gridLayout->addWidget(d->languageEdit,    2, 1, 1, 1);

    d->UGridContainer->setLayout(gridLayout);

    d->separator         = new DLineWidget(Qt::Horizontal, this);
    vBoxLayout->addWidget(d->separator);

    d->buttonHideOptions = new QPushButton(i18n("Less options"), this);
    vBoxLayout->addWidget(d->buttonHideOptions);

    d->LGridContainer              = new QWidget(this);
    vBoxLayout->addWidget(d->LGridContainer);
    QGridLayout* const LGridLayout = new QGridLayout(d->LGridContainer);

    d->tagsLoc = new QCheckBox(i18n("Write locations as tags"), d->LGridContainer);
    d->metaLoc = new QCheckBox(i18n("Write locations metadata"), d->LGridContainer);

    LGridLayout->addWidget(d->tagsLoc, 0, 0, 1, 3);
    LGridLayout->addWidget(d->metaLoc, 1, 0, 1, 3);

    d->LGridContainer->setLayout(LGridLayout);

    d->buttonRGSelected = new QPushButton(i18n("Process reverse geocoding"), this);
    vBoxLayout->addWidget(d->buttonRGSelected);

    d->backendRGList.append(new BackendOsmRG(this));
    d->backendRGList.append(new BackendGeonamesRG(this));
    d->backendRGList.append(new BackendGeonamesUSRG(this));

    d->tagTreeView->installEventFilter(this);

    createContryCodesMap();

    updateUIState();

    connect(d->buttonRGSelected, SIGNAL(clicked()),
            this, SLOT(slotButtonRGSelected()));

    connect(d->buttonHideOptions, SIGNAL(clicked()),
            this, SLOT(slotHideOptions()));

    connect(d->selectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(updateUIState()));

    connect(d->actionAddCountry, SIGNAL(triggered(bool)),
            this, SLOT(slotAddSingleSpacer()));

    connect(d->actionAddState, SIGNAL(triggered(bool)),
            this, SLOT(slotAddSingleSpacer()));

    connect(d->actionAddStateDistrict, SIGNAL(triggered(bool)),
            this, SLOT(slotAddSingleSpacer()));

    connect(d->actionAddCounty, SIGNAL(triggered(bool)),
            this, SLOT(slotAddSingleSpacer()));

    connect(d->actionAddCity, SIGNAL(triggered(bool)),
            this, SLOT(slotAddSingleSpacer()));

    connect(d->actionAddCityDistrict, SIGNAL(triggered(bool)),
            this, SLOT(slotAddSingleSpacer()));

    connect(d->actionAddSuburb, SIGNAL(triggered(bool)),
            this, SLOT(slotAddSingleSpacer()));

    connect(d->actionAddTown, SIGNAL(triggered(bool)),
            this, SLOT(slotAddSingleSpacer()));

    connect(d->actionAddVillage, SIGNAL(triggered(bool)),
            this, SLOT(slotAddSingleSpacer()));

    connect(d->actionAddHamlet, SIGNAL(triggered(bool)),
            this, SLOT(slotAddSingleSpacer()));

    connect(d->actionAddHouseNumber, SIGNAL(triggered(bool)),
            this, SLOT(slotAddSingleSpacer()));

    connect(d->actionAddStreet, SIGNAL(triggered(bool)),
            this, SLOT(slotAddSingleSpacer()));

    connect(d->actionAddPlace, SIGNAL(triggered(bool)),
            this, SLOT(slotAddSingleSpacer()));

    connect(d->actionAddLAU2, SIGNAL(triggered(bool)),
            this, SLOT(slotAddSingleSpacer()));

    connect(d->actionAddLAU1, SIGNAL(triggered(bool)),
            this, SLOT(slotAddSingleSpacer()));

    connect(d->actionAddCustomizedSpacer, SIGNAL(triggered(bool)),
            this, SLOT(slotAddCustomizedSpacer()));

    connect(d->actionAddAllAddressElementsToTag, SIGNAL(triggered(bool)),
            this, SLOT(slotAddAllAddressElementsToTag()));

    connect(d->imageModel, SIGNAL(dataChanged(QModelIndex,QModelIndex)),
            this, SLOT(slotRegenerateNewTags()));

    connect(d->actionRemoveTag, SIGNAL(triggered(bool)),
            this, SLOT(slotRemoveTag()));

    connect(d->actionRemoveAllSpacers, SIGNAL(triggered(bool)),
            this, SLOT(slotRemoveAllSpacers()));

    for (int i = 0 ; i < d->backendRGList.count() ; ++i)
    {
        connect(d->backendRGList[i], SIGNAL(signalRGReady(QList<RGInfo>&)),
                this, SLOT(slotRGReady(QList<RGInfo>&)));
    }

    int currentServiceIndex = d->serviceComboBox->currentIndex();
    d->currentBackend       = d->backendRGList[currentServiceIndex];
}

/**
 * Destructor
 */
RGWidget::~RGWidget()
{
    delete d;
}

/**
 * Enables or disables the containing widgets.
 */
void RGWidget::updateUIState()
{
    const bool haveSelection = d->selectionModel->hasSelection();

    d->buttonRGSelected->setEnabled(d->UIEnabled && haveSelection);
    d->serviceLabel->setEnabled(d->UIEnabled);
    d->serviceComboBox->setEnabled(d->UIEnabled);
    d->languageLabel->setEnabled(d->UIEnabled);
    d->languageEdit->setEnabled(d->UIEnabled);
    d->buttonHideOptions->setEnabled(d->UIEnabled);
    d->tagsLoc->setEnabled(d->UIEnabled);
    d->metaLoc->setEnabled(d->UIEnabled);
}

/**
 * This slot triggers when the button that start the reverse geocoding process is pressed.
 */
void RGWidget::slotButtonRGSelected()
{
    // get the selected images

    const QModelIndexList selectedItems = d->selectionModel->selectedRows();
    int currentServiceIndex             = d->serviceComboBox->currentIndex();
    d->currentBackend                   = d->backendRGList[currentServiceIndex];
    d->undoCommand                      = new GPSUndoCommand();
    d->undoCommand->setText(i18n("Image tags are changed."));

    QList<RGInfo> photoList;
    QString wantedLanguage                 = d->languageEdit->itemData(d->languageEdit->currentIndex()).toString();
    QList<QList<TagData> > returnedSpacers = d->tagModel->getSpacers();

    for (int i = 0 ; i < selectedItems.count() ; ++i)
    {
        const QPersistentModelIndex itemIndex = selectedItems.at(i);
        GPSItemContainer* const selectedItem  = d->imageModel->itemFromIndex(itemIndex);
        const GPSDataContainer gpsData        = selectedItem->gpsData();

        if (!gpsData.hasCoordinates())
        {
            continue;
        }

        const qreal latitude  = gpsData.getCoordinates().lat();
        const qreal longitude = gpsData.getCoordinates().lon();

        RGInfo photoObj;
        photoObj.id           = itemIndex;
        photoObj.coordinates  = GeoCoordinates(latitude, longitude);

        photoList << photoObj;

        selectedItem->writeTagsToXmp(d->tagsLoc->isChecked());
        selectedItem->writeLocations(d->metaLoc->isChecked());
    }

    if (!photoList.isEmpty())
    {
        d->receivedRGCount  = 0;
        d->requestedRGCount = photoList.count();

        Q_EMIT signalSetUIEnabled(false, this, QString::fromUtf8(SLOT(slotRGCanceled())));
        Q_EMIT signalProgressSetup(d->requestedRGCount, i18n("Retrieving RG info -"));

        d->currentBackend->callRGBackend(photoList, wantedLanguage);
    }
}

/**
 * Hide or shows the extra options.
 */
void RGWidget::slotHideOptions()
{
    if (d->hideOptions)
    {
        d->LGridContainer->hide();
        d->hideOptions = false;
        d->buttonHideOptions->setText(i18n("More options"));
    }
    else
    {
        d->LGridContainer->show();
        d->hideOptions = true;
        d->buttonHideOptions->setText(i18n("Less options"));
    }
}

/**
 * The data has returned from backend and now it's processed here.
 * @param returnedRGList Contains the data returned by backend.
 */
void RGWidget::slotRGReady(QList<RGInfo>& returnedRGList)
{
    const QString errorString = d->currentBackend->getErrorMessage();

    if (!errorString.isEmpty())
    {
        /// @todo This collides with the message box displayed if the user aborts the RG process

        QMessageBox::critical(this, qApp->applicationName(), errorString);

        d->receivedRGCount+=returnedRGList.count();

        Q_EMIT signalSetUIEnabled(true);

        return;
    }

    QString address;

    for (int i = 0 ; i < returnedRGList.count() ; ++i)
    {
        QPersistentModelIndex currentImageIndex = returnedRGList[i].id;

        if (!returnedRGList[i].rgData.isEmpty())
        {
            QString addressElementsWantedFormat;

            if      (d->currentBackend->backendName() == QLatin1String("Geonames"))
            {
                addressElementsWantedFormat.append(QLatin1String("/{Country}/{Country code}/{Place}"));
            }
            else if (d->currentBackend->backendName() == QLatin1String("GeonamesUS"))
            {
                addressElementsWantedFormat.append(QLatin1String("/{LAU2}/{LAU1}/{City}"));
            }
            else
            {
                addressElementsWantedFormat.append(QLatin1String("/{Country}/{Country code}/{State}/{State district}"
                                                                 "/{County}/{City}/{City district}/{Suburb}/{Town}"
                                                                 "/{Village}/{Hamlet}/{Street}/{House number}"));
            }

            QStringList combinedResult = makeTagString(returnedRGList[i],
                                                       addressElementsWantedFormat,
                                                       d->currentBackend->backendName());

            QString addressFormat      = combinedResult[0];
            QString addressElements    = combinedResult[1];

            // removes first "/" from tag addresses

            addressFormat.remove(0, 1);
            addressElements.remove(0, 1);
            addressElementsWantedFormat.remove(0, 1);

            const QStringList listAddressElementsWantedFormat = addressElementsWantedFormat.split(QLatin1Char('/'));
            const QStringList listAddressElements             = addressElements.split(QLatin1Char('/'));
            const QStringList listAddressFormat               = addressFormat.split(QLatin1Char('/'));
            QStringList elements, resultedData;

            for (int j = 0 ; j < listAddressElementsWantedFormat.count() ; ++j)
            {
                QString currentAddressFormat = listAddressElementsWantedFormat.at(j);
                int currentIndexFormat       = listAddressFormat.indexOf(currentAddressFormat);

                if (currentIndexFormat != -1)
                {
                    elements << currentAddressFormat;
                    resultedData << listAddressElements.at(currentIndexFormat);
                }
            }

            QList<QList<TagData> > returnedTags = d->tagModel->addNewData(elements, resultedData);

            int countryCodeIndex = listAddressFormat.indexOf(QLatin1String("{Country code}"));

            if (countryCodeIndex != -1)
            {
                TagData countryCode;
                countryCode.tagName = d->countryCodes.value(listAddressElements.at(countryCodeIndex).toUpper());
                countryCode.tipName = QLatin1String("{Country code}");
                returnedTags << (QList<TagData>() << countryCode);
            }

            GPSItemContainer* const currentItem = d->imageModel->itemFromIndex(currentImageIndex);

            GPSUndoCommand::UndoInfo undoInfo(currentImageIndex);
            undoInfo.readOldDataFromItem(currentItem);

            currentItem->setTagList(returnedTags);

            undoInfo.readNewDataFromItem(currentItem);
            d->undoCommand->addUndoInfo(undoInfo);
        }
    }

    d->receivedRGCount += returnedRGList.count();

    if (d->receivedRGCount >= d->requestedRGCount)
    {
        if (d->currentlyAskingCancelQuestion)
        {
            // if the user is currently answering the cancel question, do nothing, only report progress

            Q_EMIT signalProgressChanged(d->receivedRGCount);
        }
        else
        {
            Q_EMIT signalUndoCommand(d->undoCommand);

            d->undoCommand = nullptr;

            Q_EMIT signalSetUIEnabled(true);
        }
    }
    else
    {
        Q_EMIT signalProgressChanged(d->receivedRGCount);
    }
}

/**
 * Sets whether the containing widgets are enabled or disabled.
 * @param state If true, the controls are enabled.
 */
void RGWidget::setUIEnabled(const bool state)
{
    d->UIEnabled = state;
    updateUIState();
}

/**
 * Here are filtered the events.
 */
bool RGWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == d->tagTreeView)
    {
        if ((event->type() == QEvent::ContextMenu) && d->UIEnabled)
        {
            QMenu* const menu             = new QMenu(d->tagTreeView);
            const int currentServiceIndex = d->serviceComboBox->currentIndex();
            d->currentBackend             = d->backendRGList[currentServiceIndex];
            QString backendName           = d->currentBackend->backendName();
            QContextMenuEvent* const e    = static_cast<QContextMenuEvent*>(event);
            d->currentTagTreeIndex        = d->tagTreeView->indexAt(e->pos());
            const Type tagType            = d->tagModel->getTagType(d->currentTagTreeIndex);

            if      (backendName == QLatin1String("OSM"))
            {
                menu->addAction(d->actionAddAllAddressElementsToTag);
                menu->addSeparator();
                menu->addAction(d->actionAddCountry);
                menu->addAction(d->actionAddState);
                menu->addAction(d->actionAddStateDistrict);
                menu->addAction(d->actionAddCounty);
                menu->addAction(d->actionAddCity);
                menu->addAction(d->actionAddCityDistrict);
                menu->addAction(d->actionAddSuburb);
                menu->addAction(d->actionAddTown);
                menu->addAction(d->actionAddVillage);
                menu->addAction(d->actionAddHamlet);
                menu->addAction(d->actionAddStreet);
                menu->addAction(d->actionAddHouseNumber);
            }
            else if (backendName == QLatin1String("Geonames"))
            {
                menu->addAction(d->actionAddAllAddressElementsToTag);
                menu->addAction(d->actionAddCountry);
                menu->addAction(d->actionAddPlace);
            }
            else if (backendName == QLatin1String("GeonamesUS"))
            {
                menu->addAction(d->actionAddAllAddressElementsToTag);
                menu->addAction(d->actionAddLAU2);
                menu->addAction(d->actionAddLAU1);
                menu->addAction(d->actionAddCity);
            }

            menu->addSeparator();
            menu->addAction(d->actionAddCustomizedSpacer);
            menu->addSeparator();

            if (tagType == TypeSpacer)
            {
                menu->addAction(d->actionRemoveTag);
            }

            menu->addAction(d->actionRemoveAllSpacers);
            menu->exec(e->globalPos());
            delete menu;
        }
    }

    return QObject::eventFilter(watched, event);
}

/**
 * Saves the settings of widgets contained in reverse geocoding widget.
 * @param group Here are stored the settings.
 */
void RGWidget::saveSettingsToGroup(KConfigGroup* const group)
{
    group->writeEntry("RG Backend",        d->serviceComboBox->currentIndex());
    group->writeEntry("Language",          d->languageEdit->currentIndex());
    group->writeEntry("Hide options",      d->hideOptions);
    group->writeEntry("Tags location",     d->tagsLoc->isChecked());
    group->writeEntry("Metadata location", d->metaLoc->isChecked());

    QList<QList<TagData> > currentSpacerList = d->tagModel->getSpacers();
    const int spacerCount                    = currentSpacerList.count();
    group->writeEntry("Spacers count", spacerCount);

    for (int i = 0 ; i < currentSpacerList.count() ; ++i)
    {
        QString spacerName;
        spacerName.append(QString::fromLatin1("Spacerlistname %1").arg(i));
        QString spacerType;
        spacerType.append(QString::fromLatin1("Spacerlisttype %1").arg(i));

        QStringList spacerTagNames;
        QStringList spacerTypes;

        for (int j = 0 ; j < currentSpacerList[i].count() ; ++j)
        {
            spacerTagNames.append(currentSpacerList[i].at(j).tagName);

            if      (currentSpacerList[i].at(j).tagType == TypeSpacer)
            {
                spacerTypes.append(QLatin1String("Spacer"));
            }
            else if (currentSpacerList[i].at(j).tagType == TypeNewChild)
            {
                spacerTypes.append(QLatin1String("NewChild"));
            }
            else
            {
                spacerTypes.append(QLatin1String("OldChild"));
            }
        }

        group->writeEntry(spacerName, spacerTagNames);
        group->writeEntry(spacerType, spacerTypes);
    }
}

/**
 * Restores the settings of widgets contained in reverse geocoding widget.
 * @param group Here are stored the settings.
 */
void RGWidget::readSettingsFromGroup(const KConfigGroup* const group)
{
    const int spacerCount = group->readEntry("Spacers count", 0);
    QList<QList<TagData> > spacersList;

    for (int i = 0 ; i < spacerCount ; ++i)
    {
        QStringList spacerTagNames = group->readEntry(QString::fromLatin1("Spacerlistname %1").arg(i), QStringList());
        QStringList spacerTypes    = group->readEntry(QString::fromLatin1("Spacerlisttype %1").arg(i), QStringList());
        QList<TagData> currentSpacerAddress;

        for (int j = 0 ; j < spacerTagNames.count() ; ++j)
        {
            TagData currentTagData;
            currentTagData.tagName = spacerTagNames.at(j);
            QString currentTagType = spacerTypes.at(j);

            if      (currentTagType == QLatin1String("Spacer"))
            {
                currentTagData.tagType = TypeSpacer;
            }
            else if (currentTagType == QLatin1String("NewChild"))
            {
                currentTagData.tagType = TypeNewChild;
            }
            else if (currentTagType == QLatin1String("OldChild"))
            {
                currentTagData.tagType = TypeChild;
            }
            else
            {
                qCWarning(DIGIKAM_GENERAL_LOG) << "Unknown tag type" << currentTagType;
                continue;
            }

            currentSpacerAddress.append(currentTagData);
        }

        spacersList.append(currentSpacerAddress);
    }

    // this make sure that all external tags are added to tag tree view before spacers are re-added

    d->tagModel->addAllExternalTagsToTreeView();
    d->tagModel->readdNewTags(spacersList);

    d->serviceComboBox->setCurrentIndex(group->readEntry("RG Backend", 0));
    d->languageEdit->setCurrentIndex(group->readEntry("Language",      0));

    d->hideOptions = !(group->readEntry("Hide options",                false));
    slotHideOptions();

    d->tagsLoc->setChecked(group->readEntry("Tags location",           false));
    d->metaLoc->setChecked(group->readEntry("Metadata location",       false));
}

/**
 * Adds a tag to tag tree.
 */
void RGWidget::slotAddSingleSpacer()
{
/*
    const QModelIndex baseIndex = d->tagSelectionModel->currentIndex();
*/
    QModelIndex baseIndex;

    if (!d->currentTagTreeIndex.isValid())
    {
        baseIndex = d->currentTagTreeIndex;
    }
    else
    {
        baseIndex = d->tagSelectionModel->currentIndex();
    }

    QAction* const senderAction = qobject_cast<QAction*>(sender());
    QString currentSpacerName   = senderAction->data().toString();

    d->tagModel->addSpacerTag(baseIndex, currentSpacerName);
}

/**
 * Adds a new tag to the tag tree.
 */
void RGWidget::slotAddCustomizedSpacer()
{
    QModelIndex baseIndex;

    if (!d->currentTagTreeIndex.isValid())
    {
        baseIndex = d->currentTagTreeIndex;
    }
    else
    {
        baseIndex = d->tagSelectionModel->currentIndex();
    }

    bool ok            = false;
    QString textString = QInputDialog::getText(this, i18nc("@title:window", "Add new tag:"),
                                               i18n("Select a name for the new tag:"),
                                               QLineEdit::Normal, QString(), &ok);

    if (ok && !textString.isEmpty())
    {
        d->tagModel->addSpacerTag(baseIndex, textString);
    }
}

/**
 * Removes a tag from tag tree.
 * Note: If the tag is an external, it is no more deleted.
 */
void RGWidget::slotRemoveTag()
{
    const QModelIndex baseIndex = d->tagSelectionModel->currentIndex();
    d->tagModel->deleteTag(baseIndex);
}

/**
 * Removes all spacers.
 */
void RGWidget::slotRemoveAllSpacers()
{
    QString whatShouldRemove = QLatin1String("Spacers");
    QModelIndex baseIndex;

    if (!d->currentTagTreeIndex.isValid())
    {
        baseIndex = d->currentTagTreeIndex;
    }
    else
    {
        baseIndex = d->tagSelectionModel->currentIndex();
    }

    d->tagModel->deleteAllSpacersOrNewTags(baseIndex, TypeSpacer);
}

/**
 * Re-adds all deleted tags based on Undo/Redo widget.
 */
void RGWidget::slotReaddNewTags()
{
    for (int row = 0 ; row < d->imageModel->rowCount() ; ++row)
    {
        GPSItemContainer* const currentItem = d->imageModel->itemFromIndex(d->imageModel->index(row, 0));
        QList<QList<TagData> > tagAddresses = currentItem->getTagList();

        if (!tagAddresses.isEmpty())
        {
            d->tagModel->readdNewTags(tagAddresses);
        }
    }
}

/**
 * Deletes and re-adds all new added tags.
 */
void RGWidget::slotRegenerateNewTags()
{
    QModelIndex baseIndex = QModelIndex();
    d->tagModel->deleteAllSpacersOrNewTags(baseIndex, TypeNewChild);

    slotReaddNewTags();
}

/**
 * Adds all address elements below the selected tag. The address ellements are order by area size.
 * For example: country > state > state district > city ...
 */
void RGWidget::slotAddAllAddressElementsToTag()
{
    QModelIndex baseIndex;

    if (!d->currentTagTreeIndex.isValid())
    {
        baseIndex = d->currentTagTreeIndex;
    }
    else
    {
        baseIndex = d->tagSelectionModel->currentIndex();
    }

    QStringList spacerList;

    if      (d->currentBackend->backendName() == QLatin1String("OSM"))
    {
        /// @todo Why are these wrapped in QString?

        spacerList.append(QLatin1String("{Country}"));
        spacerList.append(QLatin1String("{State}"));
        spacerList.append(QLatin1String("{State district}"));
        spacerList.append(QLatin1String("{County}"));
        spacerList.append(QLatin1String("{City}"));
        spacerList.append(QLatin1String("{City district}"));
        spacerList.append(QLatin1String("{Suburb}"));
        spacerList.append(QLatin1String("{Town}"));
        spacerList.append(QLatin1String("{Village}"));
        spacerList.append(QLatin1String("{Hamlet}"));
        spacerList.append(QLatin1String("{Street}"));
        spacerList.append(QLatin1String("{House number}"));
    }
    else if (d->currentBackend->backendName() == QLatin1String("Geonames"))
    {
        spacerList.append(QLatin1String("{Country}"));
        spacerList.append(QLatin1String("{Place}"));
    }
    else
    {
        spacerList.append(QLatin1String("{LAU1}"));
        spacerList.append(QLatin1String("{LAU2}"));
        spacerList.append(QLatin1String("{City}"));
    }

    d->tagModel->addAllSpacersToTag(baseIndex, spacerList,0);
}

void RGWidget::slotRGCanceled()
{
    if (!d->undoCommand)
    {
        // the undo command object is not available, therefore
        // RG has probably been finished already

        return;
    }

    if (d->receivedRGCount > 0)
    {
        // Before we abort, ask the user whether he wants to discard
        // the information obtained so far.

        // ATTENTION: While we ask the question, the RG backend continues running
        //            and sends information about new images to this widget.
        //            This means that RG might finish while we ask the question!!!

        d->currentlyAskingCancelQuestion = true;

        QString question;

        if (d->receivedRGCount > 1)
        {
            question = i18n("%1 out of %2 images have been reverse geocoded. "
                            "Would you like to keep the tags which were "
                            "already obtained?",
                            d->receivedRGCount, d->requestedRGCount);
        }
        else
        {
            question = i18n("1 image have been reverse geocoded. "
                            "Would you like to keep the tags which were "
                            "already obtained?");
        }

        const int result = DMessageBox::showYesNo(QMessageBox::Warning, this,
                                                  i18n("Abort reverse geocoding?"),
                                                  question);

        d->currentlyAskingCancelQuestion = false;

        if (result == QMessageBox::Cancel)
        {
            // continue

            // did RG finish while we asked the question?

            if (d->receivedRGCount==d->requestedRGCount)
            {
                // the undo data was delayed, now send it

                if (d->undoCommand)
                {
                    Q_EMIT signalUndoCommand(d->undoCommand);
                    d->undoCommand = nullptr;
                }

                // unlock the UI

                Q_EMIT signalSetUIEnabled(true);
            }

            return;
        }

        if (result == QMessageBox::No)
        {
            // discard the tags

            d->undoCommand->undo();
        }

        if (result == QMessageBox::Yes)
        {
            if (d->undoCommand)
            {
                Q_EMIT signalUndoCommand(d->undoCommand);
                d->undoCommand = nullptr;
            }
        }
    }

    // clean up the RG request:

    d->currentBackend->cancelRequests();

    if (d->undoCommand)
    {
        delete d->undoCommand;
        d->undoCommand = nullptr;
    }

    Q_EMIT signalSetUIEnabled(true);
}

void RGWidget::createContryCodesMap()
{
    d->countryCodes.insert(QLatin1String("AF"), QLatin1String("AFG"));
    d->countryCodes.insert(QLatin1String("AL"), QLatin1String("ALB"));
    d->countryCodes.insert(QLatin1String("DZ"), QLatin1String("DZA"));
    d->countryCodes.insert(QLatin1String("AS"), QLatin1String("ASM"));
    d->countryCodes.insert(QLatin1String("AD"), QLatin1String("AND"));
    d->countryCodes.insert(QLatin1String("AO"), QLatin1String("AGO"));
    d->countryCodes.insert(QLatin1String("AI"), QLatin1String("AIA"));
    d->countryCodes.insert(QLatin1String("AQ"), QLatin1String("ATA"));
    d->countryCodes.insert(QLatin1String("AG"), QLatin1String("ATG"));
    d->countryCodes.insert(QLatin1String("AR"), QLatin1String("ARG"));
    d->countryCodes.insert(QLatin1String("AM"), QLatin1String("ARM"));
    d->countryCodes.insert(QLatin1String("AW"), QLatin1String("ABW"));
    d->countryCodes.insert(QLatin1String("AU"), QLatin1String("AUS"));
    d->countryCodes.insert(QLatin1String("AT"), QLatin1String("AUT"));
    d->countryCodes.insert(QLatin1String("AZ"), QLatin1String("AZE"));
    d->countryCodes.insert(QLatin1String("BS"), QLatin1String("BHS"));
    d->countryCodes.insert(QLatin1String("BH"), QLatin1String("BHR"));
    d->countryCodes.insert(QLatin1String("BD"), QLatin1String("BGD"));
    d->countryCodes.insert(QLatin1String("BB"), QLatin1String("BRB"));
    d->countryCodes.insert(QLatin1String("BY"), QLatin1String("BLR"));
    d->countryCodes.insert(QLatin1String("BE"), QLatin1String("BEL"));
    d->countryCodes.insert(QLatin1String("BZ"), QLatin1String("BLZ"));
    d->countryCodes.insert(QLatin1String("BJ"), QLatin1String("BEN"));
    d->countryCodes.insert(QLatin1String("BM"), QLatin1String("BMU"));
    d->countryCodes.insert(QLatin1String("BT"), QLatin1String("BTN"));
    d->countryCodes.insert(QLatin1String("BO"), QLatin1String("BOL"));
    d->countryCodes.insert(QLatin1String("BQ"), QLatin1String("BES"));
    d->countryCodes.insert(QLatin1String("BA"), QLatin1String("BIH"));
    d->countryCodes.insert(QLatin1String("BW"), QLatin1String("BWA"));
    d->countryCodes.insert(QLatin1String("BV"), QLatin1String("BVT"));
    d->countryCodes.insert(QLatin1String("BR"), QLatin1String("BRA"));
    d->countryCodes.insert(QLatin1String("IO"), QLatin1String("IOT"));
    d->countryCodes.insert(QLatin1String("BN"), QLatin1String("BRN"));
    d->countryCodes.insert(QLatin1String("BG"), QLatin1String("BGR"));
    d->countryCodes.insert(QLatin1String("BF"), QLatin1String("BFA"));
    d->countryCodes.insert(QLatin1String("BI"), QLatin1String("BDI"));
    d->countryCodes.insert(QLatin1String("CV"), QLatin1String("CPV"));
    d->countryCodes.insert(QLatin1String("KH"), QLatin1String("KHM"));
    d->countryCodes.insert(QLatin1String("CM"), QLatin1String("CMR"));
    d->countryCodes.insert(QLatin1String("CA"), QLatin1String("CAN"));
    d->countryCodes.insert(QLatin1String("KY"), QLatin1String("CYM"));
    d->countryCodes.insert(QLatin1String("CF"), QLatin1String("CAF"));
    d->countryCodes.insert(QLatin1String("TD"), QLatin1String("TCD"));
    d->countryCodes.insert(QLatin1String("CL"), QLatin1String("CHL"));
    d->countryCodes.insert(QLatin1String("CN"), QLatin1String("CHN"));
    d->countryCodes.insert(QLatin1String("CX"), QLatin1String("CXR"));
    d->countryCodes.insert(QLatin1String("CC"), QLatin1String("CCK"));
    d->countryCodes.insert(QLatin1String("CO"), QLatin1String("COL"));
    d->countryCodes.insert(QLatin1String("KM"), QLatin1String("COM"));
    d->countryCodes.insert(QLatin1String("CD"), QLatin1String("COD"));
    d->countryCodes.insert(QLatin1String("CG"), QLatin1String("COG"));
    d->countryCodes.insert(QLatin1String("CK"), QLatin1String("COK"));
    d->countryCodes.insert(QLatin1String("CR"), QLatin1String("CRI"));
    d->countryCodes.insert(QLatin1String("HR"), QLatin1String("HRV"));
    d->countryCodes.insert(QLatin1String("CU"), QLatin1String("CUB"));
    d->countryCodes.insert(QLatin1String("CW"), QLatin1String("CUW"));
    d->countryCodes.insert(QLatin1String("CY"), QLatin1String("CYP"));
    d->countryCodes.insert(QLatin1String("CZ"), QLatin1String("CZE"));
    d->countryCodes.insert(QLatin1String("CI"), QLatin1String("CIV"));
    d->countryCodes.insert(QLatin1String("DK"), QLatin1String("DNK"));
    d->countryCodes.insert(QLatin1String("DJ"), QLatin1String("DJI"));
    d->countryCodes.insert(QLatin1String("DM"), QLatin1String("DMA"));
    d->countryCodes.insert(QLatin1String("DO"), QLatin1String("DOM"));
    d->countryCodes.insert(QLatin1String("EC"), QLatin1String("ECU"));
    d->countryCodes.insert(QLatin1String("EG"), QLatin1String("EGY"));
    d->countryCodes.insert(QLatin1String("SV"), QLatin1String("SLV"));
    d->countryCodes.insert(QLatin1String("GQ"), QLatin1String("GNQ"));
    d->countryCodes.insert(QLatin1String("ER"), QLatin1String("ERI"));
    d->countryCodes.insert(QLatin1String("EE"), QLatin1String("EST"));
    d->countryCodes.insert(QLatin1String("SZ"), QLatin1String("SWZ"));
    d->countryCodes.insert(QLatin1String("ET"), QLatin1String("ETH"));
    d->countryCodes.insert(QLatin1String("FK"), QLatin1String("FLK"));
    d->countryCodes.insert(QLatin1String("FO"), QLatin1String("FRO"));
    d->countryCodes.insert(QLatin1String("FJ"), QLatin1String("FJI"));
    d->countryCodes.insert(QLatin1String("FI"), QLatin1String("FIN"));
    d->countryCodes.insert(QLatin1String("FR"), QLatin1String("FRA"));
    d->countryCodes.insert(QLatin1String("GF"), QLatin1String("GUF"));
    d->countryCodes.insert(QLatin1String("PF"), QLatin1String("PYF"));
    d->countryCodes.insert(QLatin1String("TF"), QLatin1String("ATF"));
    d->countryCodes.insert(QLatin1String("GA"), QLatin1String("GAB"));
    d->countryCodes.insert(QLatin1String("GM"), QLatin1String("GMB"));
    d->countryCodes.insert(QLatin1String("GE"), QLatin1String("GEO"));
    d->countryCodes.insert(QLatin1String("DE"), QLatin1String("DEU"));
    d->countryCodes.insert(QLatin1String("GH"), QLatin1String("GHA"));
    d->countryCodes.insert(QLatin1String("GI"), QLatin1String("GIB"));
    d->countryCodes.insert(QLatin1String("GR"), QLatin1String("GRC"));
    d->countryCodes.insert(QLatin1String("GL"), QLatin1String("GRL"));
    d->countryCodes.insert(QLatin1String("GD"), QLatin1String("GRD"));
    d->countryCodes.insert(QLatin1String("GP"), QLatin1String("GLP"));
    d->countryCodes.insert(QLatin1String("GU"), QLatin1String("GUM"));
    d->countryCodes.insert(QLatin1String("GT"), QLatin1String("GTM"));
    d->countryCodes.insert(QLatin1String("GG"), QLatin1String("GGY"));
    d->countryCodes.insert(QLatin1String("GN"), QLatin1String("GIN"));
    d->countryCodes.insert(QLatin1String("GW"), QLatin1String("GNB"));
    d->countryCodes.insert(QLatin1String("GY"), QLatin1String("GUY"));
    d->countryCodes.insert(QLatin1String("HT"), QLatin1String("HTI"));
    d->countryCodes.insert(QLatin1String("HM"), QLatin1String("HMD"));
    d->countryCodes.insert(QLatin1String("VA"), QLatin1String("VAT"));
    d->countryCodes.insert(QLatin1String("HN"), QLatin1String("HND"));
    d->countryCodes.insert(QLatin1String("HK"), QLatin1String("HKG"));
    d->countryCodes.insert(QLatin1String("HU"), QLatin1String("HUN"));
    d->countryCodes.insert(QLatin1String("IS"), QLatin1String("ISL"));
    d->countryCodes.insert(QLatin1String("IN"), QLatin1String("IND"));
    d->countryCodes.insert(QLatin1String("ID"), QLatin1String("IDN"));
    d->countryCodes.insert(QLatin1String("IR"), QLatin1String("IRN"));
    d->countryCodes.insert(QLatin1String("IQ"), QLatin1String("IRQ"));
    d->countryCodes.insert(QLatin1String("IE"), QLatin1String("IRL"));
    d->countryCodes.insert(QLatin1String("IM"), QLatin1String("IMN"));
    d->countryCodes.insert(QLatin1String("IL"), QLatin1String("ISR"));
    d->countryCodes.insert(QLatin1String("IT"), QLatin1String("ITA"));
    d->countryCodes.insert(QLatin1String("JM"), QLatin1String("JAM"));
    d->countryCodes.insert(QLatin1String("JP"), QLatin1String("JPN"));
    d->countryCodes.insert(QLatin1String("JE"), QLatin1String("JEY"));
    d->countryCodes.insert(QLatin1String("JO"), QLatin1String("JOR"));
    d->countryCodes.insert(QLatin1String("KZ"), QLatin1String("KAZ"));
    d->countryCodes.insert(QLatin1String("KE"), QLatin1String("KEN"));
    d->countryCodes.insert(QLatin1String("KI"), QLatin1String("KIR"));
    d->countryCodes.insert(QLatin1String("KP"), QLatin1String("PRK"));
    d->countryCodes.insert(QLatin1String("KR"), QLatin1String("KOR"));
    d->countryCodes.insert(QLatin1String("KW"), QLatin1String("KWT"));
    d->countryCodes.insert(QLatin1String("KG"), QLatin1String("KGZ"));
    d->countryCodes.insert(QLatin1String("LA"), QLatin1String("LAO"));
    d->countryCodes.insert(QLatin1String("LV"), QLatin1String("LVA"));
    d->countryCodes.insert(QLatin1String("LB"), QLatin1String("LBN"));
    d->countryCodes.insert(QLatin1String("LS"), QLatin1String("LSO"));
    d->countryCodes.insert(QLatin1String("LR"), QLatin1String("LBR"));
    d->countryCodes.insert(QLatin1String("LY"), QLatin1String("LBY"));
    d->countryCodes.insert(QLatin1String("LI"), QLatin1String("LIE"));
    d->countryCodes.insert(QLatin1String("LT"), QLatin1String("LTU"));
    d->countryCodes.insert(QLatin1String("LU"), QLatin1String("LUX"));
    d->countryCodes.insert(QLatin1String("MO"), QLatin1String("MAC"));
    d->countryCodes.insert(QLatin1String("MG"), QLatin1String("MDG"));
    d->countryCodes.insert(QLatin1String("MW"), QLatin1String("MWI"));
    d->countryCodes.insert(QLatin1String("MY"), QLatin1String("MYS"));
    d->countryCodes.insert(QLatin1String("MV"), QLatin1String("MDV"));
    d->countryCodes.insert(QLatin1String("ML"), QLatin1String("MLI"));
    d->countryCodes.insert(QLatin1String("MT"), QLatin1String("MLT"));
    d->countryCodes.insert(QLatin1String("MH"), QLatin1String("MHL"));
    d->countryCodes.insert(QLatin1String("MQ"), QLatin1String("MTQ"));
    d->countryCodes.insert(QLatin1String("MR"), QLatin1String("MRT"));
    d->countryCodes.insert(QLatin1String("MU"), QLatin1String("MUS"));
    d->countryCodes.insert(QLatin1String("YT"), QLatin1String("MYT"));
    d->countryCodes.insert(QLatin1String("MX"), QLatin1String("MEX"));
    d->countryCodes.insert(QLatin1String("FM"), QLatin1String("FSM"));
    d->countryCodes.insert(QLatin1String("MD"), QLatin1String("MDA"));
    d->countryCodes.insert(QLatin1String("MC"), QLatin1String("MCO"));
    d->countryCodes.insert(QLatin1String("MN"), QLatin1String("MNG"));
    d->countryCodes.insert(QLatin1String("ME"), QLatin1String("MNE"));
    d->countryCodes.insert(QLatin1String("MS"), QLatin1String("MSR"));
    d->countryCodes.insert(QLatin1String("MA"), QLatin1String("MAR"));
    d->countryCodes.insert(QLatin1String("MZ"), QLatin1String("MOZ"));
    d->countryCodes.insert(QLatin1String("MM"), QLatin1String("MMR"));
    d->countryCodes.insert(QLatin1String("NA"), QLatin1String("NAM"));
    d->countryCodes.insert(QLatin1String("NR"), QLatin1String("NRU"));
    d->countryCodes.insert(QLatin1String("NP"), QLatin1String("NPL"));
    d->countryCodes.insert(QLatin1String("NL"), QLatin1String("NLD"));
    d->countryCodes.insert(QLatin1String("NC"), QLatin1String("NCL"));
    d->countryCodes.insert(QLatin1String("NZ"), QLatin1String("NZL"));
    d->countryCodes.insert(QLatin1String("NI"), QLatin1String("NIC"));
    d->countryCodes.insert(QLatin1String("NE"), QLatin1String("NER"));
    d->countryCodes.insert(QLatin1String("NG"), QLatin1String("NGA"));
    d->countryCodes.insert(QLatin1String("NU"), QLatin1String("NIU"));
    d->countryCodes.insert(QLatin1String("NF"), QLatin1String("NFK"));
    d->countryCodes.insert(QLatin1String("MP"), QLatin1String("MNP"));
    d->countryCodes.insert(QLatin1String("NO"), QLatin1String("NOR"));
    d->countryCodes.insert(QLatin1String("OM"), QLatin1String("OMN"));
    d->countryCodes.insert(QLatin1String("PK"), QLatin1String("PAK"));
    d->countryCodes.insert(QLatin1String("PW"), QLatin1String("PLW"));
    d->countryCodes.insert(QLatin1String("PS"), QLatin1String("PSE"));
    d->countryCodes.insert(QLatin1String("PA"), QLatin1String("PAN"));
    d->countryCodes.insert(QLatin1String("PG"), QLatin1String("PNG"));
    d->countryCodes.insert(QLatin1String("PY"), QLatin1String("PRY"));
    d->countryCodes.insert(QLatin1String("PE"), QLatin1String("PER"));
    d->countryCodes.insert(QLatin1String("PH"), QLatin1String("PHL"));
    d->countryCodes.insert(QLatin1String("PN"), QLatin1String("PCN"));
    d->countryCodes.insert(QLatin1String("PL"), QLatin1String("POL"));
    d->countryCodes.insert(QLatin1String("PT"), QLatin1String("PRT"));
    d->countryCodes.insert(QLatin1String("PR"), QLatin1String("PRI"));
    d->countryCodes.insert(QLatin1String("QA"), QLatin1String("QAT"));
    d->countryCodes.insert(QLatin1String("MK"), QLatin1String("MKD"));
    d->countryCodes.insert(QLatin1String("RO"), QLatin1String("ROU"));
    d->countryCodes.insert(QLatin1String("RU"), QLatin1String("RUS"));
    d->countryCodes.insert(QLatin1String("RW"), QLatin1String("RWA"));
    d->countryCodes.insert(QLatin1String("RE"), QLatin1String("REU"));
    d->countryCodes.insert(QLatin1String("BL"), QLatin1String("BLM"));
    d->countryCodes.insert(QLatin1String("SH"), QLatin1String("SHN"));
    d->countryCodes.insert(QLatin1String("KN"), QLatin1String("KNA"));
    d->countryCodes.insert(QLatin1String("LC"), QLatin1String("LCA"));
    d->countryCodes.insert(QLatin1String("MF"), QLatin1String("MAF"));
    d->countryCodes.insert(QLatin1String("PM"), QLatin1String("SPM"));
    d->countryCodes.insert(QLatin1String("VC"), QLatin1String("VCT"));
    d->countryCodes.insert(QLatin1String("WS"), QLatin1String("WSM"));
    d->countryCodes.insert(QLatin1String("SM"), QLatin1String("SMR"));
    d->countryCodes.insert(QLatin1String("ST"), QLatin1String("STP"));
    d->countryCodes.insert(QLatin1String("SA"), QLatin1String("SAU"));
    d->countryCodes.insert(QLatin1String("SN"), QLatin1String("SEN"));
    d->countryCodes.insert(QLatin1String("RS"), QLatin1String("SRB"));
    d->countryCodes.insert(QLatin1String("SC"), QLatin1String("SYC"));
    d->countryCodes.insert(QLatin1String("SL"), QLatin1String("SLE"));
    d->countryCodes.insert(QLatin1String("SG"), QLatin1String("SGP"));
    d->countryCodes.insert(QLatin1String("SX"), QLatin1String("SXM"));
    d->countryCodes.insert(QLatin1String("SK"), QLatin1String("SVK"));
    d->countryCodes.insert(QLatin1String("SI"), QLatin1String("SVN"));
    d->countryCodes.insert(QLatin1String("SB"), QLatin1String("SLB"));
    d->countryCodes.insert(QLatin1String("SO"), QLatin1String("SOM"));
    d->countryCodes.insert(QLatin1String("ZA"), QLatin1String("ZAF"));
    d->countryCodes.insert(QLatin1String("GS"), QLatin1String("SGS"));
    d->countryCodes.insert(QLatin1String("SS"), QLatin1String("SSD"));
    d->countryCodes.insert(QLatin1String("ES"), QLatin1String("ESP"));
    d->countryCodes.insert(QLatin1String("LK"), QLatin1String("LKA"));
    d->countryCodes.insert(QLatin1String("SD"), QLatin1String("SDN"));
    d->countryCodes.insert(QLatin1String("SR"), QLatin1String("SUR"));
    d->countryCodes.insert(QLatin1String("SJ"), QLatin1String("SJM"));
    d->countryCodes.insert(QLatin1String("SE"), QLatin1String("SWE"));
    d->countryCodes.insert(QLatin1String("CH"), QLatin1String("CHE"));
    d->countryCodes.insert(QLatin1String("SY"), QLatin1String("SYR"));
    d->countryCodes.insert(QLatin1String("TW"), QLatin1String("TWN"));
    d->countryCodes.insert(QLatin1String("TJ"), QLatin1String("TJK"));
    d->countryCodes.insert(QLatin1String("TZ"), QLatin1String("TZA"));
    d->countryCodes.insert(QLatin1String("TH"), QLatin1String("THA"));
    d->countryCodes.insert(QLatin1String("TL"), QLatin1String("TLS"));
    d->countryCodes.insert(QLatin1String("TG"), QLatin1String("TGO"));
    d->countryCodes.insert(QLatin1String("TK"), QLatin1String("TKL"));
    d->countryCodes.insert(QLatin1String("TO"), QLatin1String("TON"));
    d->countryCodes.insert(QLatin1String("TT"), QLatin1String("TTO"));
    d->countryCodes.insert(QLatin1String("TN"), QLatin1String("TUN"));
    d->countryCodes.insert(QLatin1String("TR"), QLatin1String("TUR"));
    d->countryCodes.insert(QLatin1String("TM"), QLatin1String("TKM"));
    d->countryCodes.insert(QLatin1String("TC"), QLatin1String("TCA"));
    d->countryCodes.insert(QLatin1String("TV"), QLatin1String("TUV"));
    d->countryCodes.insert(QLatin1String("UG"), QLatin1String("UGA"));
    d->countryCodes.insert(QLatin1String("UA"), QLatin1String("UKR"));
    d->countryCodes.insert(QLatin1String("AE"), QLatin1String("ARE"));
    d->countryCodes.insert(QLatin1String("GB"), QLatin1String("GBR"));
    d->countryCodes.insert(QLatin1String("UM"), QLatin1String("UMI"));
    d->countryCodes.insert(QLatin1String("US"), QLatin1String("USA"));
    d->countryCodes.insert(QLatin1String("UY"), QLatin1String("URY"));
    d->countryCodes.insert(QLatin1String("UZ"), QLatin1String("UZB"));
    d->countryCodes.insert(QLatin1String("VU"), QLatin1String("VUT"));
    d->countryCodes.insert(QLatin1String("VE"), QLatin1String("VEN"));
    d->countryCodes.insert(QLatin1String("VN"), QLatin1String("VNM"));
    d->countryCodes.insert(QLatin1String("VG"), QLatin1String("VGB"));
    d->countryCodes.insert(QLatin1String("VI"), QLatin1String("VIR"));
    d->countryCodes.insert(QLatin1String("WF"), QLatin1String("WLF"));
    d->countryCodes.insert(QLatin1String("EH"), QLatin1String("ESH"));
    d->countryCodes.insert(QLatin1String("YE"), QLatin1String("YEM"));
    d->countryCodes.insert(QLatin1String("ZM"), QLatin1String("ZMB"));
    d->countryCodes.insert(QLatin1String("ZW"), QLatin1String("ZWE"));
    d->countryCodes.insert(QLatin1String("AX"), QLatin1String("ALA"));
}

} // namespace Digikam
