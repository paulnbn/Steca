//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/engine/settable.cpp
//! @brief     Implements classes QcrSettable, CModal, CModelessDialog
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#include "settable.h"
#include "qcr/engine/console.h"
#include "qcr/engine/qcrexception.h"
#include <QDebug>

//  ***********************************************************************************************
//! @class QcrSettable

QcrSettable::QcrSettable(const QString& name)
    : name_ {gConsole->learn(name, this)}
{
}

QcrSettable::~QcrSettable()
{
    gConsole->forget(name_);
}

void QcrSettable::doLog(bool softwareCalled, const QString& msg)
{
    gConsole->log2(!softwareCalled, msg);
}


//  ***********************************************************************************************
//! @class CModal

CModal::CModal(const QString& name)
{
    gConsole->call("@push "+name);
}

CModal::~CModal()
{
    gConsole->log("@close");
    gConsole->call("@pop");
}


//  ***********************************************************************************************
//! @class CModelessDialog

CModelessDialog::CModelessDialog(QWidget* parent, const QString& name)
    : QDialog(parent)
    , QcrSettable(name)
{
    setModal(false);
}

void CModelessDialog::closeEvent(QCloseEvent* event)
{
    deleteLater();
}

void CModelessDialog::executeConsoleCommand(const QString& arg)
{
    if (arg!="close")
        throw QcrException("Unexpected command in ModelessDialog "+name());
    close();
}
