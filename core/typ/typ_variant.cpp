// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      typ_variant.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle, Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "typ_variant.h"
#include "def/def_compare.h"
#include <QDate>
#include <QMetaType>

namespace typ {
//------------------------------------------------------------------------------

bool isNumeric(QVariant const& v) {
  auto type = QMetaType::Type(v.type());

  switch (type) {
  case QMetaType::Int:
  case QMetaType::UInt:
  case QMetaType::LongLong:
  case QMetaType::ULongLong:
  case QMetaType::Double:
  case QMetaType::Long:
  case QMetaType::Short:
  case QMetaType::ULong:
  case QMetaType::UShort:
  case QMetaType::Float:
    return true;
  default:
    return false;
  }
}

bool isReal(QVariant const& v) {
  auto type = QMetaType::Type(v.type());

  switch (type) {
  case QMetaType::Double:
  case QMetaType::Float:
    return true;
  default:
    return false;
  }
}

#define IMPL_CMP(name, toType)                       \
  int name(QVariant const& v1, QVariant const& v2) { \
    auto val1 = v1.toType(), val2 = v2.toType();     \
    COMPARE_VALUE2(val1, val2)                       \
    return 0;                                        \
  }

IMPL_CMP(cmp_int,  toInt)
IMPL_CMP(cmp_str,  toString)
IMPL_CMP(cmp_real, toDouble)
IMPL_CMP(cmp_date, toDate)


#undef IMPL_CMP

//------------------------------------------------------------------------------
}
// eof
