//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/calc/peak_info.cpp
//! @brief     Implements class PeakInfo
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/calc/peak_info.h"
#include "core/pars/onepeak_settings.h"
#include "core/session.h"
//#include "qcr/base/debug.h"

//  ***********************************************************************************************
//! @class PeakInfo

PeakInfo::PeakInfo(const Metadata* md, const Mapped& outcome)
    : md_{md}
    , outcome_{outcome}
{}

std::vector<QVariant> PeakInfo::peakData() const
{
    std::vector<QVariant> ret;
    for (const QString& key: {"alpha", "beta", "gamma_min", "gamma_max"})
        ret.push_back( QVariant(outcome_.at(key)) );
    for (const QString& key: {"intensity", "center", "fwhm", "gammaOverSigma"}) {
        if (outcome_.has(key)) {
            ret.push_back( QVariant(outcome_.at(key)) );
            ret.push_back( QVariant(outcome_.at("sigma_"+key)) );
        }
    }
    auto values_to_append = md_ ? md_->attributeValues() : Metadata::attributeNaNs();
    ret.insert(ret.end(), values_to_append.begin(), values_to_append.end());
    return ret;
}
