#include "thirdwindow.h"
#include "ui_thirdwindow.h"
#include "mainwindow.h"

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
#include <QPdfWriter>
#include <QPainter>
#include <QFileDialog>

thirdwindow::thirdwindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::thirdwindow)
{
    ui->setupUi(this);
    fo = new formateur();

    ui->tableView->setModel(fo->affiche());
    ui->tableView->resizeRowsToContents();  // Redimensionner les lignes selon le contenu
    ui->tableView->resizeColumnsToContents();  // Redimensionner les colonnes selon le contenu
    connect(ui->lineEdit, &QLineEdit::textChanged, this, &thirdwindow::on_lineEdit_textChanged);
    connect(ui->lineEdit_2, &QLineEdit::textChanged, this, &thirdwindow::on_lineEdit_2_textChanged);
    connect(ui->lineEdit_3, &QLineEdit::textChanged, this, &thirdwindow::on_lineEdit_3_textChanged);

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

    // Vérifiez si le CIN ou le numéro de téléphone existent déjà
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM formateur WHERE cin = :cin OR telephone = :telephone");
    query.bindValue(":cin", cinStr);
    query.bindValue(":telephone", telephoneStr);
    if (!query.exec()) {
        QMessageBox::critical(this, "Erreur de requête", "Erreur lors de la vérification de l'existence du CIN ou du téléphone.");
        return;
    }

    query.next();
    int count = query.value(0).toInt();
    if (count > 0) {
        QMessageBox::warning(this, "Erreur", "Le CIN ou le numéro de téléphone existe déjà dans la base de données.");
        return;
    }

    // Ajouter le formateur
    fo = new formateur(nomFormateur, cinStr, telephoneStr, email);
    if (fo->ajout()) {
        QMessageBox::information(this, "Ajout réussi", "Formateur ajouté avec succès.");
        ui->tableView->setModel(fo->affiche());
        ui->lineEdit_nom->clear();
        ui->lineEdit_cin->clear();
        ui->lineEdit_telephone->clear();
        ui->lineEdit_email->clear();
        on_pushButton_2_clicked(); // Rafraîchit la liste des formateurs après l'ajout
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
        ui->lineEdit_nom->clear();
        ui->lineEdit_cin->clear();
        ui->lineEdit_telephone->clear();
        ui->lineEdit_email->clear();
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
            ui->lineEdit_nom->clear();
            ui->lineEdit_cin->clear();
            ui->lineEdit_telephone->clear();
            ui->lineEdit_email->clear();
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

void thirdwindow::on_pushButton_5_clicked()
{
    // Récupérer le nom saisi par l'utilisateur dans un QLineEdit ou autre widget
    QString nomFormateur = ui->lineEdit->text();  // lineEdit_nomFormateur est un exemple de nom pour un QLineEdit

    // Créer une instance de la classe formateur
    formateur formateurInstance;

    // Effectuer la recherche par nom
    QSqlQueryModel* model = formateurInstance.rechercheParNom(nomFormateur);

    if (model) {
        // Associer le modèle de résultats à une QTableView pour afficher les résultats
        ui->tableView->setModel(model);  // tableView est un exemple de nom pour un QTableView
        ui->lineEdit->clear();

    } else {
        // Gérer le cas où la recherche a échoué
        qDebug() << "Erreur lors de la recherche par nom.";
    }
}

void thirdwindow::on_pushButton_6_clicked()
{
    // Récupérer le numéro de téléphone saisi par l'utilisateur dans un QLineEdit ou autre widget
    QString telephone = ui->lineEdit_2->text();  // lineEdit_telephone est un exemple de nom pour un QLineEdit

    // Créer une instance de la classe formateur
    formateur formateurInstance;

    // Effectuer la recherche par téléphone
    QSqlQueryModel* model = formateurInstance.rechercheParTelephone(telephone);

    if (model) {
        // Associer le modèle de résultats à une QTableView pour afficher les résultats
        ui->tableView->setModel(model);  // tableView est un exemple de nom pour un QTableView
        ui->lineEdit_2->clear();

    } else {
        // Gérer le cas où la recherche a échoué
        qDebug() << "Erreur lors de la recherche par téléphone.";
    }
}

void thirdwindow::on_pushButton_7_clicked()
{
    // Récupérer l'email saisi par l'utilisateur dans un QLineEdit ou autre widget
    QString email = ui->lineEdit_3->text();  // lineEdit_email est un exemple de nom pour un QLineEdit

    // Créer une instance de la classe formateur
    formateur formateurInstance;

    // Effectuer la recherche par email
    QSqlQueryModel* model = formateurInstance.rechercheParEmail(email);

    if (model) {
        // Associer le modèle de résultats à une QTableView pour afficher les résultats
        ui->tableView->setModel(model);  // tableView est un exemple de nom pour un QTableView
        ui->lineEdit_3->clear();

    } else {
        // Gérer le cas où la recherche a échoué
        qDebug() << "Erreur lors de la recherche par email.";
    }
}

void thirdwindow::on_pushButton_8_clicked()
{
    // Créer une instance de la classe formateur
    formateur formateurInstance;

    // Récupérer le nombre de formateurs
    int nombreFormateurs = formateurInstance.compteFormateurs();

    // Vérifier s'il y a eu une erreur lors du comptage
    if (nombreFormateurs >= 0) {
        // Afficher un message avec le nombre de formateurs
        QMessageBox::information(this, "Nombre de formateurs", "Le nombre total de formateurs est : " + QString::number(nombreFormateurs));
    } else {
        // En cas d'erreur, afficher un message d'erreur
        QMessageBox::critical(this, "Erreur", "Une erreur est survenue lors du comptage des formateurs.");
    }
}


void thirdwindow::on_pushButton_9_clicked()
{
    // Récupérer les données des formateurs
    formateur formateurInstance;
    QSqlQueryModel* model = formateurInstance.affiche();

    if (model->rowCount() == 0) {
        QMessageBox::warning(this, "Avertissement", "Aucun formateur trouvé dans la base de données.");
        return;
    }

    // Demander à l'utilisateur où enregistrer le fichier PDF
    QString filePath = QFileDialog::getSaveFileName(this, "Enregistrer le PDF", "", "PDF Files (*.pdf)");
    if (filePath.isEmpty()) {
        return; // L'utilisateur a annulé l'enregistrement
    }

    // Créer un QPdfWriter pour générer le PDF
    QPdfWriter pdfWriter(filePath);
    pdfWriter.setPageSize(QPageSize(QPageSize::A4));
    pdfWriter.setResolution(300);

    // Créer un QPainter pour dessiner sur le PDF
    QPainter painter(&pdfWriter);
    painter.setPen(Qt::black);
    painter.setFont(QFont("Arial", 12));

    // Titre du document
    painter.drawText(100, 100, "Liste des Formateurs");

    int yPosition = 150;  // Position initiale en Y pour les données

    // Récupérer le nombre de colonnes
    int columnCount = model->columnCount();

    // Dessiner les en-têtes des colonnes
    for (int column = 0; column < columnCount; ++column) {
        painter.drawText(100 + column * 200, yPosition, model->headerData(column, Qt::Horizontal).toString());
    }

    yPosition += 50;  // Déplacer la position Y pour commencer à dessiner les données

    // Dessiner les données des formateurs
    for (int row = 0; row < model->rowCount(); ++row) {
        for (int column = 0; column < columnCount; ++column) {
            painter.drawText(100 + column * 200, yPosition, model->data(model->index(row, column)).toString());
        }
        yPosition += 50;  // Passer à la ligne suivante
    }

    painter.end();  // Terminer le dessin

    QMessageBox::information(this, "Succès", "Le PDF a été généré avec succès.");
}

void thirdwindow::on_pushButton_10_clicked() {
    // Crée une instance de la classe formateur
    formateur formateurInstance;

    // Récupère les formateurs triés par nom dans l'ordre ascendant
    QSqlQueryModel* model = formateurInstance.triParNomAsc();

    if (model) {
        // Associe le modèle à la vue pour afficher les résultats
        ui->tableView->setModel(model);
    } else {
        qDebug() << "Erreur lors du tri des formateurs par nom.";
    }
}


void thirdwindow::on_pushButton_11_clicked() {
    // Crée une instance de la classe formateur
    formateur formateurInstance;

    // Récupère les formateurs triés par téléphone dans l'ordre ascendant
    QSqlQueryModel* model = formateurInstance.triParTelephoneAsc();

    if (model) {
        // Associe le modèle à la vue pour afficher les résultats
        ui->tableView->setModel(model);
    } else {
        qDebug() << "Erreur lors du tri des formateurs par téléphone.";
    }
}

void thirdwindow::on_pushButton_12_clicked() {
    // Crée une instance de la classe formateur
    formateur formateurInstance;

    // Récupère les formateurs triés par email dans l'ordre ascendant
    QSqlQueryModel* model = formateurInstance.triParCinAsc();

    if (model) {
        // Associe le modèle à la vue pour afficher les résultats
        ui->tableView->setModel(model);
    } else {
        qDebug() << "Erreur lors du tri des formateurs par email.";
    }
}

void thirdwindow::on_pushButton_13_clicked()
{
    // Créez une instance de MainWindow
    MainWindow *mainWindow = new MainWindow();

    // Affichez MainWindow
    mainWindow->show();
    mainWindow->raise();
    mainWindow->activateWindow();

    // Fermez la fenêtre actuelle (thirdwindow)
    this->close();
}

void thirdwindow::on_pushButton_14_clicked()
{
    // Récupérer le nom du formateur depuis l'interface utilisateur
    QString nomformateur = ui->lineEdit_4->text();

    if (nomformateur.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Veuillez entrer un nom de formateur.");
        return;
    }

    // Préparer et exécuter la requête SQL pour obtenir les formations
    QSqlQuery query;
    query.prepare("SELECT titre, description, date_formation, nombreplaces FROM formation WHERE nomformateur = :nomformateur");
    query.bindValue(":nomformateur", nomformateur);

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête pour afficher les formations du formateur:" << query.lastError();
        QMessageBox::critical(this, "Erreur", "Une erreur est survenue lors de la récupération des formations.");
        return;
    }

    // Construire le message à afficher
    QString message = "Formations pour " + nomformateur + " :\n\n";

    int count = 0;
    while (query.next()) {
        QString titre = query.value(0).toString();
        QString description = query.value(1).toString();
        QString dateFormation = query.value(2).toString();
        int nombrePlaces = query.value(3).toInt();

        message += "Titre: " + titre + "\n";
        message += "Description: " + description + "\n";
        message += "Date: " + dateFormation + "\n";
        message += "Nombre de Places: " + QString::number(nombrePlaces) + "\n\n";
        count++;
    }

    if (count == 0) {
        message = "Aucune formation trouvée pour le formateur " + nomformateur + ".";
    }

    // Afficher le message dans une boîte de dialogue
    QMessageBox::information(this, "Formations", message);
}

void thirdwindow::on_lineEdit_textChanged(const QString &arg1)
{
    QString nomformateur = arg1;  // Utilise l'argument comme nom du formateur

    QSqlQueryModel* model = fo->rechercheParNom(nomformateur);

    if (model && model->rowCount() > 0) {
        ui->tableView->setModel(model);
        ui->tableView->resizeRowsToContents();
        ui->tableView->resizeColumnsToContents();
    } else {
        ui->tableView->setModel(nullptr);
        QMessageBox::warning(this, "Aucun résultat", "Aucun résultat trouvé.");
    }
}

void thirdwindow::on_lineEdit_2_textChanged(const QString &arg1)
{
    QString telephone = arg1;  // Utilise l'argument comme numéro de téléphone
    QSqlQueryModel* model = fo->rechercheParTelephone(telephone);

    if (model && model->rowCount() > 0) {
        ui->tableView->setModel(model);
        ui->tableView->resizeRowsToContents();
        ui->tableView->resizeColumnsToContents();
    } else {
        ui->tableView->setModel(nullptr);
        QMessageBox::warning(this, "Aucun résultat", "Aucun résultat trouvé.");
    }
}

void thirdwindow::on_lineEdit_3_textChanged(const QString &arg1)
{
    QString email = arg1;  // Utilise l'argument comme adresse email
    QSqlQueryModel* model = fo->rechercheParEmail(email);

    if (model && model->rowCount() > 0) {
        ui->tableView->setModel(model);
        ui->tableView->resizeRowsToContents();
        ui->tableView->resizeColumnsToContents();
    } else {
        ui->tableView->setModel(nullptr);
        QMessageBox::warning(this, "Aucun résultat", "Aucun résultat trouvé.");
    }
}
