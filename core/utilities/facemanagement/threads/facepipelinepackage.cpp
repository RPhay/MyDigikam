/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-03
 * Description : Integrated, multithread face detection / recognition
 *
 * SPDX-FileCopyrightText: 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * SPDX-FileCopyrightText: 2012-2024 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * ============================================================ */

#include "facepipeline_p.h"

// Local includes

#include "databasewriter.h"
#include "detectionbenchmarker.h"
#include "detectionworker.h"
#include "recognitionbenchmarker.h"
#include "recognitionworker.h"
#include "trainerworker.h"
#include "facepreviewloader.h"
#include "faceitemretriever.h"
#include "parallelpipes.h"
#include "scanstatefilter.h"

namespace Digikam
{

FacePipelineFaceTagsIface::FacePipelineFaceTagsIface(const FaceTagsIface& face)
    : FaceTagsIface(face)
{
}

FacePipelineFaceTagsIface& FacePipelineFaceTagsIface::operator=(const FacePipelineFaceTagsIface& other)
{
    m_type         = other.m_type;
    m_imageId      = other.m_imageId;
    m_tagId        = other.m_tagId;
    m_region       = other.m_region;

    roles          = other.roles;
    assignedTagId  = other.assignedTagId;
    assignedRegion = other.assignedRegion;

    return *this;
}

// ----------------------------------------------------------------------------------------

bool FacePipelineExtendedPackage::operator==(const LoadingDescription& description) const
{
    return (filePath == description.filePath);
}

// ----------------------------------------------------------------------------------------

FacePipelineFaceTagsIfaceList::FacePipelineFaceTagsIfaceList(const QList<FaceTagsIface>& faces)
{
    operator=(faces);
}

FacePipelineFaceTagsIfaceList& FacePipelineFaceTagsIfaceList::operator=(const QList<FaceTagsIface>& faces)
{
    for (const FaceTagsIface& face : std::as_const(faces))
    {
        operator<<(FacePipelineFaceTagsIface(face));
    }

    return *this;
}

void FacePipelineFaceTagsIfaceList::setRole(FacePipelineFaceTagsIface::Roles role)
{
    for (iterator it = begin() ; it != end() ; ++it)
    {
        it->roles |= role;
    }
}

void FacePipelineFaceTagsIfaceList::clearRole(FacePipelineFaceTagsIface::Roles role)
{
    for (iterator it = begin() ; it != end() ; ++it)
    {
        it->roles &= ~role;
    }
}

void FacePipelineFaceTagsIfaceList::replaceRole(FacePipelineFaceTagsIface::Roles remove,
                                                FacePipelineFaceTagsIface::Roles add)
{
    for (iterator it = begin() ; it != end() ; ++it)
    {
        if (it->roles & remove)
        {
            it->roles &= ~remove;
            it->roles |= add;
        }
    }
}

QList<FaceTagsIface> FacePipelineFaceTagsIfaceList::toFaceTagsIfaceList() const
{
    QList<FaceTagsIface> faces;

    for (const_iterator it = constBegin() ; it != constEnd() ; ++it)
    {
        faces << *it;
    }

    return faces;
}

FacePipelineFaceTagsIfaceList FacePipelineFaceTagsIfaceList::facesForRole(FacePipelineFaceTagsIface::Roles role) const
{
    FacePipelineFaceTagsIfaceList faces;

    for (const_iterator it = constBegin() ; it != constEnd() ; ++it)
    {
        if (it->roles & role)
        {
            faces << *it;
        }
    }

    return faces;
}

FacePipelineExtendedPackage::Ptr PackageLoadingDescriptionList::take(const LoadingDescription& description)
{
    FacePipelineExtendedPackage::Ptr                  package;
    QList<FacePipelineExtendedPackage::Ptr>::iterator it;

    for (it = begin() ; it != end() ; ++it)
    {
        if (*(*it) == description)
        {
            package = *it;
            erase(it);

            break;
        }
    }

    return package;
}

} // namespace Digikam
