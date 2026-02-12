#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QVector>
#include "Etape.h"

class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();

    bool openDatabase();
    void closeDatabase();
    
    // CRUD Operations
    bool createEtape(const Etape& etape);
    QVector<Etape> readAllEtapes();
    Etape readEtape(int id);
    bool updateEtape(const Etape& etape);
    bool deleteEtape(int id);
    
    // Search
    QVector<Etape> searchEtapes(const QString& searchTerm);
    QVector<Etape> getEtapesByProduit(const QString& produit);

private:
    QSqlDatabase db;
    bool createTable();
};

#endif // DATABASEMANAGER_H
