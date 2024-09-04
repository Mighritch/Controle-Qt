#include "secondwindow.h"
#include "mainwindow.h"
#include "ui_secondwindow.h"
#include "DateDelegate.h"
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
#include <QSqlRecord>

secondwindow::secondwindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::secondwindow)
{
    ui->setupUi(this);
    fo = new formation(); // Initialiser 'fo'

    ui->tableview->setModel(fo->affiche());
    ui->tableview->resizeRowsToContents();  // Redimensionner les lignes selon le contenu
    ui->tableview->resizeColumnsToContents();  // Redimensionner les colonnes selon le contenu
    connect(ui->tableview, &QTableView::clicked, this, &secondwindow::on_tableview_clicked);
    connect(ui->lineEdit_4, &QLineEdit::textChanged, this, &secondwindow::on_lineEdit_4_textChanged);
    connect(ui->lineEdit_5, &QLineEdit::textChanged, this, &secondwindow::on_lineEdit_5_textChanged);
    connect(ui->dateEdit_2, &QDateEdit::dateChanged, this, &secondwindow::on_dateEdit_2_dateChanged);

}

secondwindow::~secondwindow()
{
    delete ui;
    delete fo;
}

void secondwindow::on_pushButton_clicked() {
    QString titre = ui->lineEdit->text();
    QString description = ui->lineEdit_2->text();
    QDate dateFormation = ui->dateEdit->date();
    QString nomFormateur = ui->lineEdit_3->text();
    int nombrePlaces = ui->lineEdit_6->text().toInt();

    if (titre.isEmpty() || description.isEmpty() || nomFormateur.isEmpty() || ui->lineEdit_6->text().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    if (dateFormation < QDate::currentDate()) {
        QMessageBox::warning(this, "Erreur", "La date de formation ne peut pas être antérieure à la date d'aujourd'hui.");
        return;
    }

    if (titre.length() > 20 || description.length() > 50 || nomFormateur.length() > 20 || nombrePlaces <= 0) {
        QMessageBox::warning(this, "Erreur", "Longueur des champs invalide ou nombre de places incorrect.");
        return;
    }

    // Vérification de l'existence d'une formation avec la même date et le même formateur
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM formation WHERE date_formation = :dateFormation AND nomformateur = :nomFormateur");
    query.bindValue(":dateFormation", dateFormation);
    query.bindValue(":nomFormateur", nomFormateur);

    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur de requête", "Erreur lors de la vérification de l'existence d'une formation similaire.");
        return;
    }

    query.next();
    int count = query.value(0).toInt();
    if (count > 0) {
        QMessageBox::warning(this, "Erreur", "Une formation avec la même date et le même formateur existe déjà.");
        return;
    }

    fo->setTitre(titre);
    fo->setDescription(description);
    fo->setDateFormation(dateFormation);
    fo->setNomFormateur(nomFormateur);
    fo->setNombrePlaces(nombrePlaces);

    if (fo->ajout()) {
        QMessageBox::information(this, "Ajout réussi", "Formation ajoutée avec succès.");
        ui->tableview->setModel(fo->affiche());
        ui->tableview->resizeRowsToContents();  // Redimensionner les lignes selon le contenu
        ui->tableview->resizeColumnsToContents();  // Redimensionner les colonnes selon le contenu
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        ui->dateEdit->setDate(QDate::currentDate());
        ui->lineEdit_3->clear();
        ui->lineEdit_6->clear();
    }
}

void secondwindow::on_pushButton_2_clicked()
{
    QSqlQueryModel* model = fo->affiche();
    if (model) {
        ui->tableview->setModel(model);
        ui->tableview->resizeRowsToContents();  // Redimensionner les lignes selon le contenu
        ui->tableview->resizeColumnsToContents();  // Redimensionner les colonnes selon le contenu
        DateDelegate *dateDelegate = new DateDelegate(this);
        ui->tableview->setItemDelegateForColumn(2, dateDelegate);
    } else {
        QMessageBox::critical(this, "Erreur d'affichage", "Erreur lors de l'affichage des formations.");
    }
}

