// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/thehub.cpp
//! @brief     Implements class TheHub
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "gui/thehub.h"
#include "core/session.h"
#include "gui/base/new_q.h"
#include "gui/mainwin.h" // TODO after merge with mainwin: replace gMainWin by this
#include "gui/output/write_file.h"
#include "gui/popup/filedialog.h"
#include <QApplication>
#include <QDir>
#include <QJsonDocument>
#include <QStringBuilder> // for ".." % ..


TheHub::TheHub()
    : isFixedIntenImageScale_(false)
    , isFixedIntenDgramScale_(false)
    , isCombinedDgram_(false)
    , settings_("main_settings")
{
    qDebug() << "TheHub/";

    // create actions

    trigger_about = newQ::Trigger("About " + qApp->applicationName());

    trigger_online = newQ::Trigger("Open docs in external browser");

    trigger_checkUpdate = newQ::Trigger("Check for update");

    trigger_quit = newQ::Trigger("Quit");
    trigger_quit->setShortcut(QKeySequence::Quit);

    toggle_viewStatusbar = newQ::Toggle("Statusbar", true);
    toggle_viewStatusbar->setShortcut(Qt::Key_F12);

    toggle_viewFiles = newQ::Toggle("Files", true);
    toggle_viewFiles->setShortcut(Qt::Key_F8);

    toggle_viewDatasets = newQ::Toggle("Datasets", true);
    toggle_viewDatasets->setShortcut(Qt::Key_F9);

    toggle_viewMetadata = newQ::Toggle("Metadata", true);
    toggle_viewMetadata->setShortcut(Qt::Key_F10);

    trigger_viewReset = newQ::Trigger("Reset");

#ifndef Q_OS_OSX
    toggle_fullScreen = newQ::Toggle("FullScreen", false);
    toggle_fullScreen->setShortcut(Qt::Key_F11);
#endif

    trigger_loadSession = newQ::Trigger("Load session...");

    trigger_saveSession = newQ::Trigger("Save session...");

    trigger_clearSession = newQ::Trigger("Clear session");

    trigger_addFiles = newQ::Trigger("Add files...", ":/icon/add");
    trigger_addFiles->setShortcut(Qt::CTRL | Qt::Key_O);

    trigger_removeFile = newQ::Trigger("Remove highlighted file", ":/icon/rem");
    trigger_removeFile->setShortcut(QKeySequence::Delete);
    trigger_removeFile->setEnabled(false);
    QObject::connect(gSession, &Session::sigFiles, [this]() {
            trigger_removeFile->setEnabled(gSession->dataset().countFiles()); });

    trigger_corrFile = newQ::Trigger("Add correction file", ":/icon/add");
    trigger_corrFile->setShortcut(Qt::SHIFT | Qt::CTRL | Qt::Key_O);
    connect(trigger_corrFile, &QAction::triggered, this, &TheHub::loadCorrFile);
    QObject::connect(gSession, &Session::sigCorr, [this]() {
            bool hasFile = gSession->hasCorrFile();
            qDebug() << "on sigCorr " << hasFile;
            trigger_corrFile->setIcon(QIcon(hasFile ? ":/icon/rem" : ":/icon/add"));
            QString text = QString(hasFile ? "Remove" : "Add") + " correction file";
            trigger_corrFile->setText(text);
            trigger_corrFile->setToolTip(text.toLower());
        });

    toggle_enableCorr = newQ::Toggle("Enable correction file", false, ":/icon/useCorrection");
    connect(toggle_enableCorr, &QAction::toggled, gSession, &Session::tryEnableCorr);
    QObject::connect(gSession, &Session::sigCorr, [this]() {
            toggle_enableCorr->setEnabled(gSession->hasCorrFile());
            toggle_enableCorr->setChecked(gSession->isCorrEnabled());
        });

    trigger_rotateImage = newQ::Trigger("Rotate", ":/icon/rotate0");
    trigger_rotateImage->setShortcut(Qt::CTRL | Qt::Key_R);
    connect(trigger_rotateImage, &QAction::triggered, [this]() {
        setImageRotate(gSession->imageTransform().nextRotate()); });

    toggle_mirrorImage = newQ::Toggle("Mirror", false, ":/icon/mirrorHorz");
    connect(toggle_mirrorImage, &QAction::toggled, [this](bool on) { setImageMirror(on); });

    toggle_linkCuts = newQ::Toggle("Link cuts", false, ":/icon/link");

    toggle_showOverlay = newQ::Toggle("Show overlay", false, ":/icon/crop");

    toggle_stepScale = newQ::Toggle("Scale in steps", false, ":/icon/steps");

    toggle_showBins = newQ::Toggle("Show bins", false, ":/icon/angle");

    toggle_fixedIntenImage = newQ::Toggle("Global intensity scale", false, ":/icon/scale");
    connect(toggle_fixedIntenImage, &QAction::toggled, [this](bool on) {
        isFixedIntenImageScale_ = on;
        emit sigDisplayChanged(); });

    toggle_fixedIntenDgram = newQ::Toggle("Fixed intensity scale", false);
    connect(toggle_fixedIntenDgram, &QAction::toggled, [this](bool on) {
        isFixedIntenDgramScale_ = on;
        emit sigDisplayChanged(); });

    toggle_combinedDgram = newQ::Toggle("All measurements", true);
    toggle_combinedDgram->setChecked(false);
    connect(toggle_combinedDgram, &QAction::toggled, [this](bool on) {
        isCombinedDgram_ = on;
        emit sigDisplayChanged(); });

    toggle_selRegions = newQ::Toggle("Select regions", false, ":/icon/selRegion");

    toggle_showBackground = newQ::Toggle("Show fitted background", false, ":/icon/showBackground");

    trigger_clearBackground = newQ::Trigger("Clear background regions", ":/icon/clear");
    connect(trigger_clearBackground, &QAction::triggered, [this]() { setBgRanges({}); });

    trigger_clearReflections = newQ::Trigger("Clear reflections", ":/icon/clear");

    trigger_addReflection = newQ::Trigger("Add reflection", ":/icon/add");

    trigger_removeReflection = newQ::Trigger("Remove reflection", ":/icon/rem");
    trigger_removeReflection->setEnabled(false);

    trigger_outputPolefigures = newQ::Trigger("Pole figures...");

    trigger_outputDiagrams = newQ::Trigger("Diagrams...");

    trigger_outputDiffractograms = newQ::Trigger("Diffractograms...");

    saveDir = settings_.readStr("export_directory");
    saveFmt = settings_.readStr("export_format");

    qDebug() << "/TheHub";
}

