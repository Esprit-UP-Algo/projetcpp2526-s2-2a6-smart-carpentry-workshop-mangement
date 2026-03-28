#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connexion.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QPrinter>
#include <QPainter>
#include <QPainterPath>
#include <QFileDialog>
#include <QDateTime>
#include <QDebug>
#include <QLabel>
#include <QPixmap>
#include <QLinearGradient>
#include <QRegularExpression>
#include <QHeaderView>
#include <QtMath>

// ═══════════════════════════════════════════════════════════════════════
//  STYLES GLOBAUX
// ═══════════════════════════════════════════════════════════════════════
static const QString STYLE_MSG = R"(
    QMessageBox { background-color: #EDE0C8; }
    QLabel { color: #2E1A00; font-size: 13px; font-weight: bold; background-color: transparent; }
    QPushButton { background-color: #7F4129; color: #FFFFFF; border: none; border-radius: 6px;
                  padding: 6px 20px; font-weight: bold; min-width: 80px; min-height: 30px; }
    QPushButton:hover { background-color: #9C5233; }
)";
static const QString STYLE_ERREUR = "border: 2px solid #D32F2F; background-color: #FFEBEE;";
static const QString STYLE_OK     = "border: 2px solid #388E3C; background-color: #F1F8E9;";
static const QString STYLE_NORMAL = "";

static void msgInfo(QWidget *p, const QString &t, const QString &msg) {
    QMessageBox b(QMessageBox::Information, t, msg, QMessageBox::Ok, p);
    b.setStyleSheet(STYLE_MSG); b.exec();
}
static void msgErreur(QWidget *p, const QString &t, const QString &msg) {
    QMessageBox b(QMessageBox::Critical, t, msg, QMessageBox::Ok, p);
    b.setStyleSheet(STYLE_MSG); b.exec();
}
static void msgWarn(QWidget *p, const QString &t, const QString &msg) {
    QMessageBox b(QMessageBox::Warning, t, msg, QMessageBox::Ok, p);
    b.setStyleSheet(STYLE_MSG); b.exec();
}
static int msgQuestion(QWidget *p, const QString &t, const QString &msg) {
    QMessageBox b(QMessageBox::Question, t, msg, QMessageBox::Yes | QMessageBox::No, p);
    b.setStyleSheet(STYLE_MSG); return b.exec();
}
static void setFieldStyle(QLineEdit *le, bool ok) {
    le->setStyleSheet(ok ? STYLE_OK : STYLE_ERREUR);
}
static void resetField(QLineEdit *le) {
    le->setStyleSheet(STYLE_NORMAL);
}

// ═══════════════════════════════════════════════════════════════════════
//  CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // ── Navigation ───────────────────────────────────────────────────────
    connect(ui->btn_bois,        &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page);
    });
    connect(ui->btn_modele,      &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page_6);
        afficherStatistiques();
    });
    connect(ui->btn_personnel,   &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page_5);
    });
    connect(ui->btn_etape,       &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page_7);
    });
    connect(ui->btn_fabrication, &QPushButton::clicked, this, [=]() {
        ui->stackedWidget->setCurrentWidget(ui->page_4);
    });

    // Stats rechargees sur l'onglet Statistiques
    connect(ui->tabWidget_modeles, &QTabWidget::currentChanged, this, [=](int idx) {
        if (idx == 1) afficherStatistiques();
    });

    // ── Config tableau Modeles ────────────────────────────────────────────
    ui->table_modeles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table_modeles->setAlternatingRowColors(true);
    ui->table_modeles->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table_modeles->horizontalHeader()->setStretchLastSection(true);
    ui->table_modeles->verticalHeader()->setVisible(false);
    ui->table_modeles->setColumnCount(9);
    ui->table_modeles->setHorizontalHeaderLabels(
        {"ID","Nom","Type","Type Bois","Longueur","Largeur","Hauteur","Cree par","Date creation"});

    // ── Config tableau Personnel ──────────────────────────────────────────
    // IMPORTANT : remplacez tab_rech_3 par le vrai objectName de votre tableau Personnel
    ui->tab_rech_3->setStyleSheet("QTableView { color: black; background-color: white; }");
    ui->tab_rech_3->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // ── Remplir combo_type ────────────────────────────────────────────────
    ui->combo_type->clear();
    ui->combo_type->addItems({"Table", "Chaise", "Armoire", "Bureau", "Bibliotheque"});

    // ── Date par defaut = aujourd'hui ─────────────────────────────────────
    ui->de_date_creation->setDate(QDate::currentDate());

    // ── Validation temps reel Modeles ─────────────────────────────────────
    connect(ui->le_nom_modele, &QLineEdit::textChanged, this, [=](const QString &txt) {
        bool ok = txt.trimmed().length() >= 2 &&
                  QRegularExpression("^[a-zA-ZÀ-ÿ\\s\\-_]+$").match(txt.trimmed()).hasMatch();
        setFieldStyle(ui->le_nom_modele, ok);
    });
    connect(ui->le_longueur, &QLineEdit::textChanged, this, [=](const QString &txt) {
        bool ok; double v = txt.toDouble(&ok);
        setFieldStyle(ui->le_longueur, ok && v > 0);
    });
    connect(ui->le_largeur, &QLineEdit::textChanged, this, [=](const QString &txt) {
        bool ok; double v = txt.toDouble(&ok);
        setFieldStyle(ui->le_largeur, ok && v > 0);
    });
    connect(ui->le_hauteur, &QLineEdit::textChanged, this, [=](const QString &txt) {
        bool ok; double v = txt.toDouble(&ok);
        setFieldStyle(ui->le_hauteur, ok && v > 0);
    });
    connect(ui->le_créepar, &QLineEdit::textChanged, this, [=](const QString &txt) {
        bool ok = txt.trimmed().length() >= 2 &&
                  QRegularExpression("^[a-zA-ZÀ-ÿ\\s]+$").match(txt.trimmed()).hasMatch();
        setFieldStyle(ui->le_créepar, ok);
    });

    // ── Connexion Oracle via Singleton ────────────────────────────────────
    if (!Connexion::createInstance().createconnect()) {
        msgErreur(this, "Erreur BDD",
                  "Connexion Oracle echouee.\nVerifiez la source ODBC, l'utilisateur et le mot de passe.");
        return;
    }

    // ── Etat initial boutons Modeles ──────────────────────────────────────
    ui->btn_ajouter_modele->setEnabled(true);
    ui->btn_modifier_modele->setEnabled(false);
    ui->btn_supprimer_modele->setEnabled(false);

    QString btnStyle =
        "QPushButton { background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px; }"
        "QPushButton:hover { background-color: #9C5233; }"
        "QPushButton:disabled { background-color: #C4A882; color: #8B6B50; }";
    ui->btn_ajouter_modele->setStyleSheet(btnStyle);
    ui->btn_modifier_modele->setStyleSheet(btnStyle);
    ui->btn_supprimer_modele->setStyleSheet(btnStyle);

    // ── Chargement initial ────────────────────────────────────────────────
    chargerTableauModeles();
    refreshTable();

    // ── Connexions Modeles ────────────────────────────────────────────────
    connect(ui->table_modeles,    &QTableWidget::cellClicked,
            this, &MainWindow::ligneSelectionnee);
    connect(ui->rech_6,           &QLineEdit::textChanged,
            this, &MainWindow::on_btn_rechercher_modele_clicked);
    connect(ui->btn_rechercher_5, &QPushButton::clicked,
            this, &MainWindow::on_btn_rechercher_modele_clicked);
    connect(ui->rech_6,           &QLineEdit::returnPressed,
            this, &MainWindow::on_btn_rechercher_modele_clicked);
    connect(ui->btn_tire,         &QPushButton::clicked,
            this, &MainWindow::on_btn_tire_clicked);

    // Double-clic = retour mode ajout
    connect(ui->table_modeles, &QTableWidget::itemDoubleClicked, this, [=]() {
        reinitialiserFormulaire();
        msgInfo(this, "Mode Ajout", "Champs reinitialises.\nVous pouvez saisir un nouveau modele.");
    });

    // ── Connexions Personnel ──────────────────────────────────────────────
    connect(ui->tab_rech_3, &QTableView::clicked,
            this, &MainWindow::on_tab_employes_clicked);
}

