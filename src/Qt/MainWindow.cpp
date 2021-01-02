#include "MainWindow.h"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include "Utils/FileReader.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_Gcm(nullptr),
    m_GcmFsTree()
{
    ui.setupUi(this);
}

MainWindow::~MainWindow()
{
    if (m_Gcm)
    {
        delete m_Gcm;
    }
}


void MainWindow::onBtnPush()
{
    QMessageBox::information(this, "Info", "Test", QMessageBox::Ok);
}

void MainWindow::onOpenGCM()
{
    //QMessageBox::information(this, "Info", "Opening GCM", QMessageBox::Ok);
    //auto path = QFileDialog::getSaveFileName(this, "Dump Memory", QString(), FILTER_BIN);
    auto path = QFileDialog::getOpenFileName(this, "Open GCM", QString(), "GCM ROM (*.gcm)");
    if (m_Gcm)
    {
        delete m_Gcm;
    }
    m_Gcm = new Gc::Gcm::Reader(std::make_unique<Utils::FileReader>(path.toStdString()));
    auto root = m_GcmFsTree.getRoot();
    //root->appendChild(new TreeItem({tr("1"), tr("2"), tr("3")}));
    root->child(0)->appendChild(new TreeItem({tr("4"), tr("5"), tr("6")}));
    ui.treeView->setModel(&m_GcmFsTree);
}