TheHub::~TheHub() {
    settings_.saveStr("export_directory", saveDir);
    settings_.saveStr("export_format", saveFmt);
}

// called through trigger_removeFile -> FilesView::removeHighlighted -> FilesModel::removeFile
void TheHub::removeFile(int i) {
    gSession->dataset().removeFile(i);
}

void TheHub::saveSession(QFileInfo const& fileInfo) const {
    WriteFile file(fileInfo.filePath());
    QDir::setCurrent(fileInfo.absolutePath());
    const int result = file.write(saveSession());
    RUNTIME_CHECK(result >= 0, "Could not write session");
}

QByteArray TheHub::saveSession() const {

    QJsonObject top;

    const Geometry& geo = gSession->geometry();
    QJsonObject sub {
        { "distance", QJsonValue(geo.detectorDistance) },
        { "pixel size", QJsonValue(geo.pixSize) },
        { "beam offset", geo.midPixOffset.to_json() }
    };
    top.insert("detector", sub);

    const ImageCut& cut = gSession->imageCut();
    sub = {
        { "left", cut.left },
        { "top", cut.top },
        { "right", cut.right },
        { "bottom", cut.bottom } };
    top.insert("cut", sub);

    const ImageTransform& trn = gSession->imageTransform();
    top.insert("image transform", trn.val);

    top.insert("files", gSession->dataset().to_json());
    top.insert("combine", gSession->dataset().binning());

    if (gSession->hasCorrFile()) {
        str absPath = gSession->corrset().raw().fileInfo().absoluteFilePath();
        str relPath = QDir::current().relativeFilePath(absPath);
        top.insert("correction file", relPath);
    }

    // TODO save cluster selection

    top.insert("background degree", gSession->bgPolyDegree());
    top.insert("background ranges", gSession->bgRanges().to_json());
    top.insert("averaged intensity ", gSession->intenScaledAvg());
    top.insert("intensity scale", qreal_to_json((qreal)gSession->intenScale()));

    QJsonArray arrReflections;
    for (auto& reflection : gSession->reflections())
        arrReflections.append(reflection->to_json());

    top.insert("reflections", arrReflections);

    return QJsonDocument(top).toJson();
}

