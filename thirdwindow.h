#ifndef THIRDWINDOW_H
#define THIRDWINDOW_H

#include <QMainWindow>
#include "formateur.h"

namespace Ui {
class thirdwindow;
}

class thirdwindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit thirdwindow(QWidget *parent = nullptr);
    ~thirdwindow();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_tableView_clicked(const QModelIndex &index);


private:
    Ui::thirdwindow *ui;
    formateur *fo;
};

#endif // THIRDWINDOW_H
