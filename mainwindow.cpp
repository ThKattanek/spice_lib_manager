#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->label_lib_directory->setText("");
    ui->label_lib_count->setText("");

    ui->lib_tree->setColumnCount(2);
    ui->lib_tree->setHeaderLabels(QStringList() << tr("Library Name") << tr("Typ"));

    ui->lib_tree->setColumnWidth(0,200);
    ui->lib_tree->setColumnWidth(1,20);

    ui->import_progress->setVisible(false);

    ui->lineEdit_search->setEnabled(false);
    ui->pushButton_search->setEnabled(false);
    ui->search_matches->setEnabled(false);

    ui->search_matches->setColumnCount(2);
    ui->search_matches->setHorizontalHeaderLabels(QStringList() << tr("Name") << tr("Library"));
    ui->search_matches->setColumnWidth(0,200);
    ui->search_matches->setColumnWidth(1,400);

    lib_count = 0;
    lib_dir = QDir(QDir::homePath());
}

MainWindow::~MainWindow()
{
    delete ui;
}

uint MainWindow::ScanLibDir(QDir &lib_dir, QTreeWidget &tree_list)
{
    uint subcircuit_count = 0;

    tree_list.clear();

    lib_dir.setFilter(QDir::Files);

    ui->import_progress->setMinimum(0);
    ui->import_progress->setMaximum(lib_dir.count()-1);
    ui->import_progress->setValue(0);
    ui->import_progress->setVisible(true);

    ui->lineEdit_search->setEnabled(false);
    ui->pushButton_search->setEnabled(false);
    ui->search_matches->setEnabled(false);

    for(uint i=0; i<lib_dir.count(); i++)
    {
        ui->import_progress->setValue(i);

        QString extension = lib_dir.entryList()[i].right(4).toLower();

        if(extension == ".lib")
        {
            QTreeWidgetItem *itm = new QTreeWidgetItem(ui->lib_tree);


            /// SUBCIRCUITS SEARCH AND ADD TO PARENT ITEM
            uint count = ScanLib(*itm, lib_dir.path() + "/" + lib_dir.entryList()[i]);
            subcircuit_count += count;

            itm->setText(0, lib_dir.entryList()[i]);

            if(count != 0) tree_list.addTopLevelItem(itm);
        }
    }

    ui->import_progress->setVisible(false);

    if(subcircuit_count > 0)
    {
        ui->lineEdit_search->setEnabled(true);
        ui->pushButton_search->setEnabled(true);
        ui->search_matches->setEnabled(true);
    }

    return subcircuit_count;
}

uint MainWindow::ScanLib(QTreeWidgetItem &itm, QString filename)
{
    uint subcircuit_count = 0;

    QFile file(filename);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
            while (!in.atEnd()) {
                QString line = in.readLine();

                CleanTextLine(line);
                QStringList list1 = line.split(" ");

                if(list1[0].compare(".SUBCKT", Qt::CaseInsensitive) == 0)
                {
                    subcircuit_count++;
                    QTreeWidgetItem *itm_child = new QTreeWidgetItem(itm);
                    itm_child->setText(0,list1[1]);
                    itm_child->setText(1,"SUBCIRCUIT");
                    itm.addChild(itm_child);
                }
                else if(list1[0].compare(".MODEL", Qt::CaseInsensitive) == 0)
                {
                    subcircuit_count++;
                    QTreeWidgetItem *itm_child = new QTreeWidgetItem(itm);
                    itm_child->setText(0,list1[1]);
                    itm_child->setText(1,"MODEL");
                    itm.addChild(itm_child);
                }
            }
        file.close();
    }

    return subcircuit_count;
}

void MainWindow::CleanTextLine(QString &str)
{
    str.replace('\t',' ');

    bool is_space = false;

    for(int i=0; i<str.length(); i++)
    {
        if(is_space)
        {
            if(str.at(i) == ' ')
            {
                str.remove(i,1);
                i--;
            }
        }

        if(str.at(i) == ' ')
            is_space = true;
        else
            is_space = false;
    }
}


void MainWindow::on_select_lib_directory_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), lib_dir.path(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    lib_dir = QDir(dir);
    ui->label_lib_directory->setText(dir);

    // clear search matches
    int count = ui->search_matches->rowCount();
    for(int i=0; i < count; i++)
        ui->search_matches->removeRow(0);

    lib_count = ScanLibDir(lib_dir, *ui->lib_tree);

    ui->label_lib_count->setText(tr("Subcircuits: ") + QVariant(lib_count).toString());
}

void MainWindow::on_pushButton_search_clicked()
{
    int count = ui->search_matches->rowCount();
    for(int i=0; i < count; i++)
        ui->search_matches->removeRow(0);

    Qt::MatchFlags match_flags = Qt::MatchRecursive;

    if(ui->checkBox_wildcard->isChecked()) match_flags |= Qt::MatchWildcard;
    if(ui->checkBox_regex->isChecked()) match_flags |= Qt::MatchRegExp;

    QList<QTreeWidgetItem*> item_list = ui->lib_tree->findItems(ui->lineEdit_search->text(), match_flags);

    for(int i=0; i<item_list.count(); i++)
    {
        ui->search_matches->insertRow(ui->search_matches->rowCount());

        // name
        QTableWidgetItem *itm = new QTableWidgetItem( item_list[i]->text(0));
        ui->search_matches->setItem(ui->search_matches->rowCount()-1,0,itm);

        // library
        itm = new QTableWidgetItem( item_list[i]->parent()->text(0));
        ui->search_matches->setItem(ui->search_matches->rowCount()-1,1,itm);
    }

}

void MainWindow::on_lineEdit_search_returnPressed()
{
    on_pushButton_search_clicked();
}
