//
// Created by aaditya on 13/02/21.
//

#include "mainwindow.h"
#include "opencvhelper.h"
#include "images2pdf.h"

#include <QtGui/QPdfWriter>
#include <QApplication>
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QKeyEvent>
#include <QDebug>
#include <QProcess>
#include <QSpacerItem>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent)
    , fileMenu(nullptr)
    , viewMenu(nullptr)
    , currentImage(nullptr)
    , _convertor(NULL)
{
    // initialize vars
    isPDF = false;
    isEdited = false;

    QString loc = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    dir = QDir(loc);
    if(!dir.exists(QString("WashDir"))){
        dir.mkdir(QString("WashDir"));
    }
    dir.cd(QString("WashDir"));

    initUI();
}

MainWindow::~MainWindow()
{
}

void MainWindow::initUI()
{
    this->resize(800, 600);
    // setup menubar
    fileMenu = menuBar()->addMenu("&File");
    viewMenu = menuBar()->addMenu("&View");

    // setup toolbar
    //fileToolBar = addToolBar("File");
    viewToolBar = addToolBar("View");
    addToolBarBreak(Qt::TopToolBarArea);
    editToolBar = addToolBar("Modify");


    // main area for image display
    imageScene = new QGraphicsScene(this);
    imageView = new QGraphicsView(imageScene);
    setCentralWidget(imageView);

    // setup status bar
    mainStatusBar = statusBar();
    mainStatusLabel = new QLabel(mainStatusBar);
    mainStatusBar->addPermanentWidget(mainStatusLabel);
    mainStatusLabel->setText("Image Information will be here!");

    createActions();
}

void MainWindow::createActions()
{

    // create actions, add them to menus
    openAction = new QAction("&Open Image", this);
    fileMenu->addAction(openAction);
    openPdfAction = new QAction("Open PDF", this);
    fileMenu->addAction(openPdfAction);
    saveAsAction = new QAction("Page -> PNG", this);
    saveAsAction->setToolTip("Save/Extract Current Page");
    fileMenu->addAction(saveAsAction);
    savePdfAction = new QAction("&Save PDF", this);
    fileMenu->addAction(savePdfAction);
    exitAction = new QAction("E&xit", this);
    fileMenu->addAction(exitAction);

    

    zoomInAction = new QAction("Zoom in", this);
    viewMenu->addAction(zoomInAction);
    zoomOutAction = new QAction("Zoom Out", this);
    viewMenu->addAction(zoomOutAction);
    prevAction = new QAction("&Previous Image", this);
    viewMenu->addAction(prevAction);
    nextAction = new QAction("&Next Image", this);
    viewMenu->addAction(nextAction);
    filterAction = new QAction("Filter", this);
    undoAction = new QAction("Undo", this);

    closeAction = new QAction("Close", this);

    shiftLeftAction = new QAction("Shift Left", this);
    shiftRightAction = new QAction("Shift Right", this);
    insertLeftAction = new QAction("Insert Left", this);
    insertRightAction = new QAction("Insert Right", this);

    // add actions to toolbars
    viewToolBar->addAction(openAction);
    viewToolBar->addAction(openPdfAction);

    QWidget* spacer1 = new QWidget(this);
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    viewToolBar->addWidget(spacer1);

    viewToolBar->addAction(shiftLeftAction);
    viewToolBar->addAction(shiftRightAction);
    viewToolBar->addAction(insertLeftAction);
    viewToolBar->addAction(insertRightAction);

    QWidget* spacer2 = new QWidget(this);
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    viewToolBar->addWidget(spacer2);

    viewToolBar->addAction(closeAction);
    viewToolBar->addAction(saveAsAction);
    viewToolBar->addAction(savePdfAction);

    QWidget* spacer3 = new QWidget(this);
    spacer3->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    editToolBar->addWidget(spacer3);

    editToolBar->addAction(zoomInAction);
    editToolBar->addAction(zoomOutAction);
    editToolBar->addAction(prevAction);
    editToolBar->addAction(nextAction);
    editToolBar->addAction(filterAction);
    editToolBar->addAction(undoAction);

    QWidget* spacer4 = new QWidget(this);
    spacer4->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    editToolBar->addWidget(spacer4);

    toggleActions(false);

    // connect the signals and slots
    connect(exitAction, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openImage()));
    connect(openPdfAction, SIGNAL(triggered(bool)), this, SLOT(openPdf()));
    connect(saveAsAction, SIGNAL(triggered(bool)), this, SLOT(saveImage()));
    connect(savePdfAction, SIGNAL(triggered(bool)), this, SLOT(savePdf()));
    connect(zoomInAction, SIGNAL(triggered(bool)), this, SLOT(zoomIn()));
    connect(zoomOutAction, SIGNAL(triggered(bool)), this, SLOT(zoomOut()));
    connect(prevAction, SIGNAL(triggered(bool)), this, SLOT(prevImage()));
    connect(nextAction, SIGNAL(triggered(bool)), this, SLOT(nextImage()));
    connect(filterAction, SIGNAL(triggered(bool)), this, SLOT(filterImage()));
    connect(undoAction, SIGNAL(triggered(bool)), this, SLOT(undoFilter()));
    connect(closeAction, SIGNAL(triggered(bool)), this, SLOT(closeThis()));
    connect(shiftLeftAction, SIGNAL(triggered(bool)), this, SLOT(shiftLeft()));
    connect(shiftRightAction, SIGNAL(triggered(bool)), this, SLOT(shiftRight()));
    connect(insertLeftAction, SIGNAL(triggered(bool)), this, SLOT(insertLeft()));
    connect(insertRightAction, SIGNAL(triggered(bool)), this, SLOT(insertRight()));

    setupShortcuts();
}

