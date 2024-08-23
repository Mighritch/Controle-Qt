#include "secondwindow.h"
#include "ui_secondwindow.h"
#include "DateDelegate.h";
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QTableView>
#include <QSqlQueryModel>
#include <QMessageBox>
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>

secondwindow::secondwindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::secondwindow)
{
    ui->setupUi(this);
    fo = new formation(); // Initialiser 'fo'

    ui->tableview->setModel(fo->affiche());

}

secondwindow::~secondwindow()
{
    delete ui;
    delete fo;
}

void secondwindow::on_pushButton_clicked()
{
    // Récupérer les données des champs
    QString titre = ui->lineEdit->text();
    QString description = ui->lineEdit_2->text();
    QDate dateformation = ui->dateEdit->date();
    QString nomformateur = ui->lineEdit_3->text();

    // Vérifiez que les champs ne sont pas vides
    if (titre.isEmpty() || description.isEmpty() || nomformateur.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    // Vérifiez que la date de formation est au moins la date actuelle
    if (dateformation < QDate::currentDate()) {
        QMessageBox::warning(this, "Erreur", "La date de formation ne peut pas être antérieure à la date d'aujourd'hui.");
        return;
    }

    // Vérifiez que les longueurs des champs sont valides
    if (titre.length() > 20) {
        QMessageBox::warning(this, "Erreur", "Le titre ne peut pas dépasser 20 caractères.");
        return;
    }

    if (description.length() > 50) {
        QMessageBox::warning(this, "Erreur", "La description ne peut pas dépasser 50 caractères.");
        return;
    }

    if (nomformateur.length() > 20) {
        QMessageBox::warning(this, "Erreur", "Le nom du formateur ne peut pas dépasser 20 caractères.");
        return;
    }

    // Créez une nouvelle formation
    fo = new formation(titre, description, dateformation, nomformateur);

    // Essayez d'ajouter la formation
    if (fo->ajout()) {
        QMessageBox::information(this, "Ajout réussi", "Formation a été ajoutée avec succès.");
        ui->tableview->setModel(fo->affiche());
        // Effacez les champs de saisie
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        ui->dateEdit->clear();
        ui->lineEdit_3->clear();
    } else {
        QMessageBox::critical(this, "Erreur d'ajout", "Le formateur a une formation dans cette date.");
    }
}

void secondwindow::on_pushButton_2_clicked()
{
    if (fo == nullptr) {
        QMessageBox::warning(this, "Erreur", "Aucune formation n'a été créée.");
        return;
    }

    QSqlQueryModel* model = fo->affiche();
    if (model) {
        ui->tableview->setModel(model);

        // Ajuster la taille des colonnes en fonction du contenu
        ui->tableview->resizeColumnsToContents();

        // Créez et appliquez le délégué pour formater les dates
        DateDelegate *dateDelegate = new DateDelegate(this);
        ui->tableview->setItemDelegateForColumn(2, dateDelegate); // Supposons que la colonne 2 contient les dates
        ui->tableview->setModel(model);
        ui->tableview->resizeColumnsToContents();
        ui->tableview->setItemDelegateForColumn(1, new QStyledItemDelegate(this));
        ui->tableview->setItemDelegateForColumn(2, new QStyledItemDelegate(this));
    } else {
        QMessageBox::critical(this, "Erreur d'affichage", "Erreur lors de l'affichage des formations.");
    }
}


void secondwindow::on_pushButton_3_clicked()
{
    // Assurez-vous que l'utilisateur a sélectionné une ligne dans la tableview
    if (!ui->tableview->currentIndex().isValid()) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une formation à modifier.");
        return;
    }

    // Supposons que le titre soit dans la première colonne
    QString ancienTitre = ui->tableview->model()->index(ui->tableview->currentIndex().row(), 0).data().toString();

    // Debug: Affichez l'ancien titre
    qDebug() << "Ancien titre: " << ancienTitre;

    // Récupérer les nouvelles valeurs des champs
    QString titre = ui->lineEdit->text();
    QString description = ui->lineEdit_2->text();
    QDate dateformation = ui->dateEdit->date();
    QString nomformateur = ui->lineEdit_3->text();

    // Vérifiez que les champs ne sont pas vides
    if (titre.isEmpty() || description.isEmpty() || nomformateur.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    fo = new formation(titre, description, dateformation, nomformateur);

    // Passe l'ancien titre pour la modification
    bool test = fo->modifie(ancienTitre);
    if (test) {
        ui->tableview->setModel(fo->affiche());
        QMessageBox::information(this, "Modification réussie", "La modification s'est faite avec succès.");
    } else {
        QMessageBox::critical(this, "Erreur de modification", "Erreur lors de la modification.");
    }
}



void secondwindow::on_pushButton_4_clicked()
{

        // Vérifiez que l'utilisateur a sélectionné une ligne dans la tableview
        if (!ui->tableview->currentIndex().isValid()) {
            QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une formation à supprimer.");
            return;
        }

        // Supposons que le titre soit dans la première colonne
        QString titreASupprimer = ui->tableview->model()->index(ui->tableview->currentIndex().row(), 0).data().toString();

        // Confirmez la suppression
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Confirmation", "Êtes-vous sûr de vouloir supprimer cette formation?",
                                      QMessageBox::Yes | QMessageBox::No);

        if (reply == QMessageBox::Yes) {
            // Supprimez la formation
            if (fo->supprime(titreASupprimer)) {
                ui->tableview->setModel(fo->affiche());  // Rafraîchissez la tableview
                QMessageBox::information(this, "Suppression réussie", "La formation a été supprimée avec succès.");
            } else {
                QMessageBox::critical(this, "Erreur de suppression", "Erreur lors de la suppression de la formation.");
            }
        }
}

