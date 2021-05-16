/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2008-09-25
 * Description : a tool to convert RAW file to DNG - Backup RAW data.
 *
 * Copyright (C) 2008-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
 * Copyright (C) 2010-2011 by Jens Mueller <tschenser at gmx dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#include "dngwriter_p.h"

// Local includes

#include "dngwriterhost.h"

namespace Digikam
{

int DNGWriter::Private::backupRaw(DNGWriterHost& host,
                                  AutoPtr<dng_negative>& negative)
{
    if (backupOriginalRawFile)
    {
        qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: Backup Original RAW file (" << inputInfo.size() << " bytes)";

        QFileInfo originalFileInfo(parent->inputFile());

        QFile originalFile(originalFileInfo.absoluteFilePath());

        if (!originalFile.open(QIODevice::ReadOnly))
        {
            qCCritical(DIGIKAM_GENERAL_LOG) << "DNGWriter: Cannot open original RAW file to backup in DNG. Aborted...";

            return PROCESS_FAILED;
        }

        QDataStream originalDataStream(&originalFile);

        quint32 forkLength = originalFileInfo.size();
        quint32 forkBlocks = (quint32)floor((forkLength + 65535.0) / 65536.0);

        QVector<quint32> offsets;
        quint32 offset     = (2 + forkBlocks) * sizeof(quint32);
        offsets.push_back(offset);

        QByteArray originalDataBlock;
        originalDataBlock.resize(CHUNK);

        QTemporaryFile compressedFile;

        if (!compressedFile.open())
        {
            qCCritical(DIGIKAM_GENERAL_LOG) << "DNGWriter: Cannot open temporary file to write Zipped Raw data. Aborted...";

            return PROCESS_FAILED;
        }

        QDataStream compressedDataStream(&compressedFile);

        for (quint32 block = 0 ; block < forkBlocks ; ++block)
        {
            int originalBlockLength = originalDataStream.readRawData(originalDataBlock.data(), CHUNK);

            QByteArray compressedDataBlock = qCompress((const uchar*)originalDataBlock.data(), originalBlockLength, -1);
            compressedDataBlock.remove(0, 4);   // removes qCompress own header
            qCDebug(DIGIKAM_GENERAL_LOG) << "DNGWriter: compressed data block " << originalBlockLength << " -> " << compressedDataBlock.size();

            offset += compressedDataBlock.size();
            offsets.push_back(offset);

            compressedDataStream.writeRawData(compressedDataBlock.data(), compressedDataBlock.size());
        }

        dng_memory_allocator memalloc2(gDefaultDNGMemoryAllocator);
        dng_memory_stream tempDataStream(memalloc2);
        tempDataStream.SetBigEndian(true);
        tempDataStream.Put_uint32(forkLength);

        for (qint32 idx = 0 ; idx < offsets.size() ; ++idx)
        {
            tempDataStream.Put_uint32(offsets[idx]);
        }

        QByteArray compressedData;
        compressedData.resize(compressedFile.size());
        compressedFile.seek(0);
        compressedDataStream.readRawData(compressedData.data(), compressedData.size());
        tempDataStream.Put(compressedData.data(), compressedData.size());

        compressedFile.remove();
        originalFile.close();

        tempDataStream.Put_uint32(0);
        tempDataStream.Put_uint32(0);
        tempDataStream.Put_uint32(0);
        tempDataStream.Put_uint32(0);
        tempDataStream.Put_uint32(0);
        tempDataStream.Put_uint32(0);
        tempDataStream.Put_uint32(0);

        AutoPtr<dng_memory_block> block(host.Allocate(tempDataStream.Length()));
        tempDataStream.SetReadPosition(0);
        tempDataStream.Get(block->Buffer(), tempDataStream.Length());

        dng_md5_printer md5;
        md5.Process(block->Buffer(), block->LogicalSize());
        negative->SetOriginalRawFileData(block);
        negative->SetOriginalRawFileDigest(md5.Result());
        negative->ValidateOriginalRawFileDigest();
    }

    if (cancel)
    {
        return PROCESS_CANCELED;
    }

    return PROCESS_CONTINUE;
}

} // namespace Digikam