void MainWindow::toggleActions(bool state){
    prevAction->setEnabled(state);
    nextAction->setEnabled(state);
    zoomInAction->setEnabled(state);
    zoomOutAction->setEnabled(state);
    filterAction->setEnabled(state);
    undoAction->setEnabled(state);
    closeAction->setEnabled(state);
    saveAsAction->setEnabled(state);
    savePdfAction->setEnabled(state);
    shiftLeftAction->setEnabled(state);
    shiftRightAction->setEnabled(state);
    insertLeftAction->setEnabled(state);
    insertRightAction->setEnabled(state);
}

void MainWindow::openImage()
{
    
    QFileDialog dialog(this);
    dialog.setWindowTitle("Open Image");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    QStringList filePaths;
    if (dialog.exec()) {
        filePaths = dialog.selectedFiles();
        isPDF = false;

        toggleActions(true);
        nextAction->setEnabled(false);
        prevAction->setEnabled(false);

        showImage(filePaths.at(0));
    }


}

void MainWindow::closeThis() {
    isPDF = false;
    isOpen = false;
    isEdited = false;
    lastImageAvailable = false;
    mainStatusLabel->setText(QString());
    currentImagePath = QString();
    history.clear();
    lastImage = QImage();
    currentImage->setPixmap(QPixmap());
    toggleActions(false);
}


void MainWindow::openPdf()
{
    // TODO: add page, move page
    // TODO: show page no.
    // TODO: add filter all buttons : progress bar
    // TODO: set default scale to something
    // TODO: maybe add scrolling view like word 

    for (const QString& dirFile : dir.entryList()) {
        dir.remove(dirFile);
    }

    QFileDialog dialog(this);
    dialog.setWindowTitle("Open Image");
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setNameFilter(tr("PDFs (*.pdf)"));
    QStringList filePaths;

    if (dialog.exec()) {
        filePaths = dialog.selectedFiles();
        QProcess process;
        process.setWorkingDirectory(QCoreApplication::applicationDirPath());

        QString basepath = dir.absolutePath() + "/pg";
        #ifdef Q_OS_LINUX
            process.start("pdftopng", QStringList() << filePaths.at(0) << basepath);
        #else
            process.start("cmd", QStringList() << "/c" << "pdftopng.exe" << filePaths.at(0) << basepath);
        #endif

        if (!process.waitForFinished()) {
            QMessageBox::information(this, "Information", "Could not convert to PNG.");
            return;
        }

        isPDF = true;

        toggleActions(true);

        dir.refresh();
        QStringList nameFilters;
        nameFilters << "*.png";
        auto filenames = dir.entryList(nameFilters);
        std::stringstream ss;
        for(const QString& oldName : filenames){
            QString newName(oldName);
            ss << "pg-" << newName.toStdString().substr(5, newName.length()-9).c_str() << "00.png";
            newName = QString(ss.str().c_str());
            ss.str("");
            ss.clear();
//            std::cout << newName.toStdString() << "\n";
            dir.rename(oldName, newName);
        }

        showImage(QString(basepath+"-000100.png"));
    }
}

void MainWindow::showImage(QString path)
{
    isEdited = false;
    history.clear();

    lastImageAvailable = false;
    imageScene->clear();
    imageView->resetMatrix();
    QPixmap image(path);
    currentImage = imageScene->addPixmap(image);
    imageScene->update();
    imageView->setSceneRect(image.rect());
    QString status = QString("%1, %2x%3, %4 Bytes").arg(path).arg(image.width())
            .arg(image.height()).arg(QFile(path).size());
    mainStatusLabel->setText(status);
    currentImagePath = path;
}

void MainWindow::zoomIn()
{
    imageView->scale(1.2, 1.2);
}

void MainWindow::zoomOut()
{
    imageView->scale(1/1.2, 1/1.2);
}

