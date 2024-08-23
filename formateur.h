#ifndef FORMATEUR_H
#define FORMATEUR_H

#include <QString>
#include <QSqlQueryModel>

class formateur
{
public:
    formateur();
    formateur(const QString& nomFormateur, const QString& cin, const QString& telephone, const QString& email);

    // Getters
    QString getNomformateur() const;
    QString getCin() const;
    QString getTelephone() const;
    QString getEmail() const;

    // Setters
    void setNomformateur(const QString& nomFormateur);
    void setCin(const QString& cin);
    void setTelephone(const QString& telephone);
    void setEmail(const QString& email);

    // Database operations
    bool ajout();
    QSqlQueryModel* affiche();
    bool modifie();
    bool supprime(const QString& cin);


private:
    QString nomformateur;
    QString cin;
    QString telephone;
    QString email;
};

#endif // FORMATEUR_H


