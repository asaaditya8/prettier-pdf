//
// Created by aaditya on 11/02/21.
//

#include <QtWidgets>
#include <QGridLayout>
#include <QPushButton>
#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent) : QWidget(parent) {
    button_ = new QPushButton(tr("Click"));
    auto layout = new QGridLayout;
    layout->addWidget(button_, 0, 0);
    setLayout(layout);
    setWindowTitle(tr("Welcome"));
}

MainWidget::~MainWidget() noexcept {
    delete button_;
}