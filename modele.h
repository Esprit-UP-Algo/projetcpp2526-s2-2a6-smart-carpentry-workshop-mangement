#ifndef MODELE_H
#define MODELE_H

#include <QString>
#include <QDate>
#include <QSqlQueryModel>

// ═══════════════════════════════════════════════════════════════════════
//  Classe Modele
//  Represente un modele de meuble en bois.
//  Contient les donnees et les methodes d'acces a la base Oracle.
// ═══════════════════════════════════════════════════════════════════════
class Modele
{
public:
    // ── Attributs ─────────────────────────────────────────────────────────
    int     id           = -1;
    QString nom          = "";
    QString type         = "";   // Table / Chaise / Armoire / Bureau / Bibliotheque
    QString bois         = "";   // Chene / Pin / Hetre / Noyer ...
    QString longueur     = "";
    QString largeur      = "";
    QString hauteur      = "";
    QString creePar      = "";
    QDate   dateCreation = QDate::currentDate();

    // ── Constructeurs ──────────────────────────────────────────────────────
    Modele();
    Modele(int id, const QString &nom, const QString &type,
           const QString &bois, const QString &longueur,
           const QString &largeur, const QString &hauteur,
           const QString &creePar, const QDate &dateCreation);

    // ── Methodes CRUD ─────────────────────────────────────────────────────
    bool            ajouter();                                        // INSERT
    bool            modifier();                                       // UPDATE
    bool            supprimer(int idASupprimer);                      // DELETE
    QSqlQueryModel* afficher();                                       // SELECT tous
    QSqlQueryModel* rechercher(const QString &critere,
                               const QString &valeur);               // SELECT filtre
    QSqlQueryModel* trier(const QString &colonne);                    // SELECT tri
};

#endif // MODELE_H
