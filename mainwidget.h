//
// Created by aaditya on 11/02/21.
//

#ifndef WASH_MAINWIDGET_H
#define WASH_MAINWIDGET_H

#include <QPushButton>
#include <QWidget>

class MainWidget : public QWidget{
    Q_OBJECT
public:
    MainWidget(QWidget *parent = nullptr);
    ~MainWidget();

private:
    QPushButton* button_;

};

#endif //WASH_MAINWIDGET_H