void MainWindow::prevImage()
{
    if (isPDF && isEdited) {
        currentImage->pixmap().save(currentImagePath, nullptr, 0);
    }
    QFileInfo current(currentImagePath);
    QDir dir = current.absoluteDir();
    QStringList nameFilters;
    nameFilters << "*.png" << "*.bmp" << "*.jpg" << "*.jpeg";
    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);
    int idx = fileNames.indexOf(QRegExp(QRegExp::escape(current.fileName())));
    if(idx > 0) {
        showImage(dir.absoluteFilePath(fileNames.at(idx - 1)));
    } else {
        QMessageBox::information(this, "Information", "Current image is the first one.");
    }
}

void MainWindow::nextImage()
{
    if (isPDF && isEdited) {
        currentImage->pixmap().save(currentImagePath, nullptr, 0);
    }
    QFileInfo current(currentImagePath);
    QDir dir = current.absoluteDir();
    QStringList nameFilters;
    nameFilters << "*.png" << "*.bmp" << "*.jpg";
    QStringList fileNames = dir.entryList(nameFilters, QDir::Files, QDir::Name);
    int idx = fileNames.indexOf(QRegExp(QRegExp::escape(current.fileName())));
    if(idx < fileNames.size() - 1) {
        showImage(dir.absoluteFilePath(fileNames.at(idx + 1)));
    } else {
        QMessageBox::information(this, "Information", "Current image is the last one.");
    }
}

void MainWindow::shiftLeft(){

}

void MainWindow::shiftRight(){

}

void MainWindow::insertLeft(){

}

void MainWindow::insertRight(){

}

void MainWindow::saveImage()
{
    if (currentImage == nullptr) {
        QMessageBox::information(this, "Information", "Nothing to save.");
        return;
    }
    QFileDialog dialog(this);
    dialog.setWindowTitle("Save Image As ...");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("Images (*.png *.bmp *.jpg)"));
    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
        if(QRegExp(".+\\.(png|bmp|jpg)").exactMatch(fileNames.at(0))) {
            currentImage->pixmap().save(fileNames.at(0));
        } else {
            QMessageBox::information(this, "Information", "Save error: bad format or filename.");
        }
    }
}



void MainWindow::savePdf() {
    if (currentImage == nullptr) {
        QMessageBox::information(this, "Information", "Nothing to save.");
        return;
    }
    QFileDialog dialog(this);
    dialog.setWindowTitle("Save PDF As ...");
    dialog.setFileMode(QFileDialog::AnyFile);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.setNameFilter(tr("PDFs (*.pdf)"));
    QStringList fileNames;
    if (dialog.exec()) {
        fileNames = dialog.selectedFiles();
        if (QRegExp(".+\\.(pdf)").exactMatch(fileNames.at(0))) {
            _convertor = new Images2PDF(dir.absolutePath(), fileNames.at(0));
            connect(_convertor, SIGNAL(finished(bool)), this, SLOT(processFinished(bool)));
            _convertor->start();
        }
        else {
            QMessageBox::information(this, "Information", "Save error: bad format or filename.");
        }
    }
}

void MainWindow::processFinished(bool success){
    if (success) {
        QMessageBox::information(this, "Information", "Succesfully saved pdf.");
    }
    else if (_convertor) {
        QMessageBox::information(this, "Information", "Error in saving pdf.");
    }
    if (_convertor) {
        delete _convertor;
        _convertor = NULL;
    }
}

void MainWindow::filterImage() {
    if (currentImage == nullptr) {
        QMessageBox::information(this, "Information", "Nothing to filter.");
        return;
    }
    history.emplace_back(&documentFilter);
    applyFilters();
}

void MainWindow::applyFilters() {
    lastImage = currentImage->pixmap().toImage();
    lastImageAvailable = true;
    isEdited = true;
    currentImage->setPixmap(QPixmap::fromImage(history.back()(lastImage)));
}

void MainWindow::undoFilter(){
    if(history.empty()){ return; }
    history.pop_back();
    if(history.empty()){
        currentImage->setPixmap(QPixmap(currentImagePath));
        lastImage = QImage();
        lastImageAvailable = false;
        isEdited = false;
        return;
    }
    if(lastImageAvailable){
        currentImage->setPixmap(QPixmap::fromImage(lastImage));
        lastImage = QImage();
        lastImageAvailable = false;
        return;
    }
    
    QImage tempImage = QImage(currentImagePath);
    for(auto it = history.begin(); it != history.end()-1; it++){
        tempImage = (*it)(tempImage);
    }
    lastImage = tempImage;
    lastImageAvailable = true;
    tempImage = (*(history.end()-1))(tempImage);
    currentImage->setPixmap(QPixmap::fromImage(tempImage));
}

void MainWindow::setupShortcuts()
{
    QList<QKeySequence> shortcuts;
    shortcuts << Qt::Key_Plus << Qt::Key_Equal;
    zoomInAction->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Minus << Qt::Key_Underscore;
    zoomOutAction->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Up << Qt::Key_Left;
    prevAction->setShortcuts(shortcuts);

    shortcuts.clear();
    shortcuts << Qt::Key_Down << Qt::Key_Right;
    nextAction->setShortcuts(shortcuts);
}
