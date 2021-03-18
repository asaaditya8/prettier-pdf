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
    void zoomIn();
    void zoomOut();
    void prevImage();
    void nextImage();
    void saveAs();
    void filterImage();
    void applyFilters();
    void undoFilter();

private:
    QMenu *fileMenu;
    QMenu *viewMenu;

    QToolBar *fileToolBar;
    QToolBar *viewToolBar;

    QGraphicsScene *imageScene;
    QGraphicsView *imageView;

    QStatusBar *mainStatusBar;
    QLabel *mainStatusLabel;

    QAction *openAction;
    QAction *saveAsAction;
    QAction *exitAction;
    QAction *zoomInAction;
    QAction *zoomOutAction;
    QAction *prevAction;
    QAction *nextAction;
    QAction *filterAction;
    QAction *undoAction;

    QString currentImagePath;
    QGraphicsPixmapItem *currentImage;
    //QImage currentRawImage;
    QImage lastImage;
    bool lastImageAvailable;
    std::vector<std::function<QImage(QImage const&)>> history;
};


#endif //WASH_MAINWINDOW_H