// ═══════════════════════════════════════════════════════════════════════
//  DESTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════
MainWindow::~MainWindow()
{
    delete ui;

}

// ═══════════════════════════════════════════════════════════════════════
//  db() — retourne la connexion via le Singleton
// ═══════════════════════════════════════════════════════════════════════
QSqlDatabase MainWindow::db() const {
    return Connexion::createInstance().getDatabase();
}

// ═══════════════════════════════════════════════════════════════════════
//  PERSONNEL — refreshTable
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::refreshTable()
{
    Personnel P;
    ui->tab_rech_3->setModel(P.afficher());
}

// ═══════════════════════════════════════════════════════════════════════
//  PERSONNEL — clearFields
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::clearFields()
{
    ui->le_cin->clear();
    ui->le_nom_2->clear();
    ui->le_prenom->clear();
    ui->le_salaire->clear();
    ui->le_tel->clear();
    ui->le_rfid->clear();
}

// ═══════════════════════════════════════════════════════════════════════
//  PERSONNEL — AJOUTER
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::on_btn_ajouter_3_clicked()
{
    if (!controlDeSaisie()) return;

    int     cin     = ui->le_cin->text().toInt();
    QString nom     = ui->le_nom_2->text();
    QString prenom  = ui->le_prenom->text();
    QDate   date_n  = ui->de_naissance->date();
    QString poste   = ui->cb_poste->currentText();
    QDate   date_e  = ui->de_embauche->date();
    int     salaire = ui->le_salaire->text().toInt();
    int     tel     = ui->le_tel->text().toInt();
    QString rfid    = ui->le_rfid->text();

    Personnel P(cin, nom, prenom, date_n, poste, date_e, salaire, tel, rfid);
    if (P.ajouter()) {
        QMessageBox::information(this, "Succes", "Employe ajoute avec succes !");
        refreshTable();
        clearFields();
    } else {
        QMessageBox::critical(this, "Erreur", "L'ajout a echoue. Verifiez si le CIN existe deja.");
        qDebug() << "=== ERREUR AJOUT PERSONNEL ===";
    }
}