void secondwindow::on_pushButton_5_clicked() {
    QString titre = ui->lineEdit_4->text();

    if (titre.isEmpty()) {
        qDebug() << "Veuillez entrer un titre à rechercher.";
        return; // Arrête l'exécution si le titre est vide
    }

    formation form;
    QSqlQueryModel* model = form.rechercherParTitre(titre);

    if (model && model->rowCount() > 0) {
        ui->tableview->setModel(model); // Affiche les résultats dans le QTableView
    } else {
        qDebug() << "Erreur : Aucun résultat trouvé.";
        delete model; // Nettoie le modèle si aucun résultat n'est trouvé
    }
}


void secondwindow::on_pushButton_6_clicked()
{
    formation form;
        QSqlQueryModel* model = form.trierParTitreAsc();

        if (model) {
            ui->tableview->setModel(model); // Supposant que votre QTableView s'appelle tableView
        } else {
            qDebug() << "Erreur : Impossible de trier les titres.";
        }
}

void secondwindow::on_pushButton_7_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save PDF", "", "*.pdf");
    if (fileName.isEmpty()) {
        return;
    }

    QPdfWriter pdfWriter(fileName);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setPageMargins(QMargins(30, 30, 30, 30));

    QPainter painter(&pdfWriter);
    int yPos = 0;
    int lineHeight = 400; // Hauteur de ligne ajustable selon la police et la taille
    int cellWidth = 1500; // Largeur de chaque cellule du tableau

    QSqlQuery query("SELECT * FROM formation");
    int columnCount = query.record().count();

    painter.drawText(0, yPos, "Nos formations");
    yPos += lineHeight; // Augmenter la position Y après le titre
    yPos += lineHeight; // Laisser un espace après le titre

    // Dessiner les en-têtes de colonnes
    for (int i = 0; i < columnCount; ++i) {
        QString headerText = query.record().fieldName(i);
        painter.drawText(i * cellWidth, yPos, cellWidth, lineHeight, Qt::AlignCenter, headerText);
    }
    yPos += lineHeight; // Augmenter la position Y après les en-têtes de colonnes

    // Dessiner les lignes de données
    while (query.next()) {
        for (int i = 0; i < columnCount; ++i) {
            QString text = query.value(i).toString();
            painter.drawText(i * cellWidth, yPos, cellWidth, lineHeight, Qt::AlignLeft, text);
        }
        yPos += lineHeight; // Augmenter la position Y après chaque ligne de données
    }

    painter.end();
    QMessageBox::information(this, "PDF Créé", "Le fichier PDF a été créé avec succès.");
}

void secondwindow::on_pushButton_8_clicked()
{
    formation form;
        QSqlQueryModel* model = form.trierParDateAsc();

        if (model) {
            ui->tableview->setModel(model); // Supposant que votre QTableView s'appelle tableView
        } else {
            qDebug() << "Erreur : Impossible de trier les dates.";
        }
}

void secondwindow::on_pushButton_9_clicked()
{
    formation form;
        QSqlQueryModel* model = form.trierParNomAsc();

        if (model) {
            ui->tableview->setModel(model); // Supposant que votre QTableView s'appelle tableView
        } else {
            qDebug() << "Erreur : Impossible de trier les nom de formateurs.";
        }
}

void secondwindow::on_pushButton_10_clicked()
{
    QString nomformateur = ui->lineEdit_5->text();

    if (nomformateur.isEmpty()) {
        qDebug() << "Veuillez entrer un titre à rechercher.";
        return; // Arrête l'exécution si le titre est vide
    }

    formation form;
    QSqlQueryModel* model = form.rechercherParNom(nomformateur);

    if (model && model->rowCount() > 0) {
        ui->tableview->setModel(model); // Affiche les résultats dans le QTableView
    } else {
        qDebug() << "Erreur : Aucun résultat trouvé.";
        delete model; // Nettoie le modèle si aucun résultat n'est trouvé
    }
}

void secondwindow::on_pushButton_11_clicked()
{
    QDate dateFormation = ui->dateEdit_2->date(); // Assurez-vous que ce nom est correct

    if (!dateFormation.isValid()) {
        qDebug() << "Veuillez sélectionner une date valide.";
        return; // Arrête l'exécution si la date n'est pas valide
    }

    formation form;
    QSqlQueryModel* model = form.rechercherParDate(dateFormation);

    if (model && model->rowCount() > 0) {
        ui->tableview->setModel(model); // Affiche les résultats dans le QTableView
    } else {
        qDebug() << "Erreur : Aucun résultat trouvé pour la date sélectionnée.";
        delete model; // Nettoie le modèle si aucun résultat n'est trouvé
    }
}

