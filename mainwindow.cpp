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
#include <QRegularExpression>
#include <QHeaderView>
#include <QTextDocument>
#include <QtMath>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QTimer>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

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

static QColor statutColor(const QString &statut)
{
    if (statut == "Planifié")  return QColor("#27AE60");
    if (statut == "En cours")  return QColor("#E67E22");
    return QColor("#C0392B");
}

// ═══════════════════════════════════════════════════════════════════════
//  CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
    m_fabSelected(false), m_currentFabIndex(-1)
{
    ui->setupUi(this);

    connect(ui->btn_bois,        &QPushButton::clicked, this, [=]() { ui->stackedWidget->setCurrentWidget(ui->page);   });
    connect(ui->btn_modele,      &QPushButton::clicked, this, [=]() { ui->stackedWidget->setCurrentWidget(ui->page_6); afficherStatistiques(); });
    connect(ui->btn_personnel,   &QPushButton::clicked, this, [=]() { ui->stackedWidget->setCurrentWidget(ui->page_5); });
    connect(ui->btn_etape,       &QPushButton::clicked, this, [=]() { ui->stackedWidget->setCurrentWidget(ui->page_7); });
    connect(ui->btn_fabrication, &QPushButton::clicked, this, [=]() { ui->stackedWidget->setCurrentWidget(ui->page_4); });

    connect(ui->tabWidget_modeles, &QTabWidget::currentChanged, this, [=](int idx) {
        if (idx == 1) afficherStatistiques();
        if (idx == 2) afficherTendances();
        if (idx == 3) afficherAnalyseCouts();         // ← index de l'onglet Tendances
    });


    // ── Config tableau Modeles ────────────────────────────────────────────
    // Ordre colonnes SQL : IDMODELE|NOM|TYPE|IDBOIS|LONGUEUR|LARGEUR|HAUTEUR|DATECREATION|CREEPAR
    ui->table_modeles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table_modeles->setAlternatingRowColors(true);
    ui->table_modeles->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table_modeles->horizontalHeader()->setStretchLastSection(true);
    ui->table_modeles->verticalHeader()->setVisible(false);
    ui->table_modeles->setColumnCount(9);
    ui->table_modeles->setHorizontalHeaderLabels(
        {"ID","Nom","Type","ID Bois","Longueur","Largeur","Hauteur","Date creation","Cree par"});

    ui->tab_rech_3->setStyleSheet("QTableView { color: black; background-color: white; }");
    ui->tab_rech_3->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tab_rech_2->setStyleSheet("QTableView { color: black; background-color: white; }");
    ui->tab_rech_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->table_modeles_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table_modeles_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->table_modeles_2->setColumnCount(6);
    ui->table_modeles_2->setHorizontalHeaderLabels(
        {"ID Fabrication", "ID Modèle", "Date Début", "Quantité", "Qualité", "Commentaire"});

    ui->tree_fabrications_etapes->setColumnCount(6);
    ui->tree_fabrications_etapes->setHeaderLabels(
        {"ID / Étape", "Nom", "Statut", "Date Début", "Date Fin", "Temps"});

    ui->de_date_creation->setDate(QDate::currentDate());
    ui->aff_date_deb->setMinimumDate(QDate::currentDate());
    ui->aff_date_deb->setDate(QDate::currentDate());

    connect(ui->le_nom_modele, &QLineEdit::textChanged, this, [=](const QString &txt) {
        bool ok = txt.trimmed().length() >= 2 &&
                  QRegularExpression("^[a-zA-ZÀ-ÿ\\s\\-_]+$").match(txt.trimmed()).hasMatch();
        setFieldStyle(ui->le_nom_modele, ok);
    });
    connect(ui->le_longueur, &QLineEdit::textChanged, this, [=](const QString &txt) {
        bool ok; double v = txt.toDouble(&ok); setFieldStyle(ui->le_longueur, ok && v > 0);
    });
    connect(ui->le_largeur, &QLineEdit::textChanged, this, [=](const QString &txt) {
        bool ok; double v = txt.toDouble(&ok); setFieldStyle(ui->le_largeur, ok && v > 0);
    });
    connect(ui->le_hauteur, &QLineEdit::textChanged, this, [=](const QString &txt) {
        bool ok; double v = txt.toDouble(&ok); setFieldStyle(ui->le_hauteur, ok && v > 0);
    });
    connect(ui->le_créepar, &QLineEdit::textChanged, this, [=](const QString &txt) {
        bool ok = txt.trimmed().length() >= 2 &&
                  QRegularExpression("^[a-zA-ZÀ-ÿ\\s]+$").match(txt.trimmed()).hasMatch();
        setFieldStyle(ui->le_créepar, ok);
    });

    if (!Connexion::createInstance().createconnect()) {
        msgErreur(this, "Erreur BDD",
                  "Connexion Oracle echouee.\nVerifiez la source ODBC, l'utilisateur et le mot de passe.");
        return;
    }

    QString btnStyle =
        "QPushButton { background-color: #7F4129; color: #FFFFFF; font-weight: bold; border-radius: 6px; }"
        "QPushButton:hover { background-color: #9C5233; }"
        "QPushButton:disabled { background-color: #C4A882; color: #8B6B50; }";
    ui->btn_ajouter_modele->setStyleSheet(btnStyle);
    ui->btn_modifier_modele->setStyleSheet(btnStyle);
    ui->btn_supprimer_modele->setStyleSheet(btnStyle);
    ui->btn_ajouter_modele->setEnabled(true);
    ui->btn_modifier_modele->setEnabled(false);
    ui->btn_supprimer_modele->setEnabled(false);

    chargerTableauModeles();
    ui->table_modeles->setColumnHidden(0, true);
    refreshTable();
    refreshBoisTable();
    loadFabrications();
    generateFabricationId();

    connect(ui->table_modeles,    &QTableWidget::cellClicked,   this, &MainWindow::ligneSelectionnee);
    connect(ui->rech_6,           &QLineEdit::textChanged,      this, &MainWindow::on_btn_rechercher_modele_clicked);
    connect(ui->btn_rechercher_5, &QPushButton::clicked,        this, &MainWindow::on_btn_rechercher_modele_clicked);
    connect(ui->rech_6,           &QLineEdit::returnPressed,    this, &MainWindow::on_btn_rechercher_modele_clicked);
    connect(ui->btn_tire,         &QPushButton::clicked,        this, &MainWindow::on_btn_tire_clicked);
    connect(ui->table_modeles, &QTableWidget::itemDoubleClicked, this, [=]() {
        reinitialiserFormulaire();
        msgInfo(this, "Mode Ajout", "Champs reinitialises.");
    });

    connect(ui->tab_rech_3, &QTableView::clicked, this, &MainWindow::on_tab_employes_clicked);
    connect(ui->tab_rech_2, &QTableView::clicked, this, &MainWindow::on_tab_bois_7_clicked);

    connect(ui->table_modeles_2, &QTableWidget::cellClicked, this, &MainWindow::on_table_modeles_2_cellClicked);
    connect(ui->rech_7, &QLineEdit::textChanged, this, &MainWindow::on_rech_7_textChanged);
    connect(ui->btn_rechercher_6, &QPushButton::clicked, this, [=]() { filterFabrications(ui->rech_7->text()); });
    connect(ui->btn_tire_2, &QPushButton::clicked, this, [=]() { sortFabrications(ui->cb_critere_recherche_modele_2->currentText()); });
    connect(ui->btn_export_pdf_modele_2, &QPushButton::clicked, this, &MainWindow::exportToPDF);
}

MainWindow::~MainWindow() { delete ui; }

QSqlDatabase MainWindow::db() const {
    return Connexion::createInstance().getDatabase();
}

// ═══════════════════════════════════════════════════════════════════════
//  BOIS
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::refreshBoisTable()
{
    Bois b;
    ui->tab_rech_2->setModel(b.afficher());
    ui->tab_rech_2->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    ui->tab_rech_2->horizontalHeader()->setStretchLastSection(true);
    ui->tab_rech_2->setStyleSheet("QTableView { color: black; background-color: white; }");
}

void MainWindow::on_la_ajouter_7_clicked()
{
    QString nom=ui->la_nom_7->currentText(), type=ui->la_type_7->currentText();
    QString etat=ui->la_etat_7->currentText(), fournisseur=ui->la_fournisseur_7->text();
    QString emplacement=ui->la_emplacement_7->text();
    double prix=ui->la_prix_7->value(), l=ui->la_l_7->value(), w=ui->la_w_7->value(), h=ui->la_h_7->value();
    QDate date=ui->la_date_7->date();
    if (nom.isEmpty()||fournisseur.isEmpty()||emplacement.isEmpty()) {
        QMessageBox::warning(this,"Erreur","Veuillez remplir tous les champs."); return; }
    Bois b(nom,type,l,w,h,etat,prix,date,fournisseur,emplacement);
    if (b.ajouter()) { QMessageBox::information(this,"Succes","Bois ajoute !"); refreshBoisTable(); }
    else QMessageBox::critical(this,"Erreur","L'ajout a echoue.");
}

void MainWindow::on_la_modifier_7_clicked()
{
    int id=ui->la_id_14->text().toInt();
    if (id==0) { QMessageBox::warning(this,"Erreur","Selectionnez un bois d'abord."); return; }
    QString nom=ui->la_nom_7->currentText(), type=ui->la_type_7->currentText();
    QString etat=ui->la_etat_7->currentText(), fournisseur=ui->la_fournisseur_7->text();
    QString emplacement=ui->la_emplacement_7->text();
    double prix=ui->la_prix_7->value(), l=ui->la_l_7->value(), w=ui->la_w_7->value(), h=ui->la_h_7->value();
    QDate date=ui->la_date_7->date();
    Bois b(nom,type,l,w,h,etat,prix,date,fournisseur,emplacement);
    if (b.modifier(id)) { QMessageBox::information(this,"Succes","Bois modifie !"); refreshBoisTable(); }
    else QMessageBox::critical(this,"Erreur","La modification a echoue.");
}

void MainWindow::on_la_supprimer_7_clicked()
{
    int id=ui->la_id_14->text().toInt();
    if (id==0) { QMessageBox::warning(this,"Erreur","Selectionnez un bois d'abord."); return; }
    Bois b;
    if (b.supprimer(id)) { QMessageBox::information(this,"Succes","Bois supprime."); refreshBoisTable(); }
    else QMessageBox::critical(this,"Erreur","La suppression a echoue.");
}

void MainWindow::on_chercher_7_clicked()
{
    Bois b; ui->tab_rech_2->setModel(b.rechercher(ui->rech_4->text()));
}

void MainWindow::on_tab_bois_7_clicked(const QModelIndex &index)
{
    int row=index.row();
    auto get=[&](int col){ return ui->tab_rech_2->model()->data(ui->tab_rech_2->model()->index(row,col)); };
    ui->la_id_14->setText(get(0).toString());
    ui->la_nom_7->setCurrentText(get(1).toString());
    ui->la_type_7->setCurrentText(get(2).toString());
    ui->la_l_7->setValue(get(3).toDouble()); ui->la_w_7->setValue(get(4).toDouble()); ui->la_h_7->setValue(get(5).toDouble());
    ui->la_etat_7->setCurrentText(get(6).toString());
    ui->la_prix_7->setValue(get(7).toDouble());
    ui->la_date_7->setDate(get(8).toDate());
    ui->la_fournisseur_7->setText(get(9).toString());
    ui->la_emplacement_7->setText(get(10).toString());
}

void MainWindow::on_la_trier_7_clicked()
{
    QString choix=ui->la_tri_7->currentText(), colonne;
    if      (choix=="Nom")  colonne="nomBois";
    else if (choix=="Date") colonne="dateEntree";
    else if (choix=="Type") colonne="typeBois";
    else if (choix=="Etat") colonne="etatBois";
    else if (choix=="Prix") colonne="prixUnitaire";
    Bois b; ui->tab_rech_2->setModel(b.trier(colonne));
}

void MainWindow::on_la_pdf_9_clicked()
{
    QAbstractItemModel *model=ui->tab_rech_2->model();
    if (!model) { QMessageBox::warning(this,"Erreur","Aucune donnee a exporter."); return; }
    QString fileName=QFileDialog::getSaveFileName(this,"Enregistrer PDF","Bois_Export.pdf","PDF Files (*.pdf)");
    if (fileName.isEmpty()) return;
    QString html="<h2 style='text-align:center;'>Liste des Bois - WoodPilot</h2>";
    html+="<table border='1' cellspacing='0' cellpadding='4' width='100%'>";
    html+="<tr style='background-color:#5c3317; color:white;'>";
    for (int c=0;c<model->columnCount();c++) html+="<th>"+model->headerData(c,Qt::Horizontal).toString()+"</th>";
    html+="</tr>";
    for (int r=0;r<model->rowCount();r++) {
        html+=(r%2==0)?"<tr>":"<tr style='background-color:#f5f0eb;'>";
        for (int c=0;c<model->columnCount();c++) html+="<td>"+model->data(model->index(r,c)).toString()+"</td>";
        html+="</tr>";
    }
    html+="</table>";
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat); printer.setOutputFileName(fileName);
    printer.setPageOrientation(QPageLayout::Landscape);
    QTextDocument doc; doc.setHtml(html); doc.print(&printer);
    QMessageBox::information(this,"Succes","PDF exporte !");
}

// ═══════════════════════════════════════════════════════════════════════
//  PERSONNEL
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::refreshTable() { Personnel P; ui->tab_rech_3->setModel(P.afficher()); }

