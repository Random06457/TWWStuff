#include "MainWindow.h"
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include "Utils/FileReader.hpp"
#include "Zelda/ArcReader.hpp"
#include "Zelda/FileKind.hpp"
#include "Zelda/Yaz0.hpp"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_Gcm(nullptr)
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

void MainWindow::onTreeItemClick(QTreeWidgetItem* item, int column)
{
    QString path = item->text(column);
    while(item->parent())
    {
        item = item->parent();
        path = ((item->parent()) ? item->text(column) : "") + "/" + path;
    }
    Gc::Gcm::FileEntry* entry = m_Gcm->getFileEntry(path.toStdString());
    
    QString msg = "Path : \"" + path + "\"\nType : ";
    
    if (entry->isDir)
        msg += "Directory";
    else
    {
        auto kind = Zelda::getFileKind(m_Gcm, entry);
        switch (kind & ~Zelda::FileKind::Yaz0)
        {
            case Zelda::FileKind::Arc:
                msg += "Arc File"; break;
            case Zelda::FileKind::Bdl:
                msg += "Binary Display List File"; break;
            case Zelda::FileKind::Bfn:
                msg += "Binary Font File"; break;
            case Zelda::FileKind::Bnr:
                msg += "Banner File"; break;
            case Zelda::FileKind::Bti:
                msg += "Binary Texture Image File"; break;
            case Zelda::FileKind::Dds:
                msg += "BC1 Compressed Image File"; break;
            case Zelda::FileKind::Jpc:
                msg += "Particle File"; break;
            case Zelda::FileKind::Unknown:
                msg += "Unkown File"; break;
            default: break;
        }
        if (static_cast<u32>(kind & Zelda::FileKind::Yaz0))
            msg += " (Yaz0 Compressed)";
    }
    QMessageBox::information(this, "Item Clicked", msg, QMessageBox::Ok);
}

void MainWindow::onOpenGCM()
{
    auto path = QFileDialog::getOpenFileName(this, "Open GCM", QString(), "GCM ROM (*.gcm)");
    if (m_Gcm)
    {
        delete m_Gcm;
    }
    m_Gcm = new Gc::Gcm::Reader(std::make_unique<Utils::FileReader>(path.toStdString()));
    
    ui.treeWidget->clear();
    ui.treeWidget->insertTopLevelItem(0, new QTreeWidgetItem({"root"}));
    auto item = ui.treeWidget->topLevelItem(0);
    u32 curLevel = 0;
    
    m_Gcm->iterFst([this, &item, &curLevel](u32 level, std::string fullPath, Gc::Gcm::FileEntry* entry)
    {
        while (level > curLevel)
        {
            item = item->child(item->childCount()-1);
            curLevel++;
        }
        while (level < curLevel)
        {
            item = item->parent();
            curLevel--;
        }
        item->addChild(new QTreeWidgetItem({m_Gcm->getName(entry)}));
        if (!entry->isDir)
        {
            auto arcItem = item->child(item->childCount()-1);
            auto kind = Zelda::getFileKind(m_Gcm, entry);
            if (static_cast<u32>(kind & Zelda::FileKind::Arc))
            {
                auto data = m_Gcm->getFileReader(entry);
                if (static_cast<u32>(kind & Zelda::FileKind::Yaz0))
                    data = Zelda::Yaz0::createReader(std::move(data));
                auto arc = Zelda::Arc::Reader(std::move(data));
                
                u32 arcLevel = 0;
                arc.iterFiles([&arc, &arcItem, &arcLevel](u32 level, std::string fullPath, Zelda::Arc::FileEntry* entry)
                {
                    while (level > arcLevel)
                    {
                        arcItem = arcItem->child(arcItem->childCount()-1);
                        arcLevel++;
                    }
                    while (level < arcLevel)
                    {
                        arcItem = arcItem->parent();
                        arcLevel--;
                    }
                    arcItem->addChild(new QTreeWidgetItem({arc.getName(entry)}));
                });
            }
        }
    });
}
