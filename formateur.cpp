#include "formateur.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

formateur::formateur() : nomformateur(""), cin(""), telephone(""), email("") {}

formateur::formateur(const QString& nomFormateur, const QString& cin, const QString& telephone, const QString& email)
    : nomformateur(nomFormateur), cin(cin), telephone(telephone), email(email) {}

// Getters
QString formateur::getNomformateur() const {
    return nomformateur;
}

QString formateur::getCin() const {
    return cin;
}

QString formateur::getTelephone() const {
    return telephone;
}

QString formateur::getEmail() const {
    return email;
}

// Setters
void formateur::setNomformateur(const QString& nomFormateur) {
    this->nomformateur = nomFormateur;
}

void formateur::setCin(const QString& cin) {
    this->cin = cin;
}

void formateur::setTelephone(const QString& telephone) {
    this->telephone = telephone;
}

void formateur::setEmail(const QString& email) {
    this->email = email;
}

bool formateur::ajout() {
    QSqlQuery query;
    query.prepare("INSERT INTO formateur (nomformateur, cin, telephone, email) "
                  "VALUES (:nomFormateur, :cin, :telephone, :email)");
    query.bindValue(":nomFormateur", nomformateur);
    query.bindValue(":cin", cin);
    query.bindValue(":telephone", telephone);
    query.bindValue(":email", email);

    return query.exec();
}

QSqlQueryModel* formateur::affiche() {
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery("SELECT * FROM formateur");
    return model;
}


bool formateur::modifie() {
    QSqlQuery query;
    query.prepare("UPDATE formateur SET nomformateur = :nomformateur, telephone = :telephone, email = :email WHERE cin = :cin");
    query.bindValue(":nomformateur", nomformateur);
    query.bindValue(":telephone", telephone);
    query.bindValue(":email", email);
    query.bindValue(":cin", cin);

    if (!query.exec()) {
        qDebug() << "Erreur lors de la modification du formateur:" << query.lastError().text();
        return false;
    }

    return true;
}

bool formateur::supprime(const QString& cin) {
    QSqlQuery query;
    query.prepare("DELETE FROM formateur WHERE cin = :cin");
    query.bindValue(":cin", cin);

    if (!query.exec()) {
        qDebug() << "Erreur lors de la suppression du formateur:" << query.lastError().text();
        return false;
    }

    return true;
}