void TheHub::sessionFromFile(rcstr filePath) THROWS {
    QFile file(filePath);
    RUNTIME_CHECK(file.open(QIODevice::ReadOnly | QIODevice::Text),
                  "Cannot open file for reading: " % filePath);
    QDir::setCurrent(QFileInfo(filePath).absolutePath());
    sessionFromJson(file.readAll());
}

void TheHub::sessionFromJson(QByteArray const& json) THROWS {
    QJsonParseError parseError;
    QJsonDocument doc(QJsonDocument::fromJson(json, &parseError));
    RUNTIME_CHECK(QJsonParseError::NoError == parseError.error, "Error parsing session file");

    TakesLongTime __;

    gSession->clear();
    TR("sessionFromJson: cleared old session");

    JsonObj top(doc.object());

    const QJsonArray& files = top.loadArr("files");
    QStringList paths;
    for (const QJsonValue& file : files) {
        str filePath = file.toString();
        QDir dir(filePath);
        RUNTIME_CHECK(dir.makeAbsolute(), str("Invalid file path: %1").arg(filePath));
        paths.append(dir.absolutePath());
    }
    gSession->dataset().addGivenFiles(paths);

    const QJsonArray& sels = top.loadArr("selected files", true);
    vec<int> selIndexes;
    for (const QJsonValue& sel : sels) {
        int i = sel.toInt(), index = qBound(0, i, files.count());
        RUNTIME_CHECK(i == index, str("Invalid selection index: %1").arg(i));
        selIndexes.append(index);
    }

    std::sort(selIndexes.begin(), selIndexes.end());
    int lastIndex = -1;
    for (int index : selIndexes) {
        RUNTIME_CHECK(lastIndex < index, str("Duplicate selection index"));
        lastIndex = index;
    }

    TR("sessionFromJson: going to collect cluster");
    gSession->dataset().setBinning(top.loadPint("combine", 1));
    onFilesSelected(selIndexes);

    TR("sessionFromJson: going to set correction file");
    gSession->corrset().loadFile(top.loadString("correction file", ""));

    TR("sessionFromJson: going to load detector geometry");
    const JsonObj& det = top.loadObj("detector");
    setGeometry(
        det.loadPreal("distance"), det.loadPreal("pixel size"),
        det.loadIJ("beam offset"));

    TR("sessionFromJson: going to load image cut");
    const JsonObj& cut = top.loadObj("cut");
    int x1 = cut.loadUint("left"), y1 = cut.loadUint("top"),
         x2 = cut.loadUint("right"), y2 = cut.loadUint("bottom");
    setImageCut(true, false, ImageCut(x1, y1, x2, y2));
    setImageRotate(ImageTransform(top.loadUint("image transform")));

    TR("sessionFromJson: going to load fit setup");
    Ranges bgRanges;
    bgRanges.from_json(top.loadArr("background ranges"));
    setBgRanges(bgRanges);
    setBgPolyDegree(top.loadUint("background degree"));

    bool arg1 = top.loadBool("averaged intensity ", true);
    qreal arg2 = top.loadPreal("intensity scale", 1);
    setIntenScaleAvg(arg1, arg2);

    TR("sessionFromJson: going to load reflections info");
    const QJsonArray& reflectionsInfo = top.loadArr("reflections");
    for_i (reflectionsInfo.count()) {
        gSession->addReflection(reflectionsInfo.at(i).toObject());
    }

    emit sigReflectionsChanged();
    TR("installed session from file");
}

void TheHub::onFilesSelected(const vec<int> indexSelection) { // TODO rm
    collectDatasetsExec();
}

void TheHub::combineMeasurementsBy(const int by) {
    gSession->dataset().setBinning(by);
    collectDatasetsExec();
}

void TheHub::collectDatasetsExec() { // TODO move to Dataset
    gSession->dataset().assembleExperiment();
}

