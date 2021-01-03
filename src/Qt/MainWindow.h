#pragma once

#include <memory>
#include <QtWidgets/QMainWindow>
#include "MainWindow.ui.h"
#include "Gc/GcmReader.hpp"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);
    ~MainWindow();

private:
    Ui::MainWindowClass ui;
    Gc::Gcm::Reader* m_Gcm;

public slots:
    void onBtnPush();
    void onOpenGCM();
    void onTreeItemClick(QTreeWidgetItem* item, int column);
};
