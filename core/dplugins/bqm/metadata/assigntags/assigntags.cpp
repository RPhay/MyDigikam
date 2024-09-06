/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2022-10-03
 * Description : auto assign tags batch tool.
 *
 * SPDX-FileCopyrightText: 2022-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2023      by Quoc Hung Tran <quochungtran1999 at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "assigntags.h"

// Qt includes

#include <QComboBox>
#include <QWidget>
#include <QLabel>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "digikam_debug.h"
#include "digikam_globals.h"
#include "dimg.h"
#include "dmetadata.h"
#include "dpluginbqm.h"
#include "dlayoutbox.h"
#include "autotagsassign.h"
#include "dfileoperations.h"
#include "localizeselector.h"
#include "previewloadthread.h"

namespace DigikamBqmAssignTagsPlugin
{

class Q_DECL_HIDDEN AssignTags::Private
{
public:

    Private() = default;

    QComboBox*            modelSelectionMode = nullptr;
    LocalizeSelectorList* trSelectorList     = nullptr;
    bool                  changeSettings     = true;
};

AssignTags::AssignTags(QObject* const parent)
    : BatchTool(QLatin1String("AssignTags"), MetadataTool, parent),
      d        (new Private)
{
}

AssignTags::~AssignTags()
{
    delete d;
}

BatchTool* AssignTags::clone(QObject* const parent) const
{
    return new AssignTags(parent);
}

void AssignTags::registerSettingsWidget()
{
    DVBox* const vbox     = new DVBox;

    QLabel* const title   = new QLabel(vbox);
    title->setText(i18nc("@label",
                        "<p><b>This tool allows to assign automatically tags to images by contents analysis using "
                        "deep-learning neural network.</b></p>"
                        "<p>The settings below determine the deep-learning model to use while parsing image "
                        "contents to determine the subjects of the photography. The neural network used in background "
                        "will generate automatically a serie of tags describing the contents and store the results in "
                        "the database.</p>"));
    title->setWordWrap(true);

    DHBox* const hbox     = new DHBox(vbox);
    new QLabel(i18n("Selection model: "), hbox);
    QWidget* const space  = new QWidget(hbox);
    hbox->setStretchFactor(space, 10);

    d->modelSelectionMode = new QComboBox(hbox);
    d->modelSelectionMode->addItem(i18n("YOLOv5 Nano"),   DetectorModel::YOLOV5NANO);
    d->modelSelectionMode->addItem(i18n("YOLOv5 XLarge"), DetectorModel::YOLOV5XLARGE);
    d->modelSelectionMode->addItem(i18n("ResNet50"),      DetectorModel::RESNET50);
    d->modelSelectionMode->setToolTip(i18nc("@info:tooltip",
        "<p><b>YOLOv5 Nano</b>: this model is a neural network which offers exceptional speed and efficiency. It enables you to swiftly "
        "evaluate the integration of smaller-scale object detection scenarios. It's designed for objects detections, capable of recognizing "
        "and extracting the location of objects within an image. The limitation on the number of recognizable objects is set to 80.</p>"
        "<p><b>YOLOv5 XLarge</b>: as the previous one, this model is a neural network dedicated for more complex object detection requirements and "
        "showcases remarkable capabilities. Despite the additional complexity introducing more time-latency and "
        "computer resources, it must be used for larger-scale object detection scenarios as it provides more accurate predictions at the expense of speed.</p>"
        "<p><b>ResNet50</b>: this model is a specific type of convolutional neural network formed by stacking residual blocks "
        "commonly used to power computer vision applications as object detections. This kind of design allows the training of very deep networks without "
        "encountering the vanishing gradient problem. Unlike YOLO, ResNet50 is primarily focused on image classification and does not provide object localization. "
        "It can recognize objects from a vast set of more than 1,000 classes, covering a wide range of objects, animals, and scenes.</p>"));

    d->trSelectorList      = new LocalizeSelectorList(vbox);
    d->trSelectorList->setTitle(i18nc("@label", "Translate Tags to:"));

    QWidget* const space2  = new QWidget(vbox);
    vbox->setStretchFactor(space2, 10);

    m_settingsWidget = vbox;

    connect(d->modelSelectionMode, SIGNAL(currentIndexChanged(int)),
            this, SLOT(slotSettingsChanged()));

    connect(d->trSelectorList, SIGNAL(signalSettingsChanged()),
            this, SLOT(slotSettingsChanged()));

    BatchTool::registerSettingsWidget();
}

BatchToolSettings AssignTags::defaultSettings()
{
    BatchToolSettings settings;

    settings.insert(QLatin1String("AutoTagModel"), (int)DetectorModel::YOLOV5NANO);

    return settings;
}

void AssignTags::slotAssignSettings2Widget()
{
    d->changeSettings = false;

    int model         = settings()[QLatin1String("AutoTagModel")].toInt();
    d->modelSelectionMode->setCurrentIndex(model);

    QStringList langs = settings()[QLatin1String("TrAutoTagsLangs")].toStringList();

    d->trSelectorList->clearLanguages();

    for (const QString& lg : std::as_const(langs))
    {
        d->trSelectorList->addLanguage(lg);
    }

    d->changeSettings = true;
}

void AssignTags::slotSettingsChanged()
{
    if (d->changeSettings)
    {
        BatchToolSettings settings;

        settings.insert(QLatin1String("AutoTagModel"),    d->modelSelectionMode->currentIndex());

        QStringList langs = d->trSelectorList->languagesList();
        settings.insert(QLatin1String("TrAutoTagsLangs"), langs);

        BatchTool::slotSettingsChanged(settings);
    }
}

bool AssignTags::toolOperations()
{
    bool ret = true;
    QScopedPointer<DMetadata> meta(new DMetadata);

    if (image().isNull())
    {
        QFile::remove(outputUrl().toLocalFile());
        ret = DFileOperations::copyFile(inputUrl().toLocalFile(), outputUrl().toLocalFile());

        if (!ret || !meta->load(outputUrl().toLocalFile()))
        {
            return ret;
        }
    }
    else
    {
        ret = savefromDImg();
        meta->setData(image().getMetadata());
    }

    DImg img           = image();
    int model         = settings()[QLatin1String("AutoTagModel")].toInt();
    QStringList langs = settings()[QLatin1String("TrAutoTagsLangs")].toStringList();

    if (ret && img.isNull())
    {
        img = PreviewLoadThread::loadFastSynchronously(outputUrl().toLocalFile(), 2000);
    }

    if (ret && !img.isNull())
    {
        QScopedPointer<AutoTagsAssign> autotagsEngine(new AutoTagsAssign(DetectorModel(model)));
        QList<QList<QString> > tagsLists = autotagsEngine->generateTagsList(QList<DImg>() << img, 16);

        if (!tagsLists.isEmpty())
        {
            QString path = outputUrl().toLocalFile();
            qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Path to process with Auto-Tags:" << path;

            QStringList tagsPath;
            const QString rootTag = QLatin1String("auto/");

            for (const auto& tag : tagsLists.at(0))
            {
                if (!langs.isEmpty())
                {
                    for (const QString& trLang : std::as_const(langs))
                    {
                        QString trOut;
                        QString error;
                        bool trRet = s_inlineTranslateString(tag, trLang, trOut, error);

                        if (trRet)
                        {
                            tagsPath << (rootTag + trLang +
                                         QLatin1Char('/') +  trOut);
                        }
                        else
                        {
                            qCDebug(DIGIKAM_DPLUGIN_BQM_LOG) << "Auto-Tags online translation error:"
                                                             << error;
                            tagsPath << (rootTag + trLang +
                                         QLatin1Char('/') +  tag);
                        }
                    }
                }
                else
                {
                    tagsPath << (rootTag + tag);
                }
            }

            if (!tagsPath.isEmpty())
            {
                meta->setItemTagsPath(tagsPath);
                ret = meta->save(outputUrl().toLocalFile());
            }
        }
    }

    return ret;
}

} // namespace DigikamBqmAssignTagsPlugin

#include "moc_assigntags.cpp"
