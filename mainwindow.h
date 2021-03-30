//
// Created by aaditya on 13/02/21.
//

#ifndef WASH_MAINWINDOW_H
#define WASH_MAINWINDOW_H


#include <QMainWindow>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QStatusBar>
#include <QLabel>
#include <QGraphicsPixmapItem>

#include <vector>
#include "images2pdf.h"

class MainWindow : public QMainWindow
{
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent=nullptr);
    ~MainWindow();

private:
    void initUI();
    void createActions();
    void showImage(QString);
    void setupShortcuts();

private slots:
    void openImage();
    void openPdf();
    void zoomIn();
    void zoomOut();
    void prevImage();
    void nextImage();
    void saveImage();
    void savePdf();
    void closeThis();
    void filterImage();
    void applyFilters();
    void undoFilter();
    void processFinished(bool);
    void shiftLeft();
    void shiftRight();
    void insertLeft();
    void insertRight();

private:
    void toggleActions(bool);

    QMenu *fileMenu;
    QMenu *viewMenu;

    //QToolBar *fileToolBar;
    QToolBar *viewToolBar;
    QToolBar *editToolBar;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;

    QStatusBar *mainStatusBar;
    QLabel *mainStatusLabel;

    QAction* openAction;
    QAction *openPdfAction;
    QAction* saveAsAction;
    QAction* savePdfAction;
    QAction* closeAction;
    QAction *exitAction;
    QAction *zoomInAction;
    QAction *zoomOutAction;
    QAction *prevAction;
    QAction *nextAction;
    QAction *filterAction;
    QAction *undoAction;
    QAction *shiftLeftAction;
    QAction *shiftRightAction;
    QAction *insertLeftAction;
    QAction *insertRightAction;


    QString currentImagePath;
    QGraphicsPixmapItem *currentImage;
    QImage lastImage;
    QDir dir;
    bool lastImageAvailable;
    bool isOpen;
    bool isPDF;
    bool isEdited;
    std::vector<std::function<QImage(QImage const&)>> history;

    Images2PDF* _convertor;
};


#endif //WASH_MAINWINDOW_H
