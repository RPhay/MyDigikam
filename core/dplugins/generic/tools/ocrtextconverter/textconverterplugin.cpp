#include "textconverterplugin.h"

// Qt includes

#include <QPointer>

// KDE includes

#include <klocalizedstring.h>

// Local includes

#include "textconverterdialog.h"

namespace DigikamGenericTextConverterPlugin
{

TextConverterPlugin::TextConverterPlugin(QObject* const parent)
    : DPluginGeneric(parent)
{
}

TextConverterPlugin::~TextConverterPlugin()
{
}

QString TextConverterPlugin::name() const
{
    return i18nc("@title", "Text Converter");
}

QString TextConverterPlugin::iid() const
{
    return QLatin1String(DPLUGIN_IID);
}

QIcon TextConverterPlugin::icon() const
{
    return QIcon::fromTheme(QLatin1String("image-x-adobe-dng"));
}

QString TextConverterPlugin::description() const
{
    return i18nc("@info", "A tool to batch convert documented images to text");
}

QString TextConverterPlugin::details() const
{
    return i18nc("@info", "This tool can convert documented images data to Text format.\n\n"
                 "by using Tesseract, is a software OCR engine, originally developed at HP labs and now sponsored by Google.\n\n"
                 "It supports multiple languages and scripts (including right-to-left text) and page layout analyse\n\n"
                 "See details on %1 for more information on Tesseract",
                 QLatin1String("<a href='https://github.com/tesseract-ocr/tesseract'>https://github.com/tesseract-ocr/tesseract</a>"));
}

QList<DPluginAuthor> TextConverterPlugin::authors() const
{
    return QList<DPluginAuthor>()
            << DPluginAuthor(QString::fromUtf8("Gilles Caulier"),
                             QString::fromUtf8("caulier dot gilles at gmail dot com"),
                             QString::fromUtf8("(C) 2008-2021"))
            << DPluginAuthor(QString::fromUtf8("Maik Qualmann"),
                             QString::fromUtf8("metzpinguin at gmail dot com"),
                             QString::fromUtf8("(C) 2020-2021"))
            ;
}

void TextConverterPlugin::setup(QObject* const parent)
{
    DPluginAction* const ac = new DPluginAction(parent);
    ac->setIcon(icon());
    ac->setText(i18nc("@action", "Text Converter..."));
    ac->setObjectName(QLatin1String("textconverter"));
    ac->setActionCategory(DPluginAction::GenericTool);

    connect(ac,   SIGNAL(triggered(bool)),
            this, SLOT(slotTextConverter()));

    addAction(ac);
}

void TextConverterPlugin::slotTextConverter()
{
    QPointer<TextConverterDialog> dialog = new TextConverterDialog(nullptr, infoIface(sender()));
    dialog->setPlugin(this);
    dialog->exec();
    delete dialog;
}

} // namespace DigikamGenericTextConverterPlugin
