#include "formation.h"
#include <QtSql>
#include <QMessageBox>
#include <QVariant>

formation::formation() {}

formation::formation(const QString titre, const QString description, const QDate& dateformation, const QString nomformateur)
    : titre(titre), description(description), dateFormation(dateformation), nomFormateur(nomformateur)
{
}

// Getters and Setters...

bool formation::ajout() {
    if (existeDeja(dateFormation, nomFormateur)) {
        return false;
    }

    QSqlQuery query;
    query.prepare("INSERT INTO formation (titre, description, date_formation, nomFormateur) "
                  "VALUES (:titre, :description, :dateFormation, :nomFormateur)");
    query.bindValue(":titre", titre);
    query.bindValue(":description", description);
    query.bindValue(":dateFormation", dateFormation.toString("dd/MM/yyyy")); // Convertir la date au format dd/MM/yyyy
    query.bindValue(":nomFormateur", nomFormateur);

    return query.exec();
}

bool formation::existeDeja(const QDate& dateformation, const QString& nomformateur) {
    QSqlQuery query;
    query.prepare("SELECT COUNT(*) FROM formation WHERE date_formation = :dateFormation AND nomFormateur = :nomFormateur");
    query.bindValue(":dateFormation", dateformation.toString("dd/MM/yyyy")); // Convertir la date au format dd/MM/yyyy
    query.bindValue(":nomFormateur", nomformateur);
    query.exec();

    if (query.next()) {
        int count = query.value(0).toInt();
        return count > 0;
    }

    return false;
}

QSqlQueryModel* formation::affiche() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM formation");
    return model;
}

bool formation::modifie(const QString& ancienTitre) {
    QSqlQuery query;
    query.prepare("UPDATE formation SET titre = :nouveauTitre, description = :description, date_Formation = :dateFormation, "
                     "nomFormateur = :nomFormateur WHERE titre = :ancienTitre");
    query.bindValue(":nouveauTitre", titre);
    query.bindValue(":description", description);
    query.bindValue(":dateFormation", dateFormation.toString("dd/MM/yyyy")); // Convertir la date au format dd/MM/yyyy
    query.bindValue(":nomFormateur", nomFormateur);
    query.bindValue(":ancienTitre", ancienTitre);

    return query.exec();
}

bool formation::supprime(QString nom) {
    QSqlQuery query;
    query.prepare("DELETE FROM formation WHERE titre = :titre");
    query.bindValue(":titre", nom);

    return query.exec();
}

QSqlQueryModel* formation::trierParTitreAsc() {
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    query.prepare("SELECT * FROM formation ORDER BY titre ASC");

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête de tri:" << query.lastError();
        delete model;
        return nullptr;
    }

    model->setQuery(query);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Titre"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Description"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date Formation"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Nom Formateur"));

    return model;
}

QSqlQueryModel* formation::trierParDateAsc() {
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    query.prepare("SELECT * FROM formation ORDER BY date_formation ASC");

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête de tri:" << query.lastError();
        delete model;
        return nullptr;
    }

    model->setQuery(query);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Titre"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Description"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date Formation"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Nom Formateur"));

    return model;
}

QSqlQueryModel* formation::trierParNomAsc() {
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    query.prepare("SELECT * FROM formation ORDER BY nomformateur ASC");

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête de tri:" << query.lastError();
        delete model;
        return nullptr;
    }

    model->setQuery(query);

    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Titre"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Description"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date Formation"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Nom Formateur"));

    return model;
}

QSqlQueryModel* formation::rechercherParTitre(const QString& titre) {
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    // Prépare la requête SQL pour rechercher les formations par titre
    query.prepare("SELECT * FROM formation WHERE titre LIKE :titre");
    query.bindValue(":titre", "%" + titre + "%"); // Recherche avec une correspondance partielle

    // Exécute la requête et vérifie s'il y a des erreurs
    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête de recherche:" << query.lastError();
        delete model;
        return nullptr;
    }

    // Définit le modèle avec les résultats de la requête
    model->setQuery(query);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Titre"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Description"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date Formation"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Nom Formateur"));

    return model;
}

QSqlQueryModel* formation::rechercherParNom(const QString& nomformateur) {
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    // Prépare la requête SQL pour rechercher les formations par titre
    query.prepare("SELECT * FROM formation WHERE nomformateur LIKE :nomformateur");
    query.bindValue(":titre", "%" + titre + "%"); // Recherche avec une correspondance partielle

    // Exécute la requête et vérifie s'il y a des erreurs
    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête de recherche:" << query.lastError();
        delete model;
        return nullptr;
    }

    // Définit le modèle avec les résultats de la requête
    model->setQuery(query);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Titre"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Description"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date Formation"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Nom Formateur"));

    return model;
}

QSqlQueryModel* formation::rechercherParDate(const QDate& dateFormation) {
    QSqlQueryModel* model = new QSqlQueryModel();
    QSqlQuery query;

    query.prepare("SELECT * FROM formation WHERE date_formation = :dateFormation");
    query.bindValue(":dateFormation", dateFormation.toString("yyyy-MM-dd")); // Assurez-vous que le format de la date est correct

    if (!query.exec()) {
        qDebug() << "Erreur lors de l'exécution de la requête:" << query.lastError();
        delete model;
        return nullptr;
    }

    model->setQuery(query);
    model->setHeaderData(0, Qt::Horizontal, QObject::tr("Titre"));
    model->setHeaderData(1, Qt::Horizontal, QObject::tr("Description"));
    model->setHeaderData(2, Qt::Horizontal, QObject::tr("Date Formation"));
    model->setHeaderData(3, Qt::Horizontal, QObject::tr("Nom Formateur"));

    return model;
}

