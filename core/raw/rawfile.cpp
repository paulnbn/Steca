//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/raw/rawfile.cpp
//! @brief     Implements class Rawfile
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "core/raw/rawfile.h"
#include <QStringBuilder> // for ".." % ..

Rawfile::Rawfile(const QString& fileName)
    : fileInfo_(fileName)
{}

//! The loaders use this function to push cluster
void Rawfile::addDataset(const Metadata& md, const size2d& sz, const QVector<float>& ivec)
{
    if (!measurements_.size())
        imageSize_ = sz;
    else if (sz != imageSize_)
        THROW("Inconsistent image size in " % fileName());
    measurements_.push_back({(int)measurements_.size(), md, sz, ivec});
}

QVector<const Measurement*> const Rawfile::measurements() const
{
    QVector<const Measurement*> ret;
    for (const Measurement& one: measurements_)
        ret.append(&one);
    return ret;
}

Image Rawfile::summedImage() const
{
    ASSERT(measurements_.size());
    Image ret(measurements_.front().imageSize(), 0.);
    for (const Measurement& one : measurements_)
        ret.addImage(one.image());
    return ret;
}
