#include "thirdwindow.h"
#include "ui_thirdwindow.h"

#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTableView>
#include <QSqlQueryModel>
#include <QStandardItemModel>
#include <QRegularExpression>
#include <QStyledItemDelegate>
#include <QString>


thirdwindow::thirdwindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::thirdwindow)
{
    ui->setupUi(this);
    fo = new formateur();
}

thirdwindow::~thirdwindow()
{
    delete ui;
    delete fo;
}

void thirdwindow::on_pushButton_clicked()
{
    QString nomFormateur = ui->lineEdit_nom->text();
    QString cinStr = ui->lineEdit_cin->text();
    QString telephoneStr = ui->lineEdit_telephone->text();
    QString email = ui->lineEdit_email->text();

    if (nomFormateur.isEmpty() || cinStr.isEmpty() || telephoneStr.isEmpty() || email.isEmpty()) {
        QMessageBox::warning(this, "Erreur de saisie", "Tous les champs doivent être remplis.");
        return;
    }

    if (cinStr.length() != 8 || telephoneStr.length() != 8) {
        QMessageBox::warning(this, "Erreur de saisie", "Le CIN et le numéro de téléphone doivent contenir 8 chiffres.");
        return;
    }

    bool cinOk, telephoneOk;
    int cin = cinStr.toInt(&cinOk);
    int telephone = telephoneStr.toInt(&telephoneOk);
    if (!cinOk || !telephoneOk) {
        QMessageBox::warning(this, "Erreur de saisie", "Le CIN et le numéro de téléphone doivent contenir uniquement des chiffres.");
        return;
    }

    QRegularExpression emailRegex("^[\\w\\.-]+@[\\w\\.-]+\\.[a-zA-Z]{2,}$");
    QRegularExpressionMatch match = emailRegex.match(email);
    if (!match.hasMatch()) {
        QMessageBox::warning(this, "Erreur de saisie", "L'adresse email n'est pas valide.");
        return;
    }

    fo = new formateur(nomFormateur, cinStr, telephoneStr, email);

    if (fo->ajout()) {
        QMessageBox::information(this, "Ajout réussi", "Formateur ajouté avec succès.");
        ui->lineEdit_nom->clear();
        ui->lineEdit_cin->clear();
        ui->lineEdit_telephone->clear();
        ui->lineEdit_email->clear();
    } else {
        QMessageBox::critical(this, "Erreur", "Échec de l'ajout du formateur.");
    }
}

void thirdwindow::on_pushButton_2_clicked()
{
    if (fo == nullptr) {
        QMessageBox::warning(this, "Erreur", "Aucun formateur n'a été créé.");
        return;
    }

    QSqlQueryModel* model = fo->affiche();
    if (model) {
        ui->tableView->setModel(model);
        ui->tableView->resizeColumnsToContents();
        ui->tableView->setItemDelegateForColumn(1, new QStyledItemDelegate(this));
        ui->tableView->setItemDelegateForColumn(2, new QStyledItemDelegate(this));
    } else {
        QMessageBox::critical(this, "Erreur d'affichage", "Erreur lors de l'affichage des formateurs.");
    }
}

void thirdwindow::on_pushButton_3_clicked()
{
    QModelIndex selectedIndex = ui->tableView->currentIndex();
    if (!selectedIndex.isValid()) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un formateur à modifier.");
        return;
    }

    QString nomFormateur = ui->lineEdit_nom->text();
    QString cinStr = ui->lineEdit_cin->text();
    QString telephoneStr = ui->lineEdit_telephone->text();
    QString email = ui->lineEdit_email->text();

    if (nomFormateur.isEmpty() || cinStr.isEmpty() || telephoneStr.isEmpty() || email.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    fo->setNomformateur(nomFormateur);
    fo->setCin(cinStr);
    fo->setTelephone(telephoneStr);
    fo->setEmail(email);

    if (fo->modifie()) {
        QMessageBox::information(this, "Modification réussie", "Les informations du formateur ont été modifiées avec succès.");
        ui->tableView->setModel(fo->affiche());
    } else {
        QMessageBox::critical(this, "Erreur de modification", "Erreur lors de la modification des informations.");
    }
}


void thirdwindow::on_pushButton_4_clicked()
{
    // Vérifiez que l'utilisateur a sélectionné une ligne dans la tableview
    if (!ui->tableView->currentIndex().isValid()) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner un formateur à supprimer.");
        return;
    }

    // Supposons que le CIN soit dans la deuxième colonne
    QString cinASupprimer = ui->tableView->model()->index(ui->tableView->currentIndex().row(), 1).data().toString();

    // Confirmez la suppression
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation", "Êtes-vous sûr de vouloir supprimer ce formateur?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Supprimez le formateur
        if (fo->supprime(cinASupprimer)) {
            ui->tableView->setModel(fo->affiche());  // Rafraîchissez la tableview
            QMessageBox::information(this, "Suppression réussie", "Le formateur a été supprimé avec succès.");
        } else {
            QMessageBox::critical(this, "Erreur de suppression", "Erreur lors de la suppression du formateur.");
        }
    }
}

void thirdwindow::on_tableView_clicked(const QModelIndex &index)
{
    if (index.isValid()) {
        QString nomFormateur = ui->tableView->model()->index(index.row(), 0).data().toString();
        QString cinStr = ui->tableView->model()->index(index.row(), 1).data().toString();
        QString telephoneStr = ui->tableView->model()->index(index.row(), 2).data().toString();
        QString email = ui->tableView->model()->index(index.row(), 3).data().toString();

        ui->lineEdit_nom->setText(nomFormateur);
        ui->lineEdit_cin->setText(cinStr);
        ui->lineEdit_telephone->setText(telephoneStr);
        ui->lineEdit_email->setText(email);
    }
}

