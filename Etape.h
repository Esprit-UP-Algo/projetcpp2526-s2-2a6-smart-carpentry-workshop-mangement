#ifndef ETAPE_H
#define ETAPE_H

#include <QString>
#include <QDate>

class Etape {
public:
    Etape();
    Etape(int id, const QString& nom, const QString& produit, int ordre,
          int tempsEstime, const QString& description, const QDate& dateDebut,
          const QDate& dateFin, const QString& etat);

    // Getters
    int getId() const { return id; }
    QString getNom() const { return nom; }
    QString getProduit() const { return produit; }
    int getOrdre() const { return ordre; }
    int getTempsEstime() const { return tempsEstime; }
    QString getDescription() const { return description; }
    QDate getDateDebut() const { return dateDebut; }
    QDate getDateFin() const { return dateFin; }
    QString getEtat() const { return etat; }

    // Setters
    void setId(int id) { this->id = id; }
    void setNom(const QString& nom) { this->nom = nom; }
    void setProduit(const QString& produit) { this->produit = produit; }
    void setOrdre(int ordre) { this->ordre = ordre; }
    void setTempsEstime(int temps) { this->tempsEstime = temps; }
    void setDescription(const QString& desc) { this->description = desc; }
    void setDateDebut(const QDate& date) { this->dateDebut = date; }
    void setDateFin(const QDate& date) { this->dateFin = date; }
    void setEtat(const QString& etat) { this->etat = etat; }

private:
    int id;
    QString nom;
    QString produit;
    int ordre;
    int tempsEstime; // en minutes
    QString description;
    QDate dateDebut;
    QDate dateFin;
    QString etat; // "En cours", "Terminé", "Non commencé"
};

#endif // ETAPE_H
