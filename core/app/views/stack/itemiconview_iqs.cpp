#include "itemiconview_p.h"

#include "setupimagequalitysorter.h"
#include "maintenancethread.h"

namespace Digikam
{


void ItemIconView::slotImageQualitySorter()
{
    Setup::execSinglePage(this, Setup::ImageQualityPage);
    
    QStringList paths;

    for (const auto& url : selectedUrls())
    {
        paths<<url.toLocalFile();
    }
    
    SetupImageQualitySorter* settingWidgets = new SetupImageQualitySorter();

    ImageQualityContainer settings = settingWidgets->getImageQualityContainer();

    MaintenanceThread* thread = new MaintenanceThread(this);

    thread->sortByImageQuality(paths, settings);
    
    thread->start();

    connect(thread, &QThread::finished, thread, &QObject::deleteLater);
}


}