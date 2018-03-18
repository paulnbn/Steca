// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/mainwin.h
//! @brief     Defines class MainWin
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef MAINWIN_H
#define MAINWIN_H

#include "core/calc/lens.h"
#include "core/data/rawfile.h"
#include "core/data/image_transform.h"
#include "core/typ/singleton.h"
#include "gui/cfg/settings.h"
#include <QDir>
#include <QMainWindow>
#include <QSplitter>

extern class MainWin* gGui; //!< global pointer to _the_ main window

// make connects shorter
#define _SLOT_(Type, method, parType) static_cast<void (Type::*)(parType)>(&Type::method)

//! The main window. Its single instance is accessible through the global pointer gGui.

class MainWin : public QMainWindow {
    Q_OBJECT
public:
    MainWin();
    ~MainWin();

    class SubframeSetup* setup() const { return frameSetup_; }

    class Triggers* triggers;
    class Toggles* toggles;

    // TODO relegate this to TabSave or similar
    QString saveDir; //!< setting: default directory for data export
    QString saveFmt; //!< setting: default format for data export

private:
    void initLayout();
    void updateActionEnabling();

    void readSettings();
    void saveSettings() const;
    void viewReset();

    void loadSession();
    void saveSession();
    void clearSession();
    QByteArray serializeSession() const;
    void sessionFromJson(const QByteArray&) THROWS;

    void addFiles();
    void loadCorrFile() THROWS;

    void setImageRotate(ImageTransform);
    void setImageMirror(bool);

    class Menus* menus;
    QDockWidget *dockFiles_, *dockClusters_, *dockMetadata_;
    QSplitter splMain_ {Qt::Vertical};
    QSplitter splTop_ {Qt::Horizontal};
    class SubframeDiffractogram* frameDiffractogram_;
    class SubframeImage* frameImage_;
    class SubframeSetup* frameSetup_;

    QByteArray initialState_;
    QDir sessionDir_ {QDir::homePath()};
    QDir dataDir_ {QDir::homePath()};
    const QString dataFormats_ {"Data files (*.dat *.mar*);;All files (*.*)"};
    XSettings settings_;

    friend Triggers;
    friend Toggles;
};

#endif // MAINWIN_H