void secondwindow::on_pushButton_3_clicked()
{
    if (!ui->tableview->currentIndex().isValid()) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une formation à modifier.");
        return;
    }

    QString ancienTitre = ui->tableview->model()->index(ui->tableview->currentIndex().row(), 0).data().toString();
    QString titre = ui->lineEdit->text();
    QString description = ui->lineEdit_2->text();
    QDate dateformation = ui->dateEdit->date();
    QString nomformateur = ui->lineEdit_3->text();
    int nombreplaces = ui->lineEdit_6->text().toInt();

    if (titre.isEmpty() || description.isEmpty() || nomformateur.isEmpty() || ui->lineEdit_6->text().isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez remplir tous les champs.");
        return;
    }

    fo->setTitre(titre);
    fo->setDescription(description);
    fo->setDateFormation(dateformation);
    fo->setNomFormateur(nomformateur);
    fo->setNombrePlaces(nombreplaces);

    if (fo->modifie(ancienTitre)) {
        ui->tableview->setModel(fo->affiche());
        ui->tableview->resizeRowsToContents();  // Redimensionner les lignes selon le contenu
        ui->tableview->resizeColumnsToContents();  // Redimensionner les colonnes selon le contenu
        QMessageBox::information(this, "Modification réussie", "La modification s'est faite avec succès.");
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        ui->dateEdit->setDate(QDate::currentDate());
        ui->lineEdit_3->clear();
        ui->lineEdit_6->clear();
    } else {
        QMessageBox::critical(this, "Erreur de modification", "Erreur lors de la modification.");
    }
}


void secondwindow::on_pushButton_4_clicked()
{
    if (!ui->tableview->currentIndex().isValid()) {
        QMessageBox::warning(this, "Erreur", "Veuillez sélectionner une formation à supprimer.");
        return;
    }

    QString titreASupprimer = ui->tableview->model()->index(ui->tableview->currentIndex().row(), 0).data().toString();

    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirmation", "Êtes-vous sûr de vouloir supprimer cette formation?",
                                  QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (fo->supprime(titreASupprimer)) {
            ui->tableview->setModel(fo->affiche());
            ui->tableview->resizeRowsToContents();  // Redimensionner les lignes selon le contenu
            ui->tableview->resizeColumnsToContents();  // Redimensionner les colonnes selon le contenu
            QMessageBox::information(this, "Suppression réussie", "La formation a été supprimée avec succès.");
            ui->lineEdit->clear();
            ui->lineEdit_2->clear();
            ui->dateEdit->setDate(QDate::currentDate());
            ui->lineEdit_3->clear();
            ui->lineEdit_6->clear();
        } else {
            QMessageBox::critical(this, "Erreur de suppression", "Erreur lors de la suppression de la formation.");
        }
    }
}

void secondwindow::on_pushButton_5_clicked()
{
    QString titre = ui->lineEdit_4->text();

    if (titre.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un titre à rechercher.");
        return;
    }

    QSqlQueryModel* model = fo->rechercherParTitre(titre);

    if (model && model->rowCount() > 0) {
        ui->tableview->setModel(model);
        ui->tableview->resizeRowsToContents();  // Redimensionner les lignes selon le contenu
        ui->tableview->resizeColumnsToContents();  // Redimensionner les colonnes selon le contenu
    } else {
        QMessageBox::warning(this, "Aucun résultat", "Aucun résultat trouvé.");
    }
}

