/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2011-05-23
 * Description : a tool to create panorama by fusion of several images.
 *
 * SPDX-FileCopyrightText: 2011-2016 by Benjamin Girault <benjamin dot girault at gmail dot com>
 * SPDX-FileCopyrightText: 2009-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#pragma once

// Qt includes

#include <QObject>
#include <QPointer>

// Local includes

#include "panoactions.h"
#include "ptotype.h"
#include "dplugingeneric.h"

using namespace Digikam;

namespace DigikamGenericPanoramaPlugin
{

class PanoActionThread;
class AutoOptimiserBinary;
class CPCleanBinary;
class CPFindBinary;
class EnblendBinary;
class MakeBinary;
class NonaBinary;
class PanoModifyBinary;
class Pto2MkBinary;
class HuginExecutorBinary;

class PanoManager : public QObject
{
    Q_OBJECT

public:

    explicit PanoManager(QObject* const parent = nullptr);
    ~PanoManager()                                            override;

public:

    static QPointer<PanoManager> internalPtr;
    static PanoManager*          instance();
    static bool                  isCreated();

public:

    bool checkBinaries();

    void checkForHugin2015();
    bool hugin2015()                                    const;

    void setGPano(bool gPano);
    bool gPano() const;
/*
     void setHDR(bool hdr);
     bool hdr() const;
*/
    void setFileFormatJPEG();
    void setFileFormatTIFF();
    void setFileFormatHDR();
    PanoramaFileType format()                           const;

    void setItemsList(const QList<QUrl>& urls);
    QList<QUrl>& itemsList()                            const;

    void setPlugin(DPlugin* const plugin);

    QUrl&                   basePtoUrl()                const;
    QSharedPointer<PTOType> basePtoData();
    void                    resetBasePto();
    QUrl&                   cpFindPtoUrl()              const;
    QSharedPointer<PTOType> cpFindPtoData();
    void                    resetCpFindPto();
    QUrl&                   cpCleanPtoUrl()             const;
    QSharedPointer<PTOType> cpCleanPtoData();
    void                    resetCpCleanPto();
    QUrl&                   autoOptimisePtoUrl()        const;
    QSharedPointer<PTOType> autoOptimisePtoData();
    void                    resetAutoOptimisePto();
    QUrl&                   viewAndCropOptimisePtoUrl() const;
    QSharedPointer<PTOType> viewAndCropOptimisePtoData();
    void                    resetViewAndCropOptimisePto();
    QUrl&                   previewPtoUrl()             const;
    QSharedPointer<PTOType> previewPtoData();
    void                    resetPreviewPto();
    QUrl&                   panoPtoUrl()                const;
    QSharedPointer<PTOType> panoPtoData();
    void                    resetPanoPto();

    QUrl&                   previewMkUrl()              const;
    void                    resetPreviewMkUrl();
    QUrl&                   previewUrl()                const;
    void                    resetPreviewUrl();
    QUrl&                   mkUrl()                     const;
    void                    resetMkUrl();
    QUrl&                   panoUrl()                   const;
    void                    resetPanoUrl();

    PanoramaItemUrlsMap&    preProcessedMap()           const;
    PanoActionThread*       thread()                    const;
    AutoOptimiserBinary&    autoOptimiserBinary()       const;
    CPCleanBinary&          cpCleanBinary()             const;
    CPFindBinary&           cpFindBinary()              const;
    EnblendBinary&          enblendBinary()             const;
    MakeBinary&             makeBinary()                const;
    NonaBinary&             nonaBinary()                const;
    PanoModifyBinary&       panoModifyBinary()          const;
    Pto2MkBinary&           pto2MkBinary()              const;
    HuginExecutorBinary&    huginExecutorBinary()       const;

    void run();

Q_SIGNALS:

    void updateHostApp(const QUrl& url);

private Q_SLOTS:

    void setPreProcessedMap(const PanoramaItemUrlsMap& urls);

private:

    void startWizard();

private:

    class Private;
    Private* const d = nullptr;
};

} // namespace DigikamGenericPanoramaPlugin
