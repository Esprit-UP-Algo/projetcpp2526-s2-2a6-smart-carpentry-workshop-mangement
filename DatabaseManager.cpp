#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QVariant>

DatabaseManager::DatabaseManager() {
}

DatabaseManager::~DatabaseManager() {
    closeDatabase();
}

bool DatabaseManager::openDatabase() {
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("etapes_fabrication.db");
    
    if (!db.open()) {
        qDebug() << "Erreur: connexion échouée" << db.lastError().text();
        return false;
    }
    
    return createTable();
}

void DatabaseManager::closeDatabase() {
    if (db.isOpen()) {
        db.close();
    }
}

bool DatabaseManager::createTable() {
    QSqlQuery query;
    QString createTableQuery = 
        "CREATE TABLE IF NOT EXISTS etapes ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "nom TEXT NOT NULL, "
        "produit TEXT NOT NULL, "
        "ordre INTEGER, "
        "temps_estime INTEGER, "
        "description TEXT, "
        "date_debut TEXT, "
        "date_fin TEXT, "
        "etat TEXT)";
    
    if (!query.exec(createTableQuery)) {
        qDebug() << "Erreur création table:" << query.lastError().text();
        return false;
    }
    
    return true;
}

// CREATE
bool DatabaseManager::createEtape(const Etape& etape) {
    QSqlQuery query;
    query.prepare("INSERT INTO etapes (nom, produit, ordre, temps_estime, description, date_debut, date_fin, etat) "
                  "VALUES (:nom, :produit, :ordre, :temps, :desc, :debut, :fin, :etat)");
    
    query.bindValue(":nom", etape.getNom());
    query.bindValue(":produit", etape.getProduit());
    query.bindValue(":ordre", etape.getOrdre());
    query.bindValue(":temps", etape.getTempsEstime());
    query.bindValue(":desc", etape.getDescription());
    query.bindValue(":debut", etape.getDateDebut().toString(Qt::ISODate));
    query.bindValue(":fin", etape.getDateFin().toString(Qt::ISODate));
    query.bindValue(":etat", etape.getEtat());
    
    if (!query.exec()) {
        qDebug() << "Erreur insertion étape:" << query.lastError().text();
        return false;
    }
    
    return true;
}

// READ ALL
QVector<Etape> DatabaseManager::readAllEtapes() {
    QVector<Etape> etapes;
    QSqlQuery query("SELECT * FROM etapes ORDER BY produit, ordre");
    
    while (query.next()) {
        Etape etape(
            query.value(0).toInt(),                                      // id
            query.value(1).toString(),                                   // nom
            query.value(2).toString(),                                   // produit
            query.value(3).toInt(),                                      // ordre
            query.value(4).toInt(),                                      // temps_estime
            query.value(5).toString(),                                   // description
            QDate::fromString(query.value(6).toString(), Qt::ISODate),  // date_debut
            QDate::fromString(query.value(7).toString(), Qt::ISODate),  // date_fin
            query.value(8).toString()                                    // etat
        );
        etapes.append(etape);
    }
    
    return etapes;
}

// READ ONE
Etape DatabaseManager::readEtape(int id) {
    QSqlQuery query;
    query.prepare("SELECT * FROM etapes WHERE id = :id");
    query.bindValue(":id", id);
    
    if (query.exec() && query.next()) {
        return Etape(
            query.value(0).toInt(),
            query.value(1).toString(),
            query.value(2).toString(),
            query.value(3).toInt(),
            query.value(4).toInt(),
            query.value(5).toString(),
            QDate::fromString(query.value(6).toString(), Qt::ISODate),
            QDate::fromString(query.value(7).toString(), Qt::ISODate),
            query.value(8).toString()
        );
    }
    
    return Etape();
}

// UPDATE
bool DatabaseManager::updateEtape(const Etape& etape) {
    QSqlQuery query;
    query.prepare("UPDATE etapes SET nom = :nom, produit = :produit, ordre = :ordre, "
                  "temps_estime = :temps, description = :desc, date_debut = :debut, "
                  "date_fin = :fin, etat = :etat WHERE id = :id");
    
    query.bindValue(":id", etape.getId());
    query.bindValue(":nom", etape.getNom());
    query.bindValue(":produit", etape.getProduit());
    query.bindValue(":ordre", etape.getOrdre());
    query.bindValue(":temps", etape.getTempsEstime());
    query.bindValue(":desc", etape.getDescription());
    query.bindValue(":debut", etape.getDateDebut().toString(Qt::ISODate));
    query.bindValue(":fin", etape.getDateFin().toString(Qt::ISODate));
    query.bindValue(":etat", etape.getEtat());
    
    if (!query.exec()) {
        qDebug() << "Erreur mise à jour étape:" << query.lastError().text();
        return false;
    }
    
    return true;
}

// DELETE
bool DatabaseManager::deleteEtape(int id) {
    QSqlQuery query;
    query.prepare("DELETE FROM etapes WHERE id = :id");
    query.bindValue(":id", id);
    
    if (!query.exec()) {
        qDebug() << "Erreur suppression étape:" << query.lastError().text();
        return false;
    }
    
    return true;
}

// SEARCH
QVector<Etape> DatabaseManager::searchEtapes(const QString& searchTerm) {
    QVector<Etape> etapes;
    QSqlQuery query;
    query.prepare("SELECT * FROM etapes WHERE nom LIKE :search OR produit LIKE :search OR description LIKE :search ORDER BY produit, ordre");
    query.bindValue(":search", "%" + searchTerm + "%");
    
    if (query.exec()) {
        while (query.next()) {
            Etape etape(
                query.value(0).toInt(),
                query.value(1).toString(),
                query.value(2).toString(),
                query.value(3).toInt(),
                query.value(4).toInt(),
                query.value(5).toString(),
                QDate::fromString(query.value(6).toString(), Qt::ISODate),
                QDate::fromString(query.value(7).toString(), Qt::ISODate),
                query.value(8).toString()
            );
            etapes.append(etape);
        }
    }
    
    return etapes;
}

// GET BY PRODUCT
QVector<Etape> DatabaseManager::getEtapesByProduit(const QString& produit) {
    QVector<Etape> etapes;
    QSqlQuery query;
    query.prepare("SELECT * FROM etapes WHERE produit = :produit ORDER BY ordre");
    query.bindValue(":produit", produit);
    
    if (query.exec()) {
        while (query.next()) {
            Etape etape(
                query.value(0).toInt(),
                query.value(1).toString(),
                query.value(2).toString(),
                query.value(3).toInt(),
                query.value(4).toInt(),
                query.value(5).toString(),
                QDate::fromString(query.value(6).toString(), Qt::ISODate),
                QDate::fromString(query.value(7).toString(), Qt::ISODate),
                query.value(8).toString()
            );
            etapes.append(etape);
        }
    }
    
    return etapes;
}
