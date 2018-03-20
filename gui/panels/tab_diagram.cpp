// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_diagram.cpp
//! @brief     Implements class DiagramWidget, and its dependences
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "tab_diagram.h"
#include "core/session.h"
#include "gui/actions/toggles.h"
#include "gui/actions/triggers.h"
#include <iostream>


// ************************************************************************** //
//  class DiagramWidget
// ************************************************************************** //

DiagramWidget::DiagramWidget()
{
    // internal connections
//    connect(&gGui->toggles->enableCorr, &QAction::toggled, [this](bool /*unused*/) { render(); });

    // layout
    /*
    box1_.addWidget(&btnScale_, Qt::AlignLeft);
    box1_.addWidget(&btnOverlay_, Qt::AlignLeft);
    controls_.addLayout(&box1_);

    box_.addLayout(&controls_);
    box_.addWidget(&imageView_);
    setLayout(&box_);
    */
}

void DiagramWidget::render()
{
}
