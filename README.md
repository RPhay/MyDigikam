# <img src="core/data/pics/digikam/logo-digikam.webp" width="48"/> DigiKam - Professional Photo Management with the Power of Open Source

| CI Job        | Status                                                                                                                                                                                                  |
|---------------|---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Gitlab Builds | [![Gitlab Builds Status](https://invent.kde.org/graphics/Digikam/badges/master/pipeline.svg)](https://invent.kde.org/graphics/Digikam/-/pipelines)                                                      |
| Coverity Scan | [![Coverity Scan Status](https://scan.coverity.com/projects/285/badge.svg)](https://scan.coverity.com/projects/Digikam)                                                                                 |

If you are reading this on Github, be aware that this is just a mirror. Our real code repository [is located here](https://invent.kde.org/graphics/Digikam)

Developers, if you want to contribute, see the online [API documentation here](https://www.Digikam.org/api)

NOTE: this project support Qt5 and Qt6 frameworks.

# About

Digikam is an advanced open-source digital photo management application that runs on Linux, Windows, and MacOS.
The application provides a comprehensive set of tools for importing, managing, editing, and sharing photos and RAW files.

[![](https://c1.staticflickr.com/5/4216/35354951072_a034561b5e_c.jpg "Albums View and Image Editor")](https://www.flickr.com/photos/Digikam/35354951072/)

You can use Digikam's import capabilities to easily transfer photos, RAW files, and videos directly from your camera
and external storage devices (SD cards, USB disks, etc.). The application allows you to configure import settings
and rules that process and organize imported items on-the-fly.

[![](https://c1.staticflickr.com/1/703/32558229094_3d7ec01d3a_c.jpg "Map View displaying rated items and Batch Queue Manager in action")](https://www.flickr.com/photos/Digikam/32558229094)

Digikam organizes photos, RAW files, and videos into albums. But the application also features powerful tagging
tools that allow you to assign tags, ratings, and labels to photos and raw files. You can then use filtering
functionality to quickly find items that match specific criteria.

[![](https://c2.staticflickr.com/4/3726/32557269024_ae870b0466_c.jpg "Search items by date rage and Geolocation editor")](https://www.flickr.com/photos/Digikam/32557269024)

In addition to filtering functionality, Digikam features powerful searching capabilities that let you search
the photo library by a wide range of criteria. You can search photos by tags, labels, rating, data, location,
and even specific EXIF, IPTC, or XMP metadata.

[![](https://c1.staticflickr.com/1/306/32217007615_db6f9d116a_c.jpg "Search by Tags with preview mode and Metadata Editor in action")](https://www.flickr.com/photos/Digikam/32217007615)

You can also combine several criteria for more advanced searches. Digikam rely on Exiv2 library to handle metadata
tag contents from files to populate the photo library.

[![](https://c1.staticflickr.com/5/4795/40743725771_0b69dca743_c.jpg "Advanced search tool and video file result played as preview")](https://www.flickr.com/photos/Digikam/40743725771)

Digikam can handle RAW files, and the application uses the excellent LibRaw library for decoding raw files.
The library is actively maintained and regularly updated to include support for the latest camera models.

[![](https://c1.staticflickr.com/1/300/31407487553_a14abd0418_c.jpg "Find by items similarity and Light Table in cation to compare side by side")](https://www.flickr.com/photos/Digikam/31407487553)

The application provides a comprehensive set of editing tools. This includes basic tools for adjusting colors,
cropping, and sharpening as well as advanced tools for, curves adjustment, panorama stitching, and much more.
A special tool based on Lensfun library permit to apply lens corrections automatically on images.

[![](https://c1.staticflickr.com/5/4649/40430534662_097b46a270_c.jpg "Panorama tool stiching photo")](https://www.flickr.com/photos/Digikam/40474079731)

Extended functionality in Digikam is implemented via a set of tools, dedicated especially to import and export
contents to remote web-services.

Digikam is based in part on the work of the Independent JPEG Group.

# Authors

See [AUTHORS](AUTHORS) file for details.

# Related URLs

* [Digikam project web site](https://www.Digikam.org)
* [Digikam handbook git repository](https://invent.kde.org/documentation/Digikam-doc)
* [Digikam web site git repository](https://invent.kde.org/websites/Digikam-org)
* [Digikam unit-test data git repository](https://invent.kde.org/graphics/Digikam-test-data)

# Contact

If you have questions, comments, and suggestions, write an email to:

Digikam-users@kde.org

IRC channel from irc.libera.chat server: #Digikam (or use [web chat](https://web.libera.chat/))

Also you can ask to the forum:
https://discuss.kde.org/tag/digikam

# Bug reports

IMPORTANT: the bug reports and wishlist entries are hosted by the Bugzilla
system which can be reached from the standard Help menu of Digikam.
A mail will automatically be sent to the Digikam development mailing list.
There is no need to contact directly the Digikam mailing list for a bug report
or a devel wish.

The current bugs and devel wishes reported to the bugzilla servers can be seen at this url:

* [Digikam](https://bugs.kde.org/buglist.cgi?product=Digikam&bug_status=UNCONFIRMED&bug_status=NEW&bug_status=ASSIGNED&bug_status=REOPENED)

Extra Bugzilla servers for shared libs used by Digikam :

* [LibRaw library](https://github.com/LibRaw/LibRaw/issues)
* [Lensfun library](https://github.com/lensfun/lensfun/issues)
* [GPhoto2 library](http://gphoto.org/bugs)
* [Exiv2 library](https://github.com/Exiv2/exiv2/issues)

# Compilation and Installation

From the **developper documentation** [available at this url](https://files.kde.org/Digikam/api/), see the sections:

- **External Dependencies**
- **Get Source Code**
- **Development Environment**
- **Cmake Configuration Options**
- **Setup Local Compilation and Run-Time**

# Donate Money

If you love Digikam, you can help developers to buy new photo devices to test
and implement new features. Thanks in advance for your generous donations.

For more information, look [at this url](https://www.Digikam.org/donate/)
