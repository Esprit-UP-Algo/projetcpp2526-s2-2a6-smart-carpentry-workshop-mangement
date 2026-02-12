#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QTabWidget>
#include <QComboBox>
#include <QDateEdit>
#include "DatabaseManager.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onAddEtape();
    void onUpdateEtape();
    void onDeleteEtape();
    void onRefreshTable();
    void onSearchEtapes();
    void onTableSelectionChanged();
    void onClearForm();
    void onFilterByProduit();

private:
    void setupUI();
    void loadEtapes();
    void clearInputs();
    void populateFormFromTable();
    QWidget* createGestionTab();
    QWidget* createStatistiquesTab();

    // UI Components
    QTabWidget* tabWidget;
    QTableWidget* tableWidget;
    QLineEdit* idEdit;
    QLineEdit* nomEdit;
    QComboBox* produitCombo;
    QLineEdit* ordreEdit;
    QLineEdit* tempsEdit;
    QTextEdit* descriptionEdit;
    QDateEdit* dateDebutEdit;
    QDateEdit* dateFinEdit;
    QComboBox* etatCombo;
    QLineEdit* searchEdit;
    QComboBox* filterProduitCombo;
    QPushButton* addButton;
    QPushButton* updateButton;
    QPushButton* deleteButton;
    QPushButton* clearButton;
    QPushButton* searchButton;
    QLabel* statusLabel;

    // Database
    DatabaseManager dbManager;
    int selectedEtapeId;
};

#endif // MAINWINDOW_H
