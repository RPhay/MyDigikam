/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2025-01-03
 * Description : Macros for simplifying the ML pipeline code
 *
 * SPDX-FileCopyrightText: 2025      by Gilles Caulier <caulier dot gilles at gmail dot com>
 * SPDX-FileCopyrightText: 2025      by Michael Miller <michael underscore miller at msn dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

/**
 * These macros are used to simplify coding of ML pipeline stages. Whenever possible, use these macros to
 * ensure that the pipeline stages are consistent and easy to read.
 */

#define MLPIPELINE_FINDER_START(nextStage, nextQueue) \
            MLPipelinePackageFoundation* mlpackage = dequeue(thisQueue); \
            if (queueEndSignal() == mlpackage) { break; } \
            performanceProfileList[thisStage].maxQueueCount = qMax(performanceProfileList[thisStage].maxQueueCount, thisQueue->size()); \
            ++performanceProfileList[thisStage].itemCount; \
            timer.start(); \
            mlpackage

#define MLPIPELINE_FINDER_END(thisStage, nextStage) \
            performanceProfileList[thisStage].itemCount = totalItemCount; \
            performanceProfileList[thisStage].elapsedTime = timer.elapsed(); \
            stageEnd(MLPipelineStage::Finder, MLPipelineStage::Loader); \
            return true;
            
#define MLPIPELINE_STAGE_START(threadPriority, thisStage, nextStage) \
            MLPipelineQueue *thisQueue = nullptr, *nextQueue = nullptr; \
            stageStart(threadPriority, thisStage, nextStage, thisQueue, nextQueue); \
            QElapsedTimer timer;

#define MLPIPELINE_STAGE_END(thisStage, nextStage) \
            stageEnd(thisStage, nextStage);

#define MLPIPELINE_LOOP_START(thisStage, thisQueue) \
            MLPipelinePackageFoundation* mlpackage = dequeue(thisQueue); \
            if (queueEndSignal() == mlpackage) { break; } \
            performanceProfileList[thisStage].maxQueueCount = qMax(performanceProfileList[thisStage].maxQueueCount, thisQueue->size()); \
            ++performanceProfileList[thisStage].itemCount; \
            timer.start(); \
            mlpackage

#define MLPIPELINE_LOOP_END(thisStage) \
            performanceProfileList[thisStage].elapsedTime += timer.elapsed(); \
            performanceProfileList[thisStage].maxElapsedTime = qMax(performanceProfileList[thisStage].maxElapsedTime, timer.elapsed());

#define MLPIPELINE_CATCH(pipelineStageName) \
        catch(const std::exception& e) \
        { \
            qCCritical(DIGIKAM_FACESENGINE_LOG) << pipelineStageName << e.what() << " Restarting..."; \
            notify(MLPipelineNotification::notifySkipped, i18n("Error"), QLatin1String(e.what()), 0, QIcon::fromTheme(QStringLiteral("error"))); \
            if (package) \
            { \
                delete package; \
            } \
        } \
        catch(...) \
        { \
            qCCritical(DIGIKAM_FACESENGINE_LOG) << pipelineStageName << "  Restarting..."; \
            notify(MLPipelineNotification::notifySkipped, i18n("Error"), QLatin1String(pipelineStageName), 0, QIcon::fromTheme(QStringLiteral("error"))); \
            if (package) \
            { \
                delete package; \
            } \
        }
