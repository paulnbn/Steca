//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/controls.cpp
//! @brief     Implements enhanced control widgets like QcrSpinBox, QcrRadioButton, and many others
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "qcr/widgets/actions.h"
#include "qcr/base/qcrexception.h"
#include "qcr/base/string_ops.h"
#include "qcr/engine/console.h"
//#include "qcr/base/debug.h"
#include <QApplication> // for qApp for new Action
#include <iostream> // debug

#define _SLOT_(Class, method, argType) static_cast<void (Class::*)(argType)>(&Class::method)

//  ***********************************************************************************************
//! @class QcrAction

QcrAction::QcrAction(const QString& text)
    : QAction(text, qApp)
    , tooltip_(text)
{}

bool QcrAction::hasFocus()
{
    for (const QWidget* w: associatedWidgets())
        if (w->hasFocus())
            return true;
    return false;
}

//  ***********************************************************************************************
//! @class QcrTrigger

QcrTrigger::QcrTrigger(const QString& rawname, const QString& text, const QString& iconFile)
    : QcrAction {text}
    , QcrRegisteredMixin {this, rawname}
{
    //QAction::setObjectName(name());
    if (iconFile!="")
        setIcon(QIcon(iconFile));
    connect(this, &QAction::triggered, [this]()->void {
            gConsole->log(name());
            triggerHook_();
            gRoot->remakeAll(); });
    connect(this, &QAction::changed, [this]()->void {
            QString txt = tooltip_;
            if (!isEnabled())
                txt += "\nThis trigger is currently inoperative.";
            setToolTip(txt); });
}

QcrTrigger::QcrTrigger(
    const QString& name, const QString& text, const QString& iconFile, const QKeySequence& shortcut)
    : QcrTrigger {name, text, iconFile}
{
    setShortcut(shortcut);
}

void QcrTrigger::setFromCommand(const QString& arg)
{
    if (arg!="")
        throw QcrException("Found unexpected argument to trigger command");
    if (!isEnabled()) {
        qDebug() << "Ignoring command" << arg << "because trigger" << name() << "is not enabled.";
        return;
    }
    trigger();
}

//  ***********************************************************************************************
//! trigger button classes

QcrTextTriggerButton::QcrTextTriggerButton(QcrTrigger* action)
    : QcrBaseMixin(this, action->name()+"Btn")
{
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonTextOnly);
    setRemake([=](){action->remake();});
}

QcrIconTriggerButton::QcrIconTriggerButton(QcrTrigger* action)
    : QcrBaseMixin(this, action->name()+"Btn")
{
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setRemake([=](){action->remake();});
}

//  ***********************************************************************************************
//! @class QcrToggle

QcrToggle::QcrToggle(const QString& rawname, const QString& text, bool on,
                     const QString& iconFile, const QKeySequence& shortcut)
    : QcrAction {text}
    , QcrSingleValue<bool> {this, rawname, on}
{
    initToggle(iconFile, shortcut);
}

QcrToggle::QcrToggle(const QString& rawname, QcrCell<bool>* cell, const QString& text,
                     const QString& iconFile, const QKeySequence& shortcut)
    : QcrAction {text}
    , QcrSingleValue<bool> {this, rawname, cell}

{
    initToggle(iconFile, shortcut);
}

void QcrToggle::initToggle(const QString& iconFile, const QKeySequence& shortcut)
{
    setShortcut(shortcut);
    setCheckable(true);
    //QAction::setObjectName(QcrRegisteredMixin::name());
    if (iconFile!="")
        setIcon(QIcon(iconFile));
    doSetValue(cell_->val());
    connect(this, &QAction::toggled, this, [this](bool val){
            //qDebug()<<"TOGGLE "<<name()<<"toggled";
            onChangedValue(val);});
    connect(this, &QAction::changed, [this]()->void {
            // Called upon any property change.
            // Also called when this is toggled (https://bugreports.qt.io/browse/QTBUG-68213)
            //qDebug()<<"TOGGLE "<<name()<<"changed";
            QString txt = tooltip_;
            if (!isEnabled())
                txt += "\nThis toggle is currently inoperative.";
            else if (isChecked())
                txt += "\nThis toggle is currently checked. Click to uncheck.";
            else
                txt += "\nThis toggle is currently unchecked. Click to check.";
            setToolTip(txt); });
}

//  ***********************************************************************************************
//! toggle button classes

QcrTextToggleButton::QcrTextToggleButton(QcrToggle* action)
    : QcrBaseMixin(this, action->name()+"Btn")
{
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonTextOnly);
    setRemake([=](){action->remake();});
}

QcrIconToggleButton::QcrIconToggleButton(QcrToggle* action)
    : QcrBaseMixin(this, action->name()+"Btn")
{
    setDefaultAction(action);
    setToolButtonStyle(Qt::ToolButtonIconOnly);
    setRemake([=](){action->remake();});
}
