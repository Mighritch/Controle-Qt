#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "secondwindow.h"
#include "thirdwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , secondWindow(nullptr)  // Initialiser le pointeur à nullptr
    , thirdWindow(nullptr)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
    // Ne pas supprimer secondWindow ici pour éviter que la fenêtre secondaire soit fermée aussi
    // Si vous créez secondWindow ici, assurez-vous qu'il n'y a pas de suppression ici.
}

void MainWindow::on_pushButton_2_clicked()
{
    if (!secondWindow) {
        secondWindow = new secondwindow();  // Créer une nouvelle instance de secondwindow sans parent
    }
    secondWindow->show();  // Afficher la fenêtre secondwindow
    secondWindow->raise(); // Amener la fenêtre au premier plan
    secondWindow->activateWindow(); // Activer la fenêtre

    this->close();  // Fermer la fenêtre principale
}

void MainWindow::on_pushButton_clicked()
{
    if (!thirdWindow) {
        thirdWindow = new thirdwindow();
    }
    thirdWindow->show();
    thirdWindow->raise();
    thirdWindow->activateWindow();

    this->close();
}
