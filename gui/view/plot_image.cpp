//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/tab_image.cpp
//! @brief     Implements class ImageView
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/view/plot_image.h"
#include "core/session.h"
#include "gui/mainwin.h"
#include "gui/view/toggles.h"
//#include "qcr/base/debug.h"
#include <QPainter>

//  ***********************************************************************************************
//! @class ImageView

ImageView::ImageView()
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(&gGui->toggles->crosshair, &QAction::toggled, [this](bool /*unused*/) { update(); });
}

void ImageView::setPixmap(const QPixmap& pixmap)
{
    original_ = pixmap;
    setScale();
}

void ImageView::setScale()
{
    if (original_.isNull()) {
        scale_ = 0;
    } else {
        const QSize& sz = size();
        const QSize& os = original_.size();
        scale_ = qMin(double(sz.width() - 2) / os.width(), double(sz.height() - 2) / os.height());
    }

    if (scale_ <= 0)
        scaled_ = QPixmap();
    else
        scaled_ = original_.scaled(original_.size() * scale_);

    update();
}

void ImageView::resizeEvent(QResizeEvent* e)
{
    QWidget::resizeEvent(e);
    setScale();
}

void ImageView::paintEvent(QPaintEvent*)
{
    // paint centered
    const QSize margin = (size() - scaled_.size()) / 2;
    const QRect rect(QPoint(margin.width(), margin.height()), scaled_.size());

    QPainter p(this);

    // image
    p.drawPixmap(rect.left(), rect.top(), scaled_);

    // crosshair overlay
    if (gGui->toggles->crosshair.getValue()) {
        p.setPen(Qt::lightGray);

        // cut
        const ImageCut& cut = gSession->params.imageCut;
        const QRect r = rect.adjusted(-1, -1, 0, 0)
                      .adjusted(
                          qRound(scale_ * cut.left.val()), qRound(scale_ * cut.top.val()),
                          -qRound(scale_ * cut.right.val()), -qRound(scale_ * cut.bottom.val()));
        p.drawRect(r);

        const QPoint rc = r.center();
        const int rcx = rc.x(), rcy = rc.y();

        int rl, rt, rr, rb;
        r.getCoords(&rl, &rt, &rr, &rb);
        const int rw = rr - rl;

        // cross
        const int x = qRound(rcx + scale_ * gSession->params.detector.pixOffset[0].val());
        const int y = qRound(rcy + scale_ * gSession->params.detector.pixOffset[1].val());
        p.drawLine(x, rt, x, rb);
        p.drawLine(rl, y, rr, y);

        // text
        QPoint pos(rr - rw / 5, rcy);
        p.setPen(Qt::cyan);
        p.drawText(pos, "γ=0");
    }

    // frame
    p.setPen(Qt::black);
    p.drawRect(rect.adjusted(-1, -1, 0, 0));
}
