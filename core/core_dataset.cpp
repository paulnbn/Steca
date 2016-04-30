// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_dataset.h
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "core_dataset.h"
#include "core_session.h"
#include "core_lens.h"
#include <QStringList>

namespace core {
//------------------------------------------------------------------------------
// dataset attributes

enum {
  attrDATE, attrCOMMENT,

  attrMOTOR_XT,  attrMOTOR_YT,  attrMOTOR_ZT,
  attrMOTOR_OMG, attrMOTOR_TTH, attrMOTOR_PHI, attrMOTOR_CHI,
  attrMOTOR_PST, attrMOTOR_SST, attrMOTOR_OMGM,
  attrDELTA_MONITOR_COUNT, attrDELTA_TIME,

  NUM_ATTRIBUTES
};

uint Dataset::numAttributes() {
  return NUM_ATTRIBUTES;
}

rcstr Dataset::attributeTag(uint i) {
  static str_lst const attributeTags = {
    "date", "comment",
    "X", "Y", "Z",
    "ω", "2θ", "φ", "χ",
    "PST", "SST", "ΩM",
    "Δmon", "Δt",
  };

  return attributeTags.at(i);
}

Dataset::Dataset(
  rcstr date, rcstr comment,
  qreal motorXT,  qreal motorYT,  qreal motorZT,
  qreal motorOmg, qreal motorTth, qreal motorPhi, qreal motorChi,
  qreal motorPST, qreal motorSST, qreal motorOMGM,
  qreal deltaMonitorCount, qreal deltaTime,
  QSize const& size, inten_t const* intens)

: datasets_(nullptr), date_(date), comment_(comment)
, motorXT_(motorXT), motorYT_(motorYT), motorZT_(motorZT)
, motorOmg_(motorOmg), motorTth_(motorTth), motorPhi_(motorPhi), motorChi_(motorChi)
, motorPST_(motorPST), motorSST_(motorSST), motorOMGM_(motorOMGM)
, deltaMonitorCount_(deltaMonitorCount), deltaTime_(deltaTime)
, image_(size,intens) {
}

Dataset::Dataset(rcDataset that)
: datasets_(nullptr), date_(that.date_), comment_(that.comment_)
, motorXT_(that.motorXT_), motorYT_(that.motorYT_), motorZT_(that.motorZT_)
, motorOmg_(that.motorOmg_), motorTth_(that.motorTth_), motorPhi_(that.motorPhi_), motorChi_(that.motorChi_)
, motorPST_(that.motorPST_), motorSST_(that.motorSST_), motorOMGM_(that.motorOMGM_)
, deltaMonitorCount_(that.deltaMonitorCount_), deltaTime_(that.deltaTime_)
, image_(that.image_) {
}

rcDatasets Dataset::datasets() const {
  ASSERT(datasets_)
  return *datasets_;
}

shp_Dataset Dataset::combine(Datasets datasets) {
  if (datasets.count() <= 0)
    return shp_Dataset();

  qreal motorXT = 0, motorYT = 0,  motorZT = 0,
        motorOmg = 0, motorTth = 0, motorPhi = 0, motorChi = 0,
        motorPST = 0, motorSST = 0, motorOMGM = 0,
        deltaMonitorCount = 0, deltaTime = 0;

  // sums: deltaMonitorCount, deltaTime
  // the rest are averaged

  for (auto const& d: datasets) {
    motorXT   += d->motorXT_;
    motorYT   += d->motorYT_;
    motorZT   += d->motorZT_;

    motorOmg  += d->motorOmg_;
    motorTth  += d->motorTth_;
    motorPhi  += d->motorPhi_;
    motorChi  += d->motorChi_;

    motorPST  += d->motorPST_;
    motorSST  += d->motorSST_;
    motorOMGM += d->motorOMGM_;

    deltaMonitorCount += d->deltaMonitorCount_;
    deltaTime         += d->deltaTime_;
  }

  uint count = datasets.count();
  motorXT  /= count;
  motorYT  /= count;
  motorZT  /= count;

  motorOmg /= count;
  motorTth /= count;
  motorPhi /= count;
  motorChi /= count;

  motorPST  /= count;
  motorSST  /= count;
  motorOMGM /= count;

  // added intensities
  Image image = datasets.folded();

  // take string attributes from the first dataset
  auto const& first = *datasets.at(0);

  rcstr date    = first.date_;
  rcstr comment = first.comment_;

  return shp_Dataset(
    new Dataset(date, comment,
                motorXT, motorYT,  motorZT,  motorOmg, motorTth,
                motorPhi, motorChi, motorPST, motorSST, motorOMGM,
                deltaMonitorCount, deltaTime,
                image.size(), image.intensData()));
}

str Dataset::attributeStrValue(uint i) const {
  qreal value = 0;

  switch (i) {
  case attrDATE:        return date_;
  case attrCOMMENT:     return comment_;

  case attrMOTOR_XT:    value = motorXT_;   break;
  case attrMOTOR_YT:    value = motorYT_;   break;
  case attrMOTOR_ZT:    value = motorZT_;   break;
  case attrMOTOR_OMG:   value = motorOmg_;  break;
  case attrMOTOR_TTH:   value = motorTth_;  break;
  case attrMOTOR_PHI:   value = motorPhi_;  break;
  case attrMOTOR_CHI:   value = motorChi_;  break;
  case attrMOTOR_PST:   value = motorPST_;  break;
  case attrMOTOR_SST:   value = motorSST_;  break;
  case attrMOTOR_OMGM:  value = motorOMGM_; break;
  case attrDELTA_MONITOR_COUNT: value = deltaMonitorCount_; break;
  case attrDELTA_TIME:  value = deltaTime_; break;
  }

  return str::number(value);
}

QSize Dataset::imageSize() const {
  return image_.size();
}

inten_t Dataset::inten(uint i, uint j) const {
  return image_.inten(i,j);
}

//------------------------------------------------------------------------------

Datasets::Datasets() {
  invalidateMutables();
}

void Datasets::appendHere(shp_Dataset dataset) {
  // a dataset can be added to Datasets only once
  ASSERT(!dataset->datasets_)
  dataset->datasets_ = this;

  super::append(dataset);
  invalidateMutables();
}

Image Datasets::folded() const THROWS {
  ASSERT(0 < count())

  Image image(first()->imageSize());
  for (auto const& dataset: *this)
    image.addIntens(dataset->image_);

  return image;
}

QSize Datasets::imageSize() const {
  if (super::isEmpty())
    return QSize(0,0);

  // guaranteed that all images have the same size; simply take the first one
  return super::first()->imageSize();
}

qreal Datasets::avgDeltaMonitorCount() const {
  if (qIsNaN(avgMonitorCount_)) {
    qreal avg = 0;

    for (auto const& dataset: *this)
      avg += dataset->deltaMonitorCount();

    if (!super::isEmpty())
      avg /= super::count();

    avgMonitorCount_ = avg;
  }

  return avgMonitorCount_;
}

qreal Datasets::avgDeltaTime() const {
  if (qIsNaN(avgDeltaTime_)) {
    qreal avg = 0;

    for (auto const& dataset: *this)
      avg += dataset->deltaTime();

    if (!super::isEmpty())
      avg /= super::count();

    avgDeltaTime_ = avg;
  }

  return avgDeltaTime_;
}

rcRange Datasets::rgeFixedInten(rcSession session, bool trans, bool cut) const {
  if (!rgeFixedInten_.isValid()) {
    for (auto const& dataset: *this) {
      auto const& image = dataset->image();
      shp_ImageLens imageLens = session.lens(image,*this,trans,cut);
      rgeFixedInten_.extendBy(imageLens->rgeInten(false));
    }
  }

  return rgeFixedInten_;
}

rcCurve Datasets::makeAvgCurve(rcSession session, bool trans, bool cut) const {
  if (!avgCurve_.isEmpty())
    return avgCurve_;

  Curve res;

  for (auto const& dataset: *this) {
    shp_Lens lens = session.lens(*dataset,*this,trans,cut,session.norm());
    Curve single = lens->makeCurve(lens->angleMap().rgeGamma(),lens->angleMap().rgeTth());
    res = res.add(single);
  }

  if (!isEmpty())
    res = res.mul(1. / count());

  return (avgCurve_ = res);
}

void Datasets::invalidateMutables() {
  avgMonitorCount_ = avgDeltaTime_ = qQNaN();
  rgeFixedInten_.invalidate();
  avgCurve_.clear();
}

//------------------------------------------------------------------------------
}
// eof
