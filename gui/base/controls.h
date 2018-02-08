// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/base/controls.h
//! @brief     Defines functions that return new Qt objects
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef CONTROLS_H
#define CONTROLS_H

#include <functional> // no auto rm
#include <QAction>
#include <QCheckBox>
#include <QComboBox>
#include <QFile>
#include <QFileDialog>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpinBox>
#include <QString>
#include <QToolButton>

//! Contains functions that return new QActions.

namespace newT {

QAction* Trigger(const QString& name, const QString& text, const QString& iconFile="");
QAction* Toggle(const QString& name, const QString& text, bool value, const QString& iconFile="");

} // namespace newT

//! QToolButton with text display and associated QAction.
class XTextButton : public QToolButton {
public:
    XTextButton(QAction*);
};

//! QToolButton with icon and associated QAction.
class XIconButton : public QToolButton {
public:
    XIconButton(QAction*);
};

//! Mix-in for control widgets that can be changed by a console command.
class CSettable {
public:
    CSettable() = delete;
    CSettable(const QString& name, std::function<void(const QString&)> setter);
    ~CSettable();
    QString name() const { return name_; }
private:
    const QString name_;
};

//! Named QSpinBox that can be set by console command.
class CSpinBox : public QSpinBox, private CSettable {
public:
    CSpinBox(const QString& name, int ndigits, bool withDot, int min = INT_MIN, int max = INT_MAX);
};

//! Named QDoubleSpinBox that can be set by console command.
class CDoubleSpinBox : public QDoubleSpinBox, private CSettable {
public:
    CDoubleSpinBox(const QString& name, int ndigits, qreal min = LLONG_MIN, qreal max = LLONG_MAX);
};

//! Named QCheckBox that can be set by console command.
class CCheckBox : public QCheckBox, private CSettable {
public:
    CCheckBox(const QString& name, QAction*);
    CCheckBox(const QString& name, const QString& text);
};

//! Named QRadioButton that can be set by console command.
class CRadioButton : public QRadioButton, private CSettable {
public:
    CRadioButton(const QString& name, const QString& text);
};

//! Named QComboBox that can be set by console command.
class CComboBox : public QComboBox, private CSettable {
public:
    CComboBox(const QString& name, const QStringList& items = {});
};

//! QFileDialog, for modal use, with console commands to select files and to close the dialog.
class CFileDialog : public QFileDialog {
public:
    CFileDialog(QWidget *parent = Q_NULLPTR, const QString &caption = QString(),
                const QString &directory = QString(), const QString &filter = QString());
    ~CFileDialog();
    int exec() override;
};

#endif // CONTROLS_H