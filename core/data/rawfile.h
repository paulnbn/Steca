// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      core/data/rawfile.h
//! @brief     Defines class Rawfile
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef RAWFILE_H
#define RAWFILE_H

#include "core/data/image.h"
#include "core/data/measurement.h"
#include "core/typ/array2d.h"
#include "core/typ/str.h"
#include "core/typ/types.h"
#include <QFileInfo>

class Metadata;

//! A file (loaded from a disk file) that contains a data sequence.
class Rawfile final {
public:
    Rawfile() = delete;
    Rawfile(const Rawfile&) = delete;
    // allow move so that the low-level loaders must not bother about shared pointers:
    Rawfile(Rawfile&&) = default;
    Rawfile(rcstr fileName);

    void addDataset(const Metadata&, size2d const&, inten_vec const&);

    QVector<const Measurement*> const measurements() const;
    int count() const { return measurements_.count(); }
    size2d imageSize() const { return imageSize_; }

    QFileInfo const& fileInfo() const;
    str fileName() const;
    shp_Image foldedImage() const;

private:
    QFileInfo fileInfo_;
    vec<shp_Measurement> measurements_;
    size2d imageSize_;
};

Q_DECLARE_METATYPE(const Rawfile*)

#endif // RAWFILE_H