// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      panel.h
//! @brief     Gui panels (in the main window).
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef PANEL_H
#define PANEL_H

#include "refhub.h"
#include "gui_helpers.h"
#include <QGroupBox>

namespace models {
class TableModel;
}

namespace gui { namespace panel {
//------------------------------------------------------------------------------

/// Just a plain panel
class BasicPanel: public QGroupBox, protected RefHub {
  SUPER(BasicPanel,QGroupBox)
public:
  BasicPanel(rcstr title,TheHub&);

  void setHorizontalStretch(int);
  void setVerticalStretch(int);
  void setStretch(int horizontal, int vertical);
};

/// A panel with a box layout
class BoxPanel: public BasicPanel {
  SUPER(BoxPanel,BasicPanel)
public:
  BoxPanel(rcstr title, TheHub&, Qt::Orientation);

protected:
  QBoxLayout *box_;
};

/// A panel with grid layout
class GridPanel: public BasicPanel {
  SUPER(GridPanel,BasicPanel)
public:
  GridPanel(rcstr title,TheHub&);

protected:
  QGridLayout *grid_;
};

//------------------------------------------------------------------------------

/// A tabbed panel
class TabsPanel: public QTabWidget, protected RefHub {
  SUPER(TabsPanel,QTabWidget)
public:
  TabsPanel(TheHub&);

  struct Tab: public QWidget {
    Tab(Qt::Orientation);
    QBoxLayout *box;
  };

  Tab& addTab(rcstr title, Qt::Orientation = Qt::Vertical);
  Tab& tab(uint);
};

//------------------------------------------------------------------------------
}}
#endif // PANEL_H