// ═══════════════════════════════════════════════════════════════════════
//  PERSONNEL — MODIFIER
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::on_btn_modifier_2_clicked()
{
    if (!controlDeSaisie()) return;

    int     cin     = ui->le_cin->text().toInt();
    QString nom     = ui->le_nom_2->text();
    QString prenom  = ui->le_prenom->text();
    QDate   date_n  = ui->de_naissance->date();
    QString poste   = ui->cb_poste->currentText();
    QDate   date_e  = ui->de_embauche->date();
    int     salaire = ui->le_salaire->text().toInt();
    int     tel     = ui->le_tel->text().toInt();
    QString rfid    = ui->le_rfid->text();

    Personnel P(cin, nom, prenom, date_n, poste, date_e, salaire, tel, rfid);
    if (P.modifier()) {
        QMessageBox::information(this, "Succes", "Donnees de l'employe mises a jour.");
        refreshTable();
    } else {
        QMessageBox::critical(this, "Erreur", "La modification a echoue.");
    }
}

// ═══════════════════════════════════════════════════════════════════════
//  PERSONNEL — SUPPRIMER
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::on_btn_supprimer_2_clicked()
{
    int cin = ui->le_cin->text().toInt();
    if (cin == 0) {
        QMessageBox::warning(this, "Avertissement", "Veuillez saisir un CIN a supprimer.");
        return;
    }
    Personnel P;
    if (P.supprimer(cin)) {
        QMessageBox::information(this, "Succes", "Employe supprime.");
        refreshTable();
    } else {
        QMessageBox::critical(this, "Erreur", "La suppression a echoue.");
    }
}

// ═══════════════════════════════════════════════════════════════════════
//  PERSONNEL — CLIC SUR LIGNE
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::on_tab_employes_clicked(const QModelIndex &index)
{
    int row = index.row();
    auto getData = [&](int col) -> QString {
        return ui->tab_rech_3->model()->data(
                                          ui->tab_rech_3->model()->index(row, col)).toString();
    };

    ui->le_cin->setText(getData(0));
    ui->le_nom_2->setText(getData(1));
    ui->le_prenom->setText(getData(2));

    QDate dn = QDate::fromString(getData(3), "yyyy-MM-dd");
    if (!dn.isValid()) dn = QDate::fromString(getData(3), "dd/MM/yyyy");
    ui->de_naissance->setDate(dn.isValid() ? dn : QDate::currentDate());

    ui->cb_poste->setCurrentText(getData(4));

    QDate de = QDate::fromString(getData(5), "yyyy-MM-dd");
    if (!de.isValid()) de = QDate::fromString(getData(5), "dd/MM/yyyy");
    ui->de_embauche->setDate(de.isValid() ? de : QDate::currentDate());

    ui->le_salaire->setText(getData(6));
    ui->le_tel->setText(getData(7));
    ui->le_rfid->setText(getData(8));
}

