//  ***********************************************************************************************
//
//  libqcr: capture and replay Qt widget actions
//
//! @file      qcr/widgets/model_view.h
//! @brief     Defines classes TableModel and TableView
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2018-
//! @author    Joachim Wuttke
//
//  ***********************************************************************************************

#ifndef MODEL_VIEW_H
#define MODEL_VIEW_H

#include "qcr/engine/settable.h"
#include <QTreeView>

//! Pure virtual base class of all models of rectangular table form

class TableModel : public QAbstractTableModel, public QcrSettable {
public:
    TableModel() = delete;
    TableModel(const QString& name);

    virtual void executeConsoleCommand(const QString&);
    void refreshModel(); // within rectangle plus one row
    void resetModel(); // complete reset, including cursor position
    virtual void onClicked(const QModelIndex& cell);

    int columnCount(const QModelIndex& /*unused*/) const { return columnCount(); }
    int rowCount(const QModelIndex& /*unused*/) const { return rowCount(); }

    // interaction with data
    virtual int columnCount() const = 0;
    virtual int rowCount() const = 0;
    virtual int highlighted() const = 0;
    virtual void setHighlight(int i) = 0;
};

//! Pure virtual base class for rectangular table models with rows that can be checked.

class CheckTableModel : public TableModel {
public:
    CheckTableModel(const QString& name);

    void executeConsoleCommand(const QString&) override;
    void onActivated();
    void onClicked(const QModelIndex& cell) override;
    virtual bool activated(int row) const = 0;
    virtual void setActivated(int row, bool on) = 0;
private:
    void activateAndLog(bool primaryCall, int row, bool on);

};

//! Pure virtual base class of all views of rectangular table form

//! Based on QTreeView, with hidden 1st column.
//! QTreeView inherits from QAbstractItemView.

class TableView : public QTreeView {
public:
    TableView() = delete;
    TableView(TableModel*);

    void onData();
    void onHighlight();
protected:
    int mWidth() const;
    QString name_;
    TableModel* model_;
    void gotoCurrent(const QModelIndex&);
    void updateScroll();
    void highlight(bool primaryCall, int row);
};

//! Pure virtual base class for rectangular table models with rows that can be checked.

class CheckTableView : public TableView {
public:
    CheckTableView(TableModel* model) : TableView(model) {}
    void onActivated();
private:
    CheckTableModel* model() { return static_cast<CheckTableModel*>(model_); }
};

#endif // MODEL_VIEW_H
