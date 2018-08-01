//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/fit/fit_fun.cpp
//! @brief     Implements classes Polynom and PeakFunction, and FunctionRegistry
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "fit_fun.h"
#include "core/fit/fit_methods.h"
#include "qcr/base/debug.h"

namespace {

//! Computes a low integer power of x.
static double pow_n(double x, int n)
{
    double ret = 1;
    while (n-- > 0)
        ret *= x;
    return ret;
}

} // namespace

//  ***********************************************************************************************
//! @class Polynom

double Polynom::y(double x, double const* parValues) const
{
    double ret = 0, xPow = 1;
    for (int i=0; i<parameters_.size(); ++i) {
        ret += parValue(i, parValues) * xPow;
           // outside the fit routine, functions y(x) are called with parValues==nullptr;
           // therefore we need 'parValue' to access either 'parValues' or 'parameters_'.
        xPow *= x;
    }
    return ret;
}

double Polynom::dy(double x, int i, double const*) const
{
    return pow_n(x, i);
}

Polynom Polynom::fromFit(int degree, const Curve& curve, const Ranges& ranges)
{
    ASSERT(curve.count()>0);
    ASSERT(ranges.count()>0);
    Polynom p(degree);
    FitWrapper().execFit(p, curve.intersect(ranges));
    return p;
}

//  ***********************************************************************************************
//! @class PeakFunction

PeakFunction::PeakFunction()
    : guessedPeak_(), guessedFWHM_(Q_QNAN)
{}

void PeakFunction::doFit(const Curve& curve, const Range& range)
{
    const Curve c = prepareFit(curve, range);
    if (c.isEmpty())
        return;

    //  if (!guessedPeak().isValid()) {  // calculate guesses // TODO caching
    //  temporarily disabled, until it works correctly
    const int peakIndex = c.idxMax();
    const double peakTth = c.x(peakIndex);
    const double peakIntens = c.y(peakIndex);

    // half-maximum indices
    int hmi1 = peakIndex, hmi2 = peakIndex;
    // left
    for (int i = peakIndex; i-- > 0;) {
        hmi1 = i;
        if (c.y(i) < peakIntens / 2)
            break;
    }
    // right
    for (int i = peakIndex, iCnt = c.count(); i < iCnt; ++i) {
        hmi2 = i;
        if (c.y(i) < peakIntens / 2)
            break;
    }

    setGuessedPeak(qpair(peakTth, peakIntens));
    setGuessedFWHM(c.x(hmi2) - c.x(hmi1));

    FitWrapper().execFit(*this, c);
}

Curve PeakFunction::prepareFit(const Curve& curve, const Range& range)
{
    reset();
    return curve.intersect(range);
}
