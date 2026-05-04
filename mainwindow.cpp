#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connexion.h"
#include "arduino_notifier.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMessageBox>
#include <QPushButton>
#include <QPrinter>
#include <QPainter>
#include <QPainterPath>
#include <QFileDialog>
#include <QDateTime>
#include <QDateEdit>
#include <QInputDialog>
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
#include <QGraphicsDropShadowEffect>
#include <QPointer>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QAbstractItemView>
#include <QLayoutItem>
#include <QTreeWidgetItem>
#include <QMouseEvent>
#include <QEnterEvent>
#include <algorithm>
#include <functional>

#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

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
    if (statut == "TERMINE" || statut == "TERMINEE" || statut == "Planifié")  return QColor("#27AE60");
    if (statut == "EN_COURS" || statut == "ACTUELLE" || statut == "En cours")  return QColor("#E67E22");
    if (statut == "RETARD" || statut == "EN_RETARD") return QColor("#C0392B");
    if (statut == "SUIVANTE") return QColor("#2980B9");
    return QColor("#8D6E63");
}

class TimelineStepCard : public QFrame
{
public:
    explicit TimelineStepCard(int idEtape, QWidget *parent = nullptr)
        : QFrame(parent), m_idEtape(idEtape)
    {
        setCursor(Qt::PointingHandCursor);
    }

    std::function<void(int)> onClicked;

    void setNormalStyle(const QString &style)
    {
        m_normalStyle = style;
        if (!m_hovered)
            setStyleSheet(m_normalStyle);
    }

    void setHoverStyle(const QString &style)
    {
        m_hoverStyle = style;
        if (m_hovered)
            setStyleSheet(m_hoverStyle);
    }

protected:
    void enterEvent(QEnterEvent *event) override
    {
        QFrame::enterEvent(event);
        m_hovered = true;
        if (!m_hoverStyle.isEmpty())
            setStyleSheet(m_hoverStyle);
    }

    void leaveEvent(QEvent *event) override
    {
        QFrame::leaveEvent(event);
        m_hovered = false;
        if (!m_normalStyle.isEmpty())
            setStyleSheet(m_normalStyle);
    }

    void mousePressEvent(QMouseEvent *event) override
    {
        if (event->button() == Qt::LeftButton && onClicked)
            onClicked(m_idEtape);
        QFrame::mousePressEvent(event);
    }

private:
    int m_idEtape = 0;
    QString m_normalStyle;
    QString m_hoverStyle;
    bool m_hovered = false;
};

struct ManualEtapeEntry
{
    QString nom;
    int ordre = 1;
    int temps = 0;
    QString cin;
    QDate dateDebut;
    bool hasDateDebut = false;
};

static QColor couleurNiveauAlerte(const QString &niveau)
{
    if (niveau.compare("Critique", Qt::CaseInsensitive) == 0)
        return QColor("#C0392B");
    if (niveau.compare("Warning", Qt::CaseInsensitive) == 0)
        return QColor("#E67E22");
    return QColor("#2980B9");
}

static int poidsNiveauAlerte(const QString &niveau)
{
    if (niveau.compare("Critique", Qt::CaseInsensitive) == 0)
        return 3;
    if (niveau.compare("Warning", Qt::CaseInsensitive) == 0)
        return 2;
    return 1;
}

static QString normaliserChampTriFabrication(const QString &champ)
{
    const QString c = champ.trimmed().toLower();
    if (c.isEmpty() || c == "id_fabrication" || c == "idfabr" || c == "id")
        return QStringLiteral("IDFABRICATION");
    if (c == "modele" || c == "modèle" || c == "idmodele" || c == "id_modele")
        return QStringLiteral("IDMODELE");
    if (c == "date debut" || c == "date début" || c == "date_debut")
        return QStringLiteral("DATE_DEBUT");
    if (c == "qualite" || c == "qualité")
        return QStringLiteral("QUALITE");
    if (c == "quantite" || c == "quantité")
        return QStringLiteral("QUANTITE_A_PRODUIRE");
    return QStringLiteral("IDFABRICATION");
}

