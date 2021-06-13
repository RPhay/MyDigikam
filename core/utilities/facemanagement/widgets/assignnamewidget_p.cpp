/* ============================================================
 *
 * This file is a part of digiKam project
 * https://www.digikam.org
 *
 * Date        : 2010-09-12
 * Description : Widget for assignment and confirmation of names for faces
 *
 * Copyright (C) 2010      by Aditya Bhatt <adityabhatt1991 at gmail dot com>
 * Copyright (C) 2010-2011 by Marcel Wiesweg <marcel dot wiesweg at gmx dot de>
 * Copyright (C) 2012-2021 by Gilles Caulier <caulier dot gilles at gmail dot com>
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

#include "assignnamewidget_p.h"

namespace Digikam
{

AssignNameWidget::Private::Private(AssignNameWidget* const q)
    : mode            (InvalidMode),
      layoutMode      (InvalidLayout),
      visualStyle     (InvalidVisualStyle),
      widgetMode      (InvalidTagEntryWidgetMode),
      comboBox        (nullptr),
      lineEdit        (nullptr),
      confirmButton   (nullptr),
      rejectButton    (nullptr),
      clickLabel      (nullptr),
      modelsGiven     (false),
      tagModel        (nullptr),
      tagFilterModel  (nullptr),
      tagFilteredModel(nullptr),
      layout          (nullptr),
      q               (q)
{
}

QWidget* AssignNameWidget::Private::addTagsWidget() const
{
    if (comboBox)
    {
        return comboBox;
    }
    else
    {
        return lineEdit;
    }
}

bool AssignNameWidget::Private::isValid() const
{
    return (
            (mode        != InvalidMode)                  &&
            (layoutMode  != InvalidLayout)                &&
            (visualStyle != InvalidVisualStyle)           &&
            (widgetMode  != InvalidTagEntryWidgetMode)
           );
}

void AssignNameWidget::Private::clearWidgets()
{
    delete comboBox;
    comboBox      = nullptr;

    delete lineEdit;
    lineEdit      = nullptr;

    delete confirmButton;
    confirmButton = nullptr;

    delete rejectButton;
    rejectButton  = nullptr;

    delete clickLabel;
    clickLabel    = nullptr;
}

QToolButton* AssignNameWidget::Private::createToolButton(const QIcon& icon,
                                                         const QString& text,
                                                         const QString& tip) const
{
    QToolButton* const b = new QToolButton;
    b->setIcon(icon);
    b->setText(text);
    b->setToolTip(tip);
    b->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);

    return b;
}

void AssignNameWidget::Private::updateModes()
{
    if (isValid())
    {
        clearWidgets();
        checkWidgets();
        updateLayout();
        updateVisualStyle();
    }
}

template <class T>
void AssignNameWidget::Private::setupAddTagsWidget(T* const widget)
{
    if (modelsGiven)
    {
        widget->setModel(tagModel, tagFilteredModel, tagFilterModel);
    }

    if (parentTag)
    {
        widget->setParentTag(parentTag);
    }

    q->connect(widget, SIGNAL(taggingActionActivated(TaggingAction)),
               q, SLOT(slotActionActivated(TaggingAction)));

    q->connect(widget, SIGNAL(taggingActionSelected(TaggingAction)),
               q, SLOT(slotActionSelected(TaggingAction)));
}

void AssignNameWidget::Private::checkWidgets()
{
    if (!isValid())
    {
        return;
    }

    switch (mode)
    {
        case InvalidMode:
        {
            break;
        }

        case UnconfirmedEditMode:
        case ConfirmedEditMode:
        {
            switch (widgetMode)
            {
                case InvalidTagEntryWidgetMode:
                {
                    break;
                }

                case AddTagsComboBoxMode:
                {
                    if (!comboBox)
                    {
                        comboBox = new AddTagsComboBox(q);
                        comboBox->setMinimumWidth(250);
                        setupAddTagsWidget(comboBox);
                    }

                    break;
                }

                case AddTagsLineEditMode:
                {
                    if (!lineEdit)
                    {
                        lineEdit = new AddTagsLineEdit(q);
                        setupAddTagsWidget(lineEdit);
                    }

                    break;
                }
            }

            if (!confirmButton)
            {
                confirmButton = createToolButton(QIcon::fromTheme(QLatin1String("dialog-ok-apply")), i18nc("@action", "OK"));
                confirmButton->setToolTip(i18nc("@info:tooltip", "Confirm that the selected person is shown here"));

                if (mode == UnconfirmedEditMode)
                {
                    confirmButton->setText(i18nc("@action", "Confirm"));
                }

                q->connect(confirmButton, SIGNAL(clicked()),
                           q, SLOT(slotConfirm()));
            }

            if (!rejectButton)
            {
                rejectButton = createToolButton(QIcon::fromTheme(QLatin1String("list-remove")), i18nc("@action", "Remove"));
                rejectButton->setToolTip(i18nc("@info:tooltip", "Remove this face region"));

                q->connect(rejectButton, SIGNAL(clicked()),
                           q, SLOT(slotReject()));
            }

            break;
        }

        case IgnoredMode:
        {
            if (layoutMode == Compact)
            {
                if (!confirmButton)
                {
                    confirmButton = createToolButton(QIcon::fromTheme(QLatin1String("edit-undo")), i18nc("@action", "OK"));
                    confirmButton->setToolTip(i18nc("@info:tooltip", "Unmark this face as Ignored"));

                    q->connect(confirmButton, SIGNAL(clicked()),
                               q, SLOT(slotIgnoredClicked()));
                }

                if (!rejectButton)
                {
                    rejectButton = createToolButton(QIcon::fromTheme(QLatin1String("list-remove")), i18nc("@action", "Remove"));
                    rejectButton->setToolTip(i18nc("@info:tooltip", "Remove this face region"));

                    q->connect(rejectButton, SIGNAL(clicked()),
                               q, SLOT(slotReject()));
                }
            }
            else
            {
                clickLabel = new DClickLabel;
                clickLabel->setAlignment(Qt::AlignCenter);

                connect(clickLabel, SIGNAL(activated()),
                        q, SLOT(slotIgnoredClicked()));
            }

            break;
        }

        case ConfirmedMode:
        {
            clickLabel = new DClickLabel;
            clickLabel->setAlignment(Qt::AlignCenter);

            connect(clickLabel, SIGNAL(activated()),
                    q, SLOT(slotLabelClicked()));

            break;
        }
    }
}

void AssignNameWidget::Private::layoutAddTagsWidget(bool exceedBounds, int minimumContentsLength)
{
    if      (comboBox)
    {
        comboBox->setMinimumContentsLength(minimumContentsLength);
        comboBox->lineEdit()->setAllowExceedBound(exceedBounds);
    }
    else if (lineEdit)
    {
        lineEdit->setAllowExceedBound(exceedBounds);
    }
}

void AssignNameWidget::Private::setSizePolicies(QSizePolicy::Policy h, QSizePolicy::Policy v)
{
    confirmButton->setSizePolicy(h, v);
    rejectButton->setSizePolicy(h, v);
    addTagsWidget()->setSizePolicy(h, v);
}

void AssignNameWidget::Private::setToolButtonStyles(Qt::ToolButtonStyle style)
{
    confirmButton->setToolButtonStyle(style);
    rejectButton->setToolButtonStyle(style);
}

void AssignNameWidget::Private::updateLayout()
{
    if (!isValid())
    {
        return;
    }

    delete layout;
    layout = new QGridLayout;

    switch (mode)
    {
        case InvalidMode:
        {
            break;
        }

        case UnconfirmedEditMode:
        case ConfirmedEditMode:
        {
            switch (layoutMode)
            {
                case InvalidLayout:
                {
                    break;
                }

                case FullLine:
                {
                    layout->addWidget(addTagsWidget(), 0, 0);
                    layout->addWidget(confirmButton,   0, 1);
                    layout->addWidget(rejectButton,    0, 2);
                    layout->setColumnStretch(0, 1);

                    setSizePolicies(QSizePolicy::Expanding, QSizePolicy::Preferred);
                    setToolButtonStyles(Qt::ToolButtonTextBesideIcon);
                    layoutAddTagsWidget(false, 15);

                    break;
                }

                case TwoLines:
                case Compact:
                {
                    layout->addWidget(addTagsWidget(), 0, 0, 1, 2);
                    layout->addWidget(confirmButton,   1, 0);
                    layout->addWidget(rejectButton,    1, 1);

                    setSizePolicies(QSizePolicy::Expanding, QSizePolicy::Minimum);

                    if (layoutMode == AssignNameWidget::TwoLines)
                    {
                        setToolButtonStyles(Qt::ToolButtonTextBesideIcon);
                        layoutAddTagsWidget(true, 10);
                    }
                    else
                    {
                        setToolButtonStyles(Qt::ToolButtonIconOnly);
                        layoutAddTagsWidget(true, 0);
                    }

                    break;
                }
            }

            break;
        }

        case IgnoredMode:
        {
            if (layoutMode == Compact)
            {
                layout->addWidget(confirmButton, 0, 0);
                layout->addWidget(rejectButton,  0, 1);

                confirmButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
                rejectButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
                setToolButtonStyles(Qt::ToolButtonIconOnly);
                layoutAddTagsWidget(true, 0);
            }
            else
            {
                layout->addWidget(clickLabel, 0, 0);
            }

            break;
        }

        case ConfirmedMode:
        {
            layout->addWidget(clickLabel, 0, 0);

            break;
        }
    }

    layout->setContentsMargins(1, 1, 1, 1);
    layout->setSpacing(1);
    q->setLayout(layout);
}

QString AssignNameWidget::Private::styleSheetFontDescriptor(const QFont& font) const
{
    QString s;

    s += (font.pointSize() == -1) ? QString::fromUtf8("font-size: %1px; ").arg(font.pixelSize())
                                  : QString::fromUtf8("font-size: %1pt; ").arg(font.pointSize());
    s += QString::fromUtf8("font-family: \"%1\"; ").arg(font.family());

    return s;
}

void AssignNameWidget::Private::updateVisualStyle()
{
    if (!isValid())
    {
        return;
    }

    QFont appFont = ApplicationSettings::instance()->getApplicationFont();

    if (appFont.pointSize() == -1)
    {
        appFont.setPixelSize(appFont.pixelSize() - 1);
    }
    else
    {
        appFont.setPointSize(appFont.pointSize() - 1);
    }

    switch (visualStyle)
    {
        case InvalidVisualStyle:
        {
            break;
        }

        case TranslucentDarkRound:
        {
            q->setStyleSheet(
                QString::fromUtf8(
                    "QWidget { "
                    " %1 "
                    "} "

                    "QFrame {"
                    "  background-color: rgba(0, 0, 0, 66%); "
                    "  border: 1px solid rgba(100, 100, 100, 66%); "
                    "  border-radius: %2px; "
                    "} "

                    "QToolButton { "
                    "  color: rgba(255, 255, 255, 220); "
                    "  padding: 1px; "
                    "  background-color: "
                    "    qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255, 255, 255, 100), "
                    "                    stop:1 rgba(255, 255, 255, 0)); "
                    "  border: 1px solid rgba(255, 255, 255, 127); "
                    "  border-radius: 4px; "
                    "} "

                    "QToolButton:hover { "
                    "  border-color: white; "
                    "} "

                    "QToolButton:pressed { "
                    "  background-color: "
                    "    qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255, 255, 255, 0), "
                    "                    stop:1 rgba(255, 255, 255, 100)); "
                    "  border-color: white; "
                    "} "

                    "QToolButton:disabled { "
                    "  color: rgba(255, 255, 255, 120); "
                    "  background-color: "
                    "    qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 rgba(255, 255, 255, 50), "
                    "                    stop:1 rgba(255, 255, 255, 0)); "
                    "} "

                    "QComboBox { "
                    "  color: white; "
                    "  background-color: transparent; "
                    "} "

                    "QComboBox QAbstractItemView, QListView::item:!selected { "
                    "  color: white; "
                    "  background-color: rgba(0, 0, 0, 80%); "
                    "} "

                    "QLabel { "
                    "  color: white; background-color: transparent; border: none; "
                    "}"
                ).arg(styleSheetFontDescriptor(appFont))
                 .arg(((mode == ConfirmedMode) || (mode == IgnoredMode)) ? QLatin1String("8")
                                                                         : QLatin1String("4"))
            );

            break;
        }

        case TranslucentThemedFrameless:
        {
            QColor bg = qApp->palette().color(QPalette::Base);
            q->setStyleSheet(
                QString::fromUtf8(
                    "QWidget { "
                    " %1 "
                    "} "

                    "QFrame#assignNameWidget {"
                    "  background-color: "
                    "    qradialgradient(cx:0.5 cy:0.5, fx:0, fy:0, radius: 1, stop:0 rgba(%2, %3, %4, 255), "
                    "                    stop:0.8 rgba(%2, %3, %4, 200), stop:1 rgba(%2, %3, %4, 0));"
                    "  border: none; "
                    "  border-radius: 8px; "
                    "}"
                ).arg(styleSheetFontDescriptor(appFont))
                 .arg(bg.red())
                 .arg(bg.green())
                 .arg(bg.blue())
            );

            break;
        }

        case StyledFrame:
        {
            q->setStyleSheet(QString());
            q->setFrameStyle(Raised | StyledPanel);

            break;
        }
    }
}

template <class T>
void AssignNameWidget::Private::setAddTagsWidgetContents(T* const widget)
{
    if (widget)
    {
        widget->setCurrentTag(currentTag);
        widget->setPlaceholderText((mode == UnconfirmedEditMode) ? i18nc("@label", "Who is this?")
                                                                 : QString());

        if (confirmButton)
        {
            confirmButton->setEnabled(widget->currentTaggingAction().isValid());
        }
    }
}

void AssignNameWidget::Private::updateContents()
{
    if (!isValid())
    {
        return;
    }

    if      (comboBox)
    {
        setAddTagsWidgetContents(comboBox);
    }
    else if (lineEdit)
    {
        setAddTagsWidgetContents(lineEdit);
    }

    if (clickLabel)
    {
        if (mode != IgnoredMode)
        {
            clickLabel->setText(currentTag ? currentTag->title()
                                           : QString());
        }
        else
        {
            int tagId = FaceTags::ignoredPersonTagId();
            clickLabel->setText(FaceTags::faceNameForTag(tagId));
        }
    }
}

void AssignNameWidget::Private::updateRejectButtonTooltip()
{
    if (!rejectButton)
    {
        return;
    }

    FaceTagsIface face = FaceTagsIface::fromVariant(faceIdentifier);

    if      (face.type() == FaceTagsIface::UnknownName)
    {
        rejectButton->setToolTip(i18nc("@info:tooltip", "Mark this face as Ignored"));
    }
    else if (face.type() == FaceTagsIface::UnconfirmedName)
    {
        rejectButton->setToolTip(i18nc("@info:tooltip", "Reject this suggestion"));
    }
}

} // namespace Digikam
