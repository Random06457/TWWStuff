#pragma once

#include <memory>
#include <QtWidgets/QMainWindow>
#include "MainWindow.ui.h"
#include "Gc/GcmReader.hpp"
#include "TreeModel.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow();

private:
    Ui::MainWindowClass ui;
    Gc::Gcm::Reader* m_Gcm;
    TreeModel m_GcmFsTree;

public slots:
    void onBtnPush();
    void onOpenGCM();
};
