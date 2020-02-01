#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QFileDialog>
#include <QTreeWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_select_lib_directory_clicked();

    void on_pushButton_search_clicked();

    void on_lineEdit_search_returnPressed();

private:
    uint ScanLibDir(QDir &lib_dir, QTreeWidget &tree_list);
    uint ScanLib(QTreeWidgetItem &itm, QString filename);
    void CleanTextLine(QString &str);

    Ui::MainWindow *ui;
    QDir lib_dir;
    uint lib_count;
};
#endif // MAINWINDOW_H