void secondwindow::on_pushButton_6_clicked()
{
    QSqlQueryModel* model = fo->trierParTitreAsc();

    if (model) {
        ui->tableview->setModel(model);
        ui->tableview->resizeRowsToContents();  // Redimensionner les lignes selon le contenu
        ui->tableview->resizeColumnsToContents();  // Redimensionner les colonnes selon le contenu
    } else {
        QMessageBox::warning(this, "Erreur", "Impossible de trier les titres.");
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
    int lineHeight = 400;
    int cellWidth = 1500;

    QSqlQuery query("SELECT * FROM formation");
    int columnCount = query.record().count();

    painter.drawText(0, yPos, "Nos formations");
    yPos += lineHeight * 2;

    for (int i = 0; i < columnCount; ++i) {
        QString headerText = query.record().fieldName(i);
        painter.drawText(i * cellWidth, yPos, cellWidth, lineHeight, Qt::AlignCenter, headerText);
    }
    yPos += lineHeight;

    while (query.next()) {
        for (int i = 0; i < columnCount; ++i) {
            QString text = query.value(i).toString();
            painter.drawText(i * cellWidth, yPos, cellWidth, lineHeight, Qt::AlignLeft, text);
        }
        yPos += lineHeight;
    }

    painter.end();
    QMessageBox::information(this, "PDF Créé", "Le fichier PDF a été créé avec succès.");
}

void secondwindow::on_pushButton_8_clicked()
{
    QSqlQueryModel* model = fo->trierParDateAsc();

    if (model) {
        ui->tableview->setModel(model);
    } else {
        QMessageBox::warning(this, "Erreur", "Impossible de trier les dates.");
    }
}

void secondwindow::on_pushButton_9_clicked()
{
    QSqlQueryModel* model = fo->trierParNomAsc();

    if (model) {
        ui->tableview->setModel(model);
    } else {
        QMessageBox::warning(this, "Erreur", "Impossible de trier les noms des formateurs.");
    }
}

void secondwindow::on_pushButton_10_clicked()
{
    QString nomformateur = ui->lineEdit_5->text();

    if (nomformateur.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un nom à rechercher.");
        return;
    }

    QSqlQueryModel* model = fo->rechercherParNom(nomformateur);

    if (model && model->rowCount() > 0) {
        ui->tableview->setModel(model);

    } else {
        QMessageBox::warning(this, "Aucun résultat", "Aucun résultat trouvé.");
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

void secondwindow::on_pushButton_12_clicked()
{
    QString titre = ui->lineEdit_7->text(); // Remplacez par le nom correct du champ pour le titre
      int nombrePlacesReservees = ui->lineEdit_8->text().toInt(); // Remplacez par le nom correct du champ pour le nombre de places

      // Créer une instance de votre classe formation
      formation f;

      // Appeler la méthode reserve
      bool success = f.reserve(titre, nombrePlacesReservees);

      if(success) {
          QMessageBox::information(this, "Réservation", "La réservation a été effectuée avec succès.");
          ui->tableview->setModel(fo->affiche());
      } else {
          QMessageBox::warning(this, "Erreur", "La réservation a échoué. Veuillez vérifier les informations.");
      }
}

void secondwindow::on_pushButton_13_clicked()
{
    // Créez une instance de MainWindow
    MainWindow *mainWindow = new MainWindow();

    // Affichez MainWindow
    mainWindow->show();
    mainWindow->raise();
    mainWindow->activateWindow();

    // Fermez la fenêtre actuelle (secondwindow)
    this->close();
}

void secondwindow::on_pushButton_14_clicked()
{
    // Calculer le nombre de formations
        int nombreTotalFormations = fo->calculerNombreFormations();

        // Afficher le message
        QString message = QString("Le nombre total de formations est : %1").arg(nombreTotalFormations);
        QMessageBox::information(this, "Nombre de Formations", message);
}

void secondwindow::on_tableview_clicked(const QModelIndex &index)
{
    if (index.isValid()) {
        QString titre = ui->tableview->model()->index(index.row(), 0).data().toString();
        QString description = ui->tableview->model()->index(index.row(), 1).data().toString();
        QDate dateformation = ui->tableview->model()->index(index.row(), 2).data().toDate();
        QString nomformateur = ui->tableview->model()->index(index.row(), 3).data().toString();
        int nombreplaces = ui->tableview->model()->index(index.row(), 4).data().toInt();

        // Remplir les champs du formulaire
        ui->lineEdit->setText(titre);
        ui->lineEdit_2->setText(description);
        ui->dateEdit->setDate(dateformation);
        ui->lineEdit_3->setText(nomformateur);
        ui->lineEdit_6->setText(QString::number(nombreplaces));
    }
}


void secondwindow::on_lineEdit_4_textChanged(const QString &titre)
{
    QSqlQueryModel* model = fo->rechercherParTitre(titre);

    if (model && model->rowCount() > 0) {
        ui->tableview->setModel(model);
        ui->tableview->resizeRowsToContents();
        ui->tableview->resizeColumnsToContents();
    } else {
        ui->tableview->setModel(nullptr);
        QMessageBox::warning(this, "Aucun résultat", "Aucun résultat trouvé.");
    }
}

void secondwindow::on_lineEdit_5_textChanged(const QString &nomformateur)
{
    QSqlQueryModel* model = fo->rechercherParNom(nomformateur);

    if (model && model->rowCount() > 0) {
        ui->tableview->setModel(model);
        ui->tableview->resizeRowsToContents();
        ui->tableview->resizeColumnsToContents();
    } else {
        ui->tableview->setModel(nullptr);
        QMessageBox::warning(this, "Aucun résultat", "Aucun résultat trouvé.");
    }
}


void secondwindow::on_dateEdit_2_dateChanged(const QDate &dateFormation)
{
    if (!dateFormation.isValid()) {
        return;
    }

    QSqlQueryModel* model = fo->rechercherParDate(dateFormation);

    if (model && model->rowCount() > 0) {
        ui->tableview->setModel(model);
        ui->tableview->resizeRowsToContents();
        ui->tableview->resizeColumnsToContents();
    } else {
        ui->tableview->setModel(nullptr);
        QMessageBox::warning(this, "Aucun résultat", "Aucun résultat trouvé.");
    }
}
