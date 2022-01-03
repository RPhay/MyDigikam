/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2021-01-28
 * Description : Objective-C wrapper for open-with operations under MacOS
 *
 * Copyright (C) 2021      by Robert Lindsay <robert dot lindsay at gmail dot com>
 * Copyright (C) 2021-2022 by Gilles Caulier <caulier dot gilles at gmail dot com>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

// Qt include

#include <QString>

// MacOS header

#import <Foundation/Foundation.h>

//	given a filename extension "extension", here's how to find all of the
//	applications known to the OS who can open files of that type.

CFStringRef			uti = NULL;
CFArrayRef			bundleIDs = NULL;
CFMutableArrayRef	applications = NULL;

applications = CFArrayCreateMutable(kCFAllocatorDefault, 0);

//	Make a UTI from a filename extension
uti = UTTypeCreatePreferredIdentifierForTag(kUTTagClassFilenameExtension,
											extension,
											nil);

if (NULL != uti)
{
	//	get a list of all of the application bundle IDs that know how to handle this:
	bundleIDs = LSCopyAllRoleHandlersForContentType(uti, kLSRolesViewer | kLSRolesEditor);
}

if (NULL != bundleIDs)
{
	//	find all the available applications with this bundle ID
	//	getting the display name and version is left as an exercise for the reader.
	
	const CFIndex	count = CFArrayGetCount(bundleIDs);
	
	for (CFIndex i = 0; i < count; i++)
	{
		CFArrayRef	appsForBundleID = NULL;
		
		appsForBundleID = LSCopyApplicationURLsForBundleIdentifier(bundleID, NULL);
		if (NULL != appsForBundleID)
		{
			//
			//	you could call CFURLResourceIsReachable() on each item before adding
			//	it to the big array of qualified applications.
			//
			
			CFArrayAppendArray(applications, appsForBundleID, CFRangeMake(0, CFArrayGetCount(appsForBundleID)));
			
			CFRelease(appsForBundleID);
		}
	}
}

//	"applications" now has an array of ALL of the possible applications 
//	when finished. Given a UI to choose one, you can then call 
//	LSOpenFromURLSpec() to open your file with a specific application.

//	when you're finished, don't forget to release the resources or you'll leak memory
CFRelease(uti);
CFRelease(bundleIDs);
CFRelease(applications);
