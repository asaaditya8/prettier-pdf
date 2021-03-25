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
    //closeToolBar = addToolBar("Close");


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
    saveAsAction = new QAction("&Save as", this);
    fileMenu->addAction(saveAsAction);
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

    // add actions to toolbars
    viewToolBar->addAction(openAction);
    viewToolBar->addAction(openPdfAction);

    QWidget* spacer1 = new QWidget(this);
    spacer1->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    viewToolBar->addWidget(spacer1);
    viewToolBar->addAction(zoomInAction);
    viewToolBar->addAction(zoomOutAction);
    viewToolBar->addAction(prevAction);
    viewToolBar->addAction(nextAction);
    viewToolBar->addAction(filterAction);
    viewToolBar->addAction(undoAction);
    QWidget* spacer2 = new QWidget(this);
    spacer2->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    viewToolBar->addWidget(spacer2);

    viewToolBar->addAction(closeAction);
    viewToolBar->addAction(saveAsAction);
    
    prevAction->setEnabled(false);
    nextAction->setEnabled(false);
    zoomInAction->setEnabled(false);
    zoomOutAction->setEnabled(false);
    filterAction->setEnabled(false);
    undoAction->setEnabled(false);
    closeAction->setEnabled(false);
    saveAsAction->setEnabled(false);

    // connect the signals and slots
    connect(exitAction, SIGNAL(triggered(bool)), QApplication::instance(), SLOT(quit()));
    connect(openAction, SIGNAL(triggered(bool)), this, SLOT(openImage()));
    connect(openPdfAction, SIGNAL(triggered(bool)), this, SLOT(openPdf()));
    connect(saveAsAction, SIGNAL(triggered(bool)), this, SLOT(saveAs()));
    connect(zoomInAction, SIGNAL(triggered(bool)), this, SLOT(zoomIn()));
    connect(zoomOutAction, SIGNAL(triggered(bool)), this, SLOT(zoomOut()));
    connect(prevAction, SIGNAL(triggered(bool)), this, SLOT(prevImage()));
    connect(nextAction, SIGNAL(triggered(bool)), this, SLOT(nextImage()));
    connect(filterAction, SIGNAL(triggered(bool)), this, SLOT(filterImage()));
    connect(undoAction, SIGNAL(triggered(bool)), this, SLOT(undoFilter()));
    connect(closeAction, SIGNAL(triggered(bool)), this, SLOT(close()));

    setupShortcuts();
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

        zoomInAction->setEnabled(true);
        zoomOutAction->setEnabled(true);
        filterAction->setEnabled(true);
        undoAction->setEnabled(true);
        closeAction->setEnabled(true);
        saveAsAction->setEnabled(true);
        nextAction->setEnabled(false);
        prevAction->setEnabled(false);

        showImage(filePaths.at(0));
    }


}

void MainWindow::close() {
    isPDF = false;
    isOpen = false;
    isEdited = false;
    lastImageAvailable = false;
    mainStatusLabel->setText(QString());
    currentImagePath = QString();
    history.clear();
    lastImage = QImage();
    currentImage->setPixmap(QPixmap());

    prevAction->setEnabled(false);
    nextAction->setEnabled(false);
    zoomInAction->setEnabled(false);
    zoomOutAction->setEnabled(false);
    filterAction->setEnabled(false);
    undoAction->setEnabled(false);
    closeAction->setEnabled(false);
    saveAsAction->setEnabled(false);
}


void MainWindow::openPdf()
{
    // TODO: show page no.
    // TODO: add filter all buttons 
    // TODO: set default scale to something
    // TODO: maybe add scrolling view like word 

    QDir dir(QString("C:/Users/Aaditya Singh/Pictures/out/res"));
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
        process.setWorkingDirectory("C:/Users/Aaditya Singh/Pictures/out");
        process.start("cmd", QStringList() << "/c" << "pdftopng.exe" << filePaths.at(0) << "res/pg");

        if (!process.waitForFinished()) {
            QMessageBox::information(this, "Information", "Could not convert to PNG.");
            return;
        }

        isPDF = true;

        prevAction->setEnabled(true);
        nextAction->setEnabled(true);
        zoomInAction->setEnabled(true);
        zoomOutAction->setEnabled(true);
        filterAction->setEnabled(true);
        undoAction->setEnabled(true);
        closeAction->setEnabled(true);
        saveAsAction->setEnabled(true);

        showImage(QString("C:/Users/Aaditya Singh/Pictures/out/res/pg-000001.png"));
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

void MainWindow::saveAs() {
    if (isPDF) {
        savePdf();
    }
    else {
        saveImage();
    }
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
            _convertor = new Images2PDF(QString("C:/Users/Aaditya Singh/Pictures/out/res"), fileNames.at(0));
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