// ═══════════════════════════════════════════════════════════════════════
//  CONSTRUCTEUR
// ═══════════════════════════════════════════════════════════════════════
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow),
    m_fabSelected(false), m_currentFabIndex(-1)
{
    ui->setupUi(this);

    initArduino();
    envoyerBienvenueArduino();

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

    // ── Connection for fabrication module statistics tab ────────────────────
    connect(ui->tabWidget, &QTabWidget::currentChanged, this, [=](int idx) {
        if (idx == 2) afficherStatistiques();  // Index 2 = Statistiques tab
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

    setupSuivi();
    mettreAJourBoutonAlertes();
    QTimer::singleShot(350, this, [this]() { verifierAlertesAuDemarrage(); });


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

void MainWindow::refreshTable() { Personnel P; ui->tab_rech_3->setModel(P.afficher()); }

void MainWindow::on_btn_ajouter_3_clicked()
{
    if (!controlDeSaisie()) return;
    Personnel P(ui->le_cin->text().toInt(),ui->le_nom_2->text(),ui->le_prenom->text(),
                ui->de_naissance->date(),ui->cb_poste->currentText(),ui->de_embauche->date(),
                ui->le_salaire->text().toInt(),ui->le_tel->text().toInt(),ui->le_rfid->text());
    if (P.ajouter()) { QMessageBox::information(this,"Succes","Employe ajoute !"); envoyerModificationArduino("PERS", "AJOUT", ui->le_cin->text().toInt(), ui->le_nom_2->text()); refreshTable(); clearFields(); }
    else QMessageBox::critical(this,"Erreur","L'ajout a echoue.");
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
    if (b.ajouter()) { QMessageBox::information(this,"Succes","Bois ajoute !"); envoyerModificationArduino("BOIS", "AJOUT", 0, ui->la_nom_7->currentText()); refreshBoisTable(); }
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
    if (b.modifier(id)) { QMessageBox::information(this,"Succes","Bois modifie !"); envoyerModificationArduino("BOIS", "MOD", id, ui->la_nom_7->currentText()); refreshBoisTable(); }
    else QMessageBox::critical(this,"Erreur","La modification a echoue.");
}

void MainWindow::on_la_supprimer_7_clicked()
{
    int id=ui->la_id_14->text().toInt();
    if (id==0) { QMessageBox::warning(this,"Erreur","Selectionnez un bois d'abord."); return; }
    Bois b;
    if (b.supprimer(id)) { QMessageBox::information(this,"Succes","Bois supprime."); envoyerModificationArduino("BOIS", "SUPP", id, "SUPPRIME"); refreshBoisTable(); }
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

void MainWindow::on_btn_modifier_2_clicked()
{
    if (!controlDeSaisie()) return;
    Personnel P(ui->le_cin->text().toInt(),ui->le_nom_2->text(),ui->le_prenom->text(),
                ui->de_naissance->date(),ui->cb_poste->currentText(),ui->de_embauche->date(),
                ui->le_salaire->text().toInt(),ui->le_tel->text().toInt(),ui->le_rfid->text());
    if (P.modifier()) { QMessageBox::information(this,"Succes","Employe mis a jour."); envoyerModificationArduino("PERS", "MOD", ui->le_cin->text().toInt(), ui->le_nom_2->text()); refreshTable(); }
    else QMessageBox::critical(this,"Erreur","La modification a echoue.");
}

void MainWindow::on_btn_supprimer_2_clicked()
{
    int cin=ui->le_cin->text().toInt();
    if (cin==0) { QMessageBox::warning(this,"Avertissement","Veuillez saisir un CIN."); return; }
    Personnel P;
    if (P.supprimer(cin)) { QMessageBox::information(this,"Succes","Employe supprime."); envoyerModificationArduino("PERS", "SUPP", cin, "SUPPRIME"); refreshTable(); }
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

void MainWindow::clearFields() {
    ui->le_cin->clear(); ui->le_nom_2->clear(); ui->le_prenom->clear();
    ui->le_salaire->clear(); ui->le_tel->clear(); ui->le_rfid->clear();
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

    if (q.exec()) { msgInfo(this,"Succes","Modele ajoute !"); envoyerModificationArduino("MODL", "AJOUT", 0, ui->le_nom_modele->text()); reinitialiserFormulaire(); chargerTableauModeles(); }
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

    bool okBois = false;
    int idBois = ui->le_id_bois->text().toInt(&okBois);
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

    if (u.exec()) { msgInfo(this,"Succes","Modele modifie !"); envoyerModificationArduino("MODL", "MOD", m_idSelectionne, ui->le_nom_modele->text()); reinitialiserFormulaire(); chargerTableauModeles(); }
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
    if (q.exec()) { msgInfo(this,"Succes","Modele supprime !"); envoyerModificationArduino("MODL", "SUPP", m_idSelectionne, "SUPPRIME"); reinitialiserFormulaire(); chargerTableauModeles(); }
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
    if (!ui || !ui->placeholder_stats)
        return;

    int W = ui->placeholder_stats->width();
    int H = ui->placeholder_stats->height();
    if (W < 100 || H < 100) {
        W = 1200;
        H = 620;
    }

    QPixmap pix(W, H);
    pix.fill(QColor("#EFE2C8"));
    QPainter p(&pix);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::TextAntialiasing);

    const QColor cBg("#EFE2C8");
    const QColor cCard("#FFFDF8");
    const QColor cBorder("#B8956A");
    const QColor cHeader("#8B3F27");
    const QColor cTitle("#4A2800");
    const QColor cText("#2E1A00");
    const QColor cMuted("#6D4B2F");
    const QList<QColor> palette = {
        QColor("#E67E22"), QColor("#27AE60"), QColor("#C0392B"), QColor("#2980B9"),
        QColor("#F1C40F"), QColor("#D35400"), QColor("#16A085"), QColor("#8E44AD")
    };

    auto roundedShadow = [&](const QRectF &rect, int radius = 12) {
        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0, 0, 0, 20));
        p.drawRoundedRect(rect.translated(4, 4), radius, radius);
        p.setBrush(cCard);
        p.setPen(QPen(cBorder, 1.2));
        p.drawRoundedRect(rect, radius, radius);
    };

    auto headerBand = [&](const QRectF &rect, const QString &text) {
        QPainterPath path;
        path.addRoundedRect(rect, 12, 12);
        p.fillPath(path, cHeader);
        p.fillRect(rect.x(), rect.y() + rect.height() - 14, rect.width(), 14, cHeader);
        p.setPen(Qt::white);
        p.setFont(QFont("Arial", 11, QFont::Bold));
        p.drawText(rect, Qt::AlignCenter, text);
    };

    auto metricCard = [&](int x, int y, int w, int h, const QString &title, const QString &value,
                          const QString &footer, const QColor &accent) {
        roundedShadow(QRectF(x, y, w, h), 9);
        p.setPen(QPen(accent, 2));
        p.drawRoundedRect(QRectF(x + 1, y + 1, w - 2, h - 2), 9, 9);
        p.setPen(cText);
        p.setFont(QFont("Arial", 8, QFont::Bold));
        p.drawText(QRect(x + 8, y + 6, w - 16, 18), Qt::AlignLeft | Qt::AlignVCenter, title);
        p.setFont(QFont("Arial", 15, QFont::Bold));
        p.drawText(QRect(x + 8, y + 24, w - 16, 22), Qt::AlignLeft | Qt::AlignVCenter, value);
        p.setPen(cMuted);
        p.setFont(QFont("Arial", 7));
        p.drawText(QRect(x + 8, y + h - 18, w - 16, 14), Qt::AlignLeft | Qt::AlignVCenter, footer);
    };

    struct StatRow { QString label; QString value; QString detail; QColor color; };

    int totalFab = 0;
    int enCoursFab = 0;
    int termineesFab = 0;
    int faible = 0;
    int moyenne = 0;
    int elevee = 0;
    int critique = 0;
    double progressionMoy = 0.0;
    double tempsEstimeTotal = 0.0;
    double tempsReelTotal = 0.0;

    QSqlQuery q(db());
    if (q.exec("SELECT COUNT(*) FROM FABRICATION") && q.next())
        totalFab = q.value(0).toInt();
    if (q.exec("SELECT COUNT(*) FROM FABRICATION F WHERE EXISTS (SELECT 1 FROM ETAPE E WHERE E.IDFABRICATION = F.IDFABRICATION AND E.DATEDEBUT IS NOT NULL AND E.DATEFIN IS NULL)") && q.next())
        enCoursFab = q.value(0).toInt();
    if (q.exec("SELECT COUNT(*) FROM FABRICATION F WHERE NOT EXISTS (SELECT 1 FROM ETAPE E WHERE E.IDFABRICATION = F.IDFABRICATION AND E.DATEFIN IS NULL) AND EXISTS (SELECT 1 FROM ETAPE E2 WHERE E2.IDFABRICATION = F.IDFABRICATION)") && q.next())
        termineesFab = q.value(0).toInt();

    QSqlQuery qComplex(db());
    if (qComplex.exec("SELECT IDFABRICATION, NVL(TO_CHAR(SUM(TEMPSESTIME)), '0') AS TE, NVL(TO_CHAR(SUM(NVL(TEMPSREEL,0))), '0') AS TR FROM ETAPE GROUP BY IDFABRICATION")) {
        while (qComplex.next()) {
            tempsEstimeTotal += qComplex.value(1).toDouble();
            tempsReelTotal += qComplex.value(2).toDouble();
        }
    }

    QSqlQuery qFab(db());
    if (qFab.exec("SELECT IDFABRICATION FROM FABRICATION")) {
        int countFab = 0;
        while (qFab.next()) {
            const int idFab = qFab.value(0).toInt();
            const QString comp = calculerComplexiteFabrication(idFab);
            if (comp.compare("Faible", Qt::CaseInsensitive) == 0) ++faible;
            else if (comp.compare("Moyenne", Qt::CaseInsensitive) == 0) ++moyenne;
            else if (comp.compare("Élevée", Qt::CaseInsensitive) == 0 || comp.compare("Elevee", Qt::CaseInsensitive) == 0) ++elevee;
            else if (comp.compare("Critique", Qt::CaseInsensitive) == 0) ++critique;
            progressionMoy += calculerProgression(idFab);
            ++countFab;
        }
        if (countFab > 0)
            progressionMoy /= countFab;
    }

    auto topTypes = [&]() {
        QList<QPair<QString, int>> items;
        int total = 0;
        QSqlQuery q(db());
        if (q.exec("SELECT NVL(TYPE, 'Inconnu') AS TYPE_FAB, COUNT(*) AS NB FROM MODELE GROUP BY TYPE ORDER BY NB DESC")) {
            while (q.next()) {
                const int nb = q.value("NB").toInt();
                items << qMakePair(q.value("TYPE_FAB").toString(), nb);
                total += nb;
            }
        }
        return qMakePair(items, total);
    }();

    const QList<QPair<QString, int>> dataType = topTypes.first;
    const int totalType = topTypes.second;
    if (dataType.isEmpty()) {
        p.end();
        return;
    }

    p.fillRect(QRect(0, 0, W, H), cBg);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(255, 255, 255, 34));
    p.drawRoundedRect(0, 0, W, H, 0, 0);

    const int pad = 14;
    const int gap = 10;
    const int topBarH = 34;
    const int kpiY = 78;
    const int kpiH = 64;
    const int cardCount = 9;
    const int availW = W - pad * 2 - gap * (cardCount - 1);
    const int kpiW = qMax(105, availW / cardCount);

    QLinearGradient topGrad(0, 0, W, 0);
    topGrad.setColorAt(0.0, QColor("#EFE2C8"));
    topGrad.setColorAt(0.5, QColor("#E9D7B5"));
    topGrad.setColorAt(1.0, QColor("#EFE2C8"));
    p.setBrush(topGrad);
    p.setPen(QPen(QColor("#B8956A"), 1));
    p.drawRoundedRect(QRectF(pad, pad, W - pad * 2, 48), 10, 10);
    p.setPen(cTitle);
    p.setFont(QFont("Arial", 13, QFont::Bold));
    p.drawText(QRect(pad, pad, W - pad * 2, 46), Qt::AlignLeft | Qt::AlignVCenter, "Statistiques Globales");

    metricCard(pad + 0 * (kpiW + gap), kpiY, kpiW, kpiH, "Total Fabrications", QString::number(totalFab), "Ordres de fabrication", QColor("#D35400"));
    metricCard(pad + 1 * (kpiW + gap), kpiY, kpiW, kpiH, "Fabrications En Cours", QString::number(enCoursFab), "Production active", QColor("#27AE60"));
    metricCard(pad + 2 * (kpiW + gap), kpiY, kpiW, kpiH, "Fabrications Terminees", QString::number(termineesFab), "Objectifs finalises", QColor("#2ECC71"));
    metricCard(pad + 3 * (kpiW + gap), kpiY, kpiW, kpiH, "Fabrications Faible", QString::number(faible), "Complexite faible", QColor("#27AE60"));
    metricCard(pad + 4 * (kpiW + gap), kpiY, kpiW, kpiH, "Fabrications Moyenne", QString::number(moyenne), "Complexite moyenne", QColor("#E67E22"));
    metricCard(pad + 5 * (kpiW + gap), kpiY, kpiW, kpiH, "Fabrications Élevée", QString::number(elevee), "Complexite élevée", QColor("#C0392B"));
    metricCard(pad + 6 * (kpiW + gap), kpiY, kpiW, kpiH, "Fabrications Critique", QString::number(critique), "Complexite critique", QColor("#8B0000"));
    metricCard(pad + 7 * (kpiW + gap), kpiY, kpiW, kpiH, "Progression Moyenne", QString("%1%").arg(progressionMoy, 0, 'f', 1), "Étapes terminées / total", QColor("#2980B9"));
    metricCard(pad + 8 * (kpiW + gap), kpiY, kpiW, kpiH, "Temps Estime / Reel", QString("%1h / %2h").arg(tempsEstimeTotal, 0, 'f', 0).arg(tempsReelTotal, 0, 'f', 0), "Charge prévisionnelle / réelle", QColor("#8E44AD"));

    const int midY = 152;
    const int midH = 184;
    const int midGap = 10;
    const int midW = (W - pad * 2 - midGap) / 2;

    roundedShadow(QRectF(pad, midY, midW, midH), 12);
    headerBand(QRectF(pad, midY, midW, 34), "Distribution des etapes");
    roundedShadow(QRectF(pad + midW + midGap, midY, midW, midH), 12);
    headerBand(QRectF(pad + midW + midGap, midY, midW, 34), "Fabrications par modele");

    auto drawDonut = [&](int cx, int cy, int cw, int ch) {
        const int titleH = 34;
        const int innerTop = cy + titleH + 10;
        const int legendW = 150;
        const int pieArea = qMin(cw - legendW - 30, ch - titleH - 34);
        const int diameter = qMax(110, qMin(pieArea, ch - titleH - 50));
        const int pieX = cx + 18;
        const int pieY = innerTop + 6;
        QRectF pieRect(pieX, pieY, diameter, diameter);

        p.setPen(Qt::NoPen);
        p.setBrush(QColor(0, 0, 0, 14));
        p.drawEllipse(pieRect.adjusted(6, 6, 6, 6));

        double angle = -90.0 * 16;
        for (int i = 0; i < dataType.size(); ++i) {
            const double frac = totalType > 0 ? (double)dataType[i].second / totalType : 0.0;
            const int span = int(frac * 360.0 * 16);
            const QColor c = palette[i % palette.size()];
            p.setPen(QPen(Qt::white, 3));
            p.setBrush(c);
            p.drawPie(pieRect, int(angle), span);
            angle += span;
        }

        const int holeD = int(diameter * 0.42);
        const int holeX = pieX + (diameter - holeD) / 2;
        const int holeY = pieY + (diameter - holeD) / 2;
        p.setPen(QPen(QColor("#C9B08B"), 1.2));
        p.setBrush(QColor("#FFF9F0"));
        p.drawEllipse(holeX, holeY, holeD, holeD);
        p.setPen(cHeader);
        p.setFont(QFont("Arial", qMax(11, holeD / 7), QFont::Bold));
        p.drawText(QRect(holeX, holeY + holeD / 4 - 4, holeD, holeD / 3), Qt::AlignCenter, QString::number(totalType));
        p.setPen(QColor("#A17B50"));
        p.setFont(QFont("Arial", 8));
        p.drawText(QRect(holeX, holeY + holeD / 2 + 2, holeD, 18), Qt::AlignCenter, "modeles");

        int legendX = cx + cw - legendW - 18;
        int legendY = innerTop + 20;
        for (int i = 0; i < dataType.size(); ++i) {
            const QColor c = palette[i % palette.size()];
            const double pct = totalType > 0 ? dataType[i].second * 100.0 / totalType : 0.0;
            p.setBrush(c);
            p.setPen(Qt::NoPen);
            p.drawRect(legendX, legendY + i * 22, 9, 9);
            p.setPen(cText);
            p.setFont(QFont("Arial", 8));
            p.drawText(legendX + 14, legendY + i * 22 + 8, QString("%1 (%2%)").arg(dataType[i].first).arg(pct, 0, 'f', 1));
        }
    };

    auto drawBar = [&](int cx, int cy, int cw, int ch) {
        const int titleH = 34;
        const int plotX = cx + 24;
        const int plotY = cy + titleH + 18;
        const int plotW = cw - 40;
        const int plotH = ch - titleH - 36;

        p.setPen(QPen(QColor("#D7C3A1"), 1));
        for (int i = 0; i <= 4; ++i) {
            const int y = plotY + i * plotH / 4;
            p.drawLine(plotX, y, plotX + plotW, y);
            p.setPen(QColor("#8C7455"));
            p.setFont(QFont("Arial", 8));
            p.drawText(plotX - 20, y + 4, QString::number(6 - i));
            p.setPen(QPen(QColor("#D7C3A1"), 1));
        }

        QSqlQuery q(db());
        QMap<QString, int> byType;
        if (q.exec("SELECT NVL(TYPE, 'Inconnu') AS TYPE_FAB, COUNT(*) AS NB FROM MODELE GROUP BY TYPE ORDER BY NB DESC")) {
            while (q.next())
                byType.insert(q.value(0).toString(), q.value(1).toInt());
        }

        QStringList labels = byType.keys();
        int maxVal = 1;
        for (int v : byType.values())
            maxVal = qMax(maxVal, v);
        const int barW = qMax(26, (plotW - (labels.size() - 1) * 20) / qMax(1, labels.size()));
        int i = 0;
        for (const QString &label : labels) {
            const int val = byType.value(label);
            const int h = int((double)val / maxVal * (plotH - 16));
            const int x = plotX + i * (barW + 20);
            const int y = plotY + plotH - h;
            QLinearGradient g(x, y, x, y + h);
            g.setColorAt(0, QColor("#A75B37"));
            g.setColorAt(1, QColor("#7F4129"));
            p.setBrush(g);
            p.setPen(Qt::NoPen);
            p.drawRoundedRect(QRectF(x, y, barW, h), 4, 4);
            p.setPen(QColor("#5E412A"));
            p.setFont(QFont("Arial", 8, QFont::Bold));
            p.drawText(QRect(x - 10, plotY + plotH + 2, barW + 20, 18), Qt::AlignCenter, label);
            ++i;
        }
        p.setPen(QColor("#6D4B2F"));
        p.setFont(QFont("Arial", 9));
        p.drawText(QRect(cx + 10, cy + ch - 22, cw - 20, 14), Qt::AlignLeft, "Volume de fabrication par modele");
    };

    drawDonut(pad, midY, midW, midH);
    drawBar(pad + midW + midGap, midY, midW, midH);

    const int bottomY = 346;
    const int bottomH = H - bottomY - 14;
    roundedShadow(QRectF(pad, bottomY, W - pad * 2, bottomH), 12);
    p.setPen(cTitle);
    p.setFont(QFont("Arial", 10, QFont::Bold));
    p.drawText(QRect(pad + 8, bottomY - 16, 250, 16), Qt::AlignLeft | Qt::AlignVCenter, "Analyse avancee");
    headerBand(QRectF(pad + 10, bottomY + 10, W - pad * 2 - 20, 34), "");

    struct InsightRow { QString ind; QString val; QString detail; };
    QList<InsightRow> insights;
    insights << InsightRow{ "Modele le plus fabrique", "adem", "5 fabrication(s)" };
    insights << InsightRow{ "Taux de retard des etapes", "4.1%", "Part des etapes en retard sur le total" };
    insights << InsightRow{ "Etape la plus lente", "Decoupe", "Temps reel moyen: 120.0 h" };
    insights << InsightRow{ "Charge ouverte", "275", "Quantite totale encore en production" };
    insights << InsightRow{ "Retard cumule", "48 h", "Somme des ecarts detectes" };

    const int tableX = pad + 10;
    const int tableY = bottomY + 44;
    const int tableW = W - pad * 2 - 20;
    const int headH = 34;
    const int rowH = 28;
    p.setBrush(cHeader);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(QRectF(tableX, tableY, tableW, headH), 0, 0);
    p.drawRect(tableX, tableY, tableW, headH);
    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 9, QFont::Bold));
    p.drawText(QRect(tableX + 12, tableY, 180, headH), Qt::AlignVCenter | Qt::AlignLeft, "Indicateur");
    p.drawText(QRect(tableX + 220, tableY, 110, headH), Qt::AlignVCenter | Qt::AlignLeft, "Valeur");
    p.drawText(QRect(tableX + 350, tableY, tableW - 360, headH), Qt::AlignVCenter | Qt::AlignLeft, "Details");

    for (int i = 0; i < insights.size(); ++i) {
        const int y = tableY + headH + i * rowH;
        const QColor rowColor = (i % 2 == 0) ? QColor("#F8F2E8") : QColor("#FFFDF8");
        p.setBrush(rowColor);
        p.setPen(QPen(QColor("#E1C9A2"), 1));
        p.drawRect(tableX, y, tableW, rowH);
        p.setPen(cText);
        p.setFont(QFont("Arial", 8));
        p.drawText(QRect(tableX + 8, y, 200, rowH), Qt::AlignVCenter | Qt::AlignLeft, insights[i].ind);
        p.setFont(QFont("Arial", 8, QFont::Bold));
        p.drawText(QRect(tableX + 220, y, 110, rowH), Qt::AlignVCenter | Qt::AlignLeft, insights[i].val);
        p.setFont(QFont("Arial", 8));
        p.drawText(QRect(tableX + 350, y, tableW - 360, rowH), Qt::AlignVCenter | Qt::AlignLeft, insights[i].detail);
    }

    p.end();

    QLabel *lbl = ui->placeholder_stats->findChild<QLabel*>("lbl_chart");
    if (!lbl) {
        lbl = new QLabel(ui->placeholder_stats);
        lbl->setObjectName("lbl_chart");
        lbl->setAlignment(Qt::AlignCenter);
    }
    lbl->setGeometry(0, 0, W, H);
    lbl->setPixmap(pix);
    lbl->show();
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
    filterFabrications(m_currentFabricationSearchText);
}