// ═══════════════════════════════════════════════════════════════════════
//  PERSONNEL — CONTROLE DE SAISIE
// ═══════════════════════════════════════════════════════════════════════
bool MainWindow::controlDeSaisie()
{
    QString cin     = ui->le_cin->text();
    QString nom     = ui->le_nom_2->text();
    QString prenom  = ui->le_prenom->text();
    QString tel     = ui->le_tel->text();
    QString salaire = ui->le_salaire->text();
    QDate   dn      = ui->de_naissance->date();
    QDate   de      = ui->de_embauche->date();
    QString poste   = ui->cb_poste->currentText();

    QRegularExpression regexCin("^[0-9]{8}$");
    QRegularExpression regexNom("^[A-Za-z ]{2,20}$");
    QRegularExpression regexTel("^[0-9]{8}$");

    if (cin.isEmpty() || nom.isEmpty() || prenom.isEmpty() ||
        tel.isEmpty() || salaire.isEmpty()) {
        QMessageBox::warning(this, "Erreur", "Tous les champs obligatoires doivent etre remplis.");
        return false;
    }
    if (!regexCin.match(cin).hasMatch()) {
        QMessageBox::warning(this, "Erreur CIN", "Le CIN doit contenir exactement 8 chiffres.");
        return false;
    }
    if (!regexNom.match(nom).hasMatch() || !regexNom.match(prenom).hasMatch()) {
        QMessageBox::warning(this, "Erreur Nom/Prenom", "Lettres uniquement, 2 a 20 caracteres.");
        return false;
    }
    if (!regexTel.match(tel).hasMatch()) {
        QMessageBox::warning(this, "Erreur Telephone", "Le telephone doit contenir 8 chiffres.");
        return false;
    }
    if (salaire.toInt() <= 0) {
        QMessageBox::warning(this, "Erreur Salaire", "Le salaire doit etre un nombre positif.");
        return false;
    }
    if (dn.daysTo(QDate::currentDate()) < 18 * 365) {
        QMessageBox::warning(this, "Erreur Date", "L'employe doit etre majeur (18 ans minimum).");
        return false;
    }
    if (de <= dn) {
        QMessageBox::warning(this, "Erreur Date", "La date d'embauche doit etre apres la naissance.");
        return false;
    }
    if (poste == "Poste..." || poste.isEmpty()) {
        QMessageBox::warning(this, "Erreur Poste", "Veuillez selectionner un poste.");
        return false;
    }
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — validerFormulaire
// ═══════════════════════════════════════════════════════════════════════
bool MainWindow::validerFormulaire()
{
    bool valide = true;
    QStringList erreurs;

    QString nom = ui->le_nom_modele->text().trimmed();
    if (nom.isEmpty()) {
        setFieldStyle(ui->le_nom_modele, false);
        erreurs << "• Nom : obligatoire."; valide = false;
    } else if (nom.length() < 2) {
        setFieldStyle(ui->le_nom_modele, false);
        erreurs << "• Nom : min 2 caracteres."; valide = false;
    } else if (!QRegularExpression("^[a-zA-ZÀ-ÿ\\s\\-_]+$").match(nom).hasMatch()) {
        setFieldStyle(ui->le_nom_modele, false);
        erreurs << "• Nom : lettres uniquement."; valide = false;
    } else { setFieldStyle(ui->le_nom_modele, true); }

    QString type = ui->combo_type->currentText().trimmed();
    if (type.isEmpty()) {
        ui->combo_type->setStyleSheet(STYLE_ERREUR);
        erreurs << "• Type : obligatoire."; valide = false;
    } else { ui->combo_type->setStyleSheet(""); }

    bool okL; double lon = ui->le_longueur->text().toDouble(&okL);
    if (!okL || lon <= 0) { setFieldStyle(ui->le_longueur, false); erreurs << "• Longueur : nombre positif."; valide = false; }
    else if (lon > 9999) { setFieldStyle(ui->le_longueur, false); erreurs << "• Longueur : max 9999 cm."; valide = false; }
    else { setFieldStyle(ui->le_longueur, true); }

    bool okW; double lar = ui->le_largeur->text().toDouble(&okW);
    if (!okW || lar <= 0) { setFieldStyle(ui->le_largeur, false); erreurs << "• Largeur : nombre positif."; valide = false; }
    else if (lar > 9999) { setFieldStyle(ui->le_largeur, false); erreurs << "• Largeur : max 9999 cm."; valide = false; }
    else { setFieldStyle(ui->le_largeur, true); }

    bool okH; double hau = ui->le_hauteur->text().toDouble(&okH);
    if (!okH || hau <= 0) { setFieldStyle(ui->le_hauteur, false); erreurs << "• Hauteur : nombre positif."; valide = false; }
    else if (hau > 9999) { setFieldStyle(ui->le_hauteur, false); erreurs << "• Hauteur : max 9999 cm."; valide = false; }
    else { setFieldStyle(ui->le_hauteur, true); }

    QString creepar = ui->le_créepar->text().trimmed();
    if (creepar.isEmpty()) { setFieldStyle(ui->le_créepar, false); erreurs << "• Cree par : obligatoire."; valide = false; }
    else if (creepar.length() < 2) { setFieldStyle(ui->le_créepar, false); erreurs << "• Cree par : min 2 car."; valide = false; }
    else if (!QRegularExpression("^[a-zA-ZÀ-ÿ\\s]+$").match(creepar).hasMatch()) { setFieldStyle(ui->le_créepar, false); erreurs << "• Cree par : lettres uniquement."; valide = false; }
    else { setFieldStyle(ui->le_créepar, true); }

    if (ui->de_date_creation->date() > QDate::currentDate()) {
        erreurs << "• Date : ne peut pas etre dans le futur."; valide = false;
    }

    if (!valide)
        msgWarn(this, "Erreurs de saisie",
                "Veuillez corriger les champs suivants :\n\n" + erreurs.join("\n"));
    return valide;
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — reinitialiserFormulaire
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::reinitialiserFormulaire()
{
    resetField(ui->le_nom_modele);
    resetField(ui->le_longueur); resetField(ui->le_largeur);
    resetField(ui->le_hauteur);  resetField(ui->le_créepar);

    m_idSelectionne = -1;
    ui->combo_type->setStyleSheet("");
    ui->le_nom_modele->clear();
    ui->combo_type->setCurrentIndex(0);
    ui->le_longueur->clear(); ui->le_largeur->clear();
    ui->le_hauteur->clear();  ui->le_créepar->clear();
    ui->de_date_creation->setDate(QDate::currentDate());

    ui->btn_ajouter_modele->setEnabled(true);
    ui->btn_ajouter_modele->setToolTip("");
    ui->btn_modifier_modele->setEnabled(false);
    ui->btn_supprimer_modele->setEnabled(false);
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — chargerTableauModeles
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::chargerTableauModeles()
{
    ui->table_modeles->setRowCount(0);
    QSqlQuery query(db());
    bool ok = query.exec(
        "SELECT ID_MODELE, NOM, TYPE, TYPE_BOIS, "
        "LONGUEUR, LARGEUR, HAUTEUR, CREEPAR, "
        "TO_CHAR(DATECREATION,'DD/MM/YYYY') AS DATECREATION "
        "FROM ADEM.MODELE_BOIS ORDER BY ID_MODELE");
    if (!ok) { msgErreur(this, "Erreur chargement", query.lastError().text()); return; }
    int row = 0;
    while (query.next()) {
        ui->table_modeles->insertRow(row);
        for (int c = 0; c < 9; c++)
            ui->table_modeles->setItem(row, c, new QTableWidgetItem(query.value(c).toString()));
        row++;
    }
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — ligneSelectionnee
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::ligneSelectionnee(int row, int)
{
    if (row < 0 || row >= ui->table_modeles->rowCount()) return;
    auto txt = [&](int col) -> QString {
        QTableWidgetItem *it = ui->table_modeles->item(row, col);
        return it ? it->text() : "";
    };

    m_idSelectionne = txt(0).toInt();
    ui->le_nom_modele->setText(txt(1));
    ui->combo_type->setCurrentText(txt(2));
    int idx = ui->combo_bois->findText(txt(3));
    if (idx >= 0) ui->combo_bois->setCurrentIndex(idx);
    ui->le_longueur->setText(txt(4));
    ui->le_largeur->setText(txt(5));
    ui->le_hauteur->setText(txt(6));
    ui->le_créepar->setText(txt(7));

    resetField(ui->le_nom_modele);
    resetField(ui->le_longueur); resetField(ui->le_largeur);
    resetField(ui->le_hauteur);  resetField(ui->le_créepar);

    ui->btn_ajouter_modele->setEnabled(false);
    ui->btn_ajouter_modele->setToolTip("Double-cliquez pour reinitialiser et ajouter un nouveau modele.");
    ui->btn_modifier_modele->setEnabled(true);
    ui->btn_supprimer_modele->setEnabled(true);
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — AJOUTER
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::on_btn_ajouter_modele_clicked()
{
    if (m_idSelectionne > 0) {
        msgWarn(this, "Mode modification",
                "Une ligne est selectionnee.\n"
                "Cliquez sur Modifier, ou double-cliquez pour ajouter un nouveau modele.");
        return;
    }
    if (!validerFormulaire()) return;

    bool okL, okW, okH;
    double vL = ui->le_longueur->text().toDouble(&okL);
    double vW = ui->le_largeur->text().toDouble(&okW);
    double vH = ui->le_hauteur->text().toDouble(&okH);

    QSqlQuery q(db());
    q.prepare(
        "INSERT INTO ADEM.MODELE_BOIS "
        "(ID_MODELE, NOM, TYPE, TYPE_BOIS, LONGUEUR, LARGEUR, HAUTEUR, CREEPAR, DATECREATION) "
        "VALUES (ADEM.MODELE_BOIS_SEQ.NEXTVAL, :nom, :type, :bois, "
        ":longueur, :largeur, :hauteur, :creepar, TO_DATE(:date,'YYYY-MM-DD'))");
    q.bindValue(":nom",      ui->le_nom_modele->text().trimmed());
    q.bindValue(":type",     ui->combo_type->currentText());
    q.bindValue(":bois",     ui->combo_bois->currentText());
    q.bindValue(":longueur", vL);
    q.bindValue(":largeur",  vW);
    q.bindValue(":hauteur",  vH);
    q.bindValue(":creepar",  ui->le_créepar->text().trimmed());
    q.bindValue(":date",     ui->de_date_creation->date().toString("yyyy-MM-dd"));

    if (q.exec()) {
        msgInfo(this, "Succes", "Modele ajoute avec succes !");
        reinitialiserFormulaire();
        chargerTableauModeles();
    } else {
        msgErreur(this, "Erreur INSERT", q.lastError().text());
    }
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — MODIFIER
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::on_btn_modifier_modele_clicked()
{
    if (m_idSelectionne <= 0) {
        msgWarn(this, "Aucune selection",
                "Selectionnez d'abord une ligne dans le tableau.");
        return;
    }
    if (!validerFormulaire()) return;

    bool okL, okW, okH;
    double vL = ui->le_longueur->text().toDouble(&okL);
    double vW = ui->le_largeur->text().toDouble(&okW);
    double vH = ui->le_hauteur->text().toDouble(&okH);

    QSqlQuery u(db());
    u.prepare(
        "UPDATE ADEM.MODELE_BOIS SET "
        "NOM=:nom, TYPE=:type, TYPE_BOIS=:bois, "
        "LONGUEUR=:longueur, LARGEUR=:largeur, HAUTEUR=:hauteur, "
        "CREEPAR=:creepar, DATECREATION=TO_DATE(:date,'YYYY-MM-DD') "
        "WHERE ID_MODELE=:id");
    u.bindValue(":nom",      ui->le_nom_modele->text().trimmed());
    u.bindValue(":type",     ui->combo_type->currentText());
    u.bindValue(":bois",     ui->combo_bois->currentText());
    u.bindValue(":longueur", vL);
    u.bindValue(":largeur",  vW);
    u.bindValue(":hauteur",  vH);
    u.bindValue(":creepar",  ui->le_créepar->text().trimmed());
    u.bindValue(":date",     ui->de_date_creation->date().toString("yyyy-MM-dd"));
    u.bindValue(":id",       m_idSelectionne);

    if (u.exec()) {
        msgInfo(this, "Succes", "Modele modifie avec succes !");
        reinitialiserFormulaire();
        chargerTableauModeles();
    } else {
        msgErreur(this, "Erreur UPDATE", u.lastError().text());
    }
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — SUPPRIMER
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::on_btn_supprimer_modele_clicked()
{
    if (m_idSelectionne <= 0) {
        msgWarn(this, "Aucune selection", "Cliquez d'abord sur une ligne du tableau.");
        return;
    }
    if (msgQuestion(this, "Confirmation",
                    QString("Supprimer le modele ID %1 ?\nCette action est irreversible.")
                        .arg(m_idSelectionne)) != QMessageBox::Yes) return;

    QSqlQuery q(db());
    q.prepare("DELETE FROM ADEM.MODELE_BOIS WHERE ID_MODELE = :id");
    q.bindValue(":id", m_idSelectionne);

    if (q.exec()) {
        msgInfo(this, "Succes", "Modele supprime !");
        reinitialiserFormulaire();
        chargerTableauModeles();
    } else {
        msgErreur(this, "Erreur DELETE", q.lastError().text());
    }
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — RECHERCHER
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::on_btn_rechercher_modele_clicked()
{
    QString filtre = ui->rech_6->text().trimmed();
    if (filtre.isEmpty()) { chargerTableauModeles(); return; }

    ui->table_modeles->setRowCount(0);
    QSqlQuery q(db());
    q.prepare(
        "SELECT ID_MODELE, NOM, TYPE, TYPE_BOIS, LONGUEUR, LARGEUR, HAUTEUR, CREEPAR, "
        "TO_CHAR(DATECREATION,'DD/MM/YYYY') AS DATECREATION "
        "FROM ADEM.MODELE_BOIS "
        "WHERE UPPER(NOM) LIKE UPPER(:f1) "
        "OR TO_CHAR(ID_MODELE) LIKE :f2 "
        "OR UPPER(TYPE_BOIS) LIKE UPPER(:f3) "
        "ORDER BY ID_MODELE");
    QString pat = "%" + filtre + "%";
    q.bindValue(":f1", pat); q.bindValue(":f2", pat); q.bindValue(":f3", pat);
    if (!q.exec()) { msgErreur(this, "Erreur recherche", q.lastError().text()); return; }

    int row = 0;
    while (q.next()) {
        ui->table_modeles->insertRow(row);
        for (int c = 0; c < 9; c++)
            ui->table_modeles->setItem(row, c, new QTableWidgetItem(q.value(c).toString()));
        row++;
    }
    if (row == 0)
        msgWarn(this, "Recherche", QString("Aucun modele trouve pour : %1").arg(filtre));
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — TRIER
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::on_btn_tire_clicked()
{
    QString critere = ui->cb_critere_recherche_modele->currentText();
    QMap<QString,QString> col;
    col["Nom"] = "NOM"; col["ID"] = "ID_MODELE"; col["Bois"] = "TYPE_BOIS";
    QString colSQL = col.value(critere, "NOM");

    ui->table_modeles->setRowCount(0);
    QSqlQuery q(db());
    bool ok = q.exec(
        QString("SELECT ID_MODELE, NOM, TYPE, TYPE_BOIS, LONGUEUR, LARGEUR, HAUTEUR, CREEPAR, "
                "TO_CHAR(DATECREATION,'DD/MM/YYYY') AS DATECREATION "
                "FROM ADEM.MODELE_BOIS ORDER BY %1").arg(colSQL));
    if (!ok) { msgErreur(this, "Erreur tri", q.lastError().text()); return; }

    int row = 0;
    while (q.next()) {
        ui->table_modeles->insertRow(row);
        for (int c = 0; c < 9; c++)
            ui->table_modeles->setItem(row, c, new QTableWidgetItem(q.value(c).toString()));
        row++;
    }
}

// ═══════════════════════════════════════════════════════════════════════
//  STATISTIQUES — deux donuts cote a cote
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::afficherStatistiques()
{
    QList<QPair<QString,int>> dataType; int totalType = 0;
    { QSqlQuery q(db()); q.exec("SELECT TYPE, COUNT(*) AS NB FROM ADEM.MODELE_BOIS GROUP BY TYPE ORDER BY NB DESC");
        while (q.next()) { int nb=q.value("NB").toInt(); dataType<<qMakePair(q.value("TYPE").toString(),nb); totalType+=nb; } }

    QList<QPair<QString,int>> dataBois; int totalBois = 0;
    { QSqlQuery q(db()); q.exec("SELECT TYPE_BOIS, COUNT(*) AS NB FROM ADEM.MODELE_BOIS GROUP BY TYPE_BOIS ORDER BY NB DESC");
        while (q.next()) { int nb=q.value("NB").toInt(); dataBois<<qMakePair(q.value("TYPE_BOIS").toString(),nb); totalBois+=nb; } }

    if (dataType.isEmpty() && dataBois.isEmpty()) return;

    int W = ui->placeholder_stats->width();
    int H = ui->placeholder_stats->height();
    if (W < 100 || H < 100) { W = 1100; H = 500; }

    QPixmap pix(W, H); pix.fill(QColor("#F5EFE6"));
    QPainter painter(&pix);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    QList<QColor> pal = { QColor("#7F4129"),QColor("#E8A050"),QColor("#4A7C59"),
                         QColor("#5B4A8A"),QColor("#3A7CA5"),QColor("#C85C38"),
                         QColor("#8FBC5A"),QColor("#B85C8A"),QColor("#4A6A3A"),QColor("#D4956A") };

    const QColor cCard(255,252,247),cBord(180,140,100),cTitre(70,35,10),cHeader(110,55,25);
    int pad=14, cardW=(W-3*pad)/2, cardH=H-2*pad;

    auto drawDonut=[&](int cx,int cy,int cw,int ch,const QList<QPair<QString,int>>&data,const QString&titre,int total){
        if(data.isEmpty()||total==0) return;
        painter.setPen(Qt::NoPen); painter.setBrush(QColor(0,0,0,22)); painter.drawRoundedRect(cx+5,cy+5,cw,ch,14,14);
        painter.setBrush(cCard); painter.setPen(QPen(cBord,1.2)); painter.drawRoundedRect(cx,cy,cw,ch,14,14);
        const int titleH=46; QPainterPath tp; tp.addRoundedRect(QRectF(cx,cy,cw,titleH),14,14);
        painter.fillPath(tp,cHeader); painter.fillRect(cx,cy+titleH-14,cw,14,cHeader);
        painter.setPen(Qt::white); painter.setFont(QFont("Arial",11,QFont::Bold));
        painter.drawText(QRect(cx,cy,cw,titleH),Qt::AlignCenter,titre);
        const int subH=24; painter.setPen(Qt::NoPen); painter.setBrush(QColor(180,140,90,60));
        painter.drawRect(cx,cy+titleH,cw,subH); painter.setPen(cTitre);
        painter.setFont(QFont("Arial",8,QFont::Bold));
        painter.drawText(QRect(cx+12,cy+titleH,cw-24,subH),Qt::AlignVCenter|Qt::AlignLeft,QString("%1 modele(s) au total").arg(total));
        int topY=cy+titleH+subH+8,nbLeg=data.size(),legRows=(nbLeg+1)/2,legH=legRows*24+8,pieAreaH=ch-(titleH+subH+8)-legH-8;
        int diameter=qMin(cw-40,pieAreaH)-10; if(diameter<60)diameter=60;
        int pieX=cx+(cw-diameter)/2,pieY=topY+(pieAreaH-diameter)/2;
        QRectF pieRect(pieX,pieY,diameter,diameter);
        painter.setPen(Qt::NoPen); painter.setBrush(QColor(0,0,0,28)); painter.drawEllipse(pieRect.adjusted(5,5,5,5));
        double angleStart=-90.0*16; int i=0;
        for(const auto&kv:data){ double fraction=(double)kv.second/total; int span16=(int)(fraction*360.0*16);
            QColor c=pal[i%pal.size()]; painter.setBrush(c); painter.setPen(QPen(Qt::white,3));
            painter.drawPie(pieRect,(int)angleStart,span16);
            QColor reflet=c.lighter(160); reflet.setAlpha(45); painter.setBrush(reflet); painter.setPen(Qt::NoPen);
            painter.drawPie(pieRect.adjusted(4,4,-4,-4),(int)angleStart,span16);
            angleStart+=span16; i++; }
        int holeD=(int)(diameter*0.40),holeX=pieX+(diameter-holeD)/2,holeY=pieY+(diameter-holeD)/2;
        for(int s=4;s>=1;s--){painter.setPen(Qt::NoPen);painter.setBrush(QColor(0,0,0,6));painter.drawEllipse(holeX-s,holeY-s,holeD+s*2,holeD+s*2);}
        QRadialGradient radGrad(holeX+holeD/2,holeY+holeD/2,holeD/2);
        radGrad.setColorAt(0.0,QColor(255,252,247)); radGrad.setColorAt(0.7,QColor(250,244,234)); radGrad.setColorAt(1.0,QColor(235,222,205));
        painter.setBrush(radGrad); painter.setPen(QPen(QColor(195,170,140),1.5)); painter.drawEllipse(holeX,holeY,holeD,holeD);
        int fontSize=(holeD>70)?18:(holeD>50)?13:9;
        painter.setPen(cHeader); painter.setFont(QFont("Arial",fontSize,QFont::Bold));
        painter.drawText(QRect(holeX,holeY+holeD/6,holeD,holeD*2/5),Qt::AlignCenter,QString::number(total));
        painter.setPen(QColor(150,100,60)); painter.setFont(QFont("Arial",qMax(6,fontSize/2)));
        painter.drawText(QRect(holeX,holeY+holeD*11/20,holeD,holeD/4),Qt::AlignCenter,"modeles");
        int legY0=cy+ch-legH-4,legColW=cw/2; i=0;
        for(const auto&kv:data){ QColor c=pal[i%pal.size()]; double pct=(double)kv.second*100.0/total;
            int col2=i%2,row2=i/2,lx2=cx+6+col2*legColW,ly2=legY0+row2*24;
            if((row2%2)==0){painter.setPen(Qt::NoPen);painter.setBrush(QColor(0,0,0,8));painter.drawRoundedRect(lx2,ly2,legColW-4,22,4,4);}
            painter.setPen(QPen(c.darker(140),1)); painter.setBrush(c); painter.drawEllipse(lx2+4,ly2+5,13,13);
            painter.setPen(Qt::NoPen); painter.setBrush(QColor(255,255,255,70)); painter.drawEllipse(lx2+5,ly2+6,6,5);
            painter.setPen(cTitre); painter.setFont(QFont("Arial",7,QFont::Bold)); painter.drawText(lx2+21,ly2+12,kv.first);
            painter.setPen(QColor(120,80,40)); painter.setFont(QFont("Arial",7));
            QString valTxt=QString("  %1  (%2%)").arg(kv.second).arg(pct,0,'f',1);
            int nameW=QFontMetrics(QFont("Arial",7,QFont::Bold)).horizontalAdvance(kv.first);
            painter.drawText(lx2+21+nameW,ly2+12,valTxt); i++; }
    };

    drawDonut(pad,        pad,cardW,cardH,dataType,"Repartition par Type",        totalType);
    drawDonut(pad*2+cardW,pad,cardW,cardH,dataBois,"Repartition par Type de Bois",totalBois);
    painter.end();

    QLabel *lbl=ui->placeholder_stats->findChild<QLabel*>("lbl_chart");
    if(!lbl){lbl=new QLabel(ui->placeholder_stats);lbl->setObjectName("lbl_chart");lbl->setAlignment(Qt::AlignCenter);}
    lbl->setGeometry(0,0,W,H); lbl->setPixmap(pix); lbl->show();
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — EXPORT PDF
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::on_btn_export_pdf_modele_clicked()
{
    int nbLignes = ui->table_modeles->rowCount();
    if (nbLignes == 0) { msgWarn(this,"Export PDF","Le tableau est vide."); return; }

    QString fichier = QFileDialog::getSaveFileName(
        this,"Enregistrer le PDF",
        "Modeles_Bois_"+QDate::currentDate().toString("yyyy-MM-dd")+".pdf",
        "Fichiers PDF (*.pdf)");
    if (fichier.isEmpty()) return;

    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fichier);
    printer.setPageOrientation(QPageLayout::Landscape);
    printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(10,10,10,10),QPageLayout::Millimeter);

    QPainter p;
    if (!p.begin(&printer)) { msgErreur(this,"Erreur PDF","Impossible de creer le fichier."); return; }

    const int W=static_cast<int>(printer.pageRect(QPrinter::DevicePixel).width());
    const int H=static_cast<int>(printer.pageRect(QPrinter::DevicePixel).height());
    const int dpi=printer.resolution();
    const double mm=dpi/25.4;
    const QColor cMarron(127,65,41),cBeige(237,224,200),cBeigeF(250,243,232);
    const QColor cBlanc(255,255,255),cTexte(46,26,0),cBord(180,140,100);
    const int hBandeau=static_cast<int>(14*mm),hSousBande=static_cast<int>(6*mm);
    const int hEntete=static_cast<int>(8*mm),hLigne=static_cast<int>(6*mm);
    const int hPied=static_cast<int>(5*mm),pad=static_cast<int>(1*mm);

    QStringList headers={"ID","Nom","Type","Type Bois","Long.","Larg.","Haut.","Cree par","Date"};
    QVector<double> pct={0.05,0.14,0.10,0.11,0.07,0.07,0.07,0.14,0.12};
    const int nbCols=headers.size();
    QVector<int> colW(nbCols); int sumW=0;
    for(int c=0;c<nbCols-1;c++){colW[c]=static_cast<int>(pct[c]*W);sumW+=colW[c];}
    colW[nbCols-1]=W-sumW;

    auto drawRow=[&](int y,const QStringList&cells,bool isHeader,bool odd){
        int rh=isHeader?hEntete:hLigne;
        p.setPen(Qt::NoPen); p.setBrush(isHeader?cBeige:(odd?cBlanc:cBeigeF)); p.drawRect(0,y,W,rh);
        p.setPen(QPen(cBord,qMax(1,static_cast<int>(0.3*mm)))); p.setBrush(Qt::NoBrush); p.drawRect(0,y,W,rh);
        int x=0;
        for(int c=0;c<qMin(cells.size(),nbCols);c++){
            if(c>0){p.setPen(QPen(cBord,qMax(1,static_cast<int>(0.3*mm))));p.drawLine(x,y,x,y+rh);}
            p.setPen(isHeader?cMarron:cTexte);
            p.setFont(QFont("Arial",qMax(6,static_cast<int>(isHeader?dpi*0.09/72.0:dpi*0.085/72.0)),isHeader?QFont::Bold:QFont::Normal));
            QRect rc(x+pad,y+pad,colW[c]-2*pad,rh-2*pad);
            p.drawText(rc,Qt::AlignVCenter|Qt::AlignLeft|Qt::TextSingleLine,p.fontMetrics().elidedText(cells[c],Qt::ElideRight,rc.width()));
            x+=colW[c]; }};

    int numPage=0;
    auto drawHeader=[&](bool first)->int{
        numPage++; p.setPen(Qt::NoPen); p.setBrush(cMarron); p.drawRect(0,0,W,hBandeau);
        p.setPen(Qt::white); p.setFont(QFont("Arial",qMax(8,static_cast<int>(dpi*0.16/72.0)),QFont::Bold));
        p.drawText(QRect(0,0,W,hBandeau),Qt::AlignCenter,first?"Liste des Modeles de Bois":"Liste des Modeles de Bois (suite)");
        p.setPen(Qt::NoPen); p.setBrush(QColor(200,170,130)); p.drawRect(0,hBandeau,W,hSousBande);
        p.setPen(cTexte); p.setFont(QFont("Arial",qMax(6,static_cast<int>(dpi*0.10/72.0))));
        QString info=first?QString("Genere le %1   |   %2 modele(s)").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm")).arg(nbLignes):QString("Page %1").arg(numPage);
        p.drawText(QRect(pad,hBandeau,W-2*pad,hSousBande),Qt::AlignVCenter|Qt::AlignLeft,info);
        return hBandeau+hSousBande+pad; };

    auto drawFooter=[&](){
        p.setPen(QColor(160,160,160)); p.setFont(QFont("Arial",qMax(5,static_cast<int>(dpi*0.07/72.0))));
        p.drawLine(0,H-hPied,W,H-hPied);
        p.drawText(QRect(0,H-hPied+pad,W,hPied),Qt::AlignCenter,QString("Page %1  —  Gestion Atelier Bois").arg(numPage)); };

    int yPos=drawHeader(true);
    drawRow(yPos,headers,true,false); yPos+=hEntete;
    for(int row=0;row<nbLignes;row++){
        if(yPos+hLigne>H-hPied-2*pad){ drawFooter(); printer.newPage(); yPos=drawHeader(false); drawRow(yPos,headers,true,false); yPos+=hEntete; }
        QStringList cells;
        for(int c=0;c<nbCols;c++){ QTableWidgetItem*it=ui->table_modeles->item(row,c); cells<<(it?it->text():""); }
        drawRow(yPos,cells,false,row%2==0); yPos+=hLigne; }
    drawFooter(); p.end();
    msgInfo(this,"Export PDF reussi",QString("Fichier genere !\n\n%1").arg(fichier));
}
