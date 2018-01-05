// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/output/widgets4output.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef WIDGETS4OUTPUT_H
#define WIDGETS4OUTPUT_H

#include "typ/variant.h"
#include "def/special_pointers.h"
#include "panels/panel.h"
#include "widgets/tree_views.h"

class QCheckBox;
class QGridLayout;
class QRadioButton;


class Params : public QWidget {
public:
    enum ePanels {
        REFLECTION = 0x01,
        GAMMA = 0x02,
        POINTS = 0x04,
        INTERPOLATION = 0x08,
        DIAGRAM = 0x10,
    };
    Params(ePanels);
    ~Params();
    class PanelReflection* panelReflection;
    class PanelGammaSlices* panelGammaSlices;
    class PanelGammaRange* panelGammaRange;
    class PanelPoints* panelPoints;
    class PanelInterpolation* panelInterpolation;
    class PanelDiagram* panelDiagram;
    str saveDir, saveFmt;
    void readSettings();
    void saveSettings() const;
    QBoxLayout* box_;
};


class Table : public TreeView {
public:
    Table(uint numDataColumns);
    void setColumns(QStringList const& headers, QStringList const& outHeaders, cmp_vec const&);
    QStringList const outHeaders() { return outHeaders_; }
    void clear();
    void addRow(row_t const&, bool sort);
    void sortData();
    uint rowCount() const;
    row_t const& row(uint) const;
    scoped<class TabularModel*> model_;
    QStringList outHeaders_;
};


class OutputTab : public QWidget {
public :
    OutputTab(Params&);
protected:
    Params& params_;
    QGridLayout* grid_;
};


class TabSave : public OutputTab {
public:
    TabSave(Params&, bool withTypes);
    str filePath(bool withSuffix);
    str separator() const;
    QAction *actBrowse, *actSave;
protected:
    str fileSetSuffix(rcstr);
    QLineEdit *dir_, *file_;
    QRadioButton *rbDat_, *rbCsv_;
};

#endif // WIDGETS4OUTPUT_H