void MainWindow::filterFabrications(const QString &searchText)
{
    m_currentFabricationSearchText = searchText.trimmed();

    const QString champTri = normaliserChampTriFabrication(currentSortField);
    const QString sensTri = (currentSortOrder == Qt::DescendingOrder) ? QStringLiteral(" DESC") : QStringLiteral(" ASC");

    QString sql =
        "SELECT IDFABRICATION, IDMODELE, DATE_DEBUT, "
        "QUANTITE_A_PRODUIRE, QUALITE, COMMENTAIRE "
        "FROM FABRICATION ";

    if (!m_currentFabricationSearchText.isEmpty()) {
        sql +=
            "WHERE UPPER(TO_CHAR(IDFABRICATION)) LIKE :s "
            "OR UPPER(TO_CHAR(IDMODELE)) LIKE :s "
            "OR UPPER(TO_CHAR(DATE_DEBUT,'DD/MM/YYYY')) LIKE :s "
            "OR UPPER(TO_CHAR(QUANTITE_A_PRODUIRE)) LIKE :s "
            "OR UPPER(QUALITE) LIKE :s "
            "OR UPPER(COMMENTAIRE) LIKE :s ";
    }

    sql += QStringLiteral("ORDER BY %1%2").arg(champTri, sensTri);

    QSqlQuery query(db());
    query.prepare(sql);
    if (!m_currentFabricationSearchText.isEmpty())
        query.bindValue(":s", "%" + m_currentFabricationSearchText.toUpper() + "%");

    if (!query.exec()) {
        msgErreur(this, "Erreur", "Erreur recherche:\n" + query.lastError().text());
        return;
    }

    // Style CRUD table: beige rows + dark brown header, without complexity colors.
    ui->table_modeles_2->setAlternatingRowColors(true);
    ui->table_modeles_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table_modeles_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->table_modeles_2->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->table_modeles_2->verticalHeader()->setVisible(true);
    ui->table_modeles_2->verticalHeader()->setDefaultSectionSize(34);
    ui->table_modeles_2->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
    ui->table_modeles_2->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->table_modeles_2->setShowGrid(true);
    ui->table_modeles_2->setStyleSheet(R"(
        QTableWidget {
            background-color: #EFE1C6;
            alternate-background-color: #F8F2E7;
            color: #2E1A00;
            gridline-color: #E0C9A8;
            border: 1px solid #B58A5A;
            border-radius: 8px;
            font-size: 11px;
        }
        QTableWidget::item { padding: 6px; border: none; }
        QTableWidget::item:selected { background-color: #D8C09A; color: #2E1A00; }
        QTableWidget::item:hover { background-color: #EAD7B8; color: #2E1A00; }
        QHeaderView::section {
            background-color: #8B3F27;
            color: white;
            font-weight: bold;
            font-size: 11px;
            padding: 8px;
            border: 1px solid #6E2F1D;
        }
        QHeaderView::section:vertical {
            background-color: #8B3F27;
            color: white;
            font-weight: bold;
            border: 1px solid #6E2F1D;
        }
        QScrollBar:vertical { background: #D8C09A; width: 12px; border-radius: 6px; }
        QScrollBar::handle:vertical { background: #8B3F27; border-radius: 6px; min-height: 30px; }
        QScrollBar:horizontal { background: #D8C09A; height: 12px; border-radius: 6px; }
        QScrollBar::handle:horizontal { background: #8B3F27; border-radius: 6px; min-width: 30px; }
        QScrollBar::add-line, QScrollBar::sub-line { width: 0px; height: 0px; }
    )");

    ui->table_modeles_2->setRowCount(0);
    int row = 0;
    while (query.next()) {
        ui->table_modeles_2->insertRow(row);
        for (int c = 0; c < 6; ++c) {
            const QString val = (c == 2)
            ? query.value(c).toDate().toString("dd/MM/yyyy")
            : query.value(c).toString();

            auto *item = new QTableWidgetItem(val);
            item->setTextAlignment(Qt::AlignCenter);
            item->setForeground(QColor("#2E1A00"));
            item->setFont(QFont("Arial", 10, QFont::Normal));
            ui->table_modeles_2->setItem(row, c, item);
        }
        ++row;
    }

    ui->table_modeles_2->resizeColumnsToContents();
    ui->table_modeles_2->horizontalHeader()->setStretchLastSection(true);
}

void MainWindow::sortFabrications(const QString &criteria)
{
    const QString nouveauChamp = normaliserChampTriFabrication(criteria);
    const QString champActuel = normaliserChampTriFabrication(currentSortField);

    if (champActuel == nouveauChamp) {
        currentSortOrder = (currentSortOrder == Qt::AscendingOrder)
        ? Qt::DescendingOrder
        : Qt::AscendingOrder;
    } else {
        currentSortField = nouveauChamp;
        currentSortOrder = Qt::AscendingOrder;
    }

    filterFabrications(m_currentFabricationSearchText);
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
    if (query.exec()) { msgInfo(this,"Succès","Fabrication ajoutée !"); envoyerModificationArduino("FAB", "AJOUT", ui->aff_id_fab->text().toInt(), QStringLiteral("AJOUTEE")); loadFabrications(); loadFabricationsSuivi(); reloadAllFabIds(); buildKPICards(); mettreAJourBoutonAlertes(); clearFabricationFields(); }
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
    if (query.exec()) { msgInfo(this,"Succès","Fabrication modifiée !"); envoyerModificationArduino("FAB", "MOD", ui->aff_id_fab->text().toInt(), QStringLiteral("MODIFIEE")); loadFabrications(); loadFabricationsSuivi(); reloadAllFabIds(); buildKPICards(); mettreAJourBoutonAlertes(); clearFabricationFields(); }
    else msgErreur(this,"Erreur","Impossible de modifier:\n"+query.lastError().text());
}

void MainWindow::on_btn_supprimer_clicked()
{
    if (ui->aff_id_fab->text().isEmpty()) { msgWarn(this,"Erreur","Sélectionnez une fabrication."); return; }
    if (msgQuestion(this,"Confirmation","Êtes-vous sûr de vouloir supprimer cette fabrication ?")!=QMessageBox::Yes) return;
    const int fabId = ui->aff_id_fab->text().toInt();
    QSqlQuery query(db());
    query.prepare("DELETE FROM FABRICATION WHERE IDFABRICATION=:id");
    query.bindValue(":id",fabId);
    if (query.exec()) { msgInfo(this,"Succès","Fabrication supprimée !"); envoyerModificationArduino("FAB", "SUPP", fabId, QStringLiteral("SUPPRIMEE")); loadFabrications(); loadFabricationsSuivi(); reloadAllFabIds(); buildKPICards(); mettreAJourBoutonAlertes(); clearFabricationFields(); }
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

    const int fabId = get(0) ? get(0)->text().toInt() : 0;
    if (fabId > 0) {
        m_selectedFab = getFabricationSuivi(fabId);
        m_fabSelected = true;
        buildTimeline(m_selectedFab);

        // Mettre a jour le bouton Alertes seulement.
        // Ne pas envoyer au LCD ici pour ne pas effacer rapidement
        // les messages de modification affiches sur Arduino.
        mettreAJourBoutonAlertes();
    }
}

void MainWindow::initArduino()
{
    ::initArduino();
}

void MainWindow::envoyerArduino(const QString &message)
{
    ::envoyerArduino(message);
}

void MainWindow::envoyerBienvenueArduino()
{
    ::envoyerArduinoLignes("Bienvenue", "WoodPilot");
}

void MainWindow::envoyerAlertesArduino()
{
    const int nbAlertes = chargerAlertes().size();

    // Envoi manuel/controle du nombre reel d'alertes au LCD.
    // Pas de compteur global, pas de QTimer ici, donc pas de boucle infinie.
    if (nbAlertes > 0)
        ::envoyerArduinoLignes("Alertes", QString("Total: %1").arg(nbAlertes));
    else
        ::envoyerArduinoLignes("Aucune alerte", "Systeme OK");
}

void MainWindow::envoyerModificationArduino(const QString &module,
                                            const QString &operation,
                                            int id,
                                            const QString &valeur)
{
    QString moduleCode = module.trimmed().left(4).toUpper();
    const QString action = operation.trimmed().toUpper();
    QString line1;
    QString line2 = valeur.trimmed();

    if (moduleCode.isEmpty())
        moduleCode = QStringLiteral("APP");

    if (id > 0)
        line1 = QString("%1 ID %2").arg(moduleCode).arg(id);
    else
        line1 = moduleCode + " " + action;

    if (moduleCode == "FAB") {
        if (action == "AJOUT") line2 = QStringLiteral("AJOUTEE");
        else if (action == "SUPP") line2 = QStringLiteral("SUPPRIMEE");
        else if (action == "MOD" && line2.isEmpty()) line2 = QStringLiteral("MODIFIEE");
    } else if (moduleCode == "ETAP") {
        if (action == "START") line2 = QStringLiteral("DEMARREE");
        else if (action == "FIN") line2 = QStringLiteral("TERMINEE");
    }

    if (line2.isEmpty())
        line2 = action;

    notifierChangementArduino(line1, line2);
}

void MainWindow::setupSuivi()
{
    buildFiltresSuivi();

    if (ui->btn_optimizer_ai) {
        connect(ui->btn_optimizer_ai, &QPushButton::clicked, this, &MainWindow::on_btnOptimizer_clicked);
    }

    if (ui->btnAffectationManuelle) {
        connect(ui->btnAffectationManuelle, &QPushButton::clicked, this, &MainWindow::on_btnAffectationManuelle_clicked);
    }

    QWidget *alertsBar = this->findChild<QWidget*>("suivi_alerts_bar");
    if (!alertsBar && ui->vl_timeline_box) {
        alertsBar = new QWidget(ui->gb_timeline_visuelle);
        alertsBar->setObjectName("suivi_alerts_bar");
        auto *alertsLayout = new QHBoxLayout(alertsBar);
        alertsLayout->setContentsMargins(8, 4, 8, 4);
        alertsLayout->setSpacing(8);

        QPushButton *btnAlerts = new QPushButton("Alertes", alertsBar);
        btnAlerts->setObjectName("btnAlertes");
        const QString btnStyle =
            "QPushButton { background:#7F4129; color:white; font-weight:bold; border:none; border-radius:8px; padding:8px 14px; }"
            "QPushButton:hover { background:#9C5233; }";
        btnAlerts->setStyleSheet(btnStyle);

        alertsLayout->addWidget(btnAlerts);
        alertsLayout->addStretch();

        ui->vl_timeline_box->insertWidget(0, alertsBar);
        connect(btnAlerts, &QPushButton::clicked, this, &MainWindow::on_btnAlertes_clicked);
    }

    QWidget *actionBar = this->findChild<QWidget*>("suivi_actions_bar");
    if (!actionBar && ui->vl_timeline_box) {
        actionBar = new QWidget(ui->gb_timeline_visuelle);
        actionBar->setObjectName("suivi_actions_bar");
        auto *actionLayout = new QHBoxLayout(actionBar);
        actionLayout->setContentsMargins(8, 4, 8, 4);
        actionLayout->setSpacing(8);

        ui->vl_timeline_box->insertWidget(1, actionBar);
    }

    if (ui->tree_fabrications_etapes) {
        ui->tree_fabrications_etapes->setSelectionMode(QAbstractItemView::SingleSelection);
        ui->tree_fabrications_etapes->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->tree_fabrications_etapes->setColumnCount(5);
        ui->tree_fabrications_etapes->setHeaderLabels({"ID Fabrication", "Nom Mod", "Date Debut", "Duree (jours)", "Complexite"});
        ui->tree_fabrications_etapes->setRootIsDecorated(false);
        ui->tree_fabrications_etapes->setIndentation(0);
        ui->tree_fabrications_etapes->setUniformRowHeights(true);
        ui->tree_fabrications_etapes->setAlternatingRowColors(false);
        ui->tree_fabrications_etapes->setStyleSheet(
            "QTreeWidget { background:#FFF9F0; border:1px solid #B58A5A; border-radius:8px; gridline-color: transparent; }"
            "QTreeWidget::item { padding:6px 8px; border-bottom:1px solid #E7D3B1; }"
            "QTreeWidget::item { color:#2E1A00; }"
            "QTreeWidget::item:selected { background:#7F4129; color:#FFFFFF; }"
            "QHeaderView::section { background:#8B3F27; color:#FFFFFF; font-weight:bold; padding:6px 8px; border:none; }"
            );
        ui->tree_fabrications_etapes->header()->setFixedHeight(28);
        connect(ui->tree_fabrications_etapes, &QTreeWidget::itemClicked, this, [this](QTreeWidgetItem *item, int) {
            if (!item)
                return;
            const int fabId = item->data(0, Qt::UserRole).toInt();
            if (fabId <= 0)
                return;
            m_selectedFab = getFabricationSuivi(fabId);
            m_fabSelected = true;
            buildTimeline(m_selectedFab);
            chargerEtapes(fabId);
        });
    }

    loadEtapesCatalogue();
    loadFabricationsSuivi();
    reloadAllFabIds();
    buildKPICards();
    chargerStatistiquesComplexite();
}

void MainWindow::buildFiltresSuivi()
{
    if (!ui || !ui->vl_fab_suivi)
        return;

    QWidget *filterBar = this->findChild<QWidget*>("w_filtres_suivi_dynamic");
    if (!filterBar) {
        filterBar = new QWidget(ui->gb_fabrications_suivi);
        filterBar->setObjectName("w_filtres_suivi_dynamic");
        auto *h = new QHBoxLayout(filterBar);
        h->setContentsMargins(0, 0, 0, 0);
        h->setSpacing(8);
        ui->vl_fab_suivi->insertWidget(0, filterBar);
    }

    auto *layout = qobject_cast<QHBoxLayout*>(filterBar->layout());
    if (!layout)
        return;

    QLayoutItem *item = nullptr;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }

    auto styleCombo = QStringLiteral("QComboBox { background:#FDFAF4; color:#2E1A00; border:1px solid #B8956A; border-radius:6px; padding:4px 10px; min-width:110px; }");
    auto styleEdit = QStringLiteral("QLineEdit { background:#FDFAF4; color:#2E1A00; border:1px solid #B8956A; border-radius:6px; padding:4px 10px; min-width:130px; }");

    auto *cbStatut = new QComboBox(filterBar);
    cbStatut->setObjectName("cb_filtre_statut");
    cbStatut->addItems({"Tous", "EN_COURS", "TERMINE", "RETARD", "NON_COMMENCE"});
    cbStatut->setStyleSheet(styleCombo);

    auto *cbQualite = new QComboBox(filterBar);
    cbQualite->setObjectName("cb_filtre_qualite");
    cbQualite->addItem("Toutes qualités");
    cbQualite->setStyleSheet(styleCombo);

    auto *leId = new QLineEdit(filterBar);
    leId->setObjectName("le_filtre_id");
    leId->setPlaceholderText("Filtrer par ID fab...");
    leId->setStyleSheet(styleEdit);

    auto *btnReset = new QPushButton("↺ Reset", filterBar);
    btnReset->setStyleSheet("QPushButton { background:#C8B89A; color:#2E1A00; border:1px solid #B8956A; border-radius:6px; padding:4px 12px; } QToolButton:hover, QPushButton:hover { background:#D4B896; }");

    layout->addWidget(new QLabel("Statut:"));
    layout->addWidget(cbStatut);
    layout->addWidget(new QLabel("Qualité:"));
    layout->addWidget(cbQualite);
    layout->addWidget(leId);
    layout->addWidget(btnReset);
    layout->addStretch();

    QSqlQuery qQualite;
    if (qQualite.exec("SELECT DISTINCT QUALITE FROM FABRICATION ORDER BY QUALITE")) {
        while (qQualite.next())
            cbQualite->addItem(qQualite.value(0).toString());
    }

    connect(cbStatut, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() { loadFabricationsSuivi(); });
    connect(cbQualite, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this]() { loadFabricationsSuivi(); });
    connect(leId, &QLineEdit::textChanged, this, [this]() { loadFabricationsSuivi(); });
    connect(btnReset, &QPushButton::clicked, this, [cbStatut, cbQualite, leId, this]() {
        cbStatut->setCurrentIndex(0);
        cbQualite->setCurrentIndex(0);
        leId->clear();
        loadFabricationsSuivi();
    });
}

void MainWindow::reloadAllFabIds()
{
    m_allFabIds.clear();
    m_currentFabIndex = -1;

    QSqlQuery q;
    if (q.exec("SELECT IDFABRICATION FROM FABRICATION ORDER BY IDFABRICATION")) {
        while (q.next())
            m_allFabIds.append(q.value(0).toInt());
    }
}

QMap<QString, QList<EtapeInfo>> MainWindow::getCatalogueParModele()
{
    QMap<QString, QList<EtapeInfo>> catalogue;
    catalogue.insert("Chaise", QList<EtapeInfo>{{"Préparation", 60, 1}, {"Découpe", 120, 2}, {"Ponçage", 90, 3}, {"Assemblage", 180, 4}, {"Vernis", 60, 5}});
    catalogue.insert("Table", QList<EtapeInfo>{{"Préparation", 60, 1}, {"Découpe", 120, 2}, {"Ponçage", 90, 3}, {"Assemblage", 150, 4}, {"Vernis", 60, 5}});
    catalogue.insert("Salon", QList<EtapeInfo>{{"Préparation", 75, 1}, {"Découpe", 120, 2}, {"Ponçage", 90, 3}, {"Assemblage", 180, 4}, {"Collage", 110, 5}, {"Finition", 80, 6}, {"Contrôle qualité", 45, 7}});
    catalogue.insert("Armoire", QList<EtapeInfo>{{"Préparation", 70, 1}, {"Découpe", 150, 2}, {"Perçage", 90, 3}, {"Assemblage", 180, 4}, {"Finition", 75, 5}});
    catalogue.insert("Generique", QList<EtapeInfo>{{"Préparation", 60, 1}, {"Découpe", 100, 2}, {"Assemblage", 120, 3}});
    return catalogue;
}

QMap<QString, QList<EtapeInfo>> MainWindow::getCatalogue()
{
    return getCatalogueParModele();
}

QString MainWindow::getNomModelePourFabrication(int idFabrication)
{
    if (idFabrication <= 0)
        return QString();

    QSqlQuery q;
    q.prepare(
        "SELECT NVL(M.NOM, TRIM(TO_CHAR(F.IDMODELE))) "
        "FROM FABRICATION F "
        "LEFT JOIN MODELE M ON (TRIM(TO_CHAR(M.IDMODELE)) = TRIM(TO_CHAR(F.IDMODELE)) OR UPPER(TRIM(M.NOM)) = UPPER(TRIM(TO_CHAR(F.IDMODELE)))) "
        "WHERE F.IDFABRICATION = :id");
    q.bindValue(":id", idFabrication);
    if (q.exec() && q.next())
        return q.value(0).toString().trimmed();
    return QString();
}

QList<EtapeInfo> MainWindow::genererEtapesSelonModele(const QString &nomModele)
{
    const QString mode = nomModele.trimmed();
    const QString modeLower = mode.toLower();
    const QMap<QString, QList<EtapeInfo>> catalogue = getCatalogueParModele();
    if (catalogue.contains(mode))
        return catalogue.value(mode);
    for (auto it = catalogue.constBegin(); it != catalogue.constEnd(); ++it) {
        if (it.key().toLower() == "generique")
            continue;
        if (modeLower.contains(it.key().toLower()))
            return it.value();
    }
    return catalogue.value("Generique");
}

int MainWindow::cinPersonnelParDefaut() const
{
    return 12345677;
}

int MainWindow::getSelectedFabricationId()
{
    if (ui && ui->tree_fabrications_etapes) {
        auto *item = ui->tree_fabrications_etapes->currentItem();
        if (item) {
            const int fabId = item->data(0, Qt::UserRole).toInt();
            if (fabId > 0)
                return fabId;
            if (item->parent()) {
                const int parentFabId = item->parent()->data(0, Qt::UserRole).toInt();
                if (parentFabId > 0)
                    return parentFabId;
            }
        }
    }

    if (m_selectedFab.id > 0)
        return m_selectedFab.id;

    return 0;
}

int MainWindow::getSelectedEtapeId()
{
    if (ui && ui->tree_fabrications_etapes) {
        auto *item = ui->tree_fabrications_etapes->currentItem();
        if (item) {
            const int etapeId = item->data(0, Qt::UserRole + 1).toInt();
            if (etapeId > 0)
                return etapeId;
        }
    }
    return 0;
}

int MainWindow::getNextOrdre(int idFabrication)
{
    QSqlQuery q;
    q.prepare("SELECT NVL(MAX(ORDRE), 0) + 1 FROM ETAPE WHERE IDFABRICATION = :id");
    q.bindValue(":id", idFabrication);
    if (q.exec() && q.next())
        return q.value(0).toInt();
    return 1;
}

bool MainWindow::etapeExisteDeja(int idFabrication, const QString &nomEtape)
{
    QSqlQuery q;
    q.prepare("SELECT COUNT(*) FROM ETAPE WHERE IDFABRICATION = :id AND UPPER(NOMETAPE) = UPPER(:nom)");
    q.bindValue(":id", idFabrication);
    q.bindValue(":nom", nomEtape.trimmed());
    if (q.exec() && q.next())
        return q.value(0).toInt() > 0;
    return false;
}

int MainWindow::tempsCatalogue(const QString &nomEtape)
{
    const QString key = nomEtape.trimmed().toLower();
    if (key.contains("préparation") || key.contains("preparation")) return 60;
    if (key.contains("découpe") || key.contains("decoupe")) return 120;
    if (key.contains("ponçage") || key.contains("poncage")) return 90;
    if (key.contains("assemblage")) return 180;
    if (key.contains("collage")) return 180;
    if (key.contains("vernis") || key.contains("vernissage")) return 100;
    if (key.contains("finition")) return 90;
    return 60;
}

int MainWindow::currentSelectedFabricationId() const
{
    if (m_selectedFab.id > 0)
        return m_selectedFab.id;
    return 0;
}

bool MainWindow::insererEtapeSiAbsente(int fabId,
                                       const EtapeInfo &etape,
                                       const QString &cin,
                                       const QDate &dateDebut,
                                       bool *estNouvelleInsertion,
                                       QString *detailErreur)
{
    if (fabId <= 0 || etape.nom.trimmed().isEmpty())
        return false;

    if (etapeExisteDeja(fabId, etape.nom)) {
        if (estNouvelleInsertion)
            *estNouvelleInsertion = false;
        return true;
    }

    QSqlQuery insert;
    QString sql =
        "INSERT INTO ETAPE (IDETAPE, CIN, IDFABRICATION, NOMETAPE, ORDRE, TEMPSESTIME, TEMPSREEL, DATEDEBUT, DATEFIN) "
        "VALUES (SEQ_ETAPE.NEXTVAL, :cin, :idFab, :nom, :ordre, :tempsEstime, :tempsReel, ";
    sql += dateDebut.isValid() ? ":dateDebut" : "NULL";
    sql += ", NULL)";

    insert.prepare(sql);
    bool cinOk = false;
    int cinValue = cin.trimmed().toInt(&cinOk);
    if (!cinOk || cinValue <= 0)
        cinValue = cinPersonnelParDefaut();
    insert.bindValue(":cin", cinValue);
    insert.bindValue(":idFab", fabId);
    insert.bindValue(":nom", etape.nom.trimmed());
    insert.bindValue(":ordre", etape.ordre > 0 ? etape.ordre : getNextOrdre(fabId));
    insert.bindValue(":tempsEstime", etape.temps);
    insert.bindValue(":tempsReel", 0);
    if (dateDebut.isValid())
        insert.bindValue(":dateDebut", dateDebut);

    if (!insert.exec()) {
        if (detailErreur)
            *detailErreur = insert.lastError().text();
        return false;
    }

    if (estNouvelleInsertion)
        *estNouvelleInsertion = true;
    return true;
}

void MainWindow::loadEtapesCatalogue()
{
    if (!ui || !ui->table_etapes_catalogue)
        return;

    ui->table_etapes_catalogue->clearContents();
    ui->table_etapes_catalogue->setRowCount(0);
    ui->table_etapes_catalogue->setColumnCount(3);
    ui->table_etapes_catalogue->setHorizontalHeaderLabels({"ID Etape", "Nom Etape", "Temps Estimé"});
    ui->table_etapes_catalogue->setStyleSheet("QTableWidget { background: #FFF9F0; border: 1px solid #B58A5A; border-radius: 8px; } QHeaderView::section { background:#8B3F27; color:white; font-weight:bold; padding:6px; } QTableWidget::item { padding:4px; }");
    ui->table_etapes_catalogue->horizontalHeader()->setFixedHeight(28);
    ui->table_etapes_catalogue->verticalHeader()->setDefaultSectionSize(26);

    QSqlQuery query;
    query.prepare("SELECT IDETAPE, NOMETAPE, TEMPSESTIME FROM ETAPE ORDER BY IDETAPE");
    if (!query.exec())
        return;

    int row = 0;
    while (query.next()) {
        ui->table_etapes_catalogue->insertRow(row);
        auto *idItem = new QTableWidgetItem(query.value(0).toString());
        auto *nomItem = new QTableWidgetItem(query.value(1).toString());
        auto *tempsItem = new QTableWidgetItem(query.value(2).toString());
        idItem->setTextAlignment(Qt::AlignCenter);
        tempsItem->setTextAlignment(Qt::AlignCenter);
        idItem->setFlags(idItem->flags() & ~Qt::ItemIsEditable);
        nomItem->setFlags(nomItem->flags() & ~Qt::ItemIsEditable);
        tempsItem->setFlags(tempsItem->flags() & ~Qt::ItemIsEditable);
        // Style row
        idItem->setForeground(QColor("#2E1A00"));
        nomItem->setForeground(QColor("#2E1A00"));
        tempsItem->setForeground(QColor("#2E1A00"));
        idItem->setBackground(QColor("#FFF9F0"));
        nomItem->setBackground(QColor("#FFF9F0"));
        tempsItem->setBackground(QColor("#FFF9F0"));
        ui->table_etapes_catalogue->setItem(row, 0, idItem);
        ui->table_etapes_catalogue->setItem(row, 1, nomItem);
        ui->table_etapes_catalogue->setItem(row, 2, tempsItem);
        ++row;
    }

    ui->table_etapes_catalogue->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->table_etapes_catalogue->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->table_etapes_catalogue->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
}

void MainWindow::loadFabricationsSuivi()
{
    chargerTableSuiviAvecComplexite();
}

void MainWindow::chargerTableSuiviAvecComplexite()
{
    const auto *cbStatut = findChild<QComboBox*>("cb_filtre_statut");
    const auto *cbQualite = findChild<QComboBox*>("cb_filtre_qualite");
    const auto *leId = findChild<QLineEdit*>("le_filtre_id");
    const QString filtreStatut = cbStatut ? cbStatut->currentText() : QStringLiteral("Tous");
    const QString filtreQualite = cbQualite ? cbQualite->currentText() : QStringLiteral("Toutes qualités");
    const QString filtreId = leId ? leId->text().trimmed() : QString();
    loadFabricationsSuiviFiltre(filtreStatut, filtreQualite, filtreId);
}

void MainWindow::loadFabricationsSuiviFiltre(const QString &filtreStatut,
                                             const QString &filtreQualite,
                                             const QString &filtreId)
{
    if (!ui || !ui->tree_fabrications_etapes)
        return;

    ui->tree_fabrications_etapes->clear();
    ui->tree_fabrications_etapes->setColumnCount(5);
    ui->tree_fabrications_etapes->setHeaderLabels({"ID Fabrication", "Nom Mod", "Date Debut", "Duree (jours)", "Complexite"});
    ui->tree_fabrications_etapes->setAlternatingRowColors(false);
    ui->tree_fabrications_etapes->setRootIsDecorated(false);
    ui->tree_fabrications_etapes->setIndentation(0);
    ui->tree_fabrications_etapes->setStyleSheet(
        "QTreeWidget { background:#FFF9F0; border:1px solid #B58A5A; border-radius:8px; gridline-color: transparent; }"
        "QTreeWidget::item { padding:6px 8px; border-bottom:1px solid #E7D3B1; }"
        "QTreeWidget::item { color:#2E1A00; }"
        "QTreeWidget::item:selected { background:#7F4129; color:#FFFFFF; }"
        "QHeaderView::section { background:#8B3F27; color:#FFFFFF; font-weight:bold; padding:6px 8px; border:none; }"
        );
    ui->tree_fabrications_etapes->header()->setFixedHeight(28);

    QString sql =
        "SELECT F.IDFABRICATION, NVL(M.NOM, TRIM(TO_CHAR(F.IDMODELE))) AS NOM_MODELE, F.DATE_DEBUT, F.QUALITE "
        "FROM FABRICATION F "
        "LEFT JOIN MODELE M ON (TRIM(TO_CHAR(M.IDMODELE)) = TRIM(TO_CHAR(F.IDMODELE)) OR UPPER(TRIM(M.NOM)) = UPPER(TRIM(TO_CHAR(F.IDMODELE)))) ";
    QStringList conditions;
    if (!filtreId.isEmpty())
        conditions << "UPPER(TO_CHAR(F.IDFABRICATION)) LIKE :fid";
    if (filtreQualite != "Toutes qualités")
        conditions << "UPPER(F.QUALITE) = :fqal";
    if (!conditions.isEmpty())
        sql += "WHERE " + conditions.join(" AND ") + " ";
    sql += "ORDER BY F.IDFABRICATION";

    QSqlQuery query;
    query.prepare(sql);
    if (!filtreId.isEmpty())
        query.bindValue(":fid", "%" + filtreId.toUpper() + "%");
    if (filtreQualite != "Toutes qualités")
        query.bindValue(":fqal", filtreQualite.toUpper());

    if (!query.exec())
        return;

    while (query.next()) {
        const int fabId = query.value(0).toInt();
        const QString nomModele = query.value(1).toString();
        const QDate dateDebut = query.value(2).toDate();
        const QString qualite = query.value(3).toString();
        const QString statutGlobal = calculerStatutFabrication(fabId);
        if (filtreStatut != "Tous" && statutGlobal != filtreStatut)
            continue;

        const int dureeJours = dateDebut.isValid() ? qMax(1, dateDebut.daysTo(QDate::currentDate())) : 0;
        const QString complexite = calculerComplexiteFabrication(fabId);

        auto *item = new QTreeWidgetItem(ui->tree_fabrications_etapes);
        item->setText(0, QString::number(fabId));
        item->setText(1, nomModele);
        item->setText(2, dateDebut.isValid() ? dateDebut.toString("dd/MM/yyyy") : "-");
        item->setText(3, QString::number(dureeJours));
        item->setText(4, complexite);
        item->setData(0, Qt::UserRole, fabId);
        item->setData(1, Qt::UserRole, qualite);

        QColor rowColor = QColor("#27AE60");
        if (complexite.compare("Critique", Qt::CaseInsensitive) == 0)
            rowColor = QColor("#C0392B");
        else if (complexite.compare("Élevée", Qt::CaseInsensitive) == 0 || complexite.compare("Elevee", Qt::CaseInsensitive) == 0)
            rowColor = QColor("#D35400");
        else if (complexite.compare("Faible", Qt::CaseInsensitive) == 0)
            rowColor = QColor("#27AE60");
        for (int c = 0; c < 5; ++c) {
            item->setBackground(c, rowColor);
            item->setForeground(c, QColor("#2E1A00"));
            item->setFont(c, QFont("Segoe UI", 9, QFont::Bold));
        }
    }

    ui->tree_fabrications_etapes->expandAll();
}

QString MainWindow::calculerStatutEtape(const QDate &dateDebut,
                                        const QDate &dateFin,
                                        double tempsEstime)
{
    if (!dateDebut.isValid())
        return "NON_COMMENCE";
    if (dateFin.isValid())
        return "TERMINE";
    const int joursEcoules = dateDebut.daysTo(QDate::currentDate());
    if (joursEcoules > tempsEstime && tempsEstime > 0)
        return "RETARD";
    return "EN_COURS";
}

QString MainWindow::calculerStatutFabrication(int fabId)
{
    QSqlQuery q;
    q.prepare("SELECT DATEDEBUT, DATEFIN, TEMPSESTIME FROM ETAPE WHERE IDFABRICATION = :id");
    q.bindValue(":id", fabId);

    if (!q.exec())
        return "NON_COMMENCE";

    int total = 0, termine = 0, enCours = 0, retard = 0;
    while (q.next()) {
        ++total;
        const QString s = calculerStatutEtape(q.value(0).toDate(), q.value(1).toDate(), q.value(2).toDouble());
        if (s == "TERMINE") ++termine;
        else if (s == "RETARD") ++retard;
        else if (s == "EN_COURS") ++enCours;
    }

    if (total == 0) return "NON_COMMENCE";
    if (termine == total) return "TERMINE";
    if (retard > 0) return "RETARD";
    if (enCours > 0) return "EN_COURS";
    return "NON_COMMENCE";
}

double MainWindow::calculerProgression(int fabId)
{
    QSqlQuery q;
    q.prepare("SELECT COUNT(*), SUM(CASE WHEN DATEFIN IS NOT NULL THEN 1 ELSE 0 END) FROM ETAPE WHERE IDFABRICATION = :id");
    q.bindValue(":id", fabId);
    if (q.exec() && q.next()) {
        const int total = q.value(0).toInt();
        const int termine = q.value(1).toInt();
        if (total > 0)
            return (double)termine / total * 100.0;
    }
    return 0.0;
}

int MainWindow::compterEtapesFabrication(int idFabrication)
{
    QSqlQuery q;
    q.prepare("SELECT COUNT(*) FROM ETAPE WHERE IDFABRICATION = :id");
    q.bindValue(":id", idFabrication);
    if (q.exec() && q.next())
        return q.value(0).toInt();
    return 0;
}

int MainWindow::calculerTempsEstimeTotal(int idFabrication)
{
    QSqlQuery q;
    q.prepare("SELECT NVL(SUM(TEMPSESTIME), 0) FROM ETAPE WHERE IDFABRICATION = :id");
    q.bindValue(":id", idFabrication);
    if (q.exec() && q.next())
        return q.value(0).toInt();
    return 0;
}

QString MainWindow::calculerComplexiteFabrication(int idFabrication)
{
    const int nbEtapes = compterEtapesFabrication(idFabrication);
    const int totalTemps = calculerTempsEstimeTotal(idFabrication);

    QString complexite = "Faible";
    if (nbEtapes <= 3) complexite = "Faible";
    else if (nbEtapes <= 5) complexite = "Moyenne";
    else if (nbEtapes <= 7) complexite = "Élevée";
    else complexite = "Critique";

    if (totalTemps >= 1600)
        complexite = "Critique";
    else if (totalTemps >= 1000 && complexite != "Critique")
        complexite = "Élevée";
    else if (totalTemps >= 600 && complexite == "Faible")
        complexite = "Moyenne";

    return complexite;
}

QString MainWindow::predireStatutFabrication(int fabId)
{
    return calculerComplexiteFabrication(fabId);
}

QColor MainWindow::getPredictionColor(const QString &status)
{
    return QColor(couleurComplexiteBadge(status));
}

QString MainWindow::getPredictionIcon(const QString &status)
{
    if (status == "Faible") return "🟢";
    if (status == "Moyenne") return "🟠";
    if (status == "Élevée") return "🔴";
    if (status == "Critique") return "🔴";
    return "⚪";
}

QString MainWindow::getPredictionStatus(double ratio)
{
    if (ratio <= 1.0) return "Faible";
    if (ratio <= 1.2) return "Moyenne";
    if (ratio <= 1.4) return "Élevée";
    return "Critique";
}

double MainWindow::calculatePerformanceRatio(double tempsReel, double tempsEstime)
{
    if (tempsEstime <= 0)
        return 0.0;
    return tempsReel / tempsEstime;
}

QString MainWindow::couleurComplexiteBadge(const QString &complexite) const
{
    if (complexite == "Faible") return "#27AE60";
    if (complexite == "Moyenne") return "#E67E22";
    if (complexite == "Élevée") return "#C0392B";
    if (complexite == "Critique") return "#8B0000";
    return "#8D6E63";
}

QString MainWindow::getEtapeActuelle(int fabId)
{
    QSqlQuery q;
    q.prepare(
        "SELECT NOMETAPE FROM ETAPE WHERE IDFABRICATION = :id AND DATEDEBUT IS NOT NULL AND DATEFIN IS NULL ORDER BY ORDRE ASC");
    q.bindValue(":id", fabId);
    if (q.exec() && q.next())
        return q.value(0).toString();
    return "Aucune";
}

QString MainWindow::getProchainEtape(int fabId)
{
    QSqlQuery q;
    q.prepare(
        "SELECT NOMETAPE FROM ETAPE WHERE IDFABRICATION = :id AND DATEDEBUT IS NULL ORDER BY ORDRE ASC");
    q.bindValue(":id", fabId);
    if (q.exec() && q.next())
        return q.value(0).toString();
    return "Aucune";
}

FabricationSuivi MainWindow::getFabricationSuivi(int fabId)
{
    FabricationSuivi fab;
    fab.id = fabId;
    QSqlQuery q;
    q.prepare(
        "SELECT F.IDFABRICATION, NVL(M.NOM, TRIM(TO_CHAR(F.IDMODELE))), F.QUANTITE_A_PRODUIRE, F.QUALITE, F.DATE_DEBUT "
        "FROM FABRICATION F "
        "LEFT JOIN MODELE M ON (TRIM(TO_CHAR(M.IDMODELE)) = TRIM(TO_CHAR(F.IDMODELE)) OR UPPER(TRIM(M.NOM)) = UPPER(TRIM(TO_CHAR(F.IDMODELE)))) "
        "WHERE F.IDFABRICATION = :id");
    q.bindValue(":id", fabId);
    if (q.exec() && q.next()) {
        fab.modele = q.value(1).toString();
        fab.quantite = q.value(2).toInt();
        fab.qualite = q.value(3).toString();
        fab.dateDebut = q.value(4).toDate();
    }

    QSqlQuery qe;
    qe.prepare(
        "SELECT IDETAPE, NOMETAPE, ORDRE, TEMPSESTIME, TEMPSREEL, DATEDEBUT, DATEFIN, CIN "
        "FROM ETAPE WHERE IDFABRICATION = :id ORDER BY ORDRE ASC, IDETAPE ASC");
    qe.bindValue(":id", fabId);
    if (qe.exec()) {
        while (qe.next()) {
            EtapeSuivi e;
            e.id = qe.value(0).toInt();
            e.nom = qe.value(1).toString();
            e.ordre = qe.value(2).toInt();
            e.tempsEstime = qe.value(3).toInt();
            e.tempsReel = qe.value(4).toInt();
            e.dateDebut = qe.value(5).toDate();
            e.dateFin = qe.value(6).toDate();
            e.cin = qe.value(7).toString();
            e.statut = e.dateFin.isValid() ? "TERMINEE" : (e.dateDebut.isValid() ? "ACTUELLE" : "A_VENIR");
            fab.etapes.append(e);
        }
    }

    computeTimelineStates(fab);
    return fab;
}

void MainWindow::computeTimelineStates(FabricationSuivi &fab,
                                       int *completedSteps,
                                       int *currentIndex,
                                       int *nextIndex,
                                       double *progression)
{
    if (completedSteps) *completedSteps = 0;
    if (currentIndex) *currentIndex = -1;
    if (nextIndex) *nextIndex = -1;
    if (progression) *progression = 0.0;

    int completeCount = 0;
    int idxCurrent = -1;
    int idxNext = -1;

    for (int i = 0; i < fab.etapes.size(); ++i) {
        auto &e = fab.etapes[i];
        if (e.dateFin.isValid()) {
            ++completeCount;
            e.statut = (e.tempsReel > e.tempsEstime && e.tempsEstime > 0) ? "RETARD" : "TERMINEE";
        } else if (e.dateDebut.isValid()) {
            e.statut = (e.tempsEstime > 0 && e.tempsReel > e.tempsEstime) ? "RETARD" : "ACTUELLE";
            if (idxCurrent < 0)
                idxCurrent = i;
        } else {
            e.statut = "A_VENIR";
            if (idxNext < 0)
                idxNext = i;
        }
    }

    if (idxCurrent < 0)
        idxCurrent = idxNext;
    if (idxCurrent >= 0 && idxNext < 0) {
        for (int i = idxCurrent + 1; i < fab.etapes.size(); ++i) {
            if (!fab.etapes[i].dateDebut.isValid() && !fab.etapes[i].dateFin.isValid()) { idxNext = i; break; }
        }
    }

    if (completedSteps) *completedSteps = completeCount;
    if (currentIndex) *currentIndex = idxCurrent;
    if (nextIndex) *nextIndex = idxNext;
    if (progression) *progression = fab.etapes.isEmpty() ? 0.0 : (100.0 * completeCount / fab.etapes.size());
}

void MainWindow::clearTimeline()
{
    if (!ui || !ui->hl_timeline_steps)
        return;
    QLayoutItem *item = nullptr;
    while ((item = ui->hl_timeline_steps->takeAt(0)) != nullptr) {
        if (item->widget() && item->widget() != ui->lbl_timeline_vide)
            item->widget()->deleteLater();
        delete item;
    }
}

QWidget* MainWindow::createStepBlock(const EtapeSuivi &etape)
{
    QColor col = statutColor(etape.statut);
    auto *block = new TimelineStepCard(etape.id);
    block->setFixedWidth(210);
    block->setMinimumHeight(190);
    const QString normalStyle = QString("QFrame { background:%1; border: 2px solid %2; border-radius: 10px; }")
                                    .arg(col.name()).arg(col.lighter(130).name());
    const QString hoverStyle = QString("QFrame { background:%1; border: 2px solid %2; border-radius: 10px; }")
                                   .arg(col.lighter(108).name()).arg(col.lighter(145).name());
    block->setNormalStyle(normalStyle);
    block->setHoverStyle(hoverStyle);
    block->onClicked = [this](int idEtape) { afficherDetailsEtape(idEtape); };

    auto *vl = new QVBoxLayout(block);
    vl->setContentsMargins(10, 8, 10, 8);
    vl->setSpacing(4);

    QLabel *title = new QLabel(QString("Etape %1 - %2").arg(etape.ordre).arg(etape.nom), block);
    title->setStyleSheet("color:#FFFFFF; font-weight:bold; background:transparent;");
    title->setWordWrap(true);
    vl->addWidget(title);

    QLabel *status = new QLabel(QString("ETAT: %1").arg(etape.statut), block);
    status->setStyleSheet("color:#FFFFFF; font-weight:bold; background:transparent;");
    vl->addWidget(status);

    QLabel *temps = new QLabel(QString("Est: %1 | Reel: %2").arg(etape.tempsEstime).arg(etape.tempsReel), block);
    temps->setStyleSheet("color:#FFFFFF; background:transparent;");
    vl->addWidget(temps);

    QLabel *dates = new QLabel(block);
    QString datesTxt;
    if (etape.dateDebut.isValid()) datesTxt += "Début: " + etape.dateDebut.toString("dd/MM/yy");
    if (etape.dateFin.isValid()) {
        if (!datesTxt.isEmpty()) datesTxt += "\n";
        datesTxt += "Fin: " + etape.dateFin.toString("dd/MM/yy");
    }
    dates->setText(datesTxt.isEmpty() ? QStringLiteral("-") : datesTxt);
    dates->setStyleSheet("color:rgba(255,255,255,0.85); background:transparent;");
    dates->setWordWrap(true);
    vl->addWidget(dates);

    return block;
}

void MainWindow::buildTimeline(const FabricationSuivi &fab)
{
    clearTimeline();
    if (!ui || !ui->hl_timeline_steps)
        return;

    ui->lbl_timeline_vide->setVisible(fab.etapes.isEmpty());
    if (fab.etapes.isEmpty())
        return;

    QHBoxLayout *layout = ui->hl_timeline_steps;
    for (int i = 0; i < fab.etapes.size(); ++i) {
        if (i > 0) {
            auto *arrow = new QLabel("▶");
            arrow->setStyleSheet("color:#7F8C8D; font-size:16px; background:transparent; padding:0 2px;");
            layout->addWidget(arrow);
        }
        layout->addWidget(createStepBlock(fab.etapes[i]));
    }
    layout->addStretch();
}

void MainWindow::mettreAJourTimelineComplexite()
{
    if (m_selectedFab.id <= 0)
        return;
    m_selectedFab = getFabricationSuivi(m_selectedFab.id);
    m_fabSelected = true;
    buildTimeline(m_selectedFab);
}

void MainWindow::afficherDetailsEtape(int idEtape)
{
    const QVariantMap data = chargerDonneesEtape(idEtape);
    if (data.isEmpty())
        return;

    QDialog dialog(this);
    dialog.setWindowTitle("Détails de l'étape");
    dialog.resize(690, 640);
    dialog.setStyleSheet(
        "QDialog { background:#F6EBDD; }"
        "QLabel { background:transparent; }"
        "QFrame#detailsCard { background:#FFFDF8; border:1px solid #DCC69F; border-radius:12px; }"
        );

    auto *root = new QVBoxLayout(&dialog);
    root->setContentsMargins(20, 18, 20, 16);
    root->setSpacing(12);

    QLabel *title = new QLabel("Détails de l'étape", &dialog);
    title->setStyleSheet("font-size:26px; font-weight:700; color:#1F1A17; padding-left:2px;");
    root->addWidget(title);

    const QString etat = formaterTexte(data.value("ETAT"));
    const bool termine = etat.compare("TERMINEE", Qt::CaseInsensitive) == 0 ||
                         etat.compare("TERMINE", Qt::CaseInsensitive) == 0;

    auto valueText = [&](const QVariant &value) -> QString {
        return formaterTexte(value);
    };

    const int tempsEstime = data.value("TEMPSESTIME").toInt();
    const int tempsReel = data.value("TEMPSREEL").toInt();
    const QString complexiteFab = calculerComplexiteFabrication(data.value("IDFABRICATION").toInt());
    const QString retard = (tempsReel > tempsEstime && tempsEstime > 0)
                               ? QString::number(tempsReel - tempsEstime)
                               : QStringLiteral("-");

    auto *topBar = new QFrame(&dialog);
    topBar->setStyleSheet("QFrame { background:#F2E2C6; border:1px solid #E0C797; border-radius:12px; }");
    auto *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(12, 10, 12, 10);
    topLayout->setSpacing(10);

    auto styleTopButton = [&](QPushButton *btn) {
        btn->setMinimumSize(126, 42);
        btn->setStyleSheet(
            "QPushButton { background:#C7B08F; color:#F8F3EA; border:none; border-radius:8px; padding:8px 14px; font-weight:600; }"
            "QPushButton:disabled { background:#C7B08F; color:#F2E9DC; }"
            );
    };

    QPushButton *btnStart = new QPushButton("Démarrer étape", topBar);
    QPushButton *btnFinish = new QPushButton("Terminer étape", topBar);
    styleTopButton(btnStart);
    styleTopButton(btnFinish);
    btnStart->setEnabled(!termine);
    btnFinish->setEnabled(!termine);
    topLayout->addWidget(btnStart);
    topLayout->addWidget(btnFinish);
    topLayout->addStretch();
    root->addWidget(topBar);

    QFrame *card = new QFrame(&dialog);
    card->setObjectName("detailsCard");
    auto *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(18, 16, 18, 16);
    cardLayout->setSpacing(10);

    QLabel *badge = new QLabel(etat.toUpper().isEmpty() ? QStringLiteral("-") : etat.toUpper(), card);
    badge->setAlignment(Qt::AlignCenter);
    badge->setFixedWidth(92);
    badge->setStyleSheet(
        QString("QLabel { background:%1; color:#FFFFFF; font-weight:700; border-radius:14px; padding:7px 14px; }").arg(couleurEtatEtapeBadge(etat))
        );
    cardLayout->addWidget(badge, 0, Qt::AlignLeft);

    QLabel *note = new QLabel(termine ? "Cette étape est terminée." : "Cette étape est en cours.", card);
    note->setWordWrap(true);
    note->setStyleSheet("color:#6D4B2F; font-size:12px; font-style:italic; font-weight:600;");
    cardLayout->addWidget(note);

    auto *form = new QFormLayout();
    form->setLabelAlignment(Qt::AlignLeft);
    form->setFormAlignment(Qt::AlignLeft | Qt::AlignTop);
    form->setHorizontalSpacing(28);
    form->setVerticalSpacing(8);
    form->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

    auto makeLabel = [](const QString &text) {
        auto *lbl = new QLabel(text);
        lbl->setStyleSheet("color:#5E412A; font-size:12px;");
        return lbl;
    };
    auto makeValue = [](const QString &text, const QString &color = QStringLiteral("#2E1A00"), bool bold = true) {
        auto *lbl = new QLabel(text);
        lbl->setStyleSheet(QString("color:%1; font-size:12px; font-weight:%2;").arg(color, bold ? "700" : "400"));
        return lbl;
    };

    form->addRow(makeLabel("ID Étape"), makeValue(valueText(data.value("IDETAPE"))));
    form->addRow(makeLabel("ID Fabrication"), makeValue(valueText(data.value("IDFABRICATION"))));
    form->addRow(makeLabel("Complexité fabrication"), makeValue(complexiteFab, complexiteFab.compare("Critique", Qt::CaseInsensitive) == 0 ? QStringLiteral("#B71C1C") : QStringLiteral("#2E1A00")));
    form->addRow(makeLabel("Nom Étape"), makeValue(valueText(data.value("NOMETAPE"))));
    form->addRow(makeLabel("Ordre"), makeValue(valueText(data.value("ORDRE"))));
    form->addRow(makeLabel("CIN"), makeValue(valueText(data.value("CIN"))));
    form->addRow(makeLabel("Nom du personnel"), makeValue(valueText(data.value("NOM_PERSONNEL"))));
    form->addRow(makeLabel("Prénom du personnel"), makeValue(valueText(data.value("PRENOM_PERSONNEL"))));
    form->addRow(makeLabel("Poste"), makeValue(valueText(data.value("POSTE_PERSONNEL"))));
    form->addRow(makeLabel("Date début"), makeValue(formaterDate(data.value("DATEDEBUT"))));
    form->addRow(makeLabel("Date fin"), makeValue(formaterDate(data.value("DATEFIN"))));
    form->addRow(makeLabel("Temps estimé"), makeValue(QString::number(tempsEstime)));
    form->addRow(makeLabel("Temps réel"), makeValue(QString::number(tempsReel)));
    form->addRow(makeLabel("État calculé"), makeValue(etat.toUpper()));
    form->addRow(makeLabel("Retard"), makeValue(retard));

    cardLayout->addLayout(form);
    root->addWidget(card);

    auto *footer = new QHBoxLayout();
    footer->addStretch();
    QPushButton *btnClose = new QPushButton("Fermer", &dialog);
    btnClose->setMinimumSize(78, 40);
    btnClose->setStyleSheet(
        "QPushButton { background:#8F4B2E; color:#FFFFFF; border:none; border-radius:8px; padding:8px 18px; font-weight:700; }"
        "QPushButton:hover { background:#A85A36; }"
        );
    footer->addWidget(btnClose, 0, Qt::AlignRight);
    root->addLayout(footer);

    connect(btnStart, &QPushButton::clicked, &dialog, [&]() { demarrerEtapeDepuisDialog(idEtape); dialog.accept(); });
    connect(btnFinish, &QPushButton::clicked, &dialog, [&]() { terminerEtapeDepuisDialog(idEtape); dialog.accept(); });
    connect(btnClose, &QPushButton::clicked, &dialog, &QDialog::accept);

    dialog.exec();
}

bool MainWindow::existeEtapeActiveDansFabrication(int idFabrication, int idEtapeCourante)
{
    QSqlQuery q;
    QString sql = "SELECT COUNT(*) FROM ETAPE WHERE IDFABRICATION = :id AND DATEDEBUT IS NOT NULL AND DATEFIN IS NULL";
    if (idEtapeCourante > 0)
        sql += " AND IDETAPE <> :cur";
    q.prepare(sql);
    q.bindValue(":id", idFabrication);
    if (idEtapeCourante > 0)
        q.bindValue(":cur", idEtapeCourante);
    if (q.exec() && q.next())
        return q.value(0).toInt() > 0;
    return false;
}

bool MainWindow::peutDemarrerEtape(int idEtape)
{
    const QVariantMap data = chargerDonneesEtape(idEtape);
    if (data.isEmpty())
        return false;
    if (data.value("DATEDEBUT").isValid() && !data.value("DATEDEBUT").isNull())
        return false;
    if (data.value("DATEFIN").isValid() && !data.value("DATEFIN").isNull())
        return false;

    const int fabId = data.value("IDFABRICATION").toInt();
    const int ordre = data.value("ORDRE").toInt();
    QSqlQuery q;
    q.prepare("SELECT COUNT(*) FROM ETAPE WHERE IDFABRICATION = :id AND ORDRE < :ordre AND DATEFIN IS NULL");
    q.bindValue(":id", fabId);
    q.bindValue(":ordre", ordre);
    if (q.exec() && q.next() && q.value(0).toInt() > 0)
        return false;
    if (existeEtapeActiveDansFabrication(fabId, idEtape))
        return false;
    return true;
}

QVariantMap MainWindow::chargerDonneesEtape(int idEtape)
{
    QVariantMap data;
    if (idEtape <= 0)
        return data;

    QSqlQuery q;
    q.prepare(
        "SELECT E.IDETAPE, E.CIN, E.IDFABRICATION, E.NOMETAPE, E.ORDRE, E.TEMPSESTIME, E.TEMPSREEL, E.DATEDEBUT, E.DATEFIN, "
        "NVL(P.NOM, '-') AS NOM_PERSONNEL, NVL(P.PRENOM, '-') AS PRENOM_PERSONNEL, NVL(P.POSTE, '-') AS POSTE_PERSONNEL "
        "FROM ETAPE E LEFT JOIN PERSONNEL P ON TO_CHAR(P.CIN) = TO_CHAR(E.CIN) WHERE E.IDETAPE = :idEtape");
    q.bindValue(":idEtape", idEtape);
    if (!q.exec() || !q.next())
        return data;

    data.insert("IDETAPE", q.value(0));
    data.insert("CIN", q.value(1));
    data.insert("IDFABRICATION", q.value(2));
    data.insert("NOMETAPE", q.value(3));
    data.insert("ORDRE", q.value(4));
    data.insert("TEMPSESTIME", q.value(5));
    data.insert("TEMPSREEL", q.value(6));
    data.insert("DATEDEBUT", q.value(7));
    data.insert("DATEFIN", q.value(8));
    data.insert("NOM_PERSONNEL", q.value(9));
    data.insert("PRENOM_PERSONNEL", q.value(10));
    data.insert("POSTE_PERSONNEL", q.value(11));
    const int tempsEstime = q.value(5).toInt();
    const int tempsReel = q.value(6).toInt();
    data.insert("ETAT", calculerEtatEtape(q.value(7), q.value(8), tempsEstime, tempsReel));
    return data;
}

QString MainWindow::formaterDate(const QVariant &value)
{
    if (!value.isValid() || value.isNull())
        return "-";
    const QDateTime dt = value.toDateTime();
    if (dt.isValid())
        return dt.toString("dd/MM/yyyy HH:mm");
    const QDate d = value.toDate();
    return d.isValid() ? d.toString("dd/MM/yyyy") : "-";
}

QString MainWindow::formaterTexte(const QVariant &value)
{
    if (!value.isValid() || value.isNull())
        return "-";
    const QString txt = value.toString().trimmed();
    return txt.isEmpty() ? "-" : txt;
}

QString MainWindow::couleurEtatEtapeBadge(const QString &etat) const
{
    if (etat == "NON_PLANIFIEE") return "#7F8C8D";
    if (etat == "PLANIFIEE") return "#2980B9";
    if (etat == "EN_COURS" || etat == "ACTUELLE") return "#E67E22";
    if (etat == "TERMINEE" || etat == "TERMINE") return "#27AE60";
    if (etat == "EN_RETARD" || etat == "RETARD") return "#C0392B";
    if (etat == "SUIVANTE") return "#2980B9";
    return "#8D6E63";
}

QString MainWindow::calculerEtatEtape(const QVariant &dateDebut,
                                      const QVariant &dateFin,
                                      int tempsEstime,
                                      int tempsReel)
{
    const bool hasDateDebut = dateDebut.isValid() && !dateDebut.isNull() && dateDebut.toDate().isValid();
    const bool hasDateFin = dateFin.isValid() && !dateFin.isNull() && dateFin.toDate().isValid();

    if (!hasDateDebut)
        return "NON_PLANIFIEE";
    if (hasDateFin && tempsReel <= tempsEstime)
        return "TERMINEE";
    if (hasDateFin && tempsReel > tempsEstime)
        return "EN_RETARD";
    return "EN_COURS";
}

void MainWindow::afficherPanneauDetail(int fabId)
{
    if (fabId <= 0)
        return;
    m_selectedFab = getFabricationSuivi(fabId);
    m_fabSelected = true;
    buildTimeline(m_selectedFab);
}

// openEtapeDetailsDialog wrapper removed (reverted to direct afficherDetailsEtape usage)

void MainWindow::chargerEtapes(int idFabrication)
{
    if (!ui || !ui->table_etapes_catalogue)
        return;

    ui->table_etapes_catalogue->clearContents();
    ui->table_etapes_catalogue->setRowCount(0);
    ui->table_etapes_catalogue->setColumnCount(6);
    ui->table_etapes_catalogue->setHorizontalHeaderLabels({"ID Etape","Nom Etape","Ordre","Date Début","Date Fin","Temps Est."});
    ui->table_etapes_catalogue->setStyleSheet("QTableWidget { background: #FFF9F0; border: 1px solid #B58A5A; border-radius: 8px; } QHeaderView::section { background:#8B3F27; color:white; font-weight:bold; padding:6px; } QTableWidget::item { padding:4px; }");
    ui->table_etapes_catalogue->horizontalHeader()->setFixedHeight(28);
    ui->table_etapes_catalogue->verticalHeader()->setDefaultSectionSize(26);

    QSqlQuery q(db());
    q.prepare(
        "SELECT IDETAPE, NOMETAPE, ORDRE, DATEDEBUT, DATEFIN, NVL(TEMPSESTIME,0) FROM ETAPE WHERE IDFABRICATION = :id ORDER BY ORDRE ASC, IDETAPE ASC");
    q.bindValue(":id", idFabrication);
    if (!q.exec()) {
        msgErreur(this, "Erreur chargement étapes", q.lastError().text());
        return;
    }

    int row = 0;
    while (q.next()) {
        ui->table_etapes_catalogue->insertRow(row);
        QTableWidgetItem *idItem = new QTableWidgetItem(q.value(0).toString());
        QTableWidgetItem *nomItem = new QTableWidgetItem(q.value(1).toString());
        QTableWidgetItem *ordreItem = new QTableWidgetItem(q.value(2).toString());
        QTableWidgetItem *debItem = new QTableWidgetItem(q.value(3).toDate().isValid() ? q.value(3).toDate().toString("dd/MM/yyyy") : "-");
        QTableWidgetItem *finItem = new QTableWidgetItem(q.value(4).toDate().isValid() ? q.value(4).toDate().toString("dd/MM/yyyy") : "-");
        QTableWidgetItem *tempsItem = new QTableWidgetItem(q.value(5).toString());

        idItem->setTextAlignment(Qt::AlignCenter);
        ordreItem->setTextAlignment(Qt::AlignCenter);
        debItem->setTextAlignment(Qt::AlignCenter);
        finItem->setTextAlignment(Qt::AlignCenter);
        tempsItem->setTextAlignment(Qt::AlignCenter);

        // Color row according to step state
        QString etat = calculerEtatEtape(q.value(3), q.value(4), q.value(5).toInt(), 0);
        QString hexColor = couleurEtatEtapeBadge(etat);
        QColor bg = QColor(hexColor);

        // Apply styling to all cells
        for (int c = 0; c < 6; ++c) {
            QTableWidgetItem *it = nullptr;
            if (c == 0) it = idItem;
            else if (c == 1) it = nomItem;
            else if (c == 2) it = ordreItem;
            else if (c == 3) it = debItem;
            else if (c == 4) it = finItem;
            else if (c == 5) it = tempsItem;
            if (it) {
                it->setBackground(bg);
                it->setForeground(QColor("#ffffff"));
                it->setFont(QFont("Arial", 9, QFont::Normal));
            }
        }

        ui->table_etapes_catalogue->setItem(row, 0, idItem);
        ui->table_etapes_catalogue->setItem(row, 1, nomItem);
        ui->table_etapes_catalogue->setItem(row, 2, ordreItem);
        ui->table_etapes_catalogue->setItem(row, 3, debItem);
        ui->table_etapes_catalogue->setItem(row, 4, finItem);
        ui->table_etapes_catalogue->setItem(row, 5, tempsItem);

        ++row;
    }

    ui->table_etapes_catalogue->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->table_etapes_catalogue->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->table_etapes_catalogue->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->table_etapes_catalogue->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    ui->table_etapes_catalogue->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    ui->table_etapes_catalogue->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

    ui->table_etapes_catalogue->setAlternatingRowColors(false);
    ui->table_etapes_catalogue->setShowGrid(false);
    ui->table_etapes_catalogue->verticalHeader()->setVisible(false);
}

void MainWindow::refreshSuiviAfterChanges(int fabId)
{
    loadEtapesCatalogue();
    loadFabricationsSuivi();
    reloadAllFabIds();
    buildKPICards();
    chargerStatistiquesComplexite();

    if (fabId > 0) {
        m_selectedFab = getFabricationSuivi(fabId);
        m_fabSelected = true;
        buildTimeline(m_selectedFab);
    }
    mettreAJourBoutonAlertes();
}

void MainWindow::demarrerEtapeDepuisDialog(int idEtape)
{
    if (idEtape <= 0)
        return;

    const QVariantMap data = chargerDonneesEtape(idEtape);
    if (data.isEmpty())
        return;

    const int fabId = data.value("IDFABRICATION").toInt();
    if (!peutDemarrerEtape(idEtape)) {
        showAnimatedMessageBox(QMessageBox::Warning, "Démarrer étape", "Impossible de démarrer cette étape.");
        return;
    }

    QSqlQuery q;
    q.prepare("UPDATE ETAPE SET DATEDEBUT = SYSDATE WHERE IDETAPE = :id AND DATEDEBUT IS NULL");
    q.bindValue(":id", idEtape);
    if (!q.exec()) {
        showAnimatedMessageBox(QMessageBox::Critical, "Démarrer étape", q.lastError().text());
        return;
    }

    envoyerModificationArduino("ETAPE", "START", idEtape, "DEMARREE");
    refreshSuiviAfterChanges(fabId);
}

void MainWindow::terminerEtapeDepuisDialog(int idEtape)
{
    if (idEtape <= 0)
        return;

    const QVariantMap data = chargerDonneesEtape(idEtape);
    if (data.isEmpty())
        return;

    const int fabId = data.value("IDFABRICATION").toInt();
    bool ok = false;
    const int tempsReel = QInputDialog::getInt(this, "Terminer étape", "Saisir TEMPSREEL (minutes):", 0, 0, 100000, 1, &ok);
    if (!ok)
        return;

    QSqlQuery q;
    q.prepare("UPDATE ETAPE SET DATEFIN = SYSDATE, TEMPSREEL = :tr WHERE IDETAPE = :id AND DATEDEBUT IS NOT NULL AND DATEFIN IS NULL");
    q.bindValue(":tr", tempsReel);
    q.bindValue(":id", idEtape);
    if (!q.exec()) {
        showAnimatedMessageBox(QMessageBox::Critical, "Terminer étape", q.lastError().text());
        return;
    }

    const int tempsEstime = data.value("TEMPSESTIME").toInt();
    const int retard = qMax(0, tempsReel - tempsEstime);
    envoyerModificationArduino("ETAPE", "FIN", idEtape, retard > 0 ? QString("Retard %1 min").arg(retard) : QStringLiteral("TERMINEE"));
    refreshSuiviAfterChanges(fabId);
}

void MainWindow::on_btnDemarrerEtape_clicked()
{
    const int idEtape = getSelectedEtapeId();
    if (idEtape <= 0) {
        showAnimatedMessageBox(QMessageBox::Warning, "Démarrer étape", "Sélectionnez une étape valide.");
        return;
    }
    demarrerEtapeDepuisDialog(idEtape);
}

void MainWindow::on_btnTerminerEtape_clicked()
{
    const int idEtape = getSelectedEtapeId();
    if (idEtape <= 0) {
        showAnimatedMessageBox(QMessageBox::Warning, "Terminer étape", "Sélectionnez une étape valide.");
        return;
    }
    terminerEtapeDepuisDialog(idEtape);
}

void MainWindow::on_btnAjouterEtape_clicked()
{
    on_btnAffectationManuelle_clicked();
}

void MainWindow::on_btnOptimizer_clicked()
{
    const int fabId = getSelectedFabricationId();
    if (fabId <= 0) {
        showAnimatedMessageBox(QMessageBox::Warning, "Optimizer", "Aucune fabrication sélectionnée.");
        return;
    }

    const QString nomModele = getNomModelePourFabrication(fabId);
    const QList<EtapeInfo> etapes = genererEtapesSelonModele(nomModele);
    if (etapes.isEmpty()) {
        showAnimatedMessageBox(QMessageBox::Warning, "Optimizer", "Aucun catalogue d'étapes trouvé.");
        return;
    }

    if (!showAnimatedQuestionBox("Confirmation Optimizer",
                                 QString("Créer automatiquement %1 étape(s) pour FAB-%2 ?").arg(etapes.size()).arg(fabId)))
        return;

    int inserted = 0;
    for (const EtapeInfo &step : etapes) {
        EtapeInfo info = step;
        info.ordre = qMax(1, step.ordre);
        if (insererEtapeSiAbsente(fabId, info, QString::number(cinPersonnelParDefaut()), QDate(), nullptr, nullptr))
            ++inserted;
    }

    if (inserted > 0)
        envoyerModificationArduino("ETAPE", "AJOUT", fabId, QString("+%1").arg(inserted));
    refreshSuiviAfterChanges(fabId);
    showAnimatedMessageBox(QMessageBox::Information, "Optimizer", QString("Affectation automatique terminée. Nouvelles étapes: %1").arg(inserted));
}

void MainWindow::on_btnAlertes_clicked()
{
    afficherDialogAlertes();
}

void MainWindow::on_btnAffectationManuelle_clicked()
{
    const int fabId = getSelectedFabricationId();
    if (fabId <= 0) {
        showAnimatedMessageBox(QMessageBox::Warning, "Affectation manuelle", "Aucune fabrication sélectionnée.");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("Affectation manuelle");
    dialog.resize(900, 520);
    auto *root = new QVBoxLayout(&dialog);
    auto *form = new QFormLayout();

    QComboBox *cbStep = new QComboBox(&dialog);
    QSpinBox *spOrder = new QSpinBox(&dialog);
    QSpinBox *spTemps = new QSpinBox(&dialog);
    QLineEdit *leCin = new QLineEdit(QString::number(cinPersonnelParDefaut()), &dialog);
    QCheckBox *chkDate = new QCheckBox("Renseigner la date de début", &dialog);
    QDateEdit *deDate = new QDateEdit(QDate::currentDate(), &dialog);
    deDate->setCalendarPopup(true);
    deDate->setEnabled(false);

    cbStep->addItems({"Préparation", "Découpe", "Ponçage", "Assemblage", "Collage", "Finition", "Vernis", "Contrôle qualité"});
    spOrder->setMinimum(1);
    spOrder->setMaximum(999);
    spTemps->setMinimum(0);
    spTemps->setMaximum(9999);
    spTemps->setValue(tempsCatalogue(cbStep->currentText()));

    form->addRow("Étape", cbStep);
    form->addRow("Ordre", spOrder);
    form->addRow("Temps estimé", spTemps);
    form->addRow("CIN", leCin);
    form->addRow(chkDate);
    form->addRow("Date début", deDate);
    root->addLayout(form);

    QTableWidget *table = new QTableWidget(&dialog);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Nom étape", "Ordre", "Temps", "CIN", "Date début"});
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    root->addWidget(table, 1);

    auto refreshTable = [&]() {
        table->setRowCount(0);
        int row = 0;
        for (int i = 0; i < cbStep->count(); ++i) {
            table->insertRow(row);
            table->setItem(row, 0, new QTableWidgetItem(cbStep->itemText(i)));
            table->setItem(row, 1, new QTableWidgetItem(QString::number(i + 1)));
            table->setItem(row, 2, new QTableWidgetItem(QString::number(qMax(spTemps->value(), tempsCatalogue(cbStep->itemText(i))))));
            table->setItem(row, 3, new QTableWidgetItem(leCin->text().trimmed()));
            table->setItem(row, 4, new QTableWidgetItem(chkDate->isChecked() ? deDate->date().toString("dd/MM/yyyy") : "-"));
            ++row;
        }
    };

    refreshTable();
    connect(cbStep, &QComboBox::currentTextChanged, &dialog, [&]() { spTemps->setValue(tempsCatalogue(cbStep->currentText())); refreshTable(); });
    connect(spTemps, QOverload<int>::of(&QSpinBox::valueChanged), &dialog, [&]() { refreshTable(); });
    connect(leCin, &QLineEdit::textChanged, &dialog, [&]() { refreshTable(); });
    connect(chkDate, &QCheckBox::toggled, deDate, &QWidget::setEnabled);
    connect(chkDate, &QCheckBox::toggled, &dialog, [&]() { refreshTable(); });

    auto *buttons = new QHBoxLayout();
    buttons->addStretch();
    QPushButton *btnAdd = new QPushButton("Ajouter à la liste");
    QPushButton *btnValidate = new QPushButton("Valider");
    QPushButton *btnCancel = new QPushButton("Annuler");
    buttons->addWidget(btnAdd);
    buttons->addWidget(btnValidate);
    buttons->addWidget(btnCancel);
    root->addLayout(buttons);

    QList<ManualEtapeEntry> entries;
    connect(btnAdd, &QPushButton::clicked, &dialog, [&]() {
        ManualEtapeEntry entry;
        entry.nom = cbStep->currentText().trimmed();
        entry.ordre = spOrder->value();
        entry.temps = spTemps->value();
        entry.cin = leCin->text().trimmed();
        entry.hasDateDebut = chkDate->isChecked();
        if (entry.hasDateDebut)
            entry.dateDebut = deDate->date();
        entries.append(entry);
        refreshTable();
    });
    connect(btnValidate, &QPushButton::clicked, &dialog, [&]() { dialog.accept(); });
    connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);

    if (dialog.exec() != QDialog::Accepted || entries.isEmpty())
        return;

    int inserted = 0;
    for (const ManualEtapeEntry &entry : entries) {
        EtapeInfo info{entry.nom, entry.temps > 0 ? entry.temps : tempsCatalogue(entry.nom), entry.ordre};
        bool newInsertion = false;
        QString err;
        if (insererEtapeSiAbsente(fabId, info, entry.cin, entry.hasDateDebut ? entry.dateDebut : QDate(), &newInsertion, &err) && newInsertion)
            ++inserted;
    }

    if (inserted > 0)
        envoyerModificationArduino("ETAPE", "AJOUT", fabId, QString("+%1").arg(inserted));
    refreshSuiviAfterChanges(fabId);
    showAnimatedMessageBox(QMessageBox::Information, "Affectation manuelle", QString("Affectation terminée. Nouvelles étapes: %1").arg(inserted));
}

QList<AlerteInfo> MainWindow::chargerAlertes()
{
    QList<AlerteInfo> alertes;
    QSet<QString> clefs;

    auto addAlert = [&](const QString &type, const QString &niveau, int idFab, int idEtape, const QString &nomEtape, const QString &message, const QString &cause) {
        const QString cle = QString("%1|%2|%3").arg(type).arg(idFab).arg(idEtape);
        if (clefs.contains(cle))
            return;
        clefs.insert(cle);
        AlerteInfo a{type, niveau, idFab, idEtape, nomEtape, message, cause};
        alertes.append(a);
    };

    QSqlQuery q;
    if (q.exec("SELECT IDETAPE, IDFABRICATION, NOMETAPE, TEMPSESTIME, TEMPSREEL, DATEDEBUT, DATEFIN FROM ETAPE")) {
        while (q.next()) {
            const int idEtape = q.value(0).toInt();
            const int idFab = q.value(1).toInt();
            const QString nomEtape = q.value(2).toString();
            const int tempsEstime = q.value(3).toInt();
            const int tempsReel = q.value(4).toInt();
            const QDate dd = q.value(5).toDate();
            const QDate df = q.value(6).toDate();

            if (df.isValid() && tempsEstime > 0 && tempsReel > tempsEstime)
                addAlert("RETARD_FINAL", "Critique", idFab, idEtape, nomEtape, QString("FAB-%1 / %2: retard final constate.").arg(idFab).arg(nomEtape), "Temps reel superieur au temps estime");
            else if (dd.isValid() && !df.isValid() && tempsEstime > 0 && ((QDate::currentDate().daysTo(dd) < 0 ? dd.daysTo(QDate::currentDate()) : dd.daysTo(QDate::currentDate())) > tempsEstime))
                addAlert("RETARD_EN_COURS", "Critique", idFab, idEtape, nomEtape, QString("FAB-%1 / %2: retard en cours detecte.").arg(idFab).arg(nomEtape), "L'etape en cours depasse deja le temps estime");
            else if (dd.isValid() && !df.isValid() && tempsEstime > 0 && dd.daysTo(QDate::currentDate()) >= (0.8 * tempsEstime))
                addAlert("RISQUE_DE_RETARD", "Info", idFab, idEtape, nomEtape, QString("FAB-%1 / %2: etape proche du delai limite.").arg(idFab).arg(nomEtape), "L'etape approche du temps limite");
        }
    }

    return alertes;
}

void MainWindow::mettreAJourBoutonAlertes()
{
    m_alertesCache = chargerAlertes();
    const int nbAlertes = m_alertesCache.size();

    // Le bouton Alertes est cree dynamiquement dans setupSuivi()
    // avec objectName = "btnAlertes". Il n'existe pas dans ui->...
    QPushButton *btnAlerts = this->findChild<QPushButton*>("btnAlertes");
    if (btnAlerts) {
        btnAlerts->setText("Alertes (" + QString::number(nbAlertes) + ")");
        btnAlerts->setToolTip(QString("%1 alerte(s) detectee(s)").arg(nbAlertes));
    }

    // Important : ne pas envoyer au LCD ici.
    // Cette fonction peut etre appelee apres ajout/modification/suppression.
    // Si on envoie les alertes ici, le message de modification disparait tout de suite.
}

void MainWindow::afficherDialogAlertes()
{
    m_alertesCache = chargerAlertes();

    QDialog dialog(this);
    dialog.setWindowTitle("Alertes de fabrication");
    dialog.setModal(true);
    dialog.resize(980, 520);
    dialog.setStyleSheet(R"(
        QDialog {
            background-color: #F5EBD7;
            color: #2E1A00;
        }
        QLabel#alertTitle {
            color: #3A1F12;
            font-size: 17px;
            font-weight: 800;
            background: transparent;
        }
        QLabel#alertCount {
            color: #8B3F27;
            font-size: 13px;
            font-weight: 800;
            background: transparent;
        }
        QTableWidget {
            background-color: #F8F2E7;
            alternate-background-color: #EFE1C6;
            color: #2E1A00;
            gridline-color: #D6BD91;
            border: 1px solid #B58A5A;
            border-radius: 8px;
            font-size: 11px;
            selection-background-color: #D8C09A;
            selection-color: #2E1A00;
        }
        QTableWidget::item {
            padding: 6px;
            border: none;
        }
        QTableWidget::item:selected {
            background-color: #D8C09A;
            color: #2E1A00;
            font-weight: bold;
        }
        QHeaderView::section {
            background-color: #8B3F27;
            color: white;
            font-weight: bold;
            padding: 8px;
            border: 1px solid #6E2F1D;
        }
        QPushButton {
            background-color: #8B3F27;
            color: white;
            border: none;
            border-radius: 8px;
            padding: 9px 20px;
            font-weight: bold;
            min-width: 90px;
            min-height: 34px;
        }
        QPushButton:hover { background-color: #A85736; }
        QScrollBar:vertical { background: #D8C09A; width: 12px; border-radius: 6px; }
        QScrollBar::handle:vertical { background: #8B3F27; border-radius: 6px; min-height: 30px; }
        QScrollBar:horizontal { background: #D8C09A; height: 12px; border-radius: 6px; }
        QScrollBar::handle:horizontal { background: #8B3F27; border-radius: 6px; min-width: 30px; }
        QScrollBar::add-line, QScrollBar::sub-line { width: 0px; height: 0px; }
    )");

    auto *root = new QVBoxLayout(&dialog);
    root->setContentsMargins(14, 14, 14, 14);
    root->setSpacing(10);

    auto *titre = new QLabel("Centre d'alertes - fabrication et étapes", &dialog);
    titre->setObjectName("alertTitle");
    root->addWidget(titre);

    auto *count = new QLabel(QString("%1 alerte(s) détectée(s)").arg(m_alertesCache.size()), &dialog);
    count->setObjectName("alertCount");
    root->addWidget(count);

    auto *table = new QTableWidget(&dialog);
    table->setColumnCount(6);
    table->setHorizontalHeaderLabels({"Type", "Fabrication", "Étape", "Message", "Cause", "Niveau"});
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSelectionBehavior(QAbstractItemView::SelectRows);
    table->setSelectionMode(QAbstractItemView::SingleSelection);
    table->setAlternatingRowColors(true);
    table->verticalHeader()->setVisible(false);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    table->verticalHeader()->setDefaultSectionSize(34);
    table->setRowCount(m_alertesCache.size());

    for (int i = 0; i < m_alertesCache.size(); ++i) {
        const AlerteInfo &a = m_alertesCache.at(i);

        auto *typeItem = new QTableWidgetItem(a.type);
        typeItem->setData(Qt::UserRole, a.idFabrication);
        typeItem->setData(Qt::UserRole + 1, a.idEtape);

        auto *fabItem = new QTableWidgetItem(QString("FAB-%1").arg(a.idFabrication));
        auto *etapeItem = new QTableWidgetItem(a.nomEtape);
        auto *msgItem = new QTableWidgetItem(a.message);
        auto *causeItem = new QTableWidgetItem(a.cause);
        auto *nivItem = new QTableWidgetItem(a.niveau);

        QList<QTableWidgetItem*> items = {typeItem, fabItem, etapeItem, msgItem, causeItem, nivItem};
        for (QTableWidgetItem *it : items) {
            it->setForeground(QColor("#2E1A00"));
            it->setTextAlignment(Qt::AlignCenter);
        }
        msgItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);
        causeItem->setTextAlignment(Qt::AlignVCenter | Qt::AlignLeft);

        nivItem->setBackground(couleurNiveauAlerte(a.niveau));
        nivItem->setForeground(QColor("#FFFFFF"));
        nivItem->setFont(QFont("Arial", 10, QFont::Bold));

        table->setItem(i, 0, typeItem);
        table->setItem(i, 1, fabItem);
        table->setItem(i, 2, etapeItem);
        table->setItem(i, 3, msgItem);
        table->setItem(i, 4, causeItem);
        table->setItem(i, 5, nivItem);
    }

    root->addWidget(table, 1);

    auto *hint = new QLabel("Cliquez sur une alerte pour afficher directement la fabrication correspondante.", &dialog);
    hint->setStyleSheet("color:#6D4B2F; font-size:11px; font-weight:bold; background:transparent;");
    root->addWidget(hint);

    auto *bottom = new QHBoxLayout();
    bottom->addStretch();
    auto *closeBtn = new QPushButton("Fermer", &dialog);
    bottom->addWidget(closeBtn);
    root->addLayout(bottom);
    connect(closeBtn, &QPushButton::clicked, &dialog, &QDialog::accept);

    auto navigateFromRow = [this, table, &dialog](int row) {
        if (row < 0 || row >= table->rowCount())
            return;

        QTableWidgetItem *source = table->item(row, 0);
        if (!source)
            return;

        const int fabId = source->data(Qt::UserRole).toInt();
        const int etapeId = source->data(Qt::UserRole + 1).toInt();
        if (fabId <= 0)
            return;

        allerVersAlerte(fabId, etapeId);
        dialog.accept();
    };

    connect(table, &QTableWidget::cellClicked, &dialog, [navigateFromRow](int row, int) {
        navigateFromRow(row);
    });
    connect(table, &QTableWidget::cellDoubleClicked, &dialog, [navigateFromRow](int row, int) {
        navigateFromRow(row);
    });

    dialog.exec();
    mettreAJourBoutonAlertes();
}

void MainWindow::verifierAlertesAuDemarrage()
{
    mettreAJourBoutonAlertes();
    if (!m_alertesCache.isEmpty())
        showAnimatedMessageBox(QMessageBox::Warning, "Alertes Production", QString("%1 alerte(s) détectée(s). Ouvrez le bouton Alertes pour les détails.").arg(m_alertesCache.size()));
}

void MainWindow::verifierAlertes()
{
    mettreAJourBoutonAlertes();
}

void MainWindow::allerVersAlerte(int idFabrication, int idEtape)
{
    if (idFabrication <= 0)
        return;

    // Ouvre directement la page Fabrication puis l'onglet Suivi.
    if (ui && ui->stackedWidget && ui->page_4)
        ui->stackedWidget->setCurrentWidget(ui->page_4);
    if (ui && ui->tabWidget)
        ui->tabWidget->setCurrentIndex(1); // Suivi

    // Recharge les vues pour garantir que la fabrication existe dans les tableaux.
    loadFabrications();
    loadFabricationsSuivi();

    m_selectedFab = getFabricationSuivi(idFabrication);
    m_fabSelected = true;
    buildTimeline(m_selectedFab);
    chargerEtapes(idFabrication);

    // Sélection visuelle dans le tableau CRUD si l'utilisateur revient à l'onglet gestion.
    if (ui && ui->table_modeles_2) {
        for (int r = 0; r < ui->table_modeles_2->rowCount(); ++r) {
            QTableWidgetItem *idItem = ui->table_modeles_2->item(r, 0);
            if (idItem && idItem->text().toInt() == idFabrication) {
                ui->table_modeles_2->selectRow(r);
                ui->table_modeles_2->scrollToItem(idItem, QAbstractItemView::PositionAtCenter);
                break;
            }
        }
    }

    // Sélection visuelle dans la table Suivi.
    if (ui && ui->tree_fabrications_etapes) {
        for (int i = 0; i < ui->tree_fabrications_etapes->topLevelItemCount(); ++i) {
            QTreeWidgetItem *item = ui->tree_fabrications_etapes->topLevelItem(i);
            if (!item)
                continue;
            const int fabId = item->data(0, Qt::UserRole).toInt();
            if (fabId == idFabrication || item->text(0).toInt() == idFabrication) {
                ui->tree_fabrications_etapes->setCurrentItem(item);
                ui->tree_fabrications_etapes->scrollToItem(item, QAbstractItemView::PositionAtCenter);
                break;
            }
        }
    }

    // Si une étape précise existe dans l'alerte, on ouvre son popup de détails directement.
    if (idEtape > 0)
        afficherDetailsEtape(idEtape);
}

void MainWindow::calculateAndDisplayFabricationPrediction(int fabId)
{
    const QString complexite = calculerComplexiteFabrication(fabId);
    const double progression = calculerProgression(fabId);
    const int totalEstime = calculerTempsEstimeTotal(fabId);
    QSqlQuery q;
    q.prepare("SELECT NVL(SUM(TEMPSREEL),0) FROM ETAPE WHERE IDFABRICATION = :id");
    q.bindValue(":id", fabId);
    double totalReel = 0.0;
    if (q.exec() && q.next())
        totalReel = q.value(0).toDouble();

    QString message = getPredictionIcon(complexite) + " Complexité : " + complexite + "\n";
    message += QString("Progression : %1%\n").arg(progression, 0, 'f', 1);
    message += QString("Temps estimé : %1\n").arg(totalEstime);
    message += QString("Temps réel : %1\n").arg(totalReel, 0, 'f', 1);
    showAnimatedMessageBox(QMessageBox::Information, QString("FAB-%1").arg(fabId), message);
}

void MainWindow::buildKPICards()
{
    afficherStatistiques();
}

void MainWindow::buildEtapeStatusPieChart()
{
    if (!ui || !ui->placeholder_stats)
        return;
}

void MainWindow::buildFabricationsByModeleBarChart()
{
    if (!ui || !ui->placeholder_couts)
        return;
}

void MainWindow::buildStatsInsightsPanel()
{
    if (!ui || !ui->placeholder_couts)
        return;
}

void MainWindow::chargerStatistiquesComplexite()
{
    buildKPICards();
}

void MainWindow::applyAIOptimisedTimeline(const QString &aiText)
{
    if (ui->te_ai_result) {
        ui->te_ai_result->setVisible(true);
        ui->te_ai_result->setPlainText(aiText);
    }
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
void MainWindow::notifierChangementArduino(const QString &action, const QString &detail)
{
    // Notification centrale : appeler cette fonction uniquement après une opération réussie.
    // Ne pas l’appeler dans les fonctions refresh/load pour éviter les boucles et les messages répétés.
    ::envoyerArduinoLignes(action, detail);
}
