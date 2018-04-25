//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      main/steca.cpp
//! @brief     Implements the main program
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

//! \mainpage Steca: the stress and texture calculator
//!
//! \par Homepage:
//!           http://apps.jcns.fz-juelich.de/steca
//! \par Repository:
//!           https://github.com/scgmlz/Steca

#include "manifest.h"
#include "core/session.h"
#include "gui/cfg/msg_handler.h"
#include "qcr/engine/console.h"
#include "gui/mainwin.h"

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "optparse.h"
#include "gui/dialogs/file_dialog.h"

#include <iostream>
#include <QApplication>
#include <QLoggingCategory>
#include <QStyleFactory>

const char* version =
#include "../VERSION"
    ;

int main(int argc, char* argv[]) {
    struct optparse options;
    optparse_init(&options, argv);
    int opt;
    while ((opt = optparse(&options, "hvcps")) != -1) {
        switch (opt) {
        case 'h':
            std::cout << APPLICATION_CLAIM << "\n\n"
                      << "Usage: " << APPLICATION_NAME << " [options]\n\n"
                      << "Options:\n"
                      << "  -h  Print this message.\n"
                      << "  -v  Print " << APPLICATION_NAME << " version.\n"
                      << "  -c  Read commands from console instead of starting the GUI.\n"
                      << "  -p  Sets the file override policy to 'Panic'. Default is 'Promt'.\n"
                      << "  -s  Sets the file override policy to 'Silent Override'. Default is 'Promt'.\n";
            exit(0);
        case 'v':
            std::cout << APPLICATION_NAME << " version " << version << "\n";
            exit(0);
        case 'p':
            std::cout << "fileOverridePolicy set to " << "PANIC" << "\n";
            setFileOverridePolicy(file_dialog::eFileOverridePolicy::PANIC);
            //std::cout << "fileOverridePolicy set to " << (int)file_dialog::fileOverridePolicy << "\n";
            break;
        case 's':
            std::cout << "fileOverridePolicy set to " << "SILENT_OVERRIDE" << "\n";
            setFileOverridePolicy(file_dialog::eFileOverridePolicy::SILENT_OVERRIDE);
            break;
        }
    }

    QStringList nonoptArgs;
    const char* tmp;
    while ((tmp = optparse_arg(&options)))
        nonoptArgs.append(tmp);
    if (nonoptArgs.size()>1) {
        std::cerr << "More than one command-line argument given\n";
        exit(-1);
    }

    QApplication app(argc, argv);

    app.setApplicationName(APPLICATION_NAME);
    app.setApplicationVersion(version);
    app.setOrganizationName(ORGANIZATION_NAME);
    app.setOrganizationDomain(ORGANIZATION_DOMAIN);

#if defined(Q_OS_OSX)
    app.setStyle(QStyleFactory::create("Macintosh"));
#elif defined(Q_OS_WIN)
    app.setStyle(QStyleFactory::create("Fusion"));
#else
    app.setStyle(QStyleFactory::create("Fusion"));
#endif

    QLoggingCategory::setFilterRules("*.debug=true\nqt.*.debug=false");
    qInstallMessageHandler(messageHandler);

    Session session;
    Console console;
    auto* mainwin = new MainWin; // must be pointer, because it can be deleted by 'quit' trigger
    mainwin->show();
    if (nonoptArgs.size())
        gConsole->call("@file " + nonoptArgs[0]);
    app.exec();
}