void MainWindow::clearFields() {
    ui->le_cin->clear(); ui->le_nom_2->clear(); ui->le_prenom->clear();
    ui->le_salaire->clear(); ui->le_tel->clear(); ui->le_rfid->clear();
}

void MainWindow::on_btn_ajouter_3_clicked()
{
    if (!controlDeSaisie()) return;
    Personnel P(ui->le_cin->text().toInt(),ui->le_nom_2->text(),ui->le_prenom->text(),
                ui->de_naissance->date(),ui->cb_poste->currentText(),ui->de_embauche->date(),
                ui->le_salaire->text().toInt(),ui->le_tel->text().toInt(),ui->le_rfid->text());
    if (P.ajouter()) { QMessageBox::information(this,"Succes","Employe ajoute !"); refreshTable(); clearFields(); }
    else QMessageBox::critical(this,"Erreur","L'ajout a echoue.");
}

void MainWindow::on_btn_modifier_2_clicked()
{
    if (!controlDeSaisie()) return;
    Personnel P(ui->le_cin->text().toInt(),ui->le_nom_2->text(),ui->le_prenom->text(),
                ui->de_naissance->date(),ui->cb_poste->currentText(),ui->de_embauche->date(),
                ui->le_salaire->text().toInt(),ui->le_tel->text().toInt(),ui->le_rfid->text());
    if (P.modifier()) { QMessageBox::information(this,"Succes","Employe mis a jour."); refreshTable(); }
    else QMessageBox::critical(this,"Erreur","La modification a echoue.");
}

void MainWindow::on_btn_supprimer_2_clicked()
{
    int cin=ui->le_cin->text().toInt();
    if (cin==0) { QMessageBox::warning(this,"Avertissement","Veuillez saisir un CIN."); return; }
    Personnel P;
    if (P.supprimer(cin)) { QMessageBox::information(this,"Succes","Employe supprime."); refreshTable(); }
    else QMessageBox::critical(this,"Erreur","La suppression a echoue.");
}

void MainWindow::on_tab_employes_clicked(const QModelIndex &index)
{
    int row=index.row();
    auto getData=[&](int col)->QString{
        return ui->tab_rech_3->model()->data(ui->tab_rech_3->model()->index(row,col)).toString(); };
    ui->le_cin->setText(getData(0)); ui->le_nom_2->setText(getData(1)); ui->le_prenom->setText(getData(2));
    QDate dn=QDate::fromString(getData(3),"yyyy-MM-dd");
    if (!dn.isValid()) dn=QDate::fromString(getData(3),"dd/MM/yyyy");
    ui->de_naissance->setDate(dn.isValid()?dn:QDate::currentDate());
    ui->cb_poste->setCurrentText(getData(4));
    QDate de=QDate::fromString(getData(5),"yyyy-MM-dd");
    if (!de.isValid()) de=QDate::fromString(getData(5),"dd/MM/yyyy");
    ui->de_embauche->setDate(de.isValid()?de:QDate::currentDate());
    ui->le_salaire->setText(getData(6)); ui->le_tel->setText(getData(7)); ui->le_rfid->setText(getData(8));
}

