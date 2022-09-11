/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-05-19
 * Description : Fuzzy search sidebar tab contents - private container.
 *
 * SPDX-FileCopyrightText: 2016-2018 by Mario Frank <mario dot frank at uni minus potsdam dot de>
 * SPDX-FileCopyrightText: 2008-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2008-2012 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012      by Andi Clemens <andi dot clemens at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#ifndef DIGIKAM_FUZZY_SEARCH_VIEW_P_H
#define DIGIKAM_FUZZY_SEARCH_VIEW_P_H

#include "fuzzysearchview.h"

// Qt includes

#include <QFrame>
#include <QImage>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QSpinBox>
#include <QTime>
#include <QTimer>
#include <QToolButton>
#include <QApplication>
#include <QStyle>
#include <QLineEdit>
#include <QIcon>
#include <QMessageBox>

// KDE includes

#include <klocalizedstring.h>
#include <kconfiggroup.h>

// Local includes

#include "dlayoutbox.h"
#include "digikam_debug.h"
#include "album.h"
#include "coredb.h"
#include "coredbalbuminfo.h"
#include "albummanager.h"
#include "albummodel.h"
#include "albumselectors.h"
#include "searchtreeview.h"
#include "coredbaccess.h"
#include "ddragobjects.h"
#include "editablesearchtreeview.h"
#include "findduplicatesview.h"
#include "haariface.h"
#include "iteminfo.h"
#include "itemlister.h"
#include "searchmodificationhelper.h"
#include "searchtextbardb.h"
#include "coredbsearchxml.h"
#include "sketchwidget.h"
#include "thumbnailloadthread.h"
#include "thumbnailsize.h"
#include "dhuesaturationselect.h"
#include "dcolorvalueselector.h"
#include "dexpanderbox.h"
#include "applicationsettings.h"
#include "drangebox.h"
#include "similaritydbaccess.h"
#include "similaritydb.h"
#include "dnotificationwidget.h"
#include "dtextedit.h"

namespace Digikam
{

class Q_DECL_HIDDEN FuzzySearchView::Private
{

public:

    enum FuzzySearchTab
    {
        DUPLICATES = 0,
        SIMILARS,
        SKETCH
    };

public:

    explicit Private()
      : configTabEntry                  (QLatin1String("FuzzySearch Tab")),
        configPenSketchSizeEntry        (QLatin1String("Pen Sketch Size")),
        configResultSketchItemsEntry    (QLatin1String("Result Sketch items")),
        configPenSketchHueEntry         (QLatin1String("Pen Sketch Hue")),
        configPenSketchSaturationEntry  (QLatin1String("Pen Sketch Saturation")),
        configPenSkethValueEntry        (QLatin1String("Pen Sketch Value")),
        configSimilarsThresholdEntry    (QLatin1String("Similars Threshold")),
        configSimilarsMaxThresholdEntry (QLatin1String("Similars Maximum Threshold")),
        active                          (false),                      ///< initially be active to update sketch panel when the search list is restored
        fingerprintsChecked             (false),
        resetButton                     (nullptr),
        saveBtnSketch                   (nullptr),
        undoBtnSketch                   (nullptr),
        redoBtnSketch                   (nullptr),
        saveBtnImage                    (nullptr),
        penSize                         (nullptr),
        resultsSketch                   (nullptr),
        similarityRange                 (nullptr),
        imageWidget                     (nullptr),
        timerSketch                     (nullptr),
        timerImage                      (nullptr),
        folderView                      (nullptr),
        nameEditSketch                  (nullptr),
        nameEditImage                   (nullptr),
        tabWidget                       (nullptr),
        hsSelector                      (nullptr),
        vSelector                       (nullptr),
        labelFile                       (nullptr),
        labelFolder                     (nullptr),
        searchFuzzyBar                  (nullptr),
        searchTreeView                  (nullptr),
        sketchWidget                    (nullptr),
        thumbLoadThread                 (nullptr),
        findDuplicatesPanel             (nullptr),
        imageSAlbum                     (nullptr),
        sketchSAlbum                    (nullptr),
        fuzzySearchAlbumSelectors       (nullptr),
        sketchSearchAlbumSelectors      (nullptr),
        searchModel                     (nullptr),
        searchModificationHelper        (nullptr),
        settings                        (nullptr)
    {
    }

    const QString             configTabEntry;
    const QString             configPenSketchSizeEntry;
    const QString             configResultSketchItemsEntry;
    const QString             configPenSketchHueEntry;
    const QString             configPenSketchSaturationEntry;
    const QString             configPenSkethValueEntry;
    const QString             configSimilarsThresholdEntry;
    const QString             configSimilarsMaxThresholdEntry;

    bool                      active;
    bool                      fingerprintsChecked;

    QColor                    selColor;

    QToolButton*              resetButton;
    QToolButton*              saveBtnSketch;
    QToolButton*              undoBtnSketch;
    QToolButton*              redoBtnSketch;
    QToolButton*              saveBtnImage;

    QSpinBox*                 penSize;
    QSpinBox*                 resultsSketch;

    DIntRangeBox*             similarityRange;

    QLabel*                   imageWidget;

    QTimer*                   timerSketch;
    QTimer*                   timerImage;

    DVBox*                    folderView;

    DTextEdit*                nameEditSketch;
    DTextEdit*                nameEditImage;

    QTabWidget*               tabWidget;

    DHueSaturationSelector*   hsSelector;

    DColorValueSelector*      vSelector;

    DAdjustableLabel*         labelFile;
    DAdjustableLabel*         labelFolder;

    ItemInfo                  imageInfo;
    QUrl                      imageUrl;

    SearchTextBarDb*          searchFuzzyBar;

    EditableSearchTreeView*   searchTreeView;

    SketchWidget*             sketchWidget;

    ThumbnailLoadThread*      thumbLoadThread;

    FindDuplicatesView*       findDuplicatesPanel;

    AlbumPointer<SAlbum>      imageSAlbum;
    AlbumPointer<SAlbum>      sketchSAlbum;

    AlbumSelectors*           fuzzySearchAlbumSelectors;
    AlbumSelectors*           sketchSearchAlbumSelectors;

    SearchModel*              searchModel;
    SearchModificationHelper* searchModificationHelper;

    ApplicationSettings*      settings;
};

} // namespace Digikam

#endif // DIGIKAM_FUZZY_SEARCH_VIEW_P_H