void TheHub::loadCorrFile() {
    if (gSession->corrset().hasFile()) {
        gSession->corrset().removeFile();
    } else {
        QString fileName = file_dialog::openFileName(
            gMainWin, "Set correction file", QDir::current().absolutePath(),
            "Data files (*.dat *.mar*);;All files (*.*)");
        if (fileName.isEmpty())
            return;
        QDir::setCurrent(QFileInfo(fileName).absolutePath());
        gSession->corrset().loadFile(fileName);
    }
}

void TheHub::setImageCut(bool isTopOrLeft, bool linked, ImageCut const& cut) {
    gSession->setImageCut(isTopOrLeft, linked, cut);
} // TODO rm

void TheHub::setGeometry(qreal detectorDistance, qreal pixSize, IJ const& midPixOffset) {
    gSession->setGeometry(detectorDistance, pixSize, midPixOffset);
} // TODO rm

void TheHub::setGammaRange(const Range& gammaRange) {
    gSession->setGammaRange(gammaRange);
    emit sigGammaRange();
}

void TheHub::setBgRanges(const Ranges& ranges) {
    gSession->setBgRanges(ranges);
    emit sigBgChanged();
}

void TheHub::addBgRange(const Range& range) {
    if (gSession->addBgRange(range))
        emit sigBgChanged();
}

void TheHub::removeBgRange(const Range& range) {
    if (gSession->removeBgRange(range))
        emit sigBgChanged();
}

void TheHub::setBgPolyDegree(int degree) {
    gSession->setBgPolyDegree(degree);
    emit sigBgChanged();
}

void TheHub::setIntenScaleAvg(bool avg, qreal scale) {
    gSession->setIntenScaleAvg(avg, scale);
    emit sigNormChanged(); // TODO instead of another signal
}

void TheHub::setPeakFunction(const QString& peakFunctionName) {
    if (selectedReflection_) {
        selectedReflection_->setPeakFunction(peakFunctionName);
        emit sigReflectionsChanged();
    }
}

void TheHub::addReflection(const QString& peakFunctionName) {
    gSession->addReflection(peakFunctionName);
    emit sigReflectionsChanged();
}

void TheHub::removeReflection(int i) {
    gSession->removeReflection(i);
    if (gSession->reflections().isEmpty())
        tellSelectedReflection(shp_Reflection());
    emit sigReflectionsChanged();
}

void TheHub::setFittingTab(eFittingTab tab) {
    emit sigFittingTab((fittingTab_ = tab));
}

void TheHub::setImageRotate(ImageTransform rot) {
    const char* rotateIconFile;
    const char* mirrorIconFile;

    switch (rot.val & 3) {
    default /* case */:
        rotateIconFile = ":/icon/rotate0";
        mirrorIconFile = ":/icon/mirrorHorz";
        break;
    case 1:
        rotateIconFile = ":/icon/rotate1";
        mirrorIconFile = ":/icon/mirrorVert";
        break;
    case 2:
        rotateIconFile = ":/icon/rotate2";
        mirrorIconFile = ":/icon/mirrorHorz";
        break;
    case 3:
        rotateIconFile = ":/icon/rotate3";
        mirrorIconFile = ":/icon/mirrorVert";
        break;
    }

    trigger_rotateImage->setIcon(QIcon(rotateIconFile));
    toggle_mirrorImage->setIcon(QIcon(mirrorIconFile));
    gSession->setImageTransformRotate(rot);
    gSession->setImageCut(true, false, gSession->imageCut());
}

void TheHub::setImageMirror(bool on) {
    toggle_mirrorImage->setChecked(on);
    gSession->setImageTransformMirror(on);
    emit gSession->sigDetector();
}

void TheHub::setNorm(eNorm norm) {
    gSession->setNorm(norm);
    emit sigNormChanged();
}

void TheHub::tellSelectedReflection(shp_Reflection reflection) {
    selectedReflection_ = reflection;
    emit sigReflectionSelected(reflection);
}

void TheHub::tellReflectionData(shp_Reflection reflection) {
    emit sigReflectionData(reflection);
}

void TheHub::tellReflectionValues(
    const Range& rgeTth, qpair const& peak, fwhm_t fwhm, bool withGuesses) {
    emit sigReflectionValues(rgeTth, peak, fwhm, withGuesses);
}
