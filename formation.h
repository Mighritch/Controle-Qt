#ifndef FORMATION_H
#define FORMATION_H

#include <QString>
#include <QDate>
#include <QtSql>
#include <QSqlQueryModel>
#include <QDebug>

class formation
{
public:
    formation();
    formation(const QString titre, const QString description, const QDate& dateformation, const QString nomformateur);

        QString getTitre() const;
        QString getDescription() const;
        QDate getDateFormation() const;
        QString getNomFormateur() const;

        // Setters
        void setTitre(const QString& titre);
        void setDescription(const QString& description);
        void setDateFormation(const QDate& dateformation);
        void setNomFormateur(const QString& nomformateur);

        bool ajout();
        bool existeDeja(const QDate& dateformation, const QString& nomformateur);
        QSqlQueryModel* affiche();
        bool modifie(const QString& ancienTitre);
        bool supprime(QString nom);
        QSqlQueryModel* trierParTitreAsc();
        QSqlQueryModel* trierParDateAsc();
        QSqlQueryModel* trierParNomAsc();
        QSqlQueryModel* rechercherParTitre(const QString& titre);
        QSqlQueryModel* rechercherParNom(const QString& nomformateur);
        QSqlQueryModel* rechercherParDate(const QDate& dateFormation);



private:
    QString titre;
    QString description;
    QDate dateFormation;
    QString nomFormateur;
};

#endif // FORMATION_H
