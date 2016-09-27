// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      panel_options.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "panel_options.h"
#include "thehub.h"

namespace gui { namespace panel {
//------------------------------------------------------------------------------

DatasetOptions1::DatasetOptions1(TheHub& hub)
: super(hub, Qt::Vertical)
{
  box_->addWidget(label("Beam offset"));
  auto ho = hbox();
  box_->addLayout(ho);

  ho->addWidget(label("X"));
  ho->addWidget((spinOffsetI_ = spinCell(4, 0)));
  spinOffsetI_->setToolTip("Horizontal offset from image center");
  ho->addWidget(label("Y"));
  ho->addWidget((spinOffsetJ_ = spinCell(4, 0)));
  spinOffsetJ_->setToolTip("Vertical offset from image center");
  ho->addWidget(label("pix"));
  ho->addWidget(iconButton(hub_.actions.hasBeamOffset));
  ho->addStretch();

  box_->addWidget(label("Detector"));
  auto gd = gridLayout();
  box_->addLayout(gd);

  gd->addWidget(
      (spinDistance_ = spinCell(6, typ::Geometry::MIN_DETECTOR_DISTANCE)), 0,
      0);
  spinDistance_->setToolTip("Sample to detector distance");
  gd->addWidget(label("distance mm"), 0, 1);
  gd->addWidget(
      (spinPixelSize_ = spinCell(6, typ::Geometry::MIN_DETECTOR_PIXEL_SIZE)),
      1, 0);
  spinPixelSize_->setSingleStep(.1);
  spinPixelSize_->setToolTip("Physical pixel size");
  gd->addWidget(label("pixel size mm"), 1, 1);
  gd->setColumnStretch(2, 1);

  box_->addStretch();

  onSigGeometryChanged([this]() {
    setFrom(hub_);
  });

  auto setEnabled = [this]() {
    bool on = hub_.actions.hasBeamOffset->isChecked();
    spinOffsetI_->setEnabled(on);
    spinOffsetJ_->setEnabled(on);
  };

  setEnabled();

  connect(hub_.actions.hasBeamOffset, &QAction::toggled, [this, setEnabled]() {
    setEnabled();
    setTo(hub_);
  });

  connect(spinOffsetI_, slot(QSpinBox,valueChanged,int), [this]() {
    setTo(hub_);
  });

  connect(spinOffsetJ_, slot(QSpinBox,valueChanged,int), [this]() {
    setTo(hub_);
  });

  connect(spinDistance_, slot(QDoubleSpinBox,valueChanged,double), [this]() {
    setTo(hub_);
  });

  connect(spinPixelSize_, slot(QDoubleSpinBox,valueChanged,double), [this]() {
    setTo(hub_);
  });
}

void DatasetOptions1::setTo(TheHub& hub) {
  hub.setGeometry(
    preal(qMax(qreal(typ::Geometry::MIN_DETECTOR_DISTANCE),   spinDistance_->value())),
    preal(qMax(qreal(typ::Geometry::MIN_DETECTOR_PIXEL_SIZE), spinPixelSize_->value())),
    hub.actions.hasBeamOffset->isChecked(),
    typ::IJ(spinOffsetI_->value(), spinOffsetJ_->value()));
}

void DatasetOptions1::setFrom(TheHub& hub) {
  auto& g = hub.geometry();

  hub.actions.hasBeamOffset->setChecked(g.isMidPixOffset);
  spinOffsetI_->setValue(g.midPixOffset.i);
  spinOffsetJ_->setValue(g.midPixOffset.j);

  spinDistance_->setValue(g.detectorDistance);
  spinPixelSize_->setValue(g.pixSize);
}

static str const GROUP_OPTIONS("Options");

static str const GROUP_BEAM("Beam");
static str const KEY_IS_OFFSET("is_offset");
static str const KEY_OFFSET_X("offset_x");
static str const KEY_OFFSET_Y("offset_y");

static str const GROUP_DETECTOR("Detector");
static str const KEY_DISTANCE("distance");
static str const KEY_PIXEL_SIZE("pixel_size");

//------------------------------------------------------------------------------

DatasetOptions2::DatasetOptions2(TheHub& hub)
: super(hub, Qt::Vertical)
{
  box_->addWidget(label("Image"));
  auto hb = hbox();
  box_->addLayout(hb);

  hb->addWidget(iconButton(hub_.actions.rotateImage));
  hb->addSpacing(5);
  hb->addWidget(iconButton(hub_.actions.mirrorImage));
  hb->addSpacing(5);
  hb->addWidget(iconButton(hub_.actions.fixedIntenImageScale));
  hb->addStretch();

  auto sc = hbox();
  box_->addLayout(sc);
  sc->addWidget(label("Scale"));
  sc->addSpacing(5);
  sc->addWidget((sliderImageScale_ = new QSlider(Qt::Horizontal)));
  sliderImageScale_->setToolTip("Image scale");
  sliderImageScale_->setRange(-5, +2);
  sliderImageScale_->setTickPosition(QSlider::TicksBelow);
  sc->addStretch();

  box_->addWidget(label("Cut"));
  auto gc = gridLayout();
  box_->addLayout(gc);

  gc->addWidget(icon(":/icon/cutTop"), 0, 0);
  gc->addWidget((marginTop_ = spinCell(4, 0)), 0, 1);
  marginTop_->setToolTip("Top cut");
  gc->addWidget(icon(":/icon/cutBottom"), 0, 2);
  gc->addWidget((marginBottom_ = spinCell(4, 0)), 0, 3);
  marginBottom_->setToolTip("Bottom cut");

  gc->addWidget(iconButton(hub_.actions.linkCuts), 0, 5);
  gc->addWidget(iconButton(hub_.actions.showOverlay), 1, 5);

  gc->addWidget(icon(":/icon/cutLeft"), 1, 0);
  gc->addWidget((marginLeft_ = spinCell(4, 0)), 1, 1);
  marginLeft_->setToolTip("Left cut");
  gc->addWidget(icon(":/icon/cutRight"), 1, 2);
  gc->addWidget((marginRight_ = spinCell(4, 0)), 1, 3);
  marginRight_->setToolTip("Right cut");
  gc->setColumnStretch(4, 1);

  box_->addWidget(label("Normalisation"));
  auto vn = vbox();
  box_->addLayout(vn);

  str_lst options = normStrLst();

  vn->addWidget((comboNormType_ = comboBox(options)));

  box_->addStretch();

  auto setImageCut = [this](bool topLeft, int value) {
    EXPECT(value >= 0)
    if (hub_.actions.linkCuts->isChecked())
      hub_.setImageCut(topLeft, true, typ::ImageCut(
                       to_u(value), to_u(value), to_u(value), to_u(value)));
    else
      hub_.setImageCut(topLeft, false,
                       typ::ImageCut(to_u(marginLeft_->value()),  to_u(marginTop_->value()),
                                      to_u(marginRight_->value()), to_u(marginBottom_->value())));
  };

  connect(marginLeft_, slot(QSpinBox,valueChanged,int), [setImageCut](int value) {
    setImageCut(true, value);
  });

  connect(marginTop_, slot(QSpinBox,valueChanged,int), [setImageCut](int value) {
    setImageCut(true, value);
  });

  connect(marginRight_, slot(QSpinBox,valueChanged,int), [setImageCut](int value) {
    setImageCut(false, value);
  });

  connect(marginBottom_, slot(QSpinBox,valueChanged,int), [setImageCut](int value) {
    setImageCut(false, value);
  });

  onSigGeometryChanged([this]() {
    setFrom(hub_);
  });

  connect(sliderImageScale_, slot(QSlider,valueChanged,int), [this](int value) {
    preal scale(1);

    if (value > 0)
      scale = preal(value + 1);
    else if (value < 0)
      scale = preal(1. / (-value + 1));

    emit imageScale(scale);
  });

  connect(comboNormType_, slot(QComboBox,currentIndexChanged,int), [this](int index) {
    hub_.setNorm(eNorm(index));
  });

  onSigSessionCleared([this]() {
    sliderImageScale_->setValue(sliderImageScale_->minimum());
  });
}

void DatasetOptions2::setFrom(TheHub& hub) {
  auto margins = hub.imageCut();

  marginLeft_  ->setValue(to_i(margins.left));
  marginTop_   ->setValue(to_i(margins.top));
  marginRight_ ->setValue(to_i(margins.right));
  marginBottom_->setValue(to_i(margins.bottom));
}

//------------------------------------------------------------------------------
}}
// eof