//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/dfgram.cpp
//! @brief     Implements class Dfgram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/data/dfgram.h"
#include "core/session.h"

Polynom Dfgram::bgPolynom()
{
    return Polynom::fromFit(
        gSession->baseline.polynomDegree.val(), curve, gSession->baseline.ranges);
}
