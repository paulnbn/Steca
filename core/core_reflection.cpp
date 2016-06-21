// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_reflection.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "core_reflection.h"
#include "types/core_json.h"
#include <QStringList>

namespace core {
//------------------------------------------------------------------------------

str_lst const& Reflection::typeStrLst() {
  static str_lst types{"Raw", "Gaussian", "Lorentzian", "PseudoVoigt1",
                       "PseudoVoigt2"};
  return types;
}

rcstr Reflection::typeTag(ePeakType type) {
  return typeStrLst().at(uint(type));
}

Reflection::Reflection(ePeakType type) : peakFunction_(nullptr) {
  setPeakFunction(type);
}

ePeakType Reflection::type() const {
  return peakFunction_->type();
}

void Reflection::setType(ePeakType type) {
  setPeakFunction(type);
}

fit::PeakFunction const& Reflection::peakFunction() const {
  ENSURE(peakFunction_)
  return *peakFunction_;
}

rcRange Reflection::range() const {
  return peakFunction_->range();
}

void Reflection::setRange(rcRange range) {
  peakFunction_->setRange(range);
}

void Reflection::invalidateGuesses() {
  peakFunction_->setGuessedPeak(XY());
  peakFunction_->setGuessedFWHM(qQNaN());
}

void Reflection::fit(rcCurve curve) {
  peakFunction_->fit(curve);
}

void Reflection::setPeakFunction(ePeakType type) {
  bool  haveRange = !peakFunction_.isNull();
  Range oldRange;
  if (haveRange) oldRange = peakFunction_->range();

  setPeakFunction(fit::PeakFunction::factory(type));

  if (haveRange) peakFunction_->setRange(oldRange);
}

void Reflection::setPeakFunction(fit::PeakFunction* f) {
  peakFunction_.reset(f);
}

JsonObj Reflection::saveJson() const {
  return peakFunction_->saveJson();
}

void Reflection::loadJson(rcJsonObj obj) THROWS {
  QScopedPointer<fit::Function> f(fit::Function::factory(obj));

  RUNTIME_CHECK(dynamic_cast<fit::PeakFunction*>(f.data()),
                "must be a peak function");
  setPeakFunction(static_cast<fit::PeakFunction*>(f.take()));
}

//------------------------------------------------------------------------------
}
// eof