bool MainWindow::controlDeSaisie()
{
    QString cin=ui->le_cin->text(),nom=ui->le_nom_2->text(),prenom=ui->le_prenom->text();
    QString tel=ui->le_tel->text(),salaire=ui->le_salaire->text();
    QDate dn=ui->de_naissance->date(),de=ui->de_embauche->date();
    QString poste=ui->cb_poste->currentText();
    if (cin.isEmpty()||nom.isEmpty()||prenom.isEmpty()||tel.isEmpty()||salaire.isEmpty()) {
        QMessageBox::warning(this,"Erreur","Tous les champs obligatoires doivent etre remplis."); return false; }
    if (!QRegularExpression("^[0-9]{8}$").match(cin).hasMatch()) {
        QMessageBox::warning(this,"Erreur CIN","Le CIN doit contenir exactement 8 chiffres."); return false; }
    if (!QRegularExpression("^[A-Za-z ]{2,20}$").match(nom).hasMatch()||
        !QRegularExpression("^[A-Za-z ]{2,20}$").match(prenom).hasMatch()) {
        QMessageBox::warning(this,"Erreur Nom","Lettres uniquement, 2-20 caracteres."); return false; }
    if (!QRegularExpression("^[0-9]{8}$").match(tel).hasMatch()) {
        QMessageBox::warning(this,"Erreur Tel","Le telephone doit contenir 8 chiffres."); return false; }
    if (salaire.toInt()<=0) {
        QMessageBox::warning(this,"Erreur Salaire","Le salaire doit etre positif."); return false; }
    if (dn.daysTo(QDate::currentDate())<18*365) {
        QMessageBox::warning(this,"Erreur Date","L'employe doit etre majeur (18 ans min)."); return false; }
    if (de<=dn) {
        QMessageBox::warning(this,"Erreur Date","La date d'embauche doit etre apres la naissance."); return false; }
    if (poste=="Poste..."||poste.isEmpty()) {
        QMessageBox::warning(this,"Erreur Poste","Veuillez selectionner un poste."); return false; }
    return true;
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — validerFormulaire
// ═══════════════════════════════════════════════════════════════════════
bool MainWindow::validerFormulaire()
{
    bool valide=true; QStringList erreurs;
    QString nom=ui->le_nom_modele->text().trimmed();
    if (nom.isEmpty()) { setFieldStyle(ui->le_nom_modele,false); erreurs<<"• Nom : obligatoire."; valide=false; }
    else if (nom.length()<2) { setFieldStyle(ui->le_nom_modele,false); erreurs<<"• Nom : min 2 car."; valide=false; }
    else if (!QRegularExpression("^[a-zA-ZÀ-ÿ\\s\\-_]+$").match(nom).hasMatch()) { setFieldStyle(ui->le_nom_modele,false); erreurs<<"• Nom : lettres uniquement."; valide=false; }
    else setFieldStyle(ui->le_nom_modele,true);

    QString type=ui->combo_type->currentText().trimmed();
    if (type.isEmpty()) { ui->combo_type->setStyleSheet(STYLE_ERREUR); erreurs<<"• Type : obligatoire."; valide=false; }
    else ui->combo_type->setStyleSheet("");

    bool okL; double lon=ui->le_longueur->text().toDouble(&okL);
    if (!okL||lon<=0) { setFieldStyle(ui->le_longueur,false); erreurs<<"• Longueur : nombre positif."; valide=false; }
    else if (lon>9999) { setFieldStyle(ui->le_longueur,false); erreurs<<"• Longueur : max 9999."; valide=false; }
    else setFieldStyle(ui->le_longueur,true);

    bool okW; double lar=ui->le_largeur->text().toDouble(&okW);
    if (!okW||lar<=0) { setFieldStyle(ui->le_largeur,false); erreurs<<"• Largeur : nombre positif."; valide=false; }
    else if (lar>9999) { setFieldStyle(ui->le_largeur,false); erreurs<<"• Largeur : max 9999."; valide=false; }
    else setFieldStyle(ui->le_largeur,true);

    bool okH; double hau=ui->le_hauteur->text().toDouble(&okH);
    if (!okH||hau<=0) { setFieldStyle(ui->le_hauteur,false); erreurs<<"• Hauteur : nombre positif."; valide=false; }
    else if (hau>9999) { setFieldStyle(ui->le_hauteur,false); erreurs<<"• Hauteur : max 9999."; valide=false; }
    else setFieldStyle(ui->le_hauteur,true);

    QString creepar=ui->le_créepar->text().trimmed();
    if (creepar.isEmpty()) { setFieldStyle(ui->le_créepar,false); erreurs<<"• Cree par : obligatoire."; valide=false; }
    else if (creepar.length()<2) { setFieldStyle(ui->le_créepar,false); erreurs<<"• Cree par : min 2 car."; valide=false; }
    else if (!QRegularExpression("^[a-zA-ZÀ-ÿ\\s]+$").match(creepar).hasMatch()) { setFieldStyle(ui->le_créepar,false); erreurs<<"• Cree par : lettres uniquement."; valide=false; }
    else setFieldStyle(ui->le_créepar,true);

    if (ui->de_date_creation->date()>QDate::currentDate()) { erreurs<<"• Date : pas dans le futur."; valide=false; }
    if (!valide) msgWarn(this,"Erreurs","Veuillez corriger :\n\n"+erreurs.join("\n"));
    return valide;
}

void MainWindow::reinitialiserFormulaire()
{
    resetField(ui->le_nom_modele); resetField(ui->le_longueur); resetField(ui->le_largeur);
    resetField(ui->le_hauteur); resetField(ui->le_créepar);
    m_idSelectionne=-1;
    ui->combo_type->setStyleSheet(""); ui->le_nom_modele->clear();
    ui->combo_type->setCurrentIndex(0); ui->le_longueur->clear(); ui->le_largeur->clear();
    ui->le_hauteur->clear(); ui->le_créepar->clear();
    ui->de_date_creation->setDate(QDate::currentDate());
    ui->btn_ajouter_modele->setEnabled(true); ui->btn_ajouter_modele->setToolTip("");
    ui->btn_modifier_modele->setEnabled(false); ui->btn_supprimer_modele->setEnabled(false);
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — chargerTableauModeles
//  Table : MODELE
//  Colonnes retournées (index 0..8) :
//    0=IDMODELE  1=NOM  2=TYPE  3=IDBOIS
//    4=LONGUEUR  5=LARGEUR  6=HAUTEUR  7=DATECREATION  8=CREEPAR
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::chargerTableauModeles()
{
    ui->table_modeles->setRowCount(0);
    QSqlQuery query(db());
    if (!query.exec(
            "SELECT IDMODELE, NOM, TYPE, IDBOIS, LONGUEUR, LARGEUR, HAUTEUR, "
            "TO_CHAR(DATECREATION,'DD/MM/YYYY') AS DATECREATION, CREEPAR "
            "FROM MODELE ORDER BY IDMODELE"))
    {
        msgErreur(this,"Erreur chargement",query.lastError().text()); return;
    }
    int row=0;
    while (query.next()) {
        ui->table_modeles->insertRow(row);
        for (int c=0;c<9;c++)
            ui->table_modeles->setItem(row,c,new QTableWidgetItem(query.value(c).toString()));
        row++;
    }
    ui->table_modeles->setColumnHidden(0, true);
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — ligneSelectionnee
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::ligneSelectionnee(int row, int)
{
    if (row<0||row>=ui->table_modeles->rowCount()) return;
    auto txt=[&](int col)->QString{
        QTableWidgetItem*it=ui->table_modeles->item(row,col); return it?it->text():""; };

    m_idSelectionne = txt(0).toInt();        // col0 = IDMODELE
    ui->le_nom_modele->setText(txt(1));       // col1 = NOM
    ui->combo_type->setCurrentText(txt(2));   // col2 = TYPE
    // col3 = IDBOIS → chercher dans combo_bois
    ui->le_longueur->setText(txt(4));         // col4 = LONGUEUR
    ui->le_largeur->setText(txt(5));          // col5 = LARGEUR
    ui->le_hauteur->setText(txt(6));          // col6 = HAUTEUR
    QDate d=QDate::fromString(txt(7),"dd/MM/yyyy");
    if (d.isValid()) ui->de_date_creation->setDate(d); // col7 = DATECREATION
    ui->le_créepar->setText(txt(8));          // col8 = CREEPAR

    resetField(ui->le_nom_modele); resetField(ui->le_longueur); resetField(ui->le_largeur);
    resetField(ui->le_hauteur); resetField(ui->le_créepar);
    ui->btn_ajouter_modele->setEnabled(false);
    ui->btn_ajouter_modele->setToolTip("Double-cliquez pour reinitialiser.");
    ui->btn_modifier_modele->setEnabled(true); ui->btn_supprimer_modele->setEnabled(true);
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — AJOUTER  (INSERT INTO MODELE)
// ═══════════════════════════════════════════════════════════════════════

void MainWindow::on_btn_ajouter_modele_clicked()
{
    if (m_idSelectionne>0) {
        msgWarn(this,"Mode modification","Cliquez sur Modifier ou double-cliquez pour ajouter."); return; }
    if (!validerFormulaire()) return;

    bool okL,okW,okH;
    double vL=ui->le_longueur->text().toDouble(&okL);
    double vW=ui->le_largeur->text().toDouble(&okW);
    double vH=ui->le_hauteur->text().toDouble(&okH);
    Q_UNUSED(okL) Q_UNUSED(okW) Q_UNUSED(okH)


    bool okBois;
    int idBois = ui->le_id_bois->text().toInt(&okBois);

    if (!okBois) {
        msgWarn(this, "Erreur de saisie", "L'ID Bois doit être un nombre valide.");
        return;
    }

    QSqlQuery q(db());
    q.prepare(
        "INSERT INTO MODELE (IDMODELE, NOM, TYPE, IDBOIS, "
        "LONGUEUR, LARGEUR, HAUTEUR, DATECREATION, CREEPAR) "
        "VALUES (seq_modele.NEXTVAL, :nom, :type, :idbois, "
        ":longueur, :largeur, :hauteur, TO_DATE(:date,'YYYY-MM-DD'), :creepar)");
    q.bindValue(":nom",     ui->le_nom_modele->text().trimmed());
    q.bindValue(":type",    ui->combo_type->currentText());
    q.bindValue(":idbois",  idBois);
    q.bindValue(":longueur",vL); q.bindValue(":largeur",vW); q.bindValue(":hauteur",vH);
    q.bindValue(":date",    ui->de_date_creation->date().toString("yyyy-MM-dd"));
    q.bindValue(":creepar", ui->le_créepar->text().trimmed());

    if (q.exec()) { msgInfo(this,"Succes","Modele ajoute !"); reinitialiserFormulaire(); chargerTableauModeles(); }
    else msgErreur(this,"Erreur INSERT",q.lastError().text());

}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — MODIFIER  (UPDATE MODELE)
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::on_btn_modifier_modele_clicked()
{
    if (m_idSelectionne<=0) { msgWarn(this,"Aucune selection","Selectionnez une ligne."); return; }
    if (!validerFormulaire()) return;

    bool okL,okW,okH;
    double vL=ui->le_longueur->text().toDouble(&okL);
    double vW=ui->le_largeur->text().toDouble(&okW);
    double vH=ui->le_hauteur->text().toDouble(&okH);
    Q_UNUSED(okL) Q_UNUSED(okW) Q_UNUSED(okH)

    bool okBois;
    int idBois = 1; // valeur par defaut
    if (!okBois) { msgWarn(this,"Erreur","L'ID Bois doit être un nombre valide."); return; }

    QSqlQuery u(db());
    u.prepare(
        "UPDATE MODELE SET NOM=:nom, TYPE=:type, IDBOIS=:idbois, "
        "LONGUEUR=:longueur, LARGEUR=:largeur, HAUTEUR=:hauteur, "
        "DATECREATION=TO_DATE(:date,'YYYY-MM-DD'), CREEPAR=:creepar "
        "WHERE IDMODELE=:id");
    u.bindValue(":nom",     ui->le_nom_modele->text().trimmed());
    u.bindValue(":type",    ui->combo_type->currentText());
    u.bindValue(":idbois",  idBois);
    u.bindValue(":longueur",vL); u.bindValue(":largeur",vW); u.bindValue(":hauteur",vH);
    u.bindValue(":date",    ui->de_date_creation->date().toString("yyyy-MM-dd"));
    u.bindValue(":creepar", ui->le_créepar->text().trimmed());
    u.bindValue(":id",      m_idSelectionne);

    if (u.exec()) { msgInfo(this,"Succes","Modele modifie !"); reinitialiserFormulaire(); chargerTableauModeles(); }
    else msgErreur(this,"Erreur UPDATE",u.lastError().text());
}
// ═══════════════════════════════════════════════════════════════════════
//  MODELES — SUPPRIMER  (DELETE FROM MODELE)
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::on_btn_supprimer_modele_clicked()
{
    if (m_idSelectionne<=0) { msgWarn(this,"Aucune selection","Cliquez sur une ligne."); return; }
    if (msgQuestion(this,"Confirmation",
                    QString("Supprimer le modele ID %1 ?").arg(m_idSelectionne))!=QMessageBox::Yes) return;
    QSqlQuery q(db());
    q.prepare("DELETE FROM MODELE WHERE IDMODELE=:id");
    q.bindValue(":id",m_idSelectionne);
    if (q.exec()) { msgInfo(this,"Succes","Modele supprime !"); reinitialiserFormulaire(); chargerTableauModeles(); }
    else msgErreur(this,"Erreur DELETE",q.lastError().text());
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — RECHERCHER
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::on_btn_rechercher_modele_clicked()
{
    QString filtre=ui->rech_6->text().trimmed();
    if (filtre.isEmpty()) { chargerTableauModeles(); return; }
    ui->table_modeles->setRowCount(0);
    QSqlQuery q(db());
    q.prepare(
        "SELECT IDMODELE, NOM, TYPE, IDBOIS, LONGUEUR, LARGEUR, HAUTEUR, "
        "TO_CHAR(DATECREATION,'DD/MM/YYYY') AS DATECREATION, CREEPAR "
        "FROM MODELE "
        "WHERE UPPER(NOM)    LIKE UPPER(:f1) "
        "OR TO_CHAR(IDMODELE) LIKE :f2 "
        "OR TO_CHAR(IDBOIS)   LIKE :f3 "
        "OR UPPER(TYPE)      LIKE UPPER(:f4) "
        "OR UPPER(CREEPAR)   LIKE UPPER(:f5) "
        "ORDER BY IDMODELE");
    QString pat="%"+filtre+"%";
    q.bindValue(":f1",pat); q.bindValue(":f2",pat); q.bindValue(":f3",pat);
    q.bindValue(":f4",pat); q.bindValue(":f5",pat);
    if (!q.exec()) { msgErreur(this,"Erreur",q.lastError().text()); return; }
    int row=0;
    while (q.next()) {
        ui->table_modeles->insertRow(row);
        for(int c=0;c<9;c++) ui->table_modeles->setItem(row,c,new QTableWidgetItem(q.value(c).toString()));
        row++;
    }
    if (row==0) msgWarn(this,"Recherche",QString("Aucun modele pour : %1").arg(filtre));
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — TRIER
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::on_btn_tire_clicked()  // Une seule définition (ligne 620)
{
    QMap<QString,QString> col;
    col["Nom"] = "NOM";
    col["ID Bois"] = "IDBOIS";
    col["Type"] = "TYPE";
    QString colSQL = col.value(ui->cb_critere_recherche_modele->currentText(), "NOM");
    ui->table_modeles->setRowCount(0);
    QSqlQuery q(db());
    if (!q.exec(QString("SELECT IDMODELE, NOM, TYPE, IDBOIS, LONGUEUR, LARGEUR, HAUTEUR, "
                        "TO_CHAR(DATECREATION,'DD/MM/YYYY') AS DATECREATION, CREEPAR "
                        "FROM MODELE ORDER BY %1").arg(colSQL)))
    {
        msgErreur(this, "Erreur tri", q.lastError().text());
        return;
    }
    int row = 0;
    while (q.next()) {
        ui->table_modeles->insertRow(row);
        for (int c = 0; c < 9; c++)
            ui->table_modeles->setItem(row, c, new QTableWidgetItem(q.value(c).toString()));
        row++;
    }
    ui->table_modeles->setColumnHidden(0, true);
}

// ═══════════════════════════════════════════════════════════════════════
//  STATISTIQUES
//  - Répartition par TYPE  → FROM MODELE GROUP BY TYPE
//  - Répartition par Bois  → JOIN MODELE + TYPEBOIS pour avoir NOMBOIS
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::afficherStatistiques()
{
    QList<QPair<QString,int>> dataType; int totalType=0;
    {
        QSqlQuery q(db());
        q.exec("SELECT TYPE, COUNT(*) AS NB FROM MODELE GROUP BY TYPE ORDER BY NB DESC");
        while(q.next()){ int nb=q.value("NB").toInt(); dataType<<qMakePair(q.value("TYPE").toString(),nb); totalType+=nb; }
    }

    if (dataType.isEmpty()) return;

    int W=ui->placeholder_stats->width(), H=ui->placeholder_stats->height();
    if(W<100||H<100){W=1100;H=500;}
    QPixmap pix(W,H); pix.fill(QColor("#F5EFE6"));
    QPainter painter(&pix); painter.setRenderHint(QPainter::Antialiasing);

    QList<QColor> pal={QColor("#7F4129"),QColor("#E8A050"),QColor("#4A7C59"),QColor("#5B4A8A"),
                         QColor("#3A7CA5"),QColor("#C85C38"),QColor("#8FBC5A"),QColor("#B85C8A"),
                         QColor("#4A6A3A"),QColor("#D4956A")};
    const QColor cCard(255,252,247),cBord(180,140,100),cTitre(70,35,10),cHeader(110,55,25);
    int pad=14, cardW=W-2*pad, cardH=H-2*pad;

    auto drawDonut=[&](int cx,int cy,int cw,int ch,
                         const QList<QPair<QString,int>>&data,const QString&titre,int total)
    {
        if(data.isEmpty()||total==0) return;
        painter.setPen(Qt::NoPen);painter.setBrush(QColor(0,0,0,22));painter.drawRoundedRect(cx+5,cy+5,cw,ch,14,14);
        painter.setBrush(cCard);painter.setPen(QPen(cBord,1.2));painter.drawRoundedRect(cx,cy,cw,ch,14,14);
        const int titleH=46; QPainterPath tp; tp.addRoundedRect(QRectF(cx,cy,cw,titleH),14,14);
        painter.fillPath(tp,cHeader);painter.fillRect(cx,cy+titleH-14,cw,14,cHeader);
        painter.setPen(Qt::white);painter.setFont(QFont("Arial",11,QFont::Bold));
        painter.drawText(QRect(cx,cy,cw,titleH),Qt::AlignCenter,titre);
        const int subH=24;painter.setPen(Qt::NoPen);painter.setBrush(QColor(180,140,90,60));painter.drawRect(cx,cy+titleH,cw,subH);
        painter.setPen(cTitre);painter.setFont(QFont("Arial",8,QFont::Bold));
        painter.drawText(QRect(cx+12,cy+titleH,cw-24,subH),Qt::AlignVCenter|Qt::AlignLeft,QString("%1 modele(s) au total").arg(total));
        int topY=cy+titleH+subH+8,legRows=(data.size()+1)/2,legH=legRows*24+8,pieAreaH=ch-(titleH+subH+8)-legH-8;
        int diameter=qMin(cw-40,pieAreaH)-10; if(diameter<60)diameter=60;
        int pieX=cx+(cw-diameter)/2,pieY=topY+(pieAreaH-diameter)/2;
        QRectF pieRect(pieX,pieY,diameter,diameter);
        painter.setPen(Qt::NoPen);painter.setBrush(QColor(0,0,0,28));painter.drawEllipse(pieRect.adjusted(5,5,5,5));
        double angleStart=-90.0*16; int i=0;
        for(const auto&kv:data){
            double fraction=(double)kv.second/total;int span16=(int)(fraction*360.0*16);
            QColor c=pal[i%pal.size()];painter.setBrush(c);painter.setPen(QPen(Qt::white,3));
            painter.drawPie(pieRect,(int)angleStart,span16);
            QColor reflet=c.lighter(160);reflet.setAlpha(45);painter.setBrush(reflet);painter.setPen(Qt::NoPen);
            painter.drawPie(pieRect.adjusted(4,4,-4,-4),(int)angleStart,span16);angleStart+=span16;i++;
        }
        int holeD=(int)(diameter*0.40),holeX=pieX+(diameter-holeD)/2,holeY=pieY+(diameter-holeD)/2;
        for(int s=4;s>=1;s--){painter.setPen(Qt::NoPen);painter.setBrush(QColor(0,0,0,6));painter.drawEllipse(holeX-s,holeY-s,holeD+s*2,holeD+s*2);}
        QRadialGradient radGrad(holeX+holeD/2,holeY+holeD/2,holeD/2);
        radGrad.setColorAt(0.0,QColor(255,252,247));radGrad.setColorAt(0.7,QColor(250,244,234));radGrad.setColorAt(1.0,QColor(235,222,205));
        painter.setBrush(radGrad);painter.setPen(QPen(QColor(195,170,140),1.5));painter.drawEllipse(holeX,holeY,holeD,holeD);
        int fontSize=(holeD>70)?18:(holeD>50)?13:9;
        painter.setPen(cHeader);painter.setFont(QFont("Arial",fontSize,QFont::Bold));
        painter.drawText(QRect(holeX,holeY+holeD/6,holeD,holeD*2/5),Qt::AlignCenter,QString::number(total));
        painter.setPen(QColor(150,100,60));painter.setFont(QFont("Arial",qMax(6,fontSize/2)));
        painter.drawText(QRect(holeX,holeY+holeD*11/20,holeD,holeD/4),Qt::AlignCenter,"modeles");
        int legY0=cy+ch-legH-4,legColW=cw/2; i=0;
        for(const auto&kv:data){
            QColor c=pal[i%pal.size()];double pct=(double)kv.second*100.0/total;
            int col2=i%2,row2=i/2,lx2=cx+6+col2*legColW,ly2=legY0+row2*24;
            if((row2%2)==0){painter.setPen(Qt::NoPen);painter.setBrush(QColor(0,0,0,8));painter.drawRoundedRect(lx2,ly2,legColW-4,22,4,4);}
            painter.setPen(QPen(c.darker(140),1));painter.setBrush(c);painter.drawEllipse(lx2+4,ly2+5,13,13);
            painter.setPen(Qt::NoPen);painter.setBrush(QColor(255,255,255,70));painter.drawEllipse(lx2+5,ly2+6,6,5);
            painter.setPen(cTitre);painter.setFont(QFont("Arial",7,QFont::Bold));painter.drawText(lx2+21,ly2+12,kv.first);
            painter.setPen(QColor(120,80,40));painter.setFont(QFont("Arial",7));
            QString valTxt=QString("  %1  (%2%)").arg(kv.second).arg(pct,0,'f',1);
            int nameW=QFontMetrics(QFont("Arial",7,QFont::Bold)).horizontalAdvance(kv.first);
            painter.drawText(lx2+21+nameW,ly2+12,valTxt);i++;
        }
    };

    drawDonut(pad, pad, cardW, cardH, dataType, "Repartition par Type", totalType);
    painter.end();

    QLabel *lbl=ui->placeholder_stats->findChild<QLabel*>("lbl_chart");
    if (!lbl) { lbl=new QLabel(ui->placeholder_stats); lbl->setObjectName("lbl_chart"); lbl->setAlignment(Qt::AlignCenter); }
    lbl->setGeometry(0,0,W,H); lbl->setPixmap(pix); lbl->show();
}

// ═══════════════════════════════════════════════════════════════════════
//  MODELES — EXPORT PDF
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::on_btn_export_pdf_modele_clicked()
{
    int nbLignes=ui->table_modeles->rowCount();
    if(nbLignes==0){msgWarn(this,"Export PDF","Le tableau est vide.");return;}
    QString fichier=QFileDialog::getSaveFileName(this,"Enregistrer le PDF",
                                                   "Modeles_"+QDate::currentDate().toString("yyyy-MM-dd")+".pdf",
                                                   "Fichiers PDF (*.pdf)");
    if(fichier.isEmpty()) return;
    QPrinter printer(QPrinter::HighResolution);
    printer.setOutputFormat(QPrinter::PdfFormat); printer.setOutputFileName(fichier);
    printer.setPageOrientation(QPageLayout::Landscape); printer.setPageSize(QPageSize::A4);
    printer.setPageMargins(QMarginsF(10,10,10,10),QPageLayout::Millimeter);
    QPainter p;
    if(!p.begin(&printer)){msgErreur(this,"Erreur PDF","Impossible de creer le fichier.");return;}
    const int W=static_cast<int>(printer.pageRect(QPrinter::DevicePixel).width());
    const int H=static_cast<int>(printer.pageRect(QPrinter::DevicePixel).height());
    const int dpi=printer.resolution(); const double mm=dpi/25.4;
    const QColor cMarron(127,65,41),cBeige(237,224,200),cBeigeF(250,243,232);
    const QColor cBlanc(255,255,255),cTexte(46,26,0),cBord(180,140,100);
    const int hBandeau=static_cast<int>(14*mm),hSousBande=static_cast<int>(6*mm);
    const int hEntete=static_cast<int>(8*mm),hLigne=static_cast<int>(6*mm);
    const int hPied=static_cast<int>(5*mm),pad=static_cast<int>(1*mm);
    QStringList headers={"ID","Nom","Type","ID Bois","Long.","Larg.","Haut.","Date","Cree par"};
    QVector<double> pct={0.05,0.14,0.10,0.07,0.07,0.07,0.07,0.12,0.14};
    const int nbCols=headers.size(); QVector<int> colW(nbCols); int sumW=0;
    for(int c=0;c<nbCols-1;c++){colW[c]=static_cast<int>(pct[c]*W);sumW+=colW[c];} colW[nbCols-1]=W-sumW;

    auto drawRow=[&](int y,const QStringList&cells,bool isHeader,bool odd){
        int rh=isHeader?hEntete:hLigne; p.setPen(Qt::NoPen);
        p.setBrush(isHeader?cBeige:(odd?cBlanc:cBeigeF)); p.drawRect(0,y,W,rh);
        p.setPen(QPen(cBord,qMax(1,static_cast<int>(0.3*mm)))); p.setBrush(Qt::NoBrush); p.drawRect(0,y,W,rh);
        int x=0; for(int c=0;c<qMin(cells.size(),nbCols);c++){
            if(c>0){p.setPen(QPen(cBord,qMax(1,static_cast<int>(0.3*mm))));p.drawLine(x,y,x,y+rh);}
            p.setPen(isHeader?cMarron:cTexte);
            p.setFont(QFont("Arial",qMax(6,static_cast<int>(isHeader?dpi*0.09/72.0:dpi*0.085/72.0)),isHeader?QFont::Bold:QFont::Normal));
            QRect rc(x+pad,y+pad,colW[c]-2*pad,rh-2*pad);
            p.drawText(rc,Qt::AlignVCenter|Qt::AlignLeft|Qt::TextSingleLine,p.fontMetrics().elidedText(cells[c],Qt::ElideRight,rc.width()));
            x+=colW[c];}
    };
    int numPage=0;
    auto drawHeader=[&](bool first)->int{
        numPage++; p.setPen(Qt::NoPen);p.setBrush(cMarron);p.drawRect(0,0,W,hBandeau);
        p.setPen(Qt::white);p.setFont(QFont("Arial",qMax(8,static_cast<int>(dpi*0.16/72.0)),QFont::Bold));
        p.drawText(QRect(0,0,W,hBandeau),Qt::AlignCenter,first?"Liste des Modeles":"Liste des Modeles (suite)");
        p.setPen(Qt::NoPen);p.setBrush(QColor(200,170,130));p.drawRect(0,hBandeau,W,hSousBande);
        p.setPen(cTexte);p.setFont(QFont("Arial",qMax(6,static_cast<int>(dpi*0.10/72.0))));
        p.drawText(QRect(pad,hBandeau,W-2*pad,hSousBande),Qt::AlignVCenter|Qt::AlignLeft,
                   first?QString("Genere le %1 | %2 modele(s)").arg(QDateTime::currentDateTime().toString("dd/MM/yyyy HH:mm")).arg(nbLignes):QString("Page %1").arg(numPage));
        return hBandeau+hSousBande+pad;
    };
    auto drawFooter=[&](){
        p.setPen(QColor(160,160,160));p.setFont(QFont("Arial",qMax(5,static_cast<int>(dpi*0.07/72.0))));
        p.drawLine(0,H-hPied,W,H-hPied);
        p.drawText(QRect(0,H-hPied+pad,W,hPied),Qt::AlignCenter,QString("Page %1 — Gestion Atelier Bois").arg(numPage));
    };
    int yPos=drawHeader(true); drawRow(yPos,headers,true,false); yPos+=hEntete;
    for(int row=0;row<nbLignes;row++){
        if(yPos+hLigne>H-hPied-2*pad){drawFooter();printer.newPage();yPos=drawHeader(false);drawRow(yPos,headers,true,false);yPos+=hEntete;}
        QStringList cells; for(int c=0;c<nbCols;c++){QTableWidgetItem*it=ui->table_modeles->item(row,c);cells<<(it?it->text():"");}
        drawRow(yPos,cells,false,row%2==0);yPos+=hLigne;
    }
    drawFooter(); p.end();
    msgInfo(this,"Export PDF reussi",QString("Fichier genere !\n\n%1").arg(fichier));
}

// ═══════════════════════════════════════════════════════════════════════
//  FABRICATION
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::generateFabricationId()
{
    int maxId=0;
    QSqlQuery query(db());
    if (query.exec("SELECT MAX(IDFABRICATION) FROM FABRICATION"))
        if (query.next()) maxId=query.value(0).toInt();
    ui->aff_id_fab->setText(QString::number(maxId+1));
    ui->aff_id_fab->setReadOnly(true);
}

void MainWindow::loadFabrications()
{
    QSqlQuery query(db());
    query.prepare(
        "SELECT IDFABRICATION, IDMODELE, DATE_DEBUT, "
        "QUANTITE_A_PRODUIRE, QUALITE, COMMENTAIRE "
        "FROM FABRICATION ORDER BY IDFABRICATION");
    if (!query.exec()) {
        msgErreur(this,"Erreur","Impossible de charger les fabrications:\n"+query.lastError().text()); return; }
    ui->table_modeles_2->setRowCount(0);
    int row=0;
    while (query.next()) {
        ui->table_modeles_2->insertRow(row);
        ui->table_modeles_2->setItem(row,0,new QTableWidgetItem(query.value(0).toString()));
        ui->table_modeles_2->setItem(row,1,new QTableWidgetItem(query.value(1).toString()));
        ui->table_modeles_2->setItem(row,2,new QTableWidgetItem(query.value(2).toDate().toString("dd/MM/yyyy")));
        ui->table_modeles_2->setItem(row,3,new QTableWidgetItem(query.value(3).toString()));
        ui->table_modeles_2->setItem(row,4,new QTableWidgetItem(query.value(4).toString()));
        ui->table_modeles_2->setItem(row,5,new QTableWidgetItem(query.value(5).toString()));
        row++;
    }
    ui->table_modeles_2->resizeColumnsToContents();
}

void MainWindow::filterFabrications(const QString &searchText)
{
    if (searchText.trimmed().isEmpty()) { loadFabrications(); return; }
    QSqlQuery query(db());
    query.prepare(
        "SELECT IDFABRICATION, IDMODELE, DATE_DEBUT, "
        "QUANTITE_A_PRODUIRE, QUALITE, COMMENTAIRE "
        "FROM FABRICATION "
        "WHERE TO_CHAR(IDFABRICATION) LIKE :s "
        "OR TO_CHAR(IDMODELE)         LIKE :s "
        "OR UPPER(TO_CHAR(DATE_DEBUT,'DD/MM/YYYY')) LIKE :s "
        "OR TO_CHAR(QUANTITE_A_PRODUIRE) LIKE :s "
        "OR UPPER(QUALITE)    LIKE :s "
        "OR UPPER(COMMENTAIRE) LIKE :s "
        "ORDER BY IDFABRICATION");
    query.bindValue(":s","%"+searchText.toUpper()+"%");
    if (!query.exec()) { msgErreur(this,"Erreur","Erreur recherche:\n"+query.lastError().text()); return; }
    ui->table_modeles_2->setRowCount(0);
    int row=0;
    while (query.next()) {
        ui->table_modeles_2->insertRow(row);
        for (int c=0;c<6;c++)
            ui->table_modeles_2->setItem(row,c,new QTableWidgetItem(
                                                     c==2?query.value(c).toDate().toString("dd/MM/yyyy"):query.value(c).toString()));
        row++;
    }
}

void MainWindow::sortFabrications(const QString &criteria)
{
    QString orderBy="IDFABRICATION";
    if      (criteria=="Modele")     orderBy="IDMODELE";
    else if (criteria=="Date Debut") orderBy="DATE_DEBUT";
    else if (criteria=="Qualite")    orderBy="QUALITE";
    QSqlQuery query(db());
    if (!query.exec(QString(
                        "SELECT IDFABRICATION, IDMODELE, DATE_DEBUT, "
                        "QUANTITE_A_PRODUIRE, QUALITE, COMMENTAIRE "
                        "FROM FABRICATION ORDER BY %1").arg(orderBy)))
    { msgErreur(this,"Erreur","Erreur tri:\n"+query.lastError().text()); return; }
    ui->table_modeles_2->setRowCount(0);
    int row=0;
    while (query.next()) {
        ui->table_modeles_2->insertRow(row);
        for (int c=0;c<6;c++)
            ui->table_modeles_2->setItem(row,c,new QTableWidgetItem(
                                                     c==2?query.value(c).toDate().toString("dd/MM/yyyy"):query.value(c).toString()));
        row++;
    }
}

void MainWindow::exportToPDF()
{
    if (ui->table_modeles_2->rowCount()==0) { msgWarn(this,"Aucune donnée","Rien à exporter."); return; }
    QString fileName=QFileDialog::getSaveFileName(this,"Exporter PDF","","*.pdf");
    if (fileName.isEmpty()) return;
    QPrinter printer; printer.setOutputFormat(QPrinter::PdfFormat); printer.setOutputFileName(fileName);
    QPainter painter;
    if (!painter.begin(&printer)) { msgErreur(this,"Erreur","Impossible de créer le PDF."); return; }
    int yPos=40;
    painter.setFont(QFont("Arial",16,QFont::Bold));
    painter.drawText(100,yPos,"Rapport des Fabrications - WoodPilot"); yPos+=40;
    int xPos[]={20,120,250,400,500,650};
    QString headers[]={"ID","Modele","Date Debut","Quantite","Qualite","Commentaire"};
    painter.setFont(QFont("Arial",10,QFont::Bold));
    for (int i=0;i<6;i++) painter.drawText(xPos[i],yPos,headers[i]); yPos+=20;
    painter.setFont(QFont("Arial",9));
    for (int r=0;r<ui->table_modeles_2->rowCount();r++) {
        if (yPos>printer.height()-50) {
            printer.newPage(); yPos=40;
            painter.setFont(QFont("Arial",10,QFont::Bold));
            for (int i=0;i<6;i++) painter.drawText(xPos[i],yPos,headers[i]); yPos+=20;
            painter.setFont(QFont("Arial",9));
        }
        for (int c=0;c<6;c++) {
            auto *it=ui->table_modeles_2->item(r,c);
            if (it) { QString t=it->text(); if(c==5&&t.length()>30) t=t.left(27)+"..."; painter.drawText(xPos[c],yPos,t); }
        }
        yPos+=20;
    }
    painter.end();
    msgInfo(this,"Succès","PDF exporté avec succès !");
}

bool MainWindow::validateFabricationFields()
{
    if (ui->aff_id_modele->text().trimmed().isEmpty()) {
        msgWarn(this,"Champ vide","L'ID du Modèle est obligatoire."); ui->aff_id_modele->setFocus(); return false; }
    bool ok;
    int idModele=ui->aff_id_modele->text().trimmed().toInt(&ok);
    if (!ok) { msgWarn(this,"Erreur de format","L'ID du modèle doit être un nombre."); ui->aff_id_modele->setFocus(); return false; }
    QSqlQuery checkQuery(db());
    checkQuery.prepare("SELECT COUNT(*) FROM MODELE WHERE IDMODELE = :id");
    checkQuery.bindValue(":id",idModele);
    if (checkQuery.exec()&&checkQuery.next())
        if (checkQuery.value(0).toInt()==0) {
            msgWarn(this,"Erreur",QString("L'ID Modèle '%1' n'existe pas.").arg(idModele));
            ui->aff_id_modele->setFocus(); return false; }
    if (ui->aff_date_deb->date()<QDate::currentDate()) {
        msgWarn(this,"Date invalide","La date de début ne peut pas être antérieure à aujourd'hui.");
        ui->aff_date_deb->setFocus(); return false; }
    if (ui->aff_commentaire->toPlainText().trimmed().isEmpty()) {
        msgWarn(this,"Champ vide","Le champ Commentaire est obligatoire."); ui->aff_commentaire->setFocus(); return false; }
    return true;
}

void MainWindow::clearFabricationFields()
{
    generateFabricationId(); ui->aff_id_modele->clear();
    ui->aff_date_deb->setDate(QDate::currentDate());
    ui->aff_quantite->setValue(1); ui->aff_qualite->setCurrentIndex(0); ui->aff_commentaire->clear();
}

void MainWindow::on_btn_ajouter_2_clicked()
{
    if (!validateFabricationFields()) return;
    QSqlQuery query(db());
    query.prepare(
        "INSERT INTO FABRICATION (IDFABRICATION, IDMODELE, DATE_DEBUT, "
        "QUANTITE_A_PRODUIRE, QUALITE, COMMENTAIRE) VALUES (:id,:idm,:dat,:qty,:qal,:com)");
    query.bindValue(":id", ui->aff_id_fab->text().toInt());
    query.bindValue(":idm",ui->aff_id_modele->text().trimmed().toInt());
    query.bindValue(":dat",ui->aff_date_deb->date());
    query.bindValue(":qty",ui->aff_quantite->value());
    query.bindValue(":qal",ui->aff_qualite->currentText());
    query.bindValue(":com",ui->aff_commentaire->toPlainText().trimmed());
    if (query.exec()) { msgInfo(this,"Succès","Fabrication ajoutée !"); loadFabrications(); clearFabricationFields(); }
    else msgErreur(this,"Erreur","Impossible d'ajouter:\n"+query.lastError().text());
}

void MainWindow::on_btn_modifier_clicked()
{
    if (ui->aff_id_fab->text().isEmpty()) { msgWarn(this,"Erreur","Sélectionnez une fabrication."); return; }
    if (!validateFabricationFields()) return;
    QSqlQuery query(db());
    query.prepare(
        "UPDATE FABRICATION SET IDMODELE=:idm, DATE_DEBUT=:dat, "
        "QUANTITE_A_PRODUIRE=:qty, QUALITE=:qal, COMMENTAIRE=:com WHERE IDFABRICATION=:id");
    query.bindValue(":id", ui->aff_id_fab->text().toInt());
    query.bindValue(":idm",ui->aff_id_modele->text().trimmed().toInt());
    query.bindValue(":dat",ui->aff_date_deb->date());
    query.bindValue(":qty",ui->aff_quantite->value());
    query.bindValue(":qal",ui->aff_qualite->currentText());
    query.bindValue(":com",ui->aff_commentaire->toPlainText().trimmed());
    if (query.exec()) { msgInfo(this,"Succès","Fabrication modifiée !"); loadFabrications(); clearFabricationFields(); }
    else msgErreur(this,"Erreur","Impossible de modifier:\n"+query.lastError().text());
}

void MainWindow::on_btn_supprimer_clicked()
{
    if (ui->aff_id_fab->text().isEmpty()) { msgWarn(this,"Erreur","Sélectionnez une fabrication."); return; }
    if (msgQuestion(this,"Confirmation","Êtes-vous sûr de vouloir supprimer cette fabrication ?")!=QMessageBox::Yes) return;
    QSqlQuery query(db());
    query.prepare("DELETE FROM FABRICATION WHERE IDFABRICATION=:id");
    query.bindValue(":id",ui->aff_id_fab->text().toInt());
    if (query.exec()) { msgInfo(this,"Succès","Fabrication supprimée !"); loadFabrications(); clearFabricationFields(); }
    else msgErreur(this,"Erreur","Impossible de supprimer:\n"+query.lastError().text());
}

void MainWindow::on_rech_7_textChanged(const QString &text) { filterFabrications(text); }

void MainWindow::on_table_modeles_2_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    if (row<0||row>=ui->table_modeles_2->rowCount()) return;
    auto get=[&](int col)->QTableWidgetItem*{ return ui->table_modeles_2->item(row,col); };
    if (get(0)) ui->aff_id_fab->setText(get(0)->text());
    if (get(1)) ui->aff_id_modele->setText(get(1)->text());
    if (get(2)) { QDate d=QDate::fromString(get(2)->text(),"dd/MM/yyyy"); if(d.isValid()) ui->aff_date_deb->setDate(d); }
    if (get(3)) ui->aff_quantite->setValue(get(3)->text().toInt());
    if (get(4)) { int idx=ui->aff_qualite->findText(get(4)->text()); if(idx>=0) ui->aff_qualite->setCurrentIndex(idx); }
    if (get(5)) ui->aff_commentaire->setText(get(5)->text());
}

// ═══════════════════════════════════════════════════════════════════════
//  MESSAGE BOXES ANIMÉES
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::showAnimatedMessageBox(QMessageBox::Icon icon,const QString &title,
                                        const QString &text,const QString &informativeText)
{
    QMessageBox msgBox; msgBox.setIcon(icon); msgBox.setWindowTitle(title); msgBox.setText(text);
    if (!informativeText.isEmpty()) msgBox.setInformativeText(informativeText);
    msgBox.setStyleSheet(STYLE_MSG);
    QGraphicsOpacityEffect *effect=new QGraphicsOpacityEffect(); msgBox.setGraphicsEffect(effect);
    QPropertyAnimation *anim=new QPropertyAnimation(effect,"opacity");
    anim->setDuration(300);anim->setStartValue(0.0);anim->setEndValue(1.0);anim->setEasingCurve(QEasingCurve::OutCubic);
    QTimer::singleShot(0,[&msgBox,anim](){
        anim->start();
        QPropertyAnimation *popAnim=new QPropertyAnimation(&msgBox,"geometry");
        popAnim->setDuration(200);popAnim->setEasingCurve(QEasingCurve::OutElastic);
        QRect orig=msgBox.geometry();
        popAnim->setStartValue(QRect(orig.x()+50,orig.y()+30,orig.width()-100,orig.height()-60));
        popAnim->setEndValue(orig);popAnim->start();
    });
    msgBox.exec(); delete anim;
}

bool MainWindow::showAnimatedQuestionBox(const QString &title, const QString &text)
{
    QMessageBox msgBox; msgBox.setIcon(QMessageBox::Question); msgBox.setWindowTitle(title); msgBox.setText(text);
    msgBox.setStandardButtons(QMessageBox::Yes|QMessageBox::No); msgBox.setDefaultButton(QMessageBox::No);
    msgBox.setStyleSheet(STYLE_MSG);
    QGraphicsOpacityEffect *effect=new QGraphicsOpacityEffect(); msgBox.setGraphicsEffect(effect);
    QPropertyAnimation *anim=new QPropertyAnimation(effect,"opacity");
    anim->setDuration(300);anim->setStartValue(0.0);anim->setEndValue(1.0);anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start();
    auto reply=static_cast<QMessageBox::StandardButton>(msgBox.exec());
    delete anim; return reply==QMessageBox::Yes;
}
void MainWindow::on_btn_personnel_clicked()       { ui->stackedWidget->setCurrentWidget(ui->page_5); }
void MainWindow::on_btn_modele_clicked()          { ui->stackedWidget->setCurrentWidget(ui->page_6); afficherStatistiques(); }
void MainWindow::on_btn_bois_clicked()            { ui->stackedWidget->setCurrentWidget(ui->page); }
void MainWindow::on_btn_etape_clicked()           { ui->stackedWidget->setCurrentWidget(ui->page_7); }
void MainWindow::on_btn_fabrication_clicked()     { ui->stackedWidget->setCurrentWidget(ui->page_4); }
void MainWindow::on_btn_rechercher_6_clicked()    { filterFabrications(ui->rech_7->text()); }
void MainWindow::on_btn_tire_2_clicked()          { sortFabrications(ui->cb_critere_recherche_modele_2->currentText()); }
void MainWindow::on_btn_export_pdf_modele_2_clicked() { exportToPDF(); }
void MainWindow::afficherTendances()
{
    struct StatAnnee {
        double couleur, forme, materiau, style;
    };
    QMap<int, StatAnnee> statsTable = {
        { 2025, { 80, 75, 70, 88 } },
        { 2026, { 85, 80, 88, 82 } },
        { 2027, { 75, 90, 82, 94 } }
    };
    QMap<int, StatAnnee> statsChaise = {
        { 2025, { 78, 72, 68, 85 } },
        { 2026, { 83, 78, 86, 80 } }
    };
    struct ChaiseCandidate {
        QString nom;
        double couleur, forme, materiau, style;
    };
    QList<ChaiseCandidate> candidates2027 = {
        { "Chaise Medallion", 73, 88, 80, 92 },
        { "Chaise Neo-Baroque", 70, 85, 78, 90 },
        { "Chaise Classique", 68, 82, 75, 88 },
        { "Chaise Artisanale", 65, 80, 85, 86 },
        { "Chaise Sculpte", 72, 87, 79, 91 }
    };
    struct InfoAnnee {
        QString tableStyle, tableCouleur, tableMateriau, tableForme;
        QString chaiseStyle, chaiseCouleur, chaiseMateriau, chaiseForme;
    };
    QMap<int, InfoAnnee> infos = {
        { 2025, {
                   "Minimaliste", "Blanc / Gris", "Chene clair", "Lignes droites",
                   "Scandinave", "Blanc / Gris", "Bois clair", "Dossier bas epure"
               }},
        { 2026, {
                   "Organique", "Terracotta / Ocre", "Rotin / Bambou", "Formes arrondies",
                   "Organique", "Terracotta", "Rotin", "Dossier arrondi"
               }},
        { 2027, {
                   "Neo-classique", "Bordeaux / Creme", "Bois sculpte", "Pieds tournes",
                   "", "", "", ""
               }}
    };
    QStringList criteres = { "Couleur", "Forme", "Materiau", "Style" };
    int W = ui->placeholder_tendances->width();
    int H = ui->placeholder_tendances->height();
    if (W < 100 || H < 100) { W = 1340; H = 980; }
    QPixmap pix(W, H);
    pix.fill(QColor("#EEEAE0"));
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    const QColor cCard ("#FFFFFF");
    const QColor cText ("#1A1A2E");
    const QColor cSub ("#4A4A6A");
    const QColor cTable ("#E05A00");
    const QColor cChaise ("#1565C0");
    const QColor cGrille ("#D0CCBF");
    const QColor cGold ("#F0A500");
    const QColor cSilver ("#9E9E9E");
    const QColor cBronze ("#A0522D");
    const QList<QColor> anneeColors = {
        QColor("#1565C0"),
        QColor("#00796B"),
        QColor("#6A1B9A")
    };
    int pad = 16;
    int colW = (W - 2*pad - 3*12) / 4;
    // ── Titre ─────────────────────────────────────────────────────────────
    {
        QLinearGradient grad(pad, pad, W-pad, pad+54);
        grad.setColorAt(0.0, QColor("#1A1A2E"));
        grad.setColorAt(0.5, QColor("#16213E"));
        grad.setColorAt(1.0, QColor("#0F3460"));
        QPainterPath tp;
        tp.addRoundedRect(QRectF(pad, pad, W-2*pad, 54), 14, 14);
        p.fillPath(tp, grad);
        QLinearGradient lineGrad(pad, pad+52, W-pad, pad+52);
        lineGrad.setColorAt(0.0, QColor("#E05A00"));
        lineGrad.setColorAt(0.5, QColor("#F0A500"));
        lineGrad.setColorAt(1.0, QColor("#1565C0"));
        p.setPen(QPen(QBrush(lineGrad), 3));
        p.drawLine(pad+20, pad+52, W-pad-20, pad+52);
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", 14, QFont::Bold));
        p.drawText(QRect(pad, pad, W-2*pad, 50), Qt::AlignCenter,
                   "Tendances Mobilier 2025 · 2026 · 2027 | Chaise deduite de la Table");
    }
    // ── Legende ───────────────────────────────────────────────────────────
    int legY = pad + 54 + 10;
    {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(255,255,255,180));
        p.drawRoundedRect(pad+6, legY-2, 400, 24, 8, 8);
        p.setBrush(cTable);
        p.drawRoundedRect(pad+14, legY+4, 22, 14, 3, 3);
        p.setPen(cText); p.setFont(QFont("Arial", 9, QFont::Bold));
        p.drawText(pad+42, legY+15, "Table tendance");
        QPen dp(cChaise, 2.5, Qt::DashLine);
        p.setPen(dp);
        p.drawLine(pad+180, legY+11, pad+208, legY+11);
        p.setPen(Qt::NoPen); p.setBrush(cChaise);
        p.drawEllipse(QPoint(pad+194, legY+11), 4, 4);
        p.setBrush(Qt::white);
        p.drawEllipse(QPoint(pad+194, legY+11), 2, 2);
        p.setPen(cText); p.setFont(QFont("Arial", 9, QFont::Bold));
        p.drawText(pad+216, legY+15, "Chaise deduite");
    }
    int topY = legY + 30;
    int graphH = H - topY - pad;
    // ── Fonction carte ────────────────────────────────────────────────────
    auto drawCarte = [&](int cx, int cy, int cw, int ch,
                         int annee,
                         QVector<double> tVals,
                         QVector<double> cVals,
                         bool showChaise,
                         const QString &sousTitre)
    {
        int ai = annee==2025?0:annee==2026?1:2;
        QColor ac = anneeColors[ai];
        for (int s = 6; s >= 1; s--) {
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(0,0,0,4*s));
            p.drawRoundedRect(cx+s, cy+s, cw, ch, 14, 14);
        }
        p.setBrush(cCard); p.setPen(Qt::NoPen);
        p.drawRoundedRect(cx, cy, cw, ch, 14, 14);
        p.setBrush(ac);
        p.drawRoundedRect(cx, cy+44, 5, ch-58, 3, 3);
        QLinearGradient hGrad(cx, cy, cx, cy+44);
        hGrad.setColorAt(0, ac);
        hGrad.setColorAt(1, ac.darker(130));
        QPainterPath hdr;
        hdr.addRoundedRect(QRectF(cx, cy, cw, 44), 14, 14);
        p.fillPath(hdr, hGrad);
        p.fillRect(cx, cy+30, cw, 14, ac.darker(130));
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", 12, QFont::Bold));
        p.drawText(QRect(cx, cy, cw, 26), Qt::AlignCenter,
                   QString("Tendance %1").arg(annee));
        p.setFont(QFont("Arial", 8));
        p.setPen(QColor(255,255,255,200));
        p.drawText(QRect(cx, cy+26, cw, 18), Qt::AlignCenter, sousTitre);
        int iy = cy + 50;
        int innerH = ch - 54;
        int infoH = showChaise ? 168 : 90;
        int restH = innerH - infoH - 10;
        int barH = restH * 46 / 100;
        int curvH = restH - barH - 6;
        // ── Bloc TABLE ────────────────────────────────────────────────────
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(cTable.red(), cTable.green(), cTable.blue(), 15));
        p.drawRoundedRect(cx+8, iy, cw-16, 84, 8, 8);
        QLinearGradient tBadge(cx+10, iy+6, cx+62, iy+24);
        tBadge.setColorAt(0, cTable);
        tBadge.setColorAt(1, cTable.lighter(140));
        p.setBrush(tBadge); p.setPen(Qt::NoPen);
        p.drawRoundedRect(cx+10, iy+6, 52, 18, 5, 5);
        p.setPen(Qt::white); p.setFont(QFont("Arial", 8, QFont::Bold));
        p.drawText(QRect(cx+10, iy+6, 52, 18), Qt::AlignCenter, "TABLE");
        const InfoAnnee &info = infos[annee];
        p.setPen(cText); p.setFont(QFont("Arial", 8, QFont::Bold));
        p.drawText(cx+68, iy+18, info.tableStyle);
        p.setPen(cSub); p.setFont(QFont("Arial", 8));
        p.drawText(cx+10, iy+32, QString("Couleur : %1").arg(info.tableCouleur));
        p.drawText(cx+10, iy+45, QString("Materiau : %1").arg(info.tableMateriau));
        p.drawText(cx+10, iy+58, QString("Forme : %1").arg(info.tableForme));
        p.drawText(cx+10, iy+71, QString("Style : %1").arg(info.tableStyle));
        iy += 90;
        // ── Bloc CHAISE ───────────────────────────────────────────────────
        if (showChaise && !info.chaiseStyle.isEmpty()) {
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(cChaise.red(), cChaise.green(), cChaise.blue(), 12));
            p.drawRoundedRect(cx+8, iy, cw-16, 84, 8, 8);
            QLinearGradient cBadge(cx+10, iy+6, cx+62, iy+24);
            cBadge.setColorAt(0, cChaise);
            cBadge.setColorAt(1, cChaise.lighter(140));
            p.setBrush(cBadge); p.setPen(Qt::NoPen);
            p.drawRoundedRect(cx+10, iy+6, 52, 18, 5, 5);
            p.setPen(Qt::white); p.setFont(QFont("Arial", 8, QFont::Bold));
            p.drawText(QRect(cx+10, iy+6, 52, 18), Qt::AlignCenter, "CHAISE");
            p.setPen(cText); p.setFont(QFont("Arial", 8, QFont::Bold));
            p.drawText(cx+68, iy+18, info.chaiseStyle);
            p.setPen(cSub); p.setFont(QFont("Arial", 8));
            p.drawText(cx+10, iy+32, QString("Couleur : %1").arg(info.chaiseCouleur));
            p.drawText(cx+10, iy+45, QString("Materiau : %1").arg(info.chaiseMateriau));
            p.drawText(cx+10, iy+58, QString("Forme : %1").arg(info.chaiseForme));
            p.drawText(cx+10, iy+71, QString("Style : %1").arg(info.chaiseStyle));
            iy += 90;
        }
        // ── Barres ────────────────────────────────────────────────────────
        int nC = 4;
        int gW = (cw - 18) / nC;
        double maxV = 100.0;
        p.setPen(Qt::NoPen); p.setBrush(QColor(248,246,242));
        p.drawRoundedRect(cx+6, iy, cw-12, barH, 6, 6);
        p.setPen(QPen(cGrille, 0.6, Qt::DashLine));
        for (int g = 1; g <= 4; g++)
            p.drawLine(cx+10, iy+4+(barH-26)*g/4,
                       cx+cw-10, iy+4+(barH-26)*g/4);
        QVector<int> ptX;
        for (int i = 0; i < nC; i++) {
            int gx = cx + 10 + i*gW;
            ptX << gx + gW/2;
            int usable = barH - 28;
            int hT = qMax(4, (int)(tVals[i]/maxV*usable));
            QLinearGradient tg(gx+3, iy+4+usable-hT, gx+3, iy+4+usable);
            tg.setColorAt(0, cTable.lighter(120));
            tg.setColorAt(1, cTable);
            p.setBrush(tg); p.setPen(Qt::NoPen);
            int bw = showChaise ? gW/2-3 : gW-8;
            p.drawRoundedRect(gx+3, iy+4+usable-hT, bw, hT, 3, 3);
            p.setBrush(QColor(255,255,255,210)); p.setPen(Qt::NoPen);
            p.drawRoundedRect(gx+3, iy+4+usable-hT-16, 26, 14, 4, 4);
            p.setPen(cTable.darker(150));
            p.setFont(QFont("Arial", 8, QFont::Bold));
            p.drawText(QRect(gx+3, iy+4+usable-hT-16, 26, 14),
                       Qt::AlignCenter,
                       QString::number((int)tVals[i])+"%");
            if (showChaise && !cVals.isEmpty()) {
                int hC = qMax(4, (int)(cVals[i]/maxV*usable));
                QLinearGradient cg(gx+gW/2, iy+4+usable-hC, gx+gW/2, iy+4+usable);
                cg.setColorAt(0, cChaise.lighter(130));
                cg.setColorAt(1, cChaise);
                p.setBrush(cg); p.setPen(Qt::NoPen);
                p.drawRoundedRect(gx+gW/2, iy+4+usable-hC, gW/2-3, hC, 3, 3);
                p.setBrush(QColor(255,255,255,210)); p.setPen(Qt::NoPen);
                p.drawRoundedRect(gx+gW/2, iy+4+usable-hC-16, 26, 14, 4, 4);
                p.setPen(cChaise.darker(140));
                p.setFont(QFont("Arial", 8, QFont::Bold));
                p.drawText(QRect(gx+gW/2, iy+4+usable-hC-16, 26, 14),
                           Qt::AlignCenter,
                           QString::number((int)cVals[i])+"%");
            }
            p.setPen(cSub); p.setFont(QFont("Arial", 8, QFont::Bold));
            p.drawText(QRect(gx, iy+barH-20, gW-2, 18),
                       Qt::AlignCenter, criteres[i]);
        }
        iy += barH + 6;
        // ── Courbe ────────────────────────────────────────────────────────
        p.setPen(Qt::NoPen); p.setBrush(QColor(248,246,242));
        p.drawRoundedRect(cx+6, iy, cw-12, curvH, 6, 6);
        p.setPen(QPen(cGrille, 0.6, Qt::DashLine));
        for (int g = 0; g <= 4; g++)
            p.drawLine(cx+10, iy+4+g*(curvH-8)/4,
                       cx+cw-10, iy+4+g*(curvH-8)/4);
        for (int g = 0; g <= 4; g++) {
            p.setPen(cSub.lighter(150));
            p.setFont(QFont("Arial", 7));
            p.drawText(cx+cw-28, iy+4+(4-g)*(curvH-8)/4+5,
                       QString::number(g*25)+"%");
        }
        int ch0 = iy + 4;
        int chH = curvH - 8;
        // Courbe TABLE
        {
            QVector<int> pyT;
            for (int i = 0; i < nC; i++)
                pyT << ch0 + (int)((1.0-tVals[i]/maxV)*chH);
            QPainterPath aire;
            aire.moveTo(ptX[0], ch0+chH);
            aire.lineTo(ptX[0], pyT[0]);
            for (int i = 0; i < nC-1; i++)
                aire.cubicTo((ptX[i]+ptX[i+1])/2, pyT[i],
                             (ptX[i]+ptX[i+1])/2, pyT[i+1],
                             ptX[i+1], pyT[i+1]);
            aire.lineTo(ptX[nC-1], ch0+chH);
            aire.closeSubpath();
            QLinearGradient ag(0, ch0, 0, ch0+chH);
            ag.setColorAt(0, QColor(cTable.red(),cTable.green(),cTable.blue(),60));
            ag.setColorAt(1, QColor(cTable.red(),cTable.green(),cTable.blue(),5));
            p.fillPath(aire, ag);
            QPen lp(cTable, 2.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
            for (int i = 0; i < nC-1; i++) {
                QPainterPath path;
                path.moveTo(ptX[i], pyT[i]);
                path.cubicTo((ptX[i]+ptX[i+1])/2, pyT[i],
                             (ptX[i]+ptX[i+1])/2, pyT[i+1],
                             ptX[i+1], pyT[i+1]);
                p.strokePath(path, lp);
            }
            for (int i = 0; i < nC; i++) {
                p.setPen(Qt::NoPen); p.setBrush(cTable);
                p.drawEllipse(QPoint(ptX[i],pyT[i]),6,6);
                p.setBrush(Qt::white);
                p.drawEllipse(QPoint(ptX[i],pyT[i]),3,3);
                p.setBrush(QColor(255,255,255,210)); p.setPen(Qt::NoPen);
                p.drawRoundedRect(ptX[i]-14, pyT[i]-18, 28, 14, 4, 4);
                p.setPen(cTable.darker(140));
                p.setFont(QFont("Arial", 8, QFont::Bold));
                p.drawText(QRect(ptX[i]-14, pyT[i]-18, 28, 14),
                           Qt::AlignCenter,
                           QString::number((int)tVals[i])+"%");
            }
        }
        // Courbe CHAISE
        if (showChaise && !cVals.isEmpty()) {
            QVector<int> pyC;
            for (int i = 0; i < nC; i++)
                pyC << ch0 + (int)((1.0-cVals[i]/maxV)*chH);
            QPen lp(cChaise, 2.5, Qt::DashLine, Qt::RoundCap, Qt::RoundJoin);
            for (int i = 0; i < nC-1; i++) {
                QPainterPath path;
                path.moveTo(ptX[i], pyC[i]);
                path.cubicTo((ptX[i]+ptX[i+1])/2, pyC[i],
                             (ptX[i]+ptX[i+1])/2, pyC[i+1],
                             ptX[i+1], pyC[i+1]);
                p.strokePath(path, lp);
            }
            for (int i = 0; i < nC; i++) {
                p.setPen(Qt::NoPen); p.setBrush(cChaise);
                p.drawEllipse(QPoint(ptX[i],pyC[i]),6,6);
                p.setBrush(Qt::white);
                p.drawEllipse(QPoint(ptX[i],pyC[i]),3,3);
                p.setBrush(QColor(255,255,255,210)); p.setPen(Qt::NoPen);
                p.drawRoundedRect(ptX[i]-14, pyC[i]+6, 28, 14, 4, 4);
                p.setPen(cChaise.darker(140));
                p.setFont(QFont("Arial", 8, QFont::Bold));
                p.drawText(QRect(ptX[i]-14, pyC[i]+6, 28, 14),
                           Qt::AlignCenter,
                           QString::number((int)cVals[i])+"%");
            }
        }
    };
    // ── Dessiner 3 cartes ─────────────────────────────────────────────────
    QVector<double> t25={statsTable[2025].couleur,statsTable[2025].forme,
                           statsTable[2025].materiau,statsTable[2025].style};
    QVector<double> c25={statsChaise[2025].couleur,statsChaise[2025].forme,
                           statsChaise[2025].materiau,statsChaise[2025].style};
    QVector<double> t26={statsTable[2026].couleur,statsTable[2026].forme,
                           statsTable[2026].materiau,statsTable[2026].style};
    QVector<double> c26={statsChaise[2026].couleur,statsChaise[2026].forme,
                           statsChaise[2026].materiau,statsChaise[2026].style};
    QVector<double> t27={statsTable[2027].couleur,statsTable[2027].forme,
                           statsTable[2027].materiau,statsTable[2027].style};
    drawCarte(pad+0*(colW+12), topY, colW, graphH,
              2025, t25, c25, true, "Table + Chaise deduite");
    drawCarte(pad+1*(colW+12), topY, colW, graphH,
              2026, t26, c26, true, "Table + Chaise deduite");
    drawCarte(pad+2*(colW+12), topY, colW, graphH,
              2027, t27, {}, false, "Tendance Table uniquement");
    // ── 4ème carte ────────────────────────────────────────────────────────
    {
        int cx = pad + 3*(colW+12);
        int cy = topY, cw = colW, ch = graphH;
        auto distFn = [&](const ChaiseCandidate &c) {
            return qSqrt(
                qPow(c.couleur - statsTable[2027].couleur, 2) +
                qPow(c.forme - statsTable[2027].forme, 2) +
                qPow(c.materiau - statsTable[2027].materiau, 2) +
                qPow(c.style - statsTable[2027].style, 2));
        };
        auto scoreFn = [&](const ChaiseCandidate &c) {
            return qMax(0.0, 100.0-(distFn(c)/qSqrt(4.0*100*100)*100.0));
        };
        QList<ChaiseCandidate> sorted = candidates2027;
        std::sort(sorted.begin(), sorted.end(),
                  [&](const ChaiseCandidate &a, const ChaiseCandidate &b){
                      return distFn(a) < distFn(b);
                  });
        for (int s = 6; s >= 1; s--) {
            p.setPen(Qt::NoPen);
            p.setBrush(QColor(0,0,0,4*s));
            p.drawRoundedRect(cx+s, cy+s, cw, ch, 14, 14);
        }
        p.setBrush(cCard); p.setPen(Qt::NoPen);
        p.drawRoundedRect(cx, cy, cw, ch, 14, 14);
        p.setBrush(anneeColors[2]);
        p.drawRoundedRect(cx, cy+44, 5, ch-58, 3, 3);
        QLinearGradient hg(cx, cy, cx, cy+44);
        hg.setColorAt(0, QColor("#6A1B9A"));
        hg.setColorAt(1, QColor("#4A148C"));
        QPainterPath hdr;
        hdr.addRoundedRect(QRectF(cx, cy, cw, 44), 14, 14);
        p.fillPath(hdr, hg);
        p.fillRect(cx, cy+30, cw, 14, QColor("#4A148C"));
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", 11, QFont::Bold));
        p.drawText(QRect(cx, cy, cw, 26), Qt::AlignCenter,
                   "Chaise la plus proche");
        p.setFont(QFont("Arial", 8));
        p.setPen(QColor(255,255,255,200));
        p.drawText(QRect(cx, cy+26, cw, 18), Qt::AlignCenter,
                   "pour la Table 2027");
        int iy = cy + 52;
        // ── Référence TABLE 2027 ──────────────────────────────────────────
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(cTable.red(),cTable.green(),cTable.blue(),12));
        p.drawRoundedRect(cx+8, iy, cw-16, 92, 8, 8);
        QLinearGradient tBadge(cx+10, iy+6, cx+80, iy+24);
        tBadge.setColorAt(0, cTable);
        tBadge.setColorAt(1, cTable.lighter(140));
        p.setBrush(tBadge); p.setPen(Qt::NoPen);
        p.drawRoundedRect(cx+10, iy+6, 70, 18, 5, 5);
        p.setPen(Qt::white); p.setFont(QFont("Arial", 8, QFont::Bold));
        p.drawText(QRect(cx+10, iy+6, 70, 18),
                   Qt::AlignCenter, "TABLE 2027");
        p.setPen(cText); p.setFont(QFont("Arial", 8, QFont::Bold));
        p.drawText(cx+86, iy+18, infos[2027].tableStyle);
        p.setPen(cSub); p.setFont(QFont("Arial", 8));
        p.drawText(cx+12, iy+32, "Couleur : "+infos[2027].tableCouleur);
        p.drawText(cx+12, iy+45, "Materiau : "+infos[2027].tableMateriau);
        p.drawText(cx+12, iy+58, "Forme : "+infos[2027].tableForme);
        p.drawText(cx+12, iy+71, "Style : "+infos[2027].tableStyle);
        iy += 100;
        // ── TOP 3 chaises ─────────────────────────────────────────────────
        QList<QColor> rankBg = {
            QColor("#FFF8E1"), QColor("#F5F5F5"), QColor("#FBE9E7")
    };
    QList<QColor> rankAc = { cGold, cSilver, cBronze };
    QStringList rankLb = { "1", "2", "3" };
    QStringList miniLabels = { "Coul.", "Form.", "Mat.", "Styl." };
    for (int i = 0; i < qMin(3, sorted.size()); i++) {
        const ChaiseCandidate &c = sorted[i];
        double sc = scoreFn(c);
        // Fond bloc
        p.setPen(Qt::NoPen); p.setBrush(rankBg[i]);
       p.drawRoundedRect(cx+8, iy, cw-16, 130, 8, 8);
        // Bordure rang
        p.setBrush(rankAc[i]);
        p.drawRoundedRect(cx+8, iy+4, 4, 106, 2, 2);
        // Badge cercle
        QRadialGradient rg(cx+28, iy+16, 14);
        rg.setColorAt(0, rankAc[i].lighter(140));
        rg.setColorAt(1, rankAc[i]);
        p.setBrush(rg); p.setPen(Qt::NoPen);
        p.drawEllipse(cx+16, iy+6, 26, 26);
        p.setPen(Qt::white); p.setFont(QFont("Arial", 11, QFont::Bold));
        p.drawText(QRect(cx+16, iy+6, 26, 26),
                   Qt::AlignCenter, rankLb[i]);
        // Nom chaise
        p.setPen(cText); p.setFont(QFont("Arial", 10, QFont::Bold));
        p.drawText(cx+48, iy+22, c.nom);
        // Stats 4 critères chacun sur sa ligne
        p.setPen(cSub); p.setFont(QFont("Arial", 8));
        p.drawText(cx+14, iy+36,
                   QString("Couleur : %1%").arg((int)c.couleur));
        p.drawText(cx+14, iy+48,
                   QString("Forme : %1%").arg((int)c.forme));
        p.drawText(cx+14, iy+60,
                   QString("Materiau : %1%").arg((int)c.materiau));
        p.drawText(cx+14, iy+72,
                   QString("Style : %1%").arg((int)c.style));
        // Mini barres table (orange) vs chaise (bleu) avec labels
        QVector<double> tv = {
            statsTable[2027].couleur, statsTable[2027].forme,
            statsTable[2027].materiau, statsTable[2027].style
        };
        QVector<double> cv = {
            c.couleur, c.forme, c.materiau, c.style
        };
        int mw = (cw - 28) / 4;
        for (int j = 0; j < 4; j++) {
            int bx = cx+12 + j*(mw+2);
            int by = iy+80;
            // Barre table
            p.setPen(Qt::NoPen); p.setBrush(cTable.lighter(150));
            p.drawRoundedRect(bx, by,
                              (int)(tv[j]/100.0*(mw-2)), 5, 2, 2);
            // Barre chaise
            p.setBrush(cChaise.lighter(150));
            p.drawRoundedRect(bx, by+7,
                              (int)(cv[j]/100.0*(mw-2)), 5, 2, 2);
            // Label critère
            p.setPen(cSub); p.setFont(QFont("Arial", 7, QFont::Bold));
            p.drawText(QRect(bx, by+14, mw, 12),
                       Qt::AlignCenter, miniLabels[j]);
        }
        // Barre score proximité
        int bx=cx+12, by=iy+98, bww=cw-26, bhh=12;
        p.setPen(Qt::NoPen); p.setBrush(QColor(220,215,230));
        p.drawRoundedRect(bx, by, bww, bhh, 5, 5);
        QLinearGradient sg(bx, by, bx+bww, by);
        sg.setColorAt(0, sc>=90?QColor("#00C853"):
                             sc>=75?QColor("#1565C0"):QColor("#E65100"));
        sg.setColorAt(1, sc>=90?QColor("#69F0AE"):
                             sc>=75?QColor("#42A5F5"):QColor("#FF8A65"));
        p.setBrush(sg);
        p.drawRoundedRect(bx, by, (int)(sc/100.0*bww), bhh, 5, 5);
        p.setPen(Qt::white); p.setFont(QFont("Arial", 8, QFont::Bold));
        p.drawText(QRect(bx, by, (int)(sc/100.0*bww), bhh),
                   Qt::AlignCenter,
                   QString("Proximite : %1%").arg((int)sc));
        iy += 80;
    }
}
p.end();
QLabel *lbl = ui->placeholder_tendances->findChild<QLabel*>("lbl_tendance");
if (!lbl) {
    lbl = new QLabel(ui->placeholder_tendances);
    lbl->setObjectName("lbl_tendance");
    lbl->setAlignment(Qt::AlignCenter);
}
lbl->setGeometry(0, 0, W, H);
lbl->setPixmap(pix);
lbl->show();
}
// ═══════════════════════════════════════════════════════════════════════
// ÉTAPE 1 : Dans le constructeur, remplace le connect tabWidget par :
// ═══════════════════════════════════════════════════════════════════════
//
// connect(ui->tabWidget_modeles, &QTabWidget::currentChanged, this, [=](int idx) {
// if (idx == 1) afficherStatistiques();
// if (idx == 2) afficherTendances();
// if (idx == 3) afficherAnalyseCouts();
// });
//
// ═══════════════════════════════════════════════════════════════════════
// ÉTAPE 2 : Dans mainwindow.h, ajouter dans private: :
// void afficherAnalyseCouts();
// ═══════════════════════════════════════════════════════════════════════
// ÉTAPE 3 : Coller cette fonction à la fin de mainwindow.cpp
// ═══════════════════════════════════════════════════════════════════════
void MainWindow::afficherAnalyseCouts()
{
    // ── Prix fixes par étape (DT) ─────────────────────────────────────────
    QMap<QString, double> prixEtapes = {
                                        {"Decoupe du bois", 25.0},
                                        {"Sechage du bois", 15.0},
                                        {"Poncage", 20.0},
                                        {"Assemblage Collage", 30.0},
                                        {"Clouage Vissage", 20.0},
                                        {"Mise en forme", 35.0},
                                        {"Controle qualite", 15.0},
                                        {"Vernissage", 25.0},
                                        {"Peinture Finition", 30.0},
                                        {"Emballage", 10.0},
                                        {"Soudure Fixation", 25.0},
                                        {"Traitement surface", 20.0},
                                        };
    // ── Structures ────────────────────────────────────────────────────────
    struct EtapeInfo {
        QString nom;
        double prix;
    };
    struct ModeleInfo {
        int id;
        QString nom;
        QString type;
        double prixBois;
        double longueur, largeur, hauteur;
        double volume;
        double coutBois;
        int nbFab;
        int qty;
        QList<EtapeInfo> etapes;
        double coutEtapes;
        double coutTotal;
    };
    // ── Charger modèles depuis DB ─────────────────────────────────────────
    QList<ModeleInfo> modeles;
    {
        QSqlQuery q(db());
        bool ok = q.exec(
            "SELECT M.IDMODELE, M.NOM, M.TYPE, "
            " NVL(M.LONGUEUR,0), NVL(M.LARGEUR,0), NVL(M.HAUTEUR,0), "
            " NVL(B.PRIXUNITAIRE,0), "
            " COUNT(F.IDFABRICATION), "
            " NVL(SUM(F.QUANTITE_A_PRODUIRE),0) "
            "FROM MODELE M "
            "LEFT JOIN TYPEBOIS B ON B.IDBOIS = M.IDBOIS "
            "LEFT JOIN FABRICATION F ON F.IDMODELE = M.IDMODELE "
            "GROUP BY M.IDMODELE, M.NOM, M.TYPE, "
            " M.LONGUEUR, M.LARGEUR, M.HAUTEUR, B.PRIXUNITAIRE "
            "ORDER BY M.NOM");
        if (!ok) {
            qDebug() << "ERREUR SQL afficherAnalyseCouts:" << q.lastError().text();
            return;
        }
        while (q.next()) {
            ModeleInfo mc;
            mc.id = q.value(0).toInt();
            mc.nom = q.value(1).toString();
            mc.type = q.value(2).toString();
            mc.longueur = q.value(3).toDouble();
            mc.largeur = q.value(4).toDouble();
            mc.hauteur = q.value(5).toDouble();
            mc.prixBois = q.value(6).toDouble();
            mc.nbFab = q.value(7).toInt();
            mc.qty = q.value(8).toInt();
            // Volume en dm³
            mc.volume = (mc.longueur * mc.largeur * mc.hauteur) / 1000.0;
            mc.coutBois = mc.prixBois * mc.volume;
            // Étapes selon type
            QList<QString> nomsEtapes;
            QString t = mc.type.toLower();
            if (t.contains("chaise")) {
                nomsEtapes = {"Decoupe du bois", "Sechage du bois",
                              "Poncage", "Assemblage Collage",
                              "Clouage Vissage", "Vernissage",
                              "Controle qualite"};
            } else if (t.contains("table")) {
                nomsEtapes = {"Decoupe du bois", "Sechage du bois",
                              "Poncage", "Mise en forme",
                              "Assemblage Collage", "Peinture Finition",
                              "Controle qualite", "Emballage"};
            } else if (t.toLower().contains("armoire") ||
                       t.toLower().contains("placard")) {
                nomsEtapes = {"Decoupe du bois", "Sechage du bois",
                              "Poncage", "Mise en forme",
                              "Assemblage Collage", "Soudure Fixation",
                              "Traitement surface", "Vernissage",
                              "Controle qualite", "Emballage"};
            } else {
                nomsEtapes = {"Decoupe du bois", "Poncage",
                              "Assemblage Collage", "Vernissage",
                              "Controle qualite"};
            }
            mc.coutEtapes = 0;
            for (const QString &ne : nomsEtapes) {
                EtapeInfo ei;
                ei.nom = ne;
                ei.prix = prixEtapes.value(ne, 0.0);
                mc.coutEtapes += ei.prix;
                mc.etapes << ei;
            }
            mc.coutTotal = mc.coutBois + mc.coutEtapes;
            modeles << mc;
        }
    }
    qDebug() << "afficherAnalyseCouts: nb modeles=" << modeles.size();
    if (modeles.isEmpty()) {
        // Afficher message si aucun modèle
        QLabel *lbl = ui->placeholder_couts->findChild<QLabel*>("lbl_couts_empty");
        if (!lbl) {
            lbl = new QLabel(ui->placeholder_couts);
            lbl->setObjectName("lbl_couts_empty");
            lbl->setAlignment(Qt::AlignCenter);
            lbl->setFont(QFont("Arial", 12));
            lbl->setStyleSheet("color: #7F4129;");
        }
        lbl->setText("Aucun modele trouve dans la base de donnees.");
        lbl->setGeometry(0, 0,
                         ui->placeholder_couts->width(),
                         ui->placeholder_couts->height());
        lbl->show();
        return;
    }
    // ── Dimensions ────────────────────────────────────────────────────────
    const int pad = 14;
    const int titreH = 54;
    const int colW = 230;
    const int colGap = 10;
    const int rowH = 26;
    int maxEtapes = 0;
    for (const auto &mc : modeles)
        maxEtapes = qMax(maxEtapes, mc.etapes.size());
    const int headerH = 90;
    const int boisH = 52;
    const int etHdrH = 22;
    const int etapesH = maxEtapes * rowH;
    const int footH = 48;
    const int cardH = headerH + boisH + etHdrH + etapesH + footH;
    const int resumeH = 50;
    int W = 2*pad + modeles.size()*colW + (modeles.size()-1)*colGap;
    W = qMax(W, ui->placeholder_couts->width());
    int H = titreH + pad + cardH + pad + resumeH + pad;
    H = qMax(H, ui->placeholder_couts->height());
    QPixmap pix(W, H);
    pix.fill(QColor("#F8F4EF"));
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);
    // Couleurs
    const QColor cBrun (110, 55, 25);
    const QColor cBleu (55, 138, 221);
    const QColor cVert (29, 158, 117);
    const QColor cCard (255, 253, 250);
    const QColor cBord (200, 175, 150);
    const QColor cTexte (40, 20, 5);
    const QColor cGris (230, 220, 210);
    QList<QColor> typeColors = {
        QColor(55, 138, 221), // bleu
        QColor(29, 158, 117), // vert
        QColor(83, 74, 183), // violet
        QColor(239,159, 39), // orange
        QColor(216, 90, 48), // rouge
        QColor(110, 55, 25), // brun
    };
    // ── Bandeau titre ─────────────────────────────────────────────────────
    {
        QPainterPath tp;
        tp.addRoundedRect(QRectF(pad, pad, W-2*pad, titreH-6), 11, 11);
        p.fillPath(tp, cBrun);
        p.fillRect(pad, pad+30, W-2*pad, titreH-6-30, cBrun);
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", 13, QFont::Bold));
        p.drawText(QRect(pad, pad, W-2*pad, titreH-6),
                   Qt::AlignCenter,
                   "Analyse des Couts - Modeles / Etapes / Prix Bois");
    }
    int startY = titreH + pad;
    // ── Cartes modèles ────────────────────────────────────────────────────
    for (int mi = 0; mi < modeles.size(); mi++) {
        const auto &mc = modeles[mi];
        int cx = pad + mi*(colW+colGap);
        int cy = startY;
        QColor cCol = typeColors[mi % typeColors.size()];
        // Ombre
        p.setPen(Qt::NoPen); p.setBrush(QColor(0,0,0,20));
        p.drawRoundedRect(cx+3, cy+3, colW, cardH, 11, 11);
        // Fond carte
        p.setBrush(cCard); p.setPen(QPen(cBord, 0.8));
        p.drawRoundedRect(cx, cy, colW, cardH, 11, 11);
        // ── En-tête ───────────────────────────────────────────────────────
        QPainterPath hdr;
        hdr.addRoundedRect(QRectF(cx, cy, colW, headerH), 11, 11);
        p.fillPath(hdr, cCol);
        p.fillRect(cx, cy+22, colW, headerH-22, cCol);
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", 10, QFont::Bold));
        // Tronquer le nom si trop long
        QString nomAff = mc.nom.length() > 18 ? mc.nom.left(16)+"..." : mc.nom;
        p.drawText(cx+8, cy+20, nomAff);
        p.setFont(QFont("Arial", 7));
        p.drawText(cx+8, cy+35,
                   QString("Type: %1").arg(mc.type));
        p.drawText(cx+8, cy+49,
                   QString("%1 x %2 x %3 cm")
                       .arg(mc.longueur,0,'f',0)
                       .arg(mc.largeur,0,'f',0)
                       .arg(mc.hauteur,0,'f',0));
        p.drawText(cx+8, cy+63,
                   QString("Fabrications: %1 | Qty: %2")
                       .arg(mc.nbFab).arg(mc.qty));
        p.setFont(QFont("Arial", 7, QFont::Bold));
        p.drawText(cx+8, cy+78,
                   QString("Volume: %1 dm3").arg(mc.volume,0,'f',2));
        // ID coin haut droit
        p.setPen(QColor(255,255,255,180));
        p.setFont(QFont("Arial", 8, QFont::Bold));
        p.drawText(cx+colW-35, cy+18, QString("#%1").arg(mc.id));
        // ── Section Bois ──────────────────────────────────────────────────
        int boisY = cy + headerH;
        p.setPen(Qt::NoPen); p.setBrush(QColor("#DCEEFB"));
        p.drawRect(cx, boisY, colW, boisH);
        p.setPen(QPen(cBord, 0.5));
        p.drawLine(cx, boisY, cx+colW, boisY);
        p.setPen(cBleu.darker(140));
        p.setFont(QFont("Arial", 7, QFont::Bold));
        p.drawText(cx+8, boisY+14, "BOIS (prix DB)");
        p.setPen(QColor(60,80,120));
        p.setFont(QFont("Arial", 7));
        p.drawText(cx+8, boisY+28,
                   QString("Prix: %1 DT/dm3 Vol: %2 dm3")
                       .arg(mc.prixBois,0,'f',2)
                       .arg(mc.volume,0,'f',2));
        // Coût bois
        p.setPen(cBleu.darker(160));
        p.setFont(QFont("Arial", 8, QFont::Bold));
        p.drawText(cx+colW-75, boisY+28,
                   QString("%1 DT").arg(mc.coutBois,0,'f',2));
        // Mini barre bois
        p.setPen(Qt::NoPen); p.setBrush(QColor(180,210,240));
        p.drawRoundedRect(cx+8, boisY+35, colW-16, 8, 4, 4);
        double refBois = 300.0;
        int fillB = qMin((int)(mc.coutBois/refBois*(colW-16)), colW-16);
        fillB = qMax(fillB, 4);
        p.setBrush(cBleu);
        p.drawRoundedRect(cx+8, boisY+35, fillB, 8, 4, 4);
        // ── En-tête étapes ────────────────────────────────────────────────
        int etHdrY = boisY + boisH;
        p.setPen(Qt::NoPen); p.setBrush(QColor("#D5F0E8"));
        p.drawRect(cx, etHdrY, colW, etHdrH);
        p.setPen(QPen(cBord, 0.5));
        p.drawLine(cx, etHdrY, cx+colW, etHdrY);
        p.setPen(cVert.darker(140));
        p.setFont(QFont("Arial", 7, QFont::Bold));
        p.drawText(cx+8, etHdrY+15, "ETAPE");
        p.drawText(cx+colW-52, etHdrY+15, "PRIX DT");
        // ── Lignes étapes ─────────────────────────────────────────────────
        int etY = etHdrY + etHdrH;
        for (int ei = 0; ei < mc.etapes.size(); ei++) {
            const auto &etape = mc.etapes[ei];
            int ey = etY + ei*rowH;
            // Fond alterné
            p.setPen(Qt::NoPen);
            p.setBrush(ei%2==0 ? QColor(252,250,247) : QColor(242,237,230));
            p.drawRect(cx, ey, colW, rowH);
            p.setPen(QPen(cGris, 0.3));
            p.drawLine(cx, ey, cx+colW, ey);
            // Numéro cercle
            p.setPen(Qt::NoPen); p.setBrush(cCol);
            p.drawEllipse(cx+6, ey+5, 15, 15);
            p.setPen(Qt::white); p.setFont(QFont("Arial", 6, QFont::Bold));
            p.drawText(QRect(cx+6, ey+5, 15, 15),
                       Qt::AlignCenter, QString::number(ei+1));
            // Nom étape
            p.setPen(cTexte); p.setFont(QFont("Arial", 7));
            p.drawText(cx+26, ey+17, etape.nom);
            // Prix
            p.setPen(cVert.darker(150));
            p.setFont(QFont("Arial", 7, QFont::Bold));
            p.drawText(cx+colW-52, ey+17,
                       QString("%1").arg(etape.prix,0,'f',0));
            // Mini barre
            p.setPen(Qt::NoPen); p.setBrush(QColor(195,230,215));
            p.drawRoundedRect(cx+colW-52, ey+19, 44, 4, 2, 2);
            int fillE = (int)(etape.prix/40.0*44);
            p.setBrush(cVert);
            p.drawRoundedRect(cx+colW-52, ey+19, fillE, 4, 2, 2);
        }
        // ── Pied carte : Total ────────────────────────────────────────────
        int footY = etY + mc.etapes.size()*rowH;
        // Remplir jusqu'au bas si moins d'étapes que maxEtapes
        int lignesRestantes = maxEtapes - mc.etapes.size();
        if (lignesRestantes > 0) {
            p.setPen(Qt::NoPen); p.setBrush(QColor(248,245,240));
            p.drawRect(cx, footY, colW, lignesRestantes*rowH);
            footY += lignesRestantes*rowH;
        }
        // Fond pied
        p.setPen(Qt::NoPen); p.setBrush(cCol.darker(135));
        p.drawRect(cx, footY, colW, footH);
        // Arrondi bas
        QPainterPath fp;
        fp.addRoundedRect(QRectF(cx, footY+footH-14, colW, 14), 11, 11);
        p.fillPath(fp, cCol.darker(135));
        p.fillRect(cx, footY+footH-14, colW, 7, cCol.darker(135));
        // Ligne sépar
        p.setPen(QPen(Qt::white, 0.5, Qt::DashLine));
        p.drawLine(cx+8, footY+20, cx+colW-8, footY+20);
        p.setPen(Qt::white); p.setFont(QFont("Arial", 7));
        p.drawText(cx+8, footY+14, "Etapes:");
        p.drawText(cx+8, footY+30, "Bois:");
        p.setFont(QFont("Arial", 7, QFont::Bold));
        p.drawText(cx+colW-72, footY+14,
                   QString("%1 DT").arg(mc.coutEtapes,0,'f',0));
        p.drawText(cx+colW-72, footY+30,
                   QString("%1 DT").arg(mc.coutBois,0,'f',2));
        // Total
        p.setFont(QFont("Arial", 9, QFont::Bold));
        p.drawText(cx+8, footY+44, "TOTAL:");
        p.drawText(cx+colW-80, footY+44,
                   QString("%1 DT").arg(mc.coutTotal,0,'f',2));
    }
    // ── Barre résumé global ───────────────────────────────────────────────
    {
        double totBois=0, totEtapes=0, totGen=0;
        for (const auto &mc : modeles) {
            totBois += mc.coutBois;
            totEtapes += mc.coutEtapes;
            totGen += mc.coutTotal;
        }
        int ry = startY + cardH + pad;
        p.setPen(Qt::NoPen); p.setBrush(cBrun);
        p.drawRoundedRect(pad, ry, W-2*pad, resumeH-4, 8, 8);
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", 9, QFont::Bold));
        p.drawText(pad+14, ry+18, "RESUME GLOBAL");
        p.setFont(QFont("Arial", 8));
        p.drawText(pad+14, ry+36,
                   QString("Bois total: %1 DT | Etapes total: %2 DT | "
                           "COUT GENERAL: %3 DT | Modeles: %4")
                       .arg(totBois,0,'f',2)
                       .arg(totEtapes,0,'f',0)
                       .arg(totGen,0,'f',2)
                       .arg(modeles.size()));
    }
    p.end();
    // ── Affichage dans QScrollArea ────────────────────────────────────────
    // Supprimer anciens widgets
    QList<QScrollArea*> oldScrolls =
        ui->placeholder_couts->findChildren<QScrollArea*>();
    for (auto *s : oldScrolls) s->deleteLater();
    QList<QLabel*> oldLabels =
        ui->placeholder_couts->findChildren<QLabel*>();
    for (auto *l : oldLabels) l->deleteLater();
    QScrollArea *scroll = new QScrollArea(ui->placeholder_couts);
    scroll->setObjectName("scroll_couts");
    scroll->setWidgetResizable(false);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setGeometry(0, 0,
                        ui->placeholder_couts->width(),
                        ui->placeholder_couts->height());
    QLabel *lbl = new QLabel();
    lbl->setObjectName("lbl_couts");
    lbl->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    lbl->setPixmap(pix);
    lbl->resize(W, H);
    scroll->setWidget(lbl);
    scroll->show();
    qDebug() << "afficherAnalyseCouts: dessin OK W=" << W << "H=" << H;
}
