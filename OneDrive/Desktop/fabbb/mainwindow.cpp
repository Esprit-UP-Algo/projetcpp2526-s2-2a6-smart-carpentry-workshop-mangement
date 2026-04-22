#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connexion.h"

#include <QSqlError>
#include <QDate>
#include <QDateTime>
#include <QRegularExpression>
#include <QFileDialog>
#include <QPrinter>
#include <QPainter>
#include <QScrollArea>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QHeaderView>
#include <QGroupBox>
#include <QCheckBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QProgressBar>
#include <QComboBox>
#include <QLineEdit>
#include <QScrollArea>
#include <QGridLayout>
#include <QSpacerItem>
#include <QSpinBox>
#include <QDateEdit>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QInputDialog>
#include <QSet>
#include <algorithm>
#include <functional>
#include <QTimer>
#include <QPropertyAnimation>
#include <QGraphicsOpacityEffect>
#include <QEasingCurve>
#include <QTextCursor>
#include <QDebug>
#include <QMouseEvent>
#include <QEnterEvent>
#include <QApplication>
#include <QPointer>
#include <QtMath>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>

// ─────────────────────────────────────────────────────────────
//  Constantes API Anthropic
// ─────────────────────────────────────────────────────────────
const QString MainWindow::ANTHROPIC_API_KEY = "sk-ant-VOTRE_CLE_ICI";
const QString MainWindow::ANTHROPIC_API_URL = "https://api.anthropic.com/v1/messages";
const QString MainWindow::ANTHROPIC_MODEL   = "claude-3-5-sonnet-20241022";

// ─────────────────────────────────────────────────────────────
//  Palette de couleurs globale
// ─────────────────────────────────────────────────────────────
static const QString COLOR_GREEN    = "#27AE60";
static const QString COLOR_ORANGE   = "#E67E22";
static const QString COLOR_RED      = "#C0392B";
static const QString COLOR_BLUE     = "#2980B9";
static const QString COLOR_PURPLE   = "#8E44AD";
static const QString COLOR_DARK     = "#2C3E50";
static const QString COLOR_LIGHT    = "#ECF0F1";
static const QString COLOR_WHITE    = "#FFFFFF";
static const QString COLOR_YELLOW   = "#F39C12";
static const QString COLOR_BEIGE_BG = "#EDE0C8";
static const QString COLOR_BEIGE_CARD = "#FDFAF4";
static const QString COLOR_BROWN_TEXT = "#2E1A00";
static const QString COLOR_BROWN_BORDER = "#B8956A";

static QString alertDialogStyleSheet()
{
    return QStringLiteral(
        "QMessageBox {"
        " background:#FFFFFF;"
        " color:#000000;"
        " border:1px solid #D8CBB8;"
        " border-radius:12px;"
        "}"
        "QMessageBox QLabel {"
        " color:#000000;"
        " background:#FFFFFF;"
        " font-size:14px;"
        "}"
        "QMessageBox QPushButton {"
        " background:#8B4A2F;"
        " color:#FFFFFF;"
        " border:none;"
        " border-radius:8px;"
        " padding:8px 18px;"
        " min-width:80px;"
        " font-weight:600;"
        "}"
        "QMessageBox QPushButton:hover {"
        " background:#6E3823;"
        "}"
        "QMessageBox QPushButton:pressed {"
        " background:#5A2E1D;"
        "}"
    );
}

static void appliquerStyleAlertesGlobal()
{
    if (!qApp)
        return;

    static const QString kAlertStyleMarker = QStringLiteral("/* WOODPILOT_ALERT_STYLE */");
    QString styleActuel = qApp->styleSheet();
    if (styleActuel.contains(kAlertStyleMarker))
        return;

    if (!styleActuel.isEmpty() && !styleActuel.endsWith('\n'))
        styleActuel += '\n';

    styleActuel += kAlertStyleMarker + '\n' + alertDialogStyleSheet();
    qApp->setStyleSheet(styleActuel);
}

class TimelineStepCard : public QFrame
{
public:
    explicit TimelineStepCard(int idEtape, QWidget *parent = nullptr)
        : QFrame(parent)
        , m_idEtape(idEtape)
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

// ─────────────────────────────────────────────────────────────
//  Couleurs selon statut
// ─────────────────────────────────────────────────────────────
static QColor statutColor(const QString &statut)
{
    if (statut == "TERMINE")       return QColor(COLOR_GREEN);
    if (statut == "EN_COURS")      return QColor(COLOR_BLUE);
    if (statut == "RETARD")        return QColor(COLOR_RED);
    if (statut == "NON_COMMENCE")  return QColor("#7F8C8D");
    if (statut == "Planifie")      return QColor(COLOR_GREEN);
    if (statut == "En cours")      return QColor(COLOR_ORANGE);
    return QColor(COLOR_RED);
}

static QString normaliserChampTriFabrication(const QString &champ)
{
    const QString valeur = champ.trimmed().toLower();

    if (valeur == "date debut" || valeur == "date_debut")
        return QStringLiteral("date_debut");
    if (valeur == "quantite" || valeur == "quantite a produire" || valeur == "quantite_a_produire")
        return QStringLiteral("quantite");
    if (valeur == "qualite")
        return QStringLiteral("qualite");
    if (valeur == "id fabrication" || valeur == "id_fabrication" || valeur == "id")
        return QStringLiteral("id_fabrication");

    return QString();
}

static QString expressionTriFabrication(const QString &champ)
{
    if (champ == "modele")
        return QStringLiteral("NVL(M.NOM, TRIM(TO_CHAR(F.IDMODELE)))");
    if (champ == "date_debut")
        return QStringLiteral("F.DATE_DEBUT");
    if (champ == "quantite")
        return QStringLiteral("F.QUANTITE_A_PRODUIRE");
    if (champ == "qualite")
        return QStringLiteral("F.QUALITE");

    return QStringLiteral("F.IDFABRICATION");
}

static void remplirTableFabrications(QTableWidget *table, QSqlQuery &query)
{
    if (!table)
        return;

    table->setRowCount(0);

    int row = 0;
    while (query.next()) {
        table->insertRow(row);
        table->setItem(row, 0, new QTableWidgetItem(query.value(0).toString()));

        auto *modeleItem = new QTableWidgetItem(query.value(2).toString());
        modeleItem->setData(Qt::UserRole, query.value(1));
        table->setItem(row, 1, modeleItem);

        table->setItem(row, 2, new QTableWidgetItem(query.value(3).toDate().toString("dd/MM/yyyy")));
        table->setItem(row, 3, new QTableWidgetItem(query.value(4).toString()));
        table->setItem(row, 4, new QTableWidgetItem(query.value(5).toString()));
        table->setItem(row, 5, new QTableWidgetItem(query.value(6).toString()));
        ++row;
    }
}

static QString formaterRetardMinutes(int retardMinutes)
{
    if (retardMinutes <= 0)
        return QStringLiteral("-");

    if (retardMinutes < 60)
        return QStringLiteral("%1 min").arg(retardMinutes);

    const int heures = retardMinutes / 60;
    const int minutes = retardMinutes % 60;
    if (minutes == 0)
        return QStringLiteral("%1 h").arg(heures);

    return QStringLiteral("%1 h %2 min").arg(heures).arg(minutes);
}

void MainWindow::refreshFabricationsTable(const QString &searchText)
{
    if (!ui || !ui->table_modeles_2)
        return;

    const QString champTri = normaliserChampTriFabrication(currentSortField);
    const QString expressionTri = expressionTriFabrication(champTri);
    const QString sensTri = (currentSortOrder == Qt::DescendingOrder) ? " DESC" : " ASC";

    QString requeteSql =
        "SELECT F.IDFABRICATION, F.IDMODELE, "
        "       NVL(M.NOM, TRIM(TO_CHAR(F.IDMODELE))) AS NOM_MODELE, "
        "       F.DATE_DEBUT, F.QUANTITE_A_PRODUIRE, F.QUALITE, F.COMMENTAIRE "
        "FROM FABRICATION F "
        "LEFT JOIN MODELE M ON (TRIM(TO_CHAR(M.IDMODELE)) = TRIM(TO_CHAR(F.IDMODELE)) "
        "                     OR UPPER(TRIM(M.NOM)) = UPPER(TRIM(TO_CHAR(F.IDMODELE)))) ";

    const QString filtre = searchText.trimmed();
    if (!filtre.isEmpty()) {
        requeteSql +=
            "WHERE UPPER(TO_CHAR(F.IDFABRICATION)) LIKE :s "
            "OR UPPER(TO_CHAR(F.IDMODELE)) LIKE :s "
            "OR UPPER(NVL(M.NOM, TRIM(TO_CHAR(F.IDMODELE)))) LIKE :s "
            "OR UPPER(TO_CHAR(F.DATE_DEBUT,'DD/MM/YYYY')) LIKE :s "
            "OR UPPER(TO_CHAR(F.QUANTITE_A_PRODUIRE)) LIKE :s "
            "OR UPPER(F.QUALITE) LIKE :s "
            "OR UPPER(F.COMMENTAIRE) LIKE :s ";
    }

    requeteSql += QStringLiteral("ORDER BY %1%2").arg(expressionTri, sensTri);

    QSqlQuery query;
    query.prepare(requeteSql);

    if (!filtre.isEmpty())
        query.bindValue(":s", "%" + filtre.toUpper() + "%");

    if (!query.exec()) {
        showAnimatedMessageBox(QMessageBox::Critical, "Erreur",
                               "Impossible de charger les fabrications:\n" + query.lastError().text());
        return;
    }

    remplirTableFabrications(ui->table_modeles_2, query);
    ui->table_modeles_2->resizeColumnsToContents();
}

void MainWindow::trierFabrications(const QString &champ)
{
    const QString champNormalise = normaliserChampTriFabrication(champ);
    if (champNormalise.isEmpty())
        return;

    if (currentSortField == champNormalise) {
        currentSortOrder = (currentSortOrder == Qt::AscendingOrder)
                               ? Qt::DescendingOrder
                               : Qt::AscendingOrder;
    } else {
        currentSortField = champNormalise;
        currentSortOrder = Qt::AscendingOrder;
    }

    refreshFabricationsTable(m_currentFabricationSearchText);
}

void MainWindow::sortFabrications(const QString &criteria)
{
    trierFabrications(criteria);
}

struct ManualEtapeEntry
{
    QString nom;
    int ordre = 1;
    int temps = 0;
    QString cin;
    QDate dateDebut;
    bool hasDateDebut = false;
};

class AffectationManuelleDialog : public QDialog
{
public:
    explicit AffectationManuelleDialog(const QMap<QString, QList<EtapeInfo>> &catalogue,
                                      QWidget *parent = nullptr)
        : QDialog(parent)
        , m_catalogue(catalogue)
    {
        setWindowTitle("Affectation manuelle");
        setModal(true);
        resize(960, 600);

        QVBoxLayout *root = new QVBoxLayout(this);
        root->setContentsMargins(14, 14, 14, 14);
        root->setSpacing(10);

        QLabel *intro = new QLabel("Composer les étapes à affecter avant validation.");
        intro->setStyleSheet("color:#2E1A00; font-weight:bold;");
        root->addWidget(intro);

        QHBoxLayout *top = new QHBoxLayout();
        top->setSpacing(12);
        root->addLayout(top);

        QGroupBox *leftBox = new QGroupBox("Étapes disponibles");
        QVBoxLayout *leftLayout = new QVBoxLayout(leftBox);
        m_comboStep = new QComboBox();
        m_comboStep->setEditable(false);
        leftLayout->addWidget(m_comboStep);

        QGroupBox *rightBox = new QGroupBox("Paramètres");
        QGridLayout *form = new QGridLayout(rightBox);

        m_spinOrdre = new QSpinBox();
        m_spinOrdre->setMinimum(1);
        m_spinOrdre->setMaximum(999);

        m_spinTemps = new QSpinBox();
        m_spinTemps->setMinimum(0);
        m_spinTemps->setMaximum(9999);

        m_editCin = new QLineEdit();
        m_editCin->setPlaceholderText("Optionnel");
        m_editCin->setText(QStringLiteral("12345677"));
        m_editCin->setReadOnly(true);

        m_chkDate = new QCheckBox("Renseigner la date de début");
        m_dateDebut = new QDateEdit(QDate::currentDate());
        m_dateDebut->setCalendarPopup(true);
        m_dateDebut->setEnabled(false);

        form->addWidget(new QLabel("Ordre :"), 0, 0);
        form->addWidget(m_spinOrdre, 0, 1);
        form->addWidget(new QLabel("Temps estimé :"), 1, 0);
        form->addWidget(m_spinTemps, 1, 1);
        form->addWidget(new QLabel("CIN :"), 2, 0);
        form->addWidget(m_editCin, 2, 1);
        form->addWidget(m_chkDate, 3, 0, 1, 2);
        form->addWidget(new QLabel("Date début :"), 4, 0);
        form->addWidget(m_dateDebut, 4, 1);

        top->addWidget(leftBox, 2);
        top->addWidget(rightBox, 3);

        m_table = new QTableWidget();
        m_table->setColumnCount(5);
        m_table->setHorizontalHeaderLabels({"Nom étape", "Ordre", "Temps", "CIN", "Date début"});
        m_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
        m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
        m_table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
        m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_table->setSelectionMode(QAbstractItemView::SingleSelection);
        m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_table->verticalHeader()->setVisible(false);
        m_table->setAlternatingRowColors(true);
        root->addWidget(m_table, 1);

        QHBoxLayout *buttons = new QHBoxLayout();
        buttons->addStretch();
        QPushButton *btnAdd = new QPushButton("Ajouter à la liste");
        QPushButton *btnValidate = new QPushButton("Valider affectation");
        QPushButton *btnCancel = new QPushButton("Annuler");
        buttons->addWidget(btnAdd);
        buttons->addWidget(btnValidate);
        buttons->addWidget(btnCancel);
        root->addLayout(buttons);

        fillCatalogueSteps();
        syncDefaultTemps();
        refreshTable();

        connect(m_comboStep, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this]() { syncDefaultTemps(); });
        connect(m_chkDate, &QCheckBox::toggled, m_dateDebut, &QWidget::setEnabled);
        connect(btnAdd, &QPushButton::clicked, this, [this]() { addOrUpdateCurrentStep(); });
        connect(btnValidate, &QPushButton::clicked, this, [this]() { validateAndAccept(); });
        connect(btnCancel, &QPushButton::clicked, this, &QDialog::reject);
    }

    QList<ManualEtapeEntry> entries() const
    {
        return m_entries;
    }

private:
    void fillCatalogueSteps()
    {
        const QStringList allowedSteps = {
            "Préparation", "Découpe", "Ponçage", "Assemblage",
            "Collage", "Finition", "Vernis", "Contrôle qualité"
        };

        QSet<QString> uniqueNames;
        for (auto it = m_catalogue.constBegin(); it != m_catalogue.constEnd(); ++it) {
            for (const EtapeInfo &step : it.value()) {
                if (!allowedSteps.contains(step.nom) || uniqueNames.contains(step.nom))
                    continue;

                    uniqueNames.insert(step.nom);
                    m_comboStep->addItem(step.nom, step.temps);
            }
        }
    }

    void syncDefaultTemps()
    {
        int temps = m_comboStep->currentData().toInt();
        if (temps <= 0)
            temps = 60;
        m_spinTemps->setValue(temps);
        if (m_spinOrdre->value() < 1)
            m_spinOrdre->setValue(1);
    }

    void addOrUpdateCurrentStep()
    {
        const QString nom = m_comboStep->currentText().trimmed();
        if (nom.isEmpty())
            return;

        ManualEtapeEntry entry;
        entry.nom = nom;
        entry.ordre = m_spinOrdre->value();
        entry.temps = m_spinTemps->value();
        entry.cin = m_editCin->text().trimmed();
        entry.hasDateDebut = m_chkDate->isChecked();
        if (entry.hasDateDebut)
            entry.dateDebut = m_dateDebut->date();

        bool updated = false;
        for (ManualEtapeEntry &existing : m_entries) {
            if (existing.nom.compare(entry.nom, Qt::CaseInsensitive) == 0) {
                existing = entry;
                updated = true;
                break;
            }
        }

        if (!updated)
            m_entries.append(entry);

        std::sort(m_entries.begin(), m_entries.end(), [](const ManualEtapeEntry &a, const ManualEtapeEntry &b) {
            if (a.ordre == b.ordre)
                return a.nom.toLower() < b.nom.toLower();
            return a.ordre < b.ordre;
        });

        refreshTable();
    }

    void refreshTable()
    {
        m_table->setRowCount(0);
        for (int row = 0; row < m_entries.size(); ++row) {
            const ManualEtapeEntry &entry = m_entries.at(row);
            m_table->insertRow(row);

            auto *nomItem = new QTableWidgetItem(entry.nom);
            auto *ordreItem = new QTableWidgetItem(QString::number(entry.ordre));
            auto *tempsItem = new QTableWidgetItem(QString::number(entry.temps));
            auto *cinItem = new QTableWidgetItem(entry.cin.isEmpty() ? "-" : entry.cin);
            auto *dateItem = new QTableWidgetItem(entry.hasDateDebut ? entry.dateDebut.toString("dd/MM/yyyy") : "-");

            nomItem->setFlags(nomItem->flags() & ~Qt::ItemIsEditable);
            ordreItem->setFlags(ordreItem->flags() & ~Qt::ItemIsEditable);
            tempsItem->setFlags(tempsItem->flags() & ~Qt::ItemIsEditable);
            cinItem->setFlags(cinItem->flags() & ~Qt::ItemIsEditable);
            dateItem->setFlags(dateItem->flags() & ~Qt::ItemIsEditable);

            nomItem->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
            ordreItem->setTextAlignment(Qt::AlignCenter);
            tempsItem->setTextAlignment(Qt::AlignCenter);
            cinItem->setTextAlignment(Qt::AlignCenter);
            dateItem->setTextAlignment(Qt::AlignCenter);

            m_table->setItem(row, 0, nomItem);
            m_table->setItem(row, 1, ordreItem);
            m_table->setItem(row, 2, tempsItem);
            m_table->setItem(row, 3, cinItem);
            m_table->setItem(row, 4, dateItem);
        }
    }

    void validateAndAccept()
    {
        if (m_entries.isEmpty()) {
            QMessageBox::warning(this, "Affectation manuelle", "Ajoutez au moins une étape avant de valider.");
            return;
        }
        accept();
    }

    QMap<QString, QList<EtapeInfo>> m_catalogue;
    QList<ManualEtapeEntry> m_entries;
    QComboBox *m_comboStep = nullptr;
    QSpinBox *m_spinOrdre = nullptr;
    QSpinBox *m_spinTemps = nullptr;
    QLineEdit *m_editCin = nullptr;
    QCheckBox *m_chkDate = nullptr;
    QDateEdit *m_dateDebut = nullptr;
    QTableWidget *m_table = nullptr;
};

// ═════════════════════════════════════════════════════════════
//  PARTIE 1 — LOGIQUE MÉTIER : calcul statut d'une étape
// ═════════════════════════════════════════════════════════════

/*
 * Règle de calcul du statut d'une étape (sans champ STATUT en base) :
 *   - DATEDEBUT null                            → NON_COMMENCE
 *   - DATEDEBUT non null, DATEFIN non null      → TERMINE
 *   - DATEDEBUT non null, DATEFIN null,
 *     jours écoulés > TEMPSESTIME              → RETARD
 *   - DATEDEBUT non null, DATEFIN null          → EN_COURS
 */
QString MainWindow::calculerStatutEtape(const QDate &dateDebut,
                                        const QDate &dateFin,
                                        double       tempsEstime)
{
    if (!dateDebut.isValid())
        return "NON_COMMENCE";

    if (dateFin.isValid())
        return "TERMINE";

    // En cours : vérifier si en retard
    int joursEcoules = dateDebut.daysTo(QDate::currentDate());
    if (joursEcoules > tempsEstime && tempsEstime > 0)
        return "RETARD";

    return "EN_COURS";
}

/*
 * Statut global d'une fabrication calculé à partir de ses étapes :
 *   - TERMINE       : toutes les étapes sont TERMINE
 *   - RETARD        : au moins une étape est RETARD
 *   - EN_COURS      : au moins une étape est EN_COURS ou RETARD
 *   - NON_COMMENCE  : aucune étape commencée
 */
QString MainWindow::calculerStatutFabrication(int fabId)
{
    QSqlQuery q;
    q.prepare(
        "SELECT DATEDEBUT, DATEFIN, TEMPSESTIME "
        "FROM ETAPE WHERE IDFABRICATION = :id");
    q.bindValue(":id", fabId);

    if (!q.exec()) return "NON_COMMENCE";

    int total = 0, termine = 0, enCours = 0, retard = 0;

    while (q.next()) {
        total++;
        QDate dd = q.value(0).toDate();
        QDate df = q.value(1).toDate();
        double te = q.value(2).toDouble();

        QString s = calculerStatutEtape(dd, df, te);
        if (s == "TERMINE")      termine++;
        else if (s == "RETARD")  retard++;
        else if (s == "EN_COURS") enCours++;
    }

    if (total == 0)            return "NON_COMMENCE";
    if (termine == total)      return "TERMINE";
    if (retard > 0)            return "RETARD";
    if (enCours > 0 || retard > 0) return "EN_COURS";
    return "NON_COMMENCE";
}

/*
 * Progression d'une fabrication = étapes terminées / étapes totales × 100
 */
double MainWindow::calculerProgression(int fabId)
{
    QSqlQuery q;
    q.prepare(
        "SELECT COUNT(*) as total, "
        "SUM(CASE WHEN DATEFIN IS NOT NULL THEN 1 ELSE 0 END) as termine "
        "FROM ETAPE WHERE IDFABRICATION = :id");
    q.bindValue(":id", fabId);

    if (q.exec() && q.next()) {
        int total   = q.value(0).toInt();
        int termine = q.value(1).toInt();
        if (total > 0) return (double)termine / total * 100.0;
    }
    return 0.0;
}

int MainWindow::compterEtapesFabrication(int idFabrication)
{
    if (idFabrication <= 0)
        return 0;

    QSqlQuery q;
    q.prepare(
        "SELECT COUNT(*) "
        "FROM ETAPE "
        "WHERE IDFABRICATION = :idFab");
    q.bindValue(":idFab", idFabrication);

    if (!q.exec() || !q.next())
        return 0;

    return q.value(0).toInt();
}

int MainWindow::calculerTempsEstimeTotal(int idFabrication)
{
    if (idFabrication <= 0)
        return 0;

    QSqlQuery q;
    q.prepare(
        "SELECT NVL(SUM(TEMPSESTIME), 0) "
        "FROM ETAPE "
        "WHERE IDFABRICATION = :idFab");
    q.bindValue(":idFab", idFabrication);

    if (!q.exec() || !q.next())
        return 0;

    return q.value(0).toInt();
}

QString MainWindow::calculerComplexiteFabrication(int idFabrication)
{
    const int nbEtapes = compterEtapesFabrication(idFabrication);
    const int totalTempsEstime = calculerTempsEstimeTotal(idFabrication);

    QString complexite = QStringLiteral("Faible");

    if (nbEtapes <= 3) {
        complexite = QStringLiteral("Faible");
    } else if (nbEtapes <= 5) {
        complexite = QStringLiteral("Moyenne");
    } else if (nbEtapes <= 7) {
        complexite = QStringLiteral("Élevée");
    } else {
        complexite = QStringLiteral("Critique");
    }

    // Ajustement secondaire par charge estimée globale.
    if (totalTempsEstime >= 1600) {
        complexite = QStringLiteral("Critique");
    } else if (totalTempsEstime >= 1000 && complexite != QStringLiteral("Critique")) {
        complexite = QStringLiteral("Élevée");
    } else if (totalTempsEstime >= 600 && complexite == QStringLiteral("Faible")) {
        complexite = QStringLiteral("Moyenne");
    }

    return complexite;
}

QString MainWindow::predireStatutFabrication(int fabId)
{
    // Compatibilité avec les appels historiques: la prédiction est remplacée par la complexité.
    return calculerComplexiteFabrication(fabId);
}

QString MainWindow::couleurComplexiteBadge(const QString &complexite) const
{
    if (complexite == QStringLiteral("Faible"))
        return QStringLiteral("#27AE60");
    if (complexite == QStringLiteral("Moyenne"))
        return QStringLiteral("#E67E22");
    if (complexite == QStringLiteral("Élevée"))
        return QStringLiteral("#C0392B");
    if (complexite == QStringLiteral("Critique"))
        return QStringLiteral("#8B0000");
    return QStringLiteral("#8D6E63");
}

QColor MainWindow::getPredictionColor(const QString &status)
{
    return QColor(couleurComplexiteBadge(status));
}

QString MainWindow::getPredictionIcon(const QString &status)
{
    if (status == QStringLiteral("Faible"))
        return QStringLiteral("🟢");
    if (status == QStringLiteral("Moyenne"))
        return QStringLiteral("🟠");
    if (status == QStringLiteral("Élevée"))
        return QStringLiteral("🔴");
    if (status == QStringLiteral("Critique"))
        return QStringLiteral("🔴");
    return QStringLiteral("⚪");
}

double MainWindow::calculatePerformanceRatio(double tempsReel, double tempsEstime)
{
    if (tempsEstime <= 0) return 0.0;
    return tempsReel / tempsEstime;
}

QString MainWindow::getPredictionStatus(double ratio)
{
    if (ratio <= 1.0) return QStringLiteral("Faible");
    if (ratio <= 1.2) return QStringLiteral("Moyenne");
    if (ratio <= 1.4) return QStringLiteral("Élevée");
    return QStringLiteral("Critique");
}

// ─────────────────────────────────────────────────────────────
//  Étape actuelle d'une fabrication
// ─────────────────────────────────────────────────────────────
QString MainWindow::getEtapeActuelle(int fabId)
{
    // 1) Chercher étape EN_COURS (DATEDEBUT non null, DATEFIN null)
    QSqlQuery q;
    q.prepare(
        "SELECT NOMETAPE FROM ETAPE "
        "WHERE IDFABRICATION = :id "
        "  AND DATEDEBUT IS NOT NULL AND DATEFIN IS NULL "
        "ORDER BY ORDRE ASC");
    q.bindValue(":id", fabId);
    if (q.exec() && q.next()) return q.value(0).toString();

    // 2) Première étape NON_COMMENCE
    q.prepare(
        "SELECT NOMETAPE FROM ETAPE "
        "WHERE IDFABRICATION = :id AND DATEDEBUT IS NULL "
        "ORDER BY ORDRE ASC");
    q.bindValue(":id", fabId);
    if (q.exec() && q.next()) return q.value(0).toString();

    // 3) Tout terminé → dernière étape
    q.prepare(
        "SELECT NOMETAPE FROM ETAPE "
        "WHERE IDFABRICATION = :id "
        "ORDER BY ORDRE DESC");
    q.bindValue(":id", fabId);
    if (q.exec() && q.next()) return q.value(0).toString();

    return "Aucune";
}

// ─────────────────────────────────────────────────────────────
//  Prochaine étape d'une fabrication
// ─────────────────────────────────────────────────────────────
QString MainWindow::getProchainEtape(int fabId)
{
    // Trouver l'ordre de l'étape actuelle
    QSqlQuery q;
    q.prepare(
        "SELECT ORDRE FROM ETAPE "
        "WHERE IDFABRICATION = :id "
        "  AND DATEDEBUT IS NOT NULL AND DATEFIN IS NULL "
        "ORDER BY ORDRE ASC");
    q.bindValue(":id", fabId);

    int ordreActuel = -1;
    if (q.exec() && q.next()) {
        ordreActuel = q.value(0).toInt();
    } else {
        // Pas d'étape en cours : chercher la première non commencée
        q.prepare(
            "SELECT ORDRE FROM ETAPE "
            "WHERE IDFABRICATION = :id AND DATEDEBUT IS NULL "
            "ORDER BY ORDRE ASC");
        q.bindValue(":id", fabId);
        if (q.exec() && q.next()) ordreActuel = q.value(0).toInt() - 1;
    }

    if (ordreActuel < 0) return "Aucune";

    q.prepare(
        "SELECT NOMETAPE FROM ETAPE "
        "WHERE IDFABRICATION = :id AND ORDRE > :ordre "
        "ORDER BY ORDRE ASC");
    q.bindValue(":id", fabId);
    q.bindValue(":ordre", ordreActuel);
    if (q.exec() && q.next()) return q.value(0).toString();

    return "Aucune";
}

// ═════════════════════════════════════════════════════════════
//  CONSTRUCTEUR / DESTRUCTEUR
// ═════════════════════════════════════════════════════════════
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_fabSelected(false)
    , m_currentFabIndex(-1)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
{
    ui->setupUi(this);
    appliquerStyleAlertesGlobal();

    ui->aff_date_deb->setMinimumDate(QDate::currentDate());
    ui->aff_date_deb->setDate(QDate::currentDate());

    ui->table_modeles_2->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->table_modeles_2->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->table_modeles_2->setSortingEnabled(false);
    ui->table_modeles_2->horizontalHeader()->setSectionsClickable(true);
    connect(ui->table_modeles_2->horizontalHeader(), &QHeaderView::sectionClicked,
            this, [this](int section) {
                switch (section) {
                case 1:
                    trierFabrications(QStringLiteral("modele"));
                    break;
                case 2:
                    trierFabrications(QStringLiteral("date_debut"));
                    break;
                case 3:
                    trierFabrications(QStringLiteral("quantite"));
                    break;
                case 4:
                    trierFabrications(QStringLiteral("qualite"));
                    break;
                default:
                    break;
                }
            });

    Connexion &conn = Connexion::createInstance();
    if (conn.createconnect()) {
        qDebug() << "Database connected successfully!";
        loadFabrications();
        generateFabricationId();
    } else {
        showAnimatedMessageBox(QMessageBox::Critical, "Erreur",
                               "Impossible de se connecter a la base de donnees Oracle.\n\n"
                               "Verifiez la source ODBC 'OracleXE' ou 'woodpilot'.");
    }

    connect(ui->rech_7, &QLineEdit::textChanged,
            this, &MainWindow::on_rech_7_textChanged);

    setupSuivi();
}

MainWindow::~MainWindow()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply->deleteLater();
    }
    delete ui;
}

// ═════════════════════════════════════════════════════════════
//  SETUP SUIVI
// ═════════════════════════════════════════════════════════════
void MainWindow::setupSuivi()
{
    // Supprimer visuellement le chatbot et son bloc résultat de la zone Suivi.
    if (ui->te_ai_result) {
        ui->te_ai_result->setVisible(false);
        ui->te_ai_result->setMinimumHeight(0);
        ui->te_ai_result->setMaximumHeight(0);
    }

    if (QGroupBox *gbChatbot = this->findChild<QGroupBox*>("gb_chatbot")) {
        gbChatbot->setVisible(false);
        gbChatbot->setMinimumHeight(0);
        gbChatbot->setMaximumHeight(0);
    }

    // Créer un onglet "Statistique" dédié pour les KPI.
    if (ui->tabWidget && !this->findChild<QWidget*>("tab_statistique")) {
        QWidget *tabStat = new QWidget();
        tabStat->setObjectName("tab_statistique");

        auto *vlStat = new QVBoxLayout(tabStat);
        vlStat->setObjectName("vl_statistiques_global");
        vlStat->setContentsMargins(12, 12, 12, 12);
        vlStat->setSpacing(10);

        QLabel *title = new QLabel("Statistiques Globales");
        title->setStyleSheet("color:#4A2800; font-size:16px; font-weight:bold; background:transparent;");
        vlStat->addWidget(title);

        QFrame *kpiFrame = new QFrame(tabStat);
        kpiFrame->setObjectName("frame_kpi_cards_dynamic");
        kpiFrame->setStyleSheet("QFrame#frame_kpi_cards_dynamic { background: transparent; }");

        auto *hlKpi = new QHBoxLayout(kpiFrame);
        hlKpi->setObjectName("hl_kpi_cards");
        hlKpi->setContentsMargins(0, 0, 0, 0);
        hlKpi->setSpacing(8);
        vlStat->addWidget(kpiFrame);
        vlStat->addStretch();

        ui->tabWidget->addTab(tabStat, "Statistique");
    }

    // Légende colorée de la timeline.
    if (QLabel *lbl = this->findChild<QLabel*>("lbl_leg_v"))
        lbl->setStyleSheet("color:#27AE60; font-size:11px; font-weight:bold; background:transparent; padding-left:10px;");
    if (QLabel *lbl = this->findChild<QLabel*>("lbl_leg_o"))
        lbl->setStyleSheet("color:#E67E22; font-size:11px; font-weight:bold; background:transparent; padding-left:10px;");
    if (QLabel *lbl = this->findChild<QLabel*>("lbl_leg_r"))
        lbl->setStyleSheet("color:#C0392B; font-size:11px; font-weight:bold; background:transparent; padding-left:10px;");

    if (QPushButton *btnChatbot = this->findChild<QPushButton*>("btn_chatbot")) {
        btnChatbot->setVisible(false);
    }

    // Supprimer le panneau détail dynamique pour garder une timeline propre.
    if (QFrame *detail = this->findChild<QFrame*>("frame_detail_fab_dynamic")) {
        detail->deleteLater();
    }

    // Construire les KPI cards dynamiquement
    buildKPICards();

    // Construire les filtres du suivi
    buildFiltresSuivi();

    // Barre d'actions au-dessus de la timeline.
    if (ui->vl_timeline_box && !this->findChild<QWidget*>("suivi_actions_bar")) {
        QWidget *actionBar = new QWidget(ui->gb_timeline_visuelle);
        actionBar->setObjectName("suivi_actions_bar");
        auto *actionLayout = new QHBoxLayout(actionBar);
        actionLayout->setContentsMargins(8, 4, 8, 4);
        actionLayout->setSpacing(8);

        QPushButton *btnOptimizer = new QPushButton("Optimizer", actionBar);
        btnOptimizer->setObjectName("btnOptimizer");
        btnOptimizer->setStyleSheet(
            "QPushButton { background:#27AE60; color:white; font-weight:bold; "
            "border:none; border-radius:8px; padding:8px 14px; }"
            "QPushButton:hover { background:#219150; }"
            "QPushButton:pressed { background:#1E8449; }");

        QPushButton *btnAffectation = new QPushButton("Affectation manuelle", actionBar);
        btnAffectation->setObjectName("btnAffectationManuelle");
        btnAffectation->setStyleSheet(
            "QPushButton { background:#E67E22; color:white; font-weight:bold; "
            "border:none; border-radius:8px; padding:8px 14px; }"
            "QPushButton:hover { background:#CF711C; }"
            "QPushButton:pressed { background:#A85A13; }");

        actionLayout->addStretch();
        actionLayout->addWidget(btnOptimizer);
        actionLayout->addWidget(btnAffectation);

        ui->vl_timeline_box->insertWidget(1, actionBar);

        connect(btnOptimizer, &QPushButton::clicked,
                this, &MainWindow::on_btnOptimizer_clicked);
        connect(btnAffectation, &QPushButton::clicked,
                this, &MainWindow::on_btnAffectationManuelle_clicked);
    }

    // Boutons de pilotage des étapes (si présents dans l'interface).
    if (QPushButton *btnDemarrer = this->findChild<QPushButton*>("btnDemarrerEtape")) {
        connect(btnDemarrer, &QPushButton::clicked,
                this, &MainWindow::on_btnDemarrerEtape_clicked);
    }
    if (QPushButton *btnTerminer = this->findChild<QPushButton*>("btnTerminerEtape")) {
        connect(btnTerminer, &QPushButton::clicked,
                this, &MainWindow::on_btnTerminerEtape_clicked);
    }

    loadEtapesCatalogue();
    loadFabricationsSuivi();
    reloadAllFabIds();

    if (!m_allFabIds.isEmpty()) {
        for (int r = 0; r < ui->table_fabrications_etapes->rowCount(); r++) {
            if (ui->table_fabrications_etapes->item(r, 0) &&
                ui->table_fabrications_etapes->item(r, 0)->text().toInt() == m_allFabIds.first()) {
                ui->table_fabrications_etapes->setCurrentCell(r, 0);
                on_table_fabrications_etapes_cellClicked(r, 0);
                break;
            }
        }
    }

    connect(ui->table_fabrications_etapes, &QTableWidget::cellClicked,
            this, &MainWindow::on_table_fabrications_etapes_cellClicked);

}

void MainWindow::reloadAllFabIds()
{
    m_allFabIds.clear();
    m_currentFabIndex = -1;

    QSqlQuery q;
    q.prepare("SELECT IDFABRICATION FROM FABRICATION ORDER BY IDFABRICATION");
    if (q.exec())
        while (q.next())
            m_allFabIds.append(q.value(0).toInt());

    qDebug() << "Fabrications IDs charges:" << m_allFabIds;
}

QMap<QString, QList<EtapeInfo>> MainWindow::getCatalogue()
{
    // Compatibilité avec l'affectation manuelle existante.
    return getCatalogueParModele();
}

QMap<QString, QList<EtapeInfo>> MainWindow::getCatalogueParModele()
{
    QMap<QString, QList<EtapeInfo>> catalogue;

    // Modèle court: exemple demandé (2 étapes)
    catalogue.insert("Chaise", QList<EtapeInfo>{
        {"Découpe", 90, 1},
        {"Assemblage", 120, 2}
    });

    // Modèle moyen: exemple demandé (5 étapes)
    catalogue.insert("Table", QList<EtapeInfo>{
        {"Préparation", 60, 1},
        {"Découpe", 120, 2},
        {"Ponçage", 90, 3},
        {"Assemblage", 150, 4},
        {"Vernis", 60, 5}
    });

    // Modèle long: exemple demandé (7 étapes)
    catalogue.insert("Salon", QList<EtapeInfo>{
        {"Préparation", 75, 1},
        {"Découpe", 120, 2},
        {"Ponçage", 90, 3},
        {"Assemblage", 180, 4},
        {"Collage", 110, 5},
        {"Finition", 80, 6},
        {"Contrôle qualité", 45, 7}
    });

    // Autres modèles disponibles dans l'application
    catalogue.insert("Armoire", QList<EtapeInfo>{
        {"Préparation", 70, 1},
        {"Découpe", 150, 2},
        {"Perçage", 90, 3},
        {"Assemblage", 180, 4},
        {"Finition", 75, 5}
    });

    // Fallback générique si le nom du modèle ne matche aucun catalogue dédié.
    catalogue.insert("Generique", QList<EtapeInfo>{
        {"Préparation", 60, 1},
        {"Découpe", 100, 2},
        {"Assemblage", 120, 3}
    });

    return catalogue;
}

QString MainWindow::getNomModelePourFabrication(int idFabrication)
{
    if (idFabrication <= 0)
        return QString();

    QSqlQuery q;
    q.prepare(
        "SELECT M.NOM "
        "FROM FABRICATION F "
        "LEFT JOIN MODELE M ON (TRIM(TO_CHAR(M.IDMODELE)) = TRIM(TO_CHAR(F.IDMODELE)) "
        "                     OR UPPER(TRIM(M.NOM)) = UPPER(TRIM(TO_CHAR(F.IDMODELE)))) "
        "WHERE F.IDFABRICATION = :id");
    q.bindValue(":id", idFabrication);

    if (q.exec() && q.next())
        return q.value(0).toString().trimmed();

    // Fallback pour les schémas MODELE utilisant la colonne NOM.
    QSqlQuery qFallback;
    qFallback.prepare(
        "SELECT M.NOM "
        "FROM FABRICATION F "
        "LEFT JOIN MODELE M ON (TRIM(TO_CHAR(M.IDMODELE)) = TRIM(TO_CHAR(F.IDMODELE)) "
        "                     OR UPPER(TRIM(M.NOM)) = UPPER(TRIM(TO_CHAR(F.IDMODELE)))) "
        "WHERE F.IDFABRICATION = :id");
    qFallback.bindValue(":id", idFabrication);

    if (qFallback.exec() && qFallback.next())
        return qFallback.value(0).toString().trimmed();

    qDebug() << "getNomModelePourFabrication erreur:" << q.lastError().text()
             << " / fallback:" << qFallback.lastError().text();
    return QString();
}

QList<EtapeInfo> MainWindow::genererEtapesSelonModele(const QString &nomModele)
{
    const QString mode = nomModele.trimmed();
    const QString modeLower = mode.toLower();

    const QMap<QString, QList<EtapeInfo>> catalogue = getCatalogueParModele();

    // Match direct d'abord
    if (catalogue.contains(mode))
        return catalogue.value(mode);

    // Match souple ensuite (contains en minuscule)
    for (auto it = catalogue.constBegin(); it != catalogue.constEnd(); ++it) {
        if (it.key().toLower() == "generique")
            continue;
        if (modeLower.contains(it.key().toLower()))
            return it.value();
    }

    return catalogue.value("Generique");
}

int MainWindow::getSelectedFabricationId()
{
    // Priorité à la sélection explicite dans le tableau de suivi.
    if (ui && ui->table_fabrications_etapes) {
        int row = ui->table_fabrications_etapes->currentRow();
        if (row >= 0) {
            if (QTableWidgetItem *item = ui->table_fabrications_etapes->item(row, 0)) {
                bool ok = false;
                int id = item->text().toInt(&ok);
                if (ok && id > 0)
                    return id;
            }
        }
    }

    // Fallback sur la fabrication mémorisée dans la timeline.
    if (m_selectedFab.id > 0)
        return m_selectedFab.id;

    return 0;
}

void MainWindow::chargerEtapes(int idFabrication)
{
    if (idFabrication <= 0)
        return;

    // Utilise une table dédiée si disponible, sinon fallback sur table_etapes_catalogue.
    QTableWidget *tableEtapes = this->findChild<QTableWidget*>("table_etapes_fabrication");
    if (!tableEtapes)
        tableEtapes = ui->table_etapes_catalogue;

    if (!tableEtapes)
        return;

    QSqlDatabase db = Connexion::createInstance().getDatabase();
    if (!db.isOpen()) {
        QMessageBox::critical(this, "Chargement étapes", "Connexion Oracle indisponible.");
        return;
    }

    QSqlQuery query(db);
    query.prepare(
        "SELECT IDETAPE, NOMETAPE, ORDRE, DATEDEBUT, DATEFIN, NVL(TEMPSESTIME,0), NVL(TEMPSREEL,0), "
        "CASE "
        "WHEN DATEDEBUT IS NULL THEN 'NON_PLANIFIEE' "
        "WHEN DATEDEBUT IS NOT NULL AND DATEFIN IS NULL THEN 'EN_COURS' "
        "WHEN DATEFIN IS NOT NULL AND NVL(TEMPSREEL,0) <= NVL(TEMPSESTIME,0) THEN 'TERMINEE' "
        "WHEN DATEFIN IS NOT NULL AND NVL(TEMPSREEL,0) > NVL(TEMPSESTIME,0) THEN 'EN_RETARD' "
        "ELSE 'INCONNU' "
        "END AS ETAT_ETAPE "
        "FROM ETAPE "
        "WHERE IDFABRICATION = :id "
        "ORDER BY ORDRE ASC");
    query.bindValue(":id", idFabrication);

    if (!query.exec()) {
        QMessageBox::critical(this,
                              "Chargement étapes",
                              QString("Erreur Oracle lors du chargement des étapes FAB-%1:\n%2")
                                  .arg(idFabrication)
                                  .arg(query.lastError().text()));
        return;
    }

    tableEtapes->clearContents();
    tableEtapes->setRowCount(0);
    tableEtapes->setColumnCount(8);
    tableEtapes->setHorizontalHeaderLabels({
        "ID Etape", "Nom Etape", "Ordre", "Date Début",
        "Date Fin", "Temps Estimé", "Temps Réel", "Etat"
    });

    int row = 0;
    while (query.next()) {
        tableEtapes->insertRow(row);

        int idEtape = query.value(0).toInt();
        QString nomEtape = query.value(1).toString();
        int ordre = query.value(2).toInt();
        QDate dateDebut = query.value(3).toDate();
        QDate dateFin = query.value(4).toDate();
        double tempsEstime = query.value(5).toDouble();
        double tempsReel = query.value(6).toDouble();
        QString etat = query.value(7).toString();

        auto makeItem = [](const QString &text, Qt::Alignment align = Qt::AlignCenter) {
            auto *it = new QTableWidgetItem(text);
            it->setFlags(it->flags() & ~Qt::ItemIsEditable);
            it->setTextAlignment(align);
            return it;
        };

        tableEtapes->setItem(row, 0, makeItem(QString::number(idEtape)));
        tableEtapes->setItem(row, 1, makeItem(nomEtape, Qt::AlignLeft | Qt::AlignVCenter));
        tableEtapes->setItem(row, 2, makeItem(QString::number(ordre)));
        tableEtapes->setItem(row, 3, makeItem(dateDebut.isValid() ? dateDebut.toString("dd/MM/yyyy") : "-"));
        tableEtapes->setItem(row, 4, makeItem(dateFin.isValid() ? dateFin.toString("dd/MM/yyyy") : "-"));
        tableEtapes->setItem(row, 5, makeItem(QString::number(tempsEstime, 'f', 0)));
        tableEtapes->setItem(row, 6, makeItem(QString::number(tempsReel, 'f', 0)));

        auto *etatItem = makeItem(etat);
        QColor etatColor = QColor("#95A5A6");
        if (etat == "EN_COURS") etatColor = QColor("#F1C40F");
        else if (etat == "TERMINEE") etatColor = QColor("#27AE60");
        else if (etat == "EN_RETARD") etatColor = QColor("#C0392B");

        etatItem->setBackground(etatColor);
        etatItem->setForeground(QColor("#FFFFFF"));
        etatItem->setFont(QFont("Segoe UI", 9, QFont::Bold));
        tableEtapes->setItem(row, 7, etatItem);

        row++;
    }

    if (row == 0) {
        tableEtapes->insertRow(0);
        auto *info = new QTableWidgetItem("Aucune étape trouvée pour cette fabrication");
        info->setFlags(info->flags() & ~Qt::ItemIsEditable);
        info->setTextAlignment(Qt::AlignCenter);
        tableEtapes->setItem(0, 1, info);
    }

    tableEtapes->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableEtapes->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    tableEtapes->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    tableEtapes->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    tableEtapes->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    tableEtapes->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);
    tableEtapes->horizontalHeader()->setSectionResizeMode(6, QHeaderView::ResizeToContents);
    tableEtapes->horizontalHeader()->setSectionResizeMode(7, QHeaderView::ResizeToContents);
    tableEtapes->verticalHeader()->setDefaultSectionSize(30);
}

int MainWindow::getSelectedEtapeId()
{
    QTableWidget *tableEtapes = this->findChild<QTableWidget*>("table_etapes_fabrication");
    if (!tableEtapes)
        tableEtapes = ui->table_etapes_catalogue;

    if (!tableEtapes)
        return 0;

    int row = tableEtapes->currentRow();
    if (row < 0)
        return 0;

    QTableWidgetItem *idItem = tableEtapes->item(row, 0);
    if (!idItem)
        return 0;

    bool ok = false;
    int idEtape = idItem->text().toInt(&ok);
    return ok ? idEtape : 0;
}

void MainWindow::on_btnDemarrerEtape_clicked()
{
    int idEtape = getSelectedEtapeId();
    if (idEtape <= 0) {
        QMessageBox::warning(this, "Démarrer étape", "Sélectionnez une étape valide.");
        return;
    }

    int fabId = getSelectedFabricationId();
    if (fabId <= 0) fabId = m_selectedFab.id;

    QSqlDatabase db = Connexion::createInstance().getDatabase();
    if (!db.isOpen()) {
        QMessageBox::critical(this, "Démarrer étape", "Connexion Oracle indisponible.");
        return;
    }

    QSqlQuery query(db);
    query.prepare(
        "UPDATE ETAPE "
        "SET DATEDEBUT = SYSDATE "
        "WHERE IDETAPE = :id "
        "  AND DATEDEBUT IS NULL");
    query.bindValue(":id", idEtape);

    if (!query.exec()) {
        QMessageBox::critical(this,
                              "Démarrer étape",
                              QString("Erreur Oracle:\n%1").arg(query.lastError().text()));
        return;
    }

    if (query.numRowsAffected() <= 0) {
        QMessageBox::information(this,
                                 "Démarrer étape",
                                 "Cette étape est déjà démarrée ou déjà terminée.");
        return;
    }

    refreshSuiviAfterChanges(fabId);
    chargerEtapes(fabId);
}

void MainWindow::on_btnTerminerEtape_clicked()
{
    int idEtape = getSelectedEtapeId();
    if (idEtape <= 0) {
        QMessageBox::warning(this, "Terminer étape", "Sélectionnez une étape valide.");
        return;
    }

    int fabId = getSelectedFabricationId();
    if (fabId <= 0) fabId = m_selectedFab.id;

    bool ok = false;
    int tempsReel = QInputDialog::getInt(this,
                                         "Terminer étape",
                                         "Saisir TEMPSREEL (minutes):",
                                         0,
                                         0,
                                         100000,
                                         1,
                                         &ok);
    if (!ok)
        return;

    QSqlDatabase db = Connexion::createInstance().getDatabase();
    if (!db.isOpen()) {
        QMessageBox::critical(this, "Terminer étape", "Connexion Oracle indisponible.");
        return;
    }

    QSqlQuery query(db);
    query.prepare(
        "UPDATE ETAPE "
        "SET DATEFIN = SYSDATE, TEMPSREEL = :tempsreel "
        "WHERE IDETAPE = :id "
        "  AND DATEDEBUT IS NOT NULL "
        "  AND DATEFIN IS NULL");
    query.bindValue(":tempsreel", tempsReel);
    query.bindValue(":id", idEtape);

    if (!query.exec()) {
        QMessageBox::critical(this,
                              "Terminer étape",
                              QString("Erreur Oracle:\n%1").arg(query.lastError().text()));
        return;
    }

    if (query.numRowsAffected() <= 0) {
        QMessageBox::information(this,
                                 "Terminer étape",
                                 "L'étape doit être démarrée et non terminée.");
        return;
    }

    refreshSuiviAfterChanges(fabId);
    chargerEtapes(fabId);
}

int MainWindow::getNextOrdre(int idFabrication)
{
    QSqlDatabase db = Connexion::createInstance().getDatabase();
    if (!db.isOpen())
        return 1;

    QSqlQuery query(db);
    query.prepare(
        "SELECT NVL(MAX(ORDRE), 0) + 1 "
        "FROM ETAPE "
        "WHERE IDFABRICATION = :id");
    query.bindValue(":id", idFabrication);

    if (!query.exec() || !query.next()) {
        qDebug() << "getNextOrdre error:" << query.lastError().text();
        return 1;
    }

    int nextOrdre = query.value(0).toInt();
    return qMax(1, nextOrdre);
}

bool MainWindow::etapeExisteDeja(int idFabrication, const QString &nomEtape)
{
    QSqlDatabase db = Connexion::createInstance().getDatabase();
    if (!db.isOpen())
        return false;

    QSqlQuery query(db);
    query.prepare(
        "SELECT COUNT(*) "
        "FROM ETAPE "
        "WHERE IDFABRICATION = :id "
        "  AND UPPER(NOMETAPE) = UPPER(:nom)");
    query.bindValue(":id", idFabrication);
    query.bindValue(":nom", nomEtape.trimmed());

    if (!query.exec() || !query.next())
        return false;

    return query.value(0).toInt() > 0;
}

int MainWindow::tempsCatalogue(const QString &nomEtape)
{
    const QString key = nomEtape.trimmed().toLower();

    if (key == "preparation" || key == "préparation") return 60;
    if (key == "decoupe" || key == "découpe") return 120;
    if (key == "poncage" || key == "ponçage") return 90;
    if (key == "assemblage") return 180;
    if (key == "collage") return 180;
    if (key == "vernis") return 100;
    if (key == "finition") return 90;

    return 60;
}

int MainWindow::currentSelectedFabricationId() const
{
    if (!ui || !ui->table_fabrications_etapes)
        return 0;

    int row = ui->table_fabrications_etapes->currentRow();
    if (row >= 0) {
        if (QTableWidgetItem *item = ui->table_fabrications_etapes->item(row, 0)) {
            int value = item->text().toInt();
            if (value > 0)
                return value;
        }
    }

    const QList<QTableWidgetItem *> selected = ui->table_fabrications_etapes->selectedItems();
    if (!selected.isEmpty()) {
        int value = selected.first()->tableWidget()->item(selected.first()->row(), 0)->text().toInt();
        if (value > 0)
            return value;
    }

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
    Q_UNUSED(cin);

    if (fabId <= 0 || etape.nom.trimmed().isEmpty())
        return false;

    QSqlDatabase db = Connexion::createInstance().getDatabase();
    if (!db.isOpen()) {
        if (detailErreur) *detailErreur = "Connexion Oracle indisponible.";
        return false;
    }

    QSqlQuery check(db);
    check.prepare("SELECT COUNT(*) FROM ETAPE WHERE IDFABRICATION = :id AND UPPER(NOMETAPE) = UPPER(:nom)");
    check.bindValue(":id", fabId);
    check.bindValue(":nom", etape.nom.trimmed());
    if (!check.exec() || !check.next()) {
        if (detailErreur) *detailErreur = check.lastError().text();
        return false;
    }

    if (check.value(0).toInt() > 0)
    {
        if (estNouvelleInsertion)
            *estNouvelleInsertion = false;
        return true;
    }

    // ORDRE calculé automatiquement pour garantir insertion en fin de séquence.
    const int ordreFinal = getNextOrdre(fabId);

    const int cinValue = cinPersonnelParDefaut();

    QSqlQuery insert(db);
    QString sql =
        "INSERT INTO ETAPE (IDETAPE, CIN, IDFABRICATION, NOMETAPE, ORDRE, TEMPSESTIME, TEMPSREEL, DATEDEBUT, DATEFIN) "
        "VALUES (SEQ_ETAPE.NEXTVAL, :cin, :idFab, :nom, :ordre, :tempsEstime, :tempsReel, ";

    sql += dateDebut.isValid() ? ":dateDebut" : "NULL";
    sql += ", NULL)";

    insert.prepare(sql);
    insert.bindValue(":cin", cinValue);
    insert.bindValue(":idFab", fabId);
    insert.bindValue(":nom", etape.nom.trimmed());
    insert.bindValue(":ordre", ordreFinal);
    insert.bindValue(":tempsEstime", etape.temps);
    insert.bindValue(":tempsReel", 0);
    if (dateDebut.isValid())
        insert.bindValue(":dateDebut", dateDebut);

    if (!insert.exec()) {
        qDebug() << "Insertion etape impossible:" << insert.lastError().text();
        if (detailErreur) *detailErreur = insert.lastError().text();
        return false;
    }

    if (estNouvelleInsertion)
        *estNouvelleInsertion = true;

    return true;
}

void MainWindow::refreshSuiviAfterChanges(int fabId)
{
    loadEtapesCatalogue();
    chargerTableSuiviAvecComplexite();
    reloadAllFabIds();
    chargerStatistiquesComplexite();

    if (fabId <= 0)
        fabId = currentSelectedFabricationId();

    if (fabId <= 0 || !ui || !ui->table_fabrications_etapes)
        return;

    for (int row = 0; row < ui->table_fabrications_etapes->rowCount(); ++row) {
        QTableWidgetItem *item = ui->table_fabrications_etapes->item(row, 0);
        if (item && item->text().toInt() == fabId) {
            ui->table_fabrications_etapes->setCurrentCell(row, 0);
            on_table_fabrications_etapes_cellClicked(row, 0);
            return;
        }
    }

    m_selectedFab = getFabricationSuivi(fabId);
    m_fabSelected = true;
    mettreAJourTimelineComplexite();
    chargerEtapes(fabId);
}

void MainWindow::on_btnOptimizer_clicked()
{
    const int fabId = getSelectedFabricationId();
    if (fabId <= 0) {
        QMessageBox::warning(this, "Optimizer",
                             "Aucune fabrication sélectionnée dans le tableau de suivi.");
        return;
    }

    QSqlDatabase db = Connexion::createInstance().getDatabase();
    if (!db.isOpen()) {
        QMessageBox::critical(this, "Optimizer",
                              "Connexion Oracle indisponible. Vérifiez QODBC.");
        return;
    }

    // 1) Récupérer le nom du modèle via FABRICATION.IDMODELE
    const QString nomModele = getNomModelePourFabrication(fabId);
    if (nomModele.isEmpty()) {
        QMessageBox::warning(this,
                             "Optimizer",
                             QString("Impossible de récupérer le nom du modèle pour FAB-%1.")
                                 .arg(fabId));
        return;
    }

    // 2) Charger la liste d'étapes adaptée au modèle sélectionné.
    QList<EtapeInfo> etapes = genererEtapesSelonModele(nomModele);
    if (etapes.isEmpty()) {
        QMessageBox::warning(this,
                             "Optimizer",
                             QString("Aucun catalogue d'étapes trouvé pour le modèle '%1'.")
                                 .arg(nomModele));
        return;
    }

    QStringList resume;
    for (const EtapeInfo &step : etapes)
        resume << QString("%1 (%2 min)").arg(step.nom).arg(step.temps);

    if (QMessageBox::question(this,
                              "Confirmation Optimizer",
                              QString("Modèle détecté : %1\n"
                                      "Créer automatiquement %2 étape(s) pour FAB-%3 ?\n\n%4")
                                  .arg(nomModele)
                                  .arg(etapes.size())
                                  .arg(fabId)
                                  .arg(resume.join("\n")),
                              QMessageBox::Yes | QMessageBox::No,
                              QMessageBox::No) != QMessageBox::Yes) {
        return;
    }

    if (!db.transaction()) {
        QMessageBox::critical(this, "Optimizer", "Impossible de démarrer la transaction Oracle.");
        return;
    }

    int inserted = 0;
    int skipped = 0;
    const int cinParDefaut = cinPersonnelParDefaut();

    // 3) Insertion ordonnée (ORDRE du catalogue, à partir de 1) sans doublons.
    for (const EtapeInfo &step : etapes) {
        if (etapeExisteDeja(fabId, step.nom)) {
            ++skipped;
            continue;
        }

        QSqlQuery insert(db);
        insert.prepare(
            "INSERT INTO ETAPE "
            "(IDETAPE, CIN, IDFABRICATION, NOMETAPE, ORDRE, TEMPSESTIME, TEMPSREEL, DATEDEBUT, DATEFIN) "
            "VALUES "
            "(SEQ_ETAPE.NEXTVAL, :cin, :idfab, :nom, :ordre, :tempsestime, :tempsreel, NULL, NULL)");

        insert.bindValue(":cin", cinParDefaut);
        insert.bindValue(":idfab", fabId);
        insert.bindValue(":nom", step.nom);
        insert.bindValue(":ordre", qMax(1, step.ordre));
        insert.bindValue(":tempsestime", step.temps);
        insert.bindValue(":tempsreel", 0);

        if (!insert.exec()) {
            db.rollback();
            QMessageBox::critical(this,
                                  "Optimizer - Erreur insertion",
                                  QString("Échec insertion de '%1' pour FAB-%2\n\nErreur Oracle:\n%3")
                                      .arg(step.nom)
                                      .arg(fabId)
                                      .arg(insert.lastError().text()));
            return;
        }
        ++inserted;
    }

    if (!db.commit()) {
        db.rollback();
        QMessageBox::critical(this, "Optimizer", "Impossible de valider la transaction Oracle.");
        return;
    }

    refreshSuiviAfterChanges(fabId);

    QMessageBox::information(this,
                             "Optimizer",
                             QString("Affectation automatique terminée (modèle: %1).\n"
                                     "Nouvelles étapes: %1\n"
                                     "Doublons ignorés: %2")
                                 .arg(nomModele)
                                 .arg(inserted)
                                 .arg(skipped));
}

void MainWindow::on_btnAffectationManuelle_clicked()
{
    const int fabId = getSelectedFabricationId();
    if (fabId <= 0) {
        QMessageBox::warning(this, "Affectation manuelle",
                             "Aucune fabrication sélectionnée dans le tableau de suivi.");
        return;
    }

    // Mode robuste: ouvrir le dialogue même si aucun widget manuel n'existe dans l'UI.
    AffectationManuelleDialog dialog(getCatalogue(), this);
    if (dialog.exec() != QDialog::Accepted)
        return;

    const QList<ManualEtapeEntry> entries = dialog.entries();
    if (entries.isEmpty())
        return;

    QSqlDatabase db = Connexion::createInstance().getDatabase();
    if (!db.isOpen()) {
        QMessageBox::critical(this, "Affectation manuelle",
                              "Connexion Oracle indisponible.");
        return;
    }

    if (!db.transaction()) {
        QMessageBox::critical(this, "Affectation manuelle",
                              "Impossible de démarrer la transaction Oracle.");
        return;
    }

    int inserted = 0;
    int skipped = 0;
    const int cinParDefaut = cinPersonnelParDefaut();

    for (const ManualEtapeEntry &entry : entries) {
        EtapeInfo info;
        info.nom = entry.nom;
        info.temps = entry.temps > 0 ? entry.temps : tempsCatalogue(entry.nom);
        // L'ordre manuel est ignoré: on ajoute toujours à la fin.
        info.ordre = getNextOrdre(fabId);

        bool newInsertion = false;
        QString oracleError;
        if (!insererEtapeSiAbsente(fabId,
                                   info,
                                   QString::number(cinParDefaut),
                                   entry.hasDateDebut ? entry.dateDebut : QDate(),
                                   &newInsertion,
                                   &oracleError)) {
            db.rollback();
            QMessageBox::critical(this,
                                  "Affectation manuelle",
                                  QString("Impossible d'insérer l'étape '%1'.\n\nErreur Oracle:\n%2")
                                      .arg(entry.nom)
                                      .arg(oracleError.isEmpty() ? "Erreur inconnue" : oracleError));
            return;
        }

        if (newInsertion)
            ++inserted;
        else
            ++skipped;
    }

    if (!db.commit()) {
        db.rollback();
        QMessageBox::critical(this, "Affectation manuelle",
                              "Impossible de valider la transaction Oracle.");
        return;
    }

    refreshSuiviAfterChanges(fabId);

    QMessageBox::information(this,
                             "Affectation manuelle",
                             QString("Affectation terminée.\n"
                                     "Nouvelles étapes: %1\n"
                                     "Doublons ignorés: %2")
                                 .arg(inserted)
                                 .arg(skipped));
}

void MainWindow::on_btnAjouterEtape_clicked()
{
    const int fabId = getSelectedFabricationId();
    if (fabId <= 0) {
        QMessageBox::warning(this, "Ajout étape", "Sélectionnez d'abord une fabrication.");
        return;
    }

    // Widgets attendus pour l'affectation manuelle.
    // Remplacez les objectName si vos noms diffèrent.
    QComboBox *cbNomEtape = this->findChild<QComboBox*>("cbNomEtape");
    if (!cbNomEtape) cbNomEtape = this->findChild<QComboBox*>("cb_etape_nom");

    QSpinBox *spTemps = this->findChild<QSpinBox*>("spTempsEstimeEtape");
    if (!spTemps) spTemps = this->findChild<QSpinBox*>("sp_temps_estime_etape");

    QLineEdit *leCin = this->findChild<QLineEdit*>("leCinEtape");
    if (!leCin) leCin = this->findChild<QLineEdit*>("le_cin_etape");
    Q_UNUSED(leCin);

    QDateEdit *deDateDebut = this->findChild<QDateEdit*>("deDateDebutEtape");
    if (!deDateDebut) deDateDebut = this->findChild<QDateEdit*>("de_date_debut_etape");

    QCheckBox *chkDate = this->findChild<QCheckBox*>("chkDateDebutEtape");
    if (!chkDate) chkDate = this->findChild<QCheckBox*>("chk_date_debut_etape");

    if (!cbNomEtape || !spTemps) {
        // Fallback automatique: ouvre le dialogue d'affectation au lieu de bloquer.
        on_btnAffectationManuelle_clicked();
        return;
    }

    const QString nomEtape = cbNomEtape->currentText().trimmed();
    if (nomEtape.isEmpty()) {
        QMessageBox::warning(this, "Ajout étape", "Choisissez un nom d'étape.");
        return;
    }

    // ORDRE toujours calculé automatiquement pour éviter doublons et trous.
    const int ordre = getNextOrdre(fabId);
    int tempsEstime = spTemps->value();
    if (tempsEstime <= 0)
        tempsEstime = tempsCatalogue(nomEtape);

    if (etapeExisteDeja(fabId, nomEtape)) {
        QMessageBox::information(this,
                                 "Ajout étape",
                                 QString("L'étape '%1' existe déjà pour FAB-%2.").arg(nomEtape).arg(fabId));
        return;
    }

    QSqlDatabase db = Connexion::createInstance().getDatabase();
    if (!db.isOpen()) {
        QMessageBox::critical(this, "Ajout étape", "Connexion Oracle indisponible.");
        return;
    }

    QSqlQuery insert(db);
    // Un seul personnel est imposé pour toutes les étapes.
    const int cinValue = cinPersonnelParDefaut();

    // DATEDEBUT optionnelle : NULL si non fournie.
    bool useDate = (deDateDebut != nullptr);
    if (chkDate)
        useDate = chkDate->isChecked();

    QString sql =
        "INSERT INTO ETAPE "
        "(IDETAPE, CIN, IDFABRICATION, NOMETAPE, ORDRE, TEMPSESTIME, TEMPSREEL, DATEDEBUT, DATEFIN) "
        "VALUES "
        "(SEQ_ETAPE.NEXTVAL, :cin, :idfab, :nom, :ordre, :tempsestime, :tempsreel, ";
    sql += useDate ? ":datedebut" : "NULL";
    sql += ", NULL)";

    insert.prepare(sql);
    insert.bindValue(":cin", cinValue);

    insert.bindValue(":idfab", fabId);
    insert.bindValue(":nom", nomEtape);
    insert.bindValue(":ordre", ordre);
    insert.bindValue(":tempsestime", tempsEstime);
    insert.bindValue(":tempsreel", 0);
    if (useDate && deDateDebut)
        insert.bindValue(":datedebut", deDateDebut->date());

    if (!insert.exec()) {
        QMessageBox::critical(this,
                              "Ajout étape - Erreur insertion",
                              QString("Impossible d'insérer l'étape '%1' pour FAB-%2\n\nErreur Oracle:\n%3")
                                  .arg(nomEtape)
                                  .arg(fabId)
                                  .arg(insert.lastError().text()));
        return;
    }

    refreshSuiviAfterChanges(fabId);

    QMessageBox::information(this,
                             "Ajout étape",
                             QString("Étape '%1' ajoutée avec succès à FAB-%2.").arg(nomEtape).arg(fabId));
}

// ═════════════════════════════════════════════════════════════
//  PARTIE 1 — KPI CARDS
// ═════════════════════════════════════════════════════════════

void MainWindow::ensureStatistiqueWidgets()
{
    QWidget *tabStat = this->findChild<QWidget*>("tab_statistique");
    QVBoxLayout *vlStat = this->findChild<QVBoxLayout*>("vl_statistiques_global");
    if (!tabStat || !vlStat)
        return;

    QFrame *kpiFrame = this->findChild<QFrame*>("frame_kpi_cards_dynamic");
    if (!kpiFrame) {
        kpiFrame = new QFrame(tabStat);
        kpiFrame->setObjectName("frame_kpi_cards_dynamic");
        kpiFrame->setStyleSheet("QFrame#frame_kpi_cards_dynamic { background: transparent; }");

        auto *h = new QHBoxLayout(kpiFrame);
        h->setObjectName("hl_kpi_cards");
        h->setContentsMargins(0, 0, 0, 0);
        h->setSpacing(8);
        vlStat->insertWidget(1, kpiFrame);
    }

    QWidget *chartsRow = this->findChild<QWidget*>("stats_charts_row");
    if (!chartsRow) {
        chartsRow = new QWidget(tabStat);
        chartsRow->setObjectName("stats_charts_row");
        auto *rowLayout = new QHBoxLayout(chartsRow);
        rowLayout->setContentsMargins(0, 0, 0, 0);
        rowLayout->setSpacing(10);

        QGroupBox *gbEtape = new QGroupBox("Distribution des etapes", chartsRow);
        gbEtape->setObjectName("gb_stats_etape");
        auto *vlEtape = new QVBoxLayout(gbEtape);
        auto *chartEtape = new QChartView(gbEtape);
        chartEtape->setObjectName("chartview_etape_status");
        chartEtape->setRenderHint(QPainter::Antialiasing);
        chartEtape->setMinimumHeight(280);
        chartEtape->setStyleSheet("background: transparent; border: none;");
        vlEtape->addWidget(chartEtape);

        QGroupBox *gbModele = new QGroupBox("Fabrications par modele", chartsRow);
        gbModele->setObjectName("gb_stats_modele");
        auto *vlModele = new QVBoxLayout(gbModele);
        auto *chartModele = new QChartView(gbModele);
        chartModele->setObjectName("chartview_modele_fabrication");
        chartModele->setRenderHint(QPainter::Antialiasing);
        chartModele->setMinimumHeight(280);
        chartModele->setStyleSheet("background: transparent; border: none;");
        vlModele->addWidget(chartModele);

        rowLayout->addWidget(gbEtape, 1);
        rowLayout->addWidget(gbModele, 1);
        vlStat->insertWidget(2, chartsRow);
    }

    QGroupBox *gbInsights = this->findChild<QGroupBox*>("gb_stats_insights");
    if (!gbInsights) {
        gbInsights = new QGroupBox("Analyse avancee", tabStat);
        gbInsights->setObjectName("gb_stats_insights");
        auto *vlInsights = new QVBoxLayout(gbInsights);

        auto *table = new QTableWidget(gbInsights);
        table->setObjectName("table_stats_insights");
        table->setColumnCount(3);
        table->setHorizontalHeaderLabels({"Indicateur", "Valeur", "Details"});
        table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
        table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        table->verticalHeader()->setVisible(false);
        table->setAlternatingRowColors(true);
        table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        table->setSelectionMode(QAbstractItemView::NoSelection);
        table->setMinimumHeight(180);
        vlInsights->addWidget(table);

        vlStat->insertWidget(3, gbInsights);
    }
}

void MainWindow::buildKPICards()
{
    ensureStatistiqueWidgets();

    QHBoxLayout *kpiLayout = this->findChild<QHBoxLayout*>("hl_kpi_cards");
    if (!kpiLayout)
        return;

    QLayoutItem *item;
    while ((item = kpiLayout->takeAt(0)) != nullptr) {
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }

    int totalFab = 0;
    int enCoursFab = 0;
    int termineesFab = 0;
    int fabFaible = 0;
    int fabMoyenne = 0;
    int fabElevee = 0;
    int fabCritique = 0;
    double progressionMoyenne = 0.0;
    double totalEstime = 0.0;
    double totalReel = 0.0;

    QSqlQuery q;

    if (q.exec("SELECT COUNT(*) FROM FABRICATION") && q.next())
        totalFab = q.value(0).toInt();

    if (q.exec(
            "SELECT COUNT(*) "
            "FROM FABRICATION F "
            "WHERE UPPER(NVL(F.ETAT, 'X')) = 'EN_COURS' "
            "   OR EXISTS ( "
            "       SELECT 1 FROM ETAPE E "
            "       WHERE E.IDFABRICATION = F.IDFABRICATION "
            "         AND E.DATEDEBUT IS NOT NULL "
            "         AND E.DATEFIN IS NULL) ") && q.next()) {
        enCoursFab = q.value(0).toInt();
    }

    if (q.exec(
            "SELECT COUNT(*) "
            "FROM FABRICATION F "
            "WHERE UPPER(NVL(F.ETAT, 'X')) = 'TERMINEE' "
            "   OR ( "
            "       EXISTS (SELECT 1 FROM ETAPE E1 WHERE E1.IDFABRICATION = F.IDFABRICATION) "
            "       AND NOT EXISTS ( "
            "           SELECT 1 FROM ETAPE E2 "
            "           WHERE E2.IDFABRICATION = F.IDFABRICATION "
            "             AND E2.DATEFIN IS NULL))") && q.next()) {
        termineesFab = q.value(0).toInt();
    }

    if (q.exec("SELECT IDFABRICATION FROM FABRICATION")) {
        while (q.next()) {
            const int fabId = q.value(0).toInt();
            const QString complexite = calculerComplexiteFabrication(fabId);
            if (complexite == QStringLiteral("Faible"))
                ++fabFaible;
            else if (complexite == QStringLiteral("Moyenne"))
                ++fabMoyenne;
            else if (complexite == QStringLiteral("Élevée"))
                ++fabElevee;
            else if (complexite == QStringLiteral("Critique"))
                ++fabCritique;
        }
    }

    if (q.exec(
            "SELECT NVL(ROUND( "
            "       (SUM(CASE WHEN DATEFIN IS NOT NULL THEN 1 ELSE 0 END) / NULLIF(COUNT(*), 0)) * 100, "
            "       1), 0) "
            "FROM ETAPE") && q.next()) {
        progressionMoyenne = q.value(0).toDouble();
    }

    if (q.exec("SELECT NVL(SUM(TEMPSESTIME),0), NVL(SUM(TEMPSREEL),0) FROM ETAPE") && q.next()) {
        totalEstime = q.value(0).toDouble();
        totalReel = q.value(1).toDouble();
    }

    struct KPIData {
        QString title;
        QString value;
        QString accent;
        QString subtitle;
    };

    const QList<KPIData> kpis = {
        {"Total Fabrications", QString::number(totalFab), "#8D6E63", "Ordres de fabrication"},
        {"Fabrications En Cours", QString::number(enCoursFab), "#D4851A", "Production active"},
        {"Fabrications Terminees", QString::number(termineesFab), "#27AE60", "Objectifs finalises"},
        {"Fabrications Faible", QString::number(fabFaible), "#27AE60", "Complexité faible"},
        {"Fabrications Moyenne", QString::number(fabMoyenne), "#E67E22", "Complexité moyenne"},
        {"Fabrications Élevée", QString::number(fabElevee), "#C0392B", "Complexité élevée"},
        {"Fabrications Critique", QString::number(fabCritique), "#8B0000", "Complexité critique"},
        {"Progression Moyenne", QString::number(progressionMoyenne, 'f', 1) + "%", "#2980B9", "Etapes terminees / total"},
        {"Temps Estime / Reel", QString::number(totalEstime, 'f', 0) + "h / " + QString::number(totalReel, 'f', 0) + "h", "#7F4129", "Charge previsionnelle vs reelle"}
    };

    for (const KPIData &kpi : kpis) {
        QFrame *card = new QFrame();
        card->setMinimumWidth(170);
        card->setFixedHeight(96);
        card->setStyleSheet(QString(
            "QFrame {"
            " background-color: %1;"
            " border-radius: 12px;"
            " border: 1px solid #B8956A;"
            "}" ).arg(COLOR_BEIGE_CARD));

        auto *vl = new QVBoxLayout(card);
        vl->setContentsMargins(12, 8, 12, 8);
        vl->setSpacing(2);

        QLabel *lblTitle = new QLabel(kpi.title, card);
        lblTitle->setStyleSheet(QString("color:%1; font-size:11px; font-weight:700; background:transparent;")
                                .arg(kpi.accent));

        QLabel *lblValue = new QLabel(kpi.value, card);
        lblValue->setStyleSheet("color:#2E1A00; font-size:20px; font-weight:800; background:transparent;");

        QLabel *lblSub = new QLabel(kpi.subtitle, card);
        lblSub->setStyleSheet("color:#6B4F35; font-size:10px; background:transparent;");

        vl->addWidget(lblTitle);
        vl->addWidget(lblValue);
        vl->addWidget(lblSub);

        kpiLayout->addWidget(card);
    }
    kpiLayout->addStretch();

    buildEtapeStatusPieChart();
    buildFabricationsByModeleBarChart();
    buildStatsInsightsPanel();
    chargerStatistiquesComplexite();
}

void MainWindow::buildEtapeStatusPieChart()
{
    auto *chartView = this->findChild<QChartView*>("chartview_etape_status");
    if (!chartView)
        return;

    QSqlQuery q;
    q.prepare(
        "SELECT ETAT_ETAPE, COUNT(*) "
        "FROM ( "
        "    SELECT CASE "
        "        WHEN DATEDEBUT IS NULL THEN 'NON_PLANIFIEE' "
        "        WHEN DATEDEBUT IS NOT NULL AND DATEFIN IS NULL THEN 'EN_COURS' "
        "        WHEN DATEFIN IS NOT NULL AND NVL(TEMPSESTIME,0) > 0 AND NVL(TEMPSREEL,0) > NVL(TEMPSESTIME,0) THEN 'EN_RETARD' "
        "        WHEN DATEFIN IS NOT NULL THEN 'TERMINEE' "
        "        ELSE 'INCONNU' "
        "    END AS ETAT_ETAPE "
        "    FROM ETAPE "
        ") "
        "GROUP BY ETAT_ETAPE "
        "ORDER BY ETAT_ETAPE");

    auto *series = new QPieSeries();
    if (q.exec()) {
        int total = 0;
        while (q.next()) {
            const QString status = q.value(0).toString();
            const int count = q.value(1).toInt();
            total += count;
            series->append(status, count);
        }
        if (total <= 0)
            series->append("Aucune donnee", 1);
    } else {
        series->append("Erreur SQL", 1);
        qDebug() << "Stats pie SQL error:" << q.lastError().text();
    }

    for (QPieSlice *slice : series->slices()) {
        const QString label = slice->label();
        if (label == "TERMINEE") slice->setBrush(QColor("#27AE60"));
        else if (label == "EN_COURS") slice->setBrush(QColor("#D4851A"));
        else if (label == "EN_RETARD") slice->setBrush(QColor("#C0392B"));
        else if (label == "NON_PLANIFIEE") slice->setBrush(QColor("#8D6E63"));
        else slice->setBrush(QColor("#B8956A"));

        slice->setLabel(QString("%1 (%2%)")
                            .arg(label)
                            .arg(QString::number(slice->percentage() * 100.0, 'f', 1)));
        slice->setLabelVisible(true);
    }
    series->setHoleSize(0.36);

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Repartition des statuts d'etapes");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignRight);
    chart->setBackgroundBrush(QColor("#FDFAF4"));
    chart->setTitleBrush(QBrush(QColor("#4A2800")));

    chartView->setChart(chart);
}

void MainWindow::buildFabricationsByModeleBarChart()
{
    auto *chartView = this->findChild<QChartView*>("chartview_modele_fabrication");
    if (!chartView)
        return;

    QSqlQuery q;
    q.prepare(
        "SELECT NVL(M.NOM, TRIM(TO_CHAR(F.IDMODELE))) AS NOM_MODELE, "
        "       COUNT(*) AS NB_FAB "
        "FROM FABRICATION F "
        "LEFT JOIN MODELE M ON (TRIM(TO_CHAR(M.IDMODELE)) = TRIM(TO_CHAR(F.IDMODELE)) "
        "                     OR UPPER(TRIM(M.NOM)) = UPPER(TRIM(TO_CHAR(F.IDMODELE)))) "
        "GROUP BY NVL(M.NOM, TRIM(TO_CHAR(F.IDMODELE))) "
        "ORDER BY NB_FAB DESC, NOM_MODELE");

    auto *set = new QBarSet("Fabrications");
    QStringList categories;
    int maxValue = 0;

    if (q.exec()) {
        while (q.next()) {
            const QString modele = q.value(0).toString();
            const int count = q.value(1).toInt();
            categories << modele;
            *set << count;
            maxValue = qMax(maxValue, count);
        }
    } else {
        qDebug() << "Stats bar SQL error:" << q.lastError().text();
    }

    if (categories.isEmpty()) {
        categories << "Aucun";
        *set << 0;
    }

    set->setColor(QColor("#7F4129"));

    auto *series = new QBarSeries();
    series->append(set);

    auto *chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Volume de fabrication par modele");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->setBackgroundBrush(QColor("#FDFAF4"));
    chart->setTitleBrush(QBrush(QColor("#4A2800")));

    auto *axisX = new QBarCategoryAxis();
    axisX->append(categories);
    axisX->setLabelsColor(QColor("#4A2800"));
    axisX->setGridLineVisible(false);

    auto *axisY = new QValueAxis();
    axisY->setRange(0, qMax(1, maxValue + 1));
    axisY->setLabelFormat("%d");
    axisY->setLabelsColor(QColor("#4A2800"));
    axisY->setGridLineColor(QColor("#DCC9AC"));

    chart->addAxis(axisX, Qt::AlignBottom);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisX);
    series->attachAxis(axisY);

    chartView->setChart(chart);
}

void MainWindow::buildStatsInsightsPanel()
{
    auto *table = this->findChild<QTableWidget*>("table_stats_insights");
    if (!table)
        return;

    table->setRowCount(0);

    auto addRow = [&](const QString &metric, const QString &value, const QString &detail) {
        int row = table->rowCount();
        table->insertRow(row);

        auto *m = new QTableWidgetItem(metric);
        auto *v = new QTableWidgetItem(value);
        auto *d = new QTableWidgetItem(detail);

        m->setFlags(m->flags() & ~Qt::ItemIsEditable);
        v->setFlags(v->flags() & ~Qt::ItemIsEditable);
        d->setFlags(d->flags() & ~Qt::ItemIsEditable);

        m->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        v->setTextAlignment(Qt::AlignCenter);
        d->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        table->setItem(row, 0, m);
        table->setItem(row, 1, v);
        table->setItem(row, 2, d);
    };

    QSqlQuery q;

    QString topModele = "N/A";
    int topModeleCount = 0;
    if (q.exec(
            "SELECT * FROM ( "
            "   SELECT NVL(M.NOM, TRIM(TO_CHAR(F.IDMODELE))) AS NOM_MODELE, COUNT(*) AS NB "
            "   FROM FABRICATION F "
            "   LEFT JOIN MODELE M ON (TRIM(TO_CHAR(M.IDMODELE)) = TRIM(TO_CHAR(F.IDMODELE)) "
            "                        OR UPPER(TRIM(M.NOM)) = UPPER(TRIM(TO_CHAR(F.IDMODELE)))) "
            "   GROUP BY NVL(M.NOM, TRIM(TO_CHAR(F.IDMODELE))) "
            "   ORDER BY NB DESC "
            ") WHERE ROWNUM = 1") && q.next()) {
        topModele = q.value(0).toString();
        topModeleCount = q.value(1).toInt();
    }
    addRow("Modele le plus fabrique", topModele,
           QString("%1 fabrication(s)").arg(topModeleCount));

    double retardPct = 0.0;
    if (q.exec(
            "SELECT NVL(ROUND((SUM(CASE WHEN DATEFIN IS NOT NULL AND NVL(TEMPSESTIME,0) > 0 "
            "      AND NVL(TEMPSREEL,0) > NVL(TEMPSESTIME,0) THEN 1 ELSE 0 END) "
            "      / NULLIF(COUNT(*),0)) * 100, 1), 0) "
            "FROM ETAPE") && q.next()) {
        retardPct = q.value(0).toDouble();
    }
    addRow("Taux de retard des etapes", QString::number(retardPct, 'f', 1) + "%",
           "Part des etapes en retard sur le total");

    QString etapeLente = "N/A";
    double avgLente = 0.0;
    if (q.exec(
            "SELECT * FROM ( "
            "   SELECT NOMETAPE, ROUND(AVG(NVL(TEMPSREEL,0)),1) AS MOY_REEL "
            "   FROM ETAPE "
            "   WHERE DATEFIN IS NOT NULL "
            "   GROUP BY NOMETAPE "
            "   ORDER BY MOY_REEL DESC "
            ") WHERE ROWNUM = 1") && q.next()) {
        etapeLente = q.value(0).toString();
        avgLente = q.value(1).toDouble();
    }
    addRow("Etape la plus lente", etapeLente,
           QString("Temps reel moyen: %1 h").arg(QString::number(avgLente, 'f', 1)));

    int chargeOuverte = 0;
    if (q.exec(
            "SELECT NVL(SUM(F.QUANTITE_A_PRODUIRE),0) "
            "FROM FABRICATION F "
            "WHERE UPPER(NVL(F.ETAT, 'X')) <> 'TERMINEE' "
            "   OR EXISTS (SELECT 1 FROM ETAPE E "
            "              WHERE E.IDFABRICATION = F.IDFABRICATION "
            "                AND E.DATEFIN IS NULL)") && q.next()) {
        chargeOuverte = q.value(0).toInt();
    }
    addRow("Charge ouverte", QString::number(chargeOuverte),
           "Quantite totale encore en production");

    QString topPersonnel = "N/A";
    int nbAffectations = 0;
    if (q.exec(
            "SELECT * FROM ( "
            "  SELECT NVL(P.NOM, 'Inconnu') || ' ' || NVL(P.PRENOM, '') AS NOM_COMPLET, COUNT(*) AS NB "
            "  FROM ETAPE E "
            "  LEFT JOIN PERSONNEL P ON P.CIN = E.CIN "
            "  GROUP BY NVL(P.NOM, 'Inconnu') || ' ' || NVL(P.PRENOM, '') "
            "  ORDER BY NB DESC "
            ") WHERE ROWNUM = 1") && q.next()) {
        topPersonnel = q.value(0).toString().trimmed();
        nbAffectations = q.value(1).toInt();
    }
    addRow("Personnel le plus actif", topPersonnel,
           QString("%1 affectation(s) d'etapes").arg(nbAffectations));

    QString qualiteDominante = "N/A";
    int nbQualite = 0;
    if (q.exec(
            "SELECT * FROM ( "
            "  SELECT NVL(QUALITE, 'N/A') AS Q, COUNT(*) AS NB "
            "  FROM FABRICATION "
            "  GROUP BY NVL(QUALITE, 'N/A') "
            "  ORDER BY NB DESC "
            ") WHERE ROWNUM = 1") && q.next()) {
        qualiteDominante = q.value(0).toString();
        nbQualite = q.value(1).toInt();
    }
    addRow("Qualite dominante", qualiteDominante,
           QString("%1 fabrication(s)").arg(nbQualite));
}

void MainWindow::chargerStatistiquesComplexite()
{
    auto *table = this->findChild<QTableWidget*>("table_stats_insights");
    if (!table)
        return;

    int faible = 0;
    int moyenne = 0;
    int elevee = 0;
    int critique = 0;

    QSqlQuery q;
    if (q.exec("SELECT IDFABRICATION FROM FABRICATION")) {
        while (q.next()) {
            const QString complexite = calculerComplexiteFabrication(q.value(0).toInt());
            if (complexite == QStringLiteral("Faible"))
                ++faible;
            else if (complexite == QStringLiteral("Moyenne"))
                ++moyenne;
            else if (complexite == QStringLiteral("Élevée"))
                ++elevee;
            else if (complexite == QStringLiteral("Critique"))
                ++critique;
        }
    }

    auto addComplexiteRow = [&](const QString &libelle, int valeur, const QString &detail) {
        const int row = table->rowCount();
        table->insertRow(row);

        auto *m = new QTableWidgetItem(libelle);
        auto *v = new QTableWidgetItem(QString::number(valeur));
        auto *d = new QTableWidgetItem(detail);

        m->setFlags(m->flags() & ~Qt::ItemIsEditable);
        v->setFlags(v->flags() & ~Qt::ItemIsEditable);
        d->setFlags(d->flags() & ~Qt::ItemIsEditable);

        m->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        v->setTextAlignment(Qt::AlignCenter);
        d->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        table->setItem(row, 0, m);
        table->setItem(row, 1, v);
        table->setItem(row, 2, d);
    };

    addComplexiteRow("Fabrications Faible", faible, "Complexité basée sur le nombre d'étapes");
    addComplexiteRow("Fabrications Moyenne", moyenne, "Complexité basée sur le nombre d'étapes");
    addComplexiteRow("Fabrications Élevée", elevee, "Complexité basée sur le nombre d'étapes");
    addComplexiteRow("Fabrications Critique", critique, "Complexité basée sur le nombre d'étapes et la charge estimée");
}

// ═════════════════════════════════════════════════════════════
//  PARTIE 2 — FILTRES DU SUIVI
// ═════════════════════════════════════════════════════════════
void MainWindow::buildFiltresSuivi()
{
    QLayout *filtreLayout = nullptr;

    // Compatibilité: utiliser le layout designer s'il existe, sinon créer une barre de filtres dynamique.
    if (QHBoxLayout *designerLayout = this->findChild<QHBoxLayout*>("hl_filtres_suivi")) {
        filtreLayout = designerLayout;
    } else {
        QWidget *filterBar = this->findChild<QWidget*>("w_filtres_suivi_dynamic");
        if (!filterBar) {
            filterBar = new QWidget(ui->gb_fabrications_suivi);
            filterBar->setObjectName("w_filtres_suivi_dynamic");
            auto *h = new QHBoxLayout(filterBar);
            h->setContentsMargins(0, 0, 0, 0);
            h->setSpacing(8);

            if (ui->vl_fab_suivi)
                ui->vl_fab_suivi->insertWidget(0, filterBar);
        }
        filtreLayout = filterBar->layout();
    }

    if (!filtreLayout)
        return;

    QLayoutItem *item;
    while ((item = filtreLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    // ComboBox statut
    auto *cbStatut = new QComboBox();
    cbStatut->setObjectName("cb_filtre_statut");
    cbStatut->addItems({"Tous", "EN_COURS", "TERMINE", "RETARD", "NON_COMMENCE"});
    cbStatut->setStyleSheet(
        "QComboBox { background:#FDFAF4; color:#2E1A00; border:1px solid #B8956A; border-radius:6px;"
        "            padding:4px 10px; font-size:10px; min-width:110px; }"
        "QComboBox::drop-down { border:none; }"
        "QComboBox QAbstractItemView { background:#FDFAF4; color:#2E1A00; }");

    // ComboBox qualité
    auto *cbQualite = new QComboBox();
    cbQualite->setObjectName("cb_filtre_qualite");
    cbQualite->addItem("Toutes qualités");
    QSqlQuery qQ;
    if (qQ.exec("SELECT DISTINCT QUALITE FROM FABRICATION ORDER BY QUALITE"))
        while (qQ.next()) cbQualite->addItem(qQ.value(0).toString());
    cbQualite->setStyleSheet(cbStatut->styleSheet());

    // Champ recherche ID
    auto *leId = new QLineEdit();
    leId->setObjectName("le_filtre_id");
    leId->setPlaceholderText("Filtrer par ID fab...");
    leId->setStyleSheet(
        "QLineEdit { background:#FDFAF4; color:#2E1A00; border:1px solid #B8956A; border-radius:6px;"
        "            padding:4px 10px; font-size:10px; min-width:130px; }"
        "QLineEdit::placeholder { color:#8D6E63; }");

    // Bouton réinitialiser
    auto *btnReset = new QPushButton("↺ Reset");
    btnReset->setStyleSheet(
        "QPushButton { background:#C8B89A; color:#2E1A00; border:1px solid #B8956A; border-radius:6px;"
        "              padding:4px 12px; font-size:10px; }"
        "QPushButton:hover { background:#D4B896; }");

    if (auto *hLayout = qobject_cast<QHBoxLayout*>(filtreLayout)) {
        hLayout->addWidget(new QLabel("Statut:"));
        hLayout->addWidget(cbStatut);
        hLayout->addWidget(new QLabel("Qualité:"));
        hLayout->addWidget(cbQualite);
        hLayout->addWidget(leId);
        hLayout->addWidget(btnReset);
        hLayout->addStretch();
    }

    // Connexions des signaux
    connect(cbStatut, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::on_filtres_suivi_changed);
    connect(cbQualite, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::on_filtres_suivi_changed);
    connect(leId, &QLineEdit::textChanged,
            this, &MainWindow::on_filtres_suivi_changed);
    connect(btnReset, &QPushButton::clicked, this, [cbStatut, cbQualite, leId, this]() {
        cbStatut->setCurrentIndex(0);
        cbQualite->setCurrentIndex(0);
        leId->clear();
        loadFabricationsSuivi();
    });
}

void MainWindow::on_filtres_suivi_changed()
{
    // Récupérer les widgets de filtre
    auto *cbStatut  = findChild<QComboBox*>("cb_filtre_statut");
    auto *cbQualite = findChild<QComboBox*>("cb_filtre_qualite");
    auto *leId      = findChild<QLineEdit*>("le_filtre_id");

    QString filtreStatut  = cbStatut  ? cbStatut->currentText()  : "Tous";
    QString filtreQualite = cbQualite ? cbQualite->currentText() : "Toutes qualités";
    QString filtreId      = leId      ? leId->text().trimmed()   : "";

    // Recharger avec filtres
    loadFabricationsSuiviFiltre(filtreStatut, filtreQualite, filtreId);
}

// ═════════════════════════════════════════════════════════════
//  PARTIE 2 — TABLEAU DES FABRICATIONS AMÉLIORÉ
// ═════════════════════════════════════════════════════════════

/*
 * Requête principale du tableau (Oracle 11g) :
 *
 * SELECT
 *   F.IDFABRICATION,
 *   F.IDMODELE,
 *   F.QUANTITE_A_PRODUIRE,
 *   F.QUALITE,
 *   F.DATE_DEBUT,
 *   NVL(SUM(NVL(E.TEMPSESTIME,0)),0) as TOTAL_ESTIME,
 *   NVL(SUM(NVL(E.TEMPSREEL,0)),0)   as TOTAL_REEL,
 *   NVL(SUM(CASE WHEN E.DATEFIN IS NOT NULL THEN 1 ELSE 0 END),0) as ETAPES_TERMINE,
 *   NVL(COUNT(E.IDETAPE),0) as TOTAL_ETAPES
 * FROM FABRICATION F
 * LEFT JOIN ETAPE E ON F.IDFABRICATION = E.IDFABRICATION
 * GROUP BY F.IDFABRICATION, F.IDMODELE, F.QUANTITE_A_PRODUIRE, F.QUALITE, F.DATE_DEBUT
 * ORDER BY F.IDFABRICATION
 */
void MainWindow::loadFabricationsSuivi()
{
    chargerTableSuiviAvecComplexite();
}

void MainWindow::chargerTableSuiviAvecComplexite()
{
    auto *cbStatut  = findChild<QComboBox*>("cb_filtre_statut");
    auto *cbQualite = findChild<QComboBox*>("cb_filtre_qualite");
    auto *leId      = findChild<QLineEdit*>("le_filtre_id");

    const QString filtreStatut  = cbStatut  ? cbStatut->currentText()  : QStringLiteral("Tous");
    const QString filtreQualite = cbQualite ? cbQualite->currentText() : QStringLiteral("Toutes qualités");
    const QString filtreId      = leId      ? leId->text().trimmed()   : QString();

    loadFabricationsSuiviFiltre(filtreStatut, filtreQualite, filtreId);
}

void MainWindow::loadFabricationsSuiviFiltre(const QString &filtreStatut,
                                             const QString &filtreQualite,
                                             const QString &filtreId)
{
    // Colonnes compactes comme interface cible.
    ui->table_fabrications_etapes->setColumnCount(5);
    QStringList headers = {
        "ID Fabrication", "Nom Mod", "Date Debut", "Duree (jours)", "Complexité"
    };
    ui->table_fabrications_etapes->setHorizontalHeaderLabels(headers);
    ui->table_fabrications_etapes->setRowCount(0);

    // Style du header
    ui->table_fabrications_etapes->horizontalHeader()->setStyleSheet(
        "QHeaderView::section {"
        "  background-color: #C8B89A; color: #2E1A00;"
        "  font-weight: bold; font-size: 10px;"
        "  padding: 6px; border: 1px solid #B8956A;"
        "}");
    ui->table_fabrications_etapes->setAlternatingRowColors(false);
    ui->table_fabrications_etapes->setStyleSheet(
        "QTableWidget { gridline-color: #D4B896; background: #EDE0C8; color: #2E1A00; }"
        "QTableWidget::item:selected { background: #D9C7A6; color:#2E1A00; }");

    // ── Requête compacte par fabrication ────────────────────
    // Les étapes sont agrégées pour éviter de dupliquer une fabrication.
    QString sql =
        "SELECT F.IDFABRICATION, "
        "       NVL(M.NOM, TRIM(TO_CHAR(F.IDMODELE))) AS NOM_MODELE, "
        "       F.DATE_DEBUT, F.QUALITE, "
        "       NVL((SELECT SUM(NVL(E.TEMPSESTIME,0)) FROM ETAPE E WHERE E.IDFABRICATION = F.IDFABRICATION), 0) AS TOTAL_ESTIME, "
        "       NVL((SELECT SUM(NVL(E.TEMPSREEL,0)) FROM ETAPE E WHERE E.IDFABRICATION = F.IDFABRICATION), 0) AS TOTAL_REEL, "
        "       NVL((SELECT COUNT(*) FROM ETAPE E WHERE E.IDFABRICATION = F.IDFABRICATION), 0) AS NB_ETAPES "
        "FROM FABRICATION F "
        "LEFT JOIN MODELE M ON (TRIM(TO_CHAR(M.IDMODELE)) = TRIM(TO_CHAR(F.IDMODELE)) "
        "                     OR UPPER(TRIM(M.NOM)) = UPPER(TRIM(TO_CHAR(F.IDMODELE)))) ";

    // Clause WHERE dynamique
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

    if (!query.exec()) {
        qDebug() << "loadFabricationsSuiviFiltre error:" << query.lastError().text();
        return;
    }

    int row = 0;
    while (query.next()) {
        int fabId = query.value(0).toInt();
        QString nomModele = query.value(1).toString();
        QDate dateDebutFab = query.value(2).toDate();
        QString qualite = query.value(3).toString();
        double tempsEstime = query.value(4).toDouble();
        double tempsReel = query.value(5).toDouble();
        int nbEtapes = query.value(6).toInt();

        int dureeJours = 0;
        if (dateDebutFab.isValid()) {
            dureeJours = qMax(1, dateDebutFab.daysTo(QDate::currentDate()));
        } else if (tempsEstime > 0) {
            dureeJours = qMax(1, (int)qRound(tempsEstime / 45.0));
        }

        Q_UNUSED(tempsReel);
        QString complexite = nbEtapes > 0
            ? calculerComplexiteFabrication(fabId)
            : QStringLiteral("Faible");
        QString statutGlobal = calculerStatutFabrication(fabId);

        // Filtrage par statut (calculé côté C++)
        if (filtreStatut != "Tous" && statutGlobal != filtreStatut)
            continue;

        ui->table_fabrications_etapes->insertRow(row);

        auto makeItem = [&](const QString &text, Qt::AlignmentFlag align = Qt::AlignCenter) {
            auto *it = new QTableWidgetItem(text);
            it->setTextAlignment(align);
            it->setForeground(QColor("#FFFFFF"));
            it->setBackground(QColor("#F39C12"));
            it->setFlags(it->flags() & ~Qt::ItemIsEditable);
            return it;
        };

                    QString complexiteLabel = nbEtapes > 0 ? complexite : QStringLiteral("Faible");
                    QColor pillColor(couleurComplexiteBadge(complexiteLabel));

        auto *idItem = makeItem(QString::number(fabId));
        auto *modItem = makeItem(nomModele, Qt::AlignLeft);
        auto *dateItem = makeItem(dateDebutFab.toString("dd/MM/yyyy"));
        auto *dureeItem = makeItem(QString::number(dureeJours));
                auto *predItem = makeItem(QString("● %1").arg(complexiteLabel), Qt::AlignLeft);

        idItem->setBackground(pillColor);
        modItem->setBackground(pillColor);
        dateItem->setBackground(pillColor);
        dureeItem->setBackground(pillColor);
        predItem->setBackground(pillColor);

        ui->table_fabrications_etapes->setItem(row, 0, idItem);
        ui->table_fabrications_etapes->setItem(row, 1, modItem);
        ui->table_fabrications_etapes->setItem(row, 2, dateItem);
        ui->table_fabrications_etapes->setItem(row, 3, dureeItem);
        ui->table_fabrications_etapes->setItem(row, 4, predItem);

        // Tooltip détaillé
        QString tooltip = QString(
                      "<b>FAB-%1</b><br>"
                      "Modele: <b>%2</b><br>"
                      "Qualite: %3<br>"
                      "Etapes: %4<br>"
                      "Duree: %5 jours<br>"
                      "Complexité: <b>%6</b><br>"
                      "Temps estimé total: %7"
                      ).arg(fabId)
                      .arg(nomModele)
                      .arg(qualite)
                      .arg(nbEtapes)
                      .arg(dureeJours)
                      .arg(complexiteLabel)
                      .arg(QString::number(tempsEstime, 'f', 0));

        for (int c = 0; c < 5; c++) {
            auto *it = ui->table_fabrications_etapes->item(row, c);
            if (it) it->setToolTip(tooltip);
        }

        row++;
    }

    // Redimensionnement des colonnes
    ui->table_fabrications_etapes->setColumnWidth(0, 115);
    ui->table_fabrications_etapes->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->table_fabrications_etapes->setColumnWidth(2, 115);
    ui->table_fabrications_etapes->setColumnWidth(3, 105);
    ui->table_fabrications_etapes->setColumnWidth(4, 115);
    ui->table_fabrications_etapes->verticalHeader()->setDefaultSectionSize(34);

    qDebug() << "Tableau suivi chargé :" << row << " lignes";
}

// ═════════════════════════════════════════════════════════════
//  PARTIE 3 — PANNEAU DÉTAIL
// ═════════════════════════════════════════════════════════════

/*
 * Requête panneau détail :
 * SELECT F.IDFABRICATION, F.IDMODELE, F.QUANTITE_A_PRODUIRE, F.QUALITE,
 *        F.COMMENTAIRE, F.DATE_DEBUT,
 *        E.IDETAPE, E.NOMETAPE, E.ORDRE, E.TEMPSESTIME, E.TEMPSREEL,
 *        E.DATEDEBUT, E.DATEFIN, E.CIN
 * FROM FABRICATION F
 * LEFT JOIN ETAPE E ON F.IDFABRICATION = E.IDFABRICATION
 * WHERE F.IDFABRICATION = :id
 * ORDER BY E.ORDRE
 */
void MainWindow::afficherPanneauDetail(int fabId)
{
    // Compatibilité: utiliser le frame designer s'il existe, sinon créer un panneau dynamique.
    QFrame *panneauDetail = this->findChild<QFrame*>("frame_detail_fab");
    if (!panneauDetail) {
        panneauDetail = this->findChild<QFrame*>("frame_detail_fab_dynamic");
        if (!panneauDetail) {
            panneauDetail = new QFrame(ui->gb_timeline_visuelle);
            panneauDetail->setObjectName("frame_detail_fab_dynamic");
            if (ui->vl_timeline_box)
                ui->vl_timeline_box->insertWidget(1, panneauDetail);
        }
    }
    if (!panneauDetail) return;

    // Vider le panneau
    QLayout *oldLayout = panneauDetail->layout();
    if (oldLayout) {
        QLayoutItem *item;
        while ((item = oldLayout->takeAt(0)) != nullptr) {
            if (item->widget()) item->widget()->deleteLater();
            delete item;
        }
        delete oldLayout;
    }

    QVBoxLayout *mainVL = new QVBoxLayout(panneauDetail);
    mainVL->setContentsMargins(12, 12, 12, 12);
    mainVL->setSpacing(8);

    panneauDetail->setStyleSheet(
        "QFrame { background: #FDFAF4; border-radius: 10px; border: 1px solid #D4B896; }");

    // ── Titre ────────────────────────────────────────────────
    QLabel *titre = new QLabel(QString("Detail - FAB-%1").arg(fabId));
    titre->setStyleSheet(
        "color: #7F4129; font-size: 13px; font-weight: bold; background: transparent;");
    mainVL->addWidget(titre);

    QFrame *sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #D4B896;");
    mainVL->addWidget(sep);

    // ── Requête données fabrication ──────────────────────────
    QSqlQuery q;
    q.prepare(
        "SELECT F.IDFABRICATION, F.IDMODELE, F.QUANTITE_A_PRODUIRE, F.QUALITE, "
        "       F.COMMENTAIRE, F.DATE_DEBUT "
        "FROM FABRICATION F WHERE F.IDFABRICATION = :id");
    q.bindValue(":id", fabId);

    if (!q.exec() || !q.next()) return;

    QString commentaire = q.value(4).toString();
    QDate   dateDebut   = q.value(5).toDate();

    // Calculs globaux
    double progression   = calculerProgression(fabId);
    QString statutGlobal = calculerStatutFabrication(fabId);
    QString complexite   = calculerComplexiteFabrication(fabId);
    QString etapeAct     = getEtapeActuelle(fabId);
    QString prochaine    = getProchainEtape(fabId);

    // Temps totaux
    double totalEstime = 0.0, totalReel = 0.0;
    QSqlQuery qT;
    qT.prepare("SELECT SUM(NVL(TEMPSESTIME,0)), SUM(NVL(TEMPSREEL,0)) "
               "FROM ETAPE WHERE IDFABRICATION = :id");
    qT.bindValue(":id", fabId);
    if (qT.exec() && qT.next()) {
        totalEstime = qT.value(0).toDouble();
        totalReel   = qT.value(1).toDouble();
    }

    int nbTotal = 0, nbTermine = 0;
    QSqlQuery qC;
    qC.prepare("SELECT COUNT(*), SUM(CASE WHEN DATEFIN IS NOT NULL THEN 1 ELSE 0 END) "
               "FROM ETAPE WHERE IDFABRICATION = :id");
    qC.bindValue(":id", fabId);
    if (qC.exec() && qC.next()) {
        nbTotal   = qC.value(0).toInt();
        nbTermine = qC.value(1).toInt();
    }

    // ── Grille d'informations ────────────────────────────────
    auto addInfoRow = [&](const QString &label, const QString &value, const QString &color = "#2E1A00") {
        QHBoxLayout *hl = new QHBoxLayout();
        hl->setSpacing(6);
        QLabel *lbl = new QLabel(label + ":");
        lbl->setStyleSheet("color: #8D6E63; font-size: 10px; background: transparent;");
        lbl->setFixedWidth(130);
        QLabel *val = new QLabel(value);
        val->setStyleSheet(QString("color: %1; font-size: 10px; font-weight: bold; background: transparent;").arg(color));
        val->setWordWrap(true);
        hl->addWidget(lbl);
        hl->addWidget(val);
        hl->addStretch();
        mainVL->addLayout(hl);
    };

    addInfoRow("ID Fabrication",     QString::number(fabId));
    addInfoRow("ID Modèle",          q.value(1).toString());
    addInfoRow("Quantité à produire",QString::number(q.value(2).toInt()));
    addInfoRow("Qualité",            q.value(3).toString());
    addInfoRow("Date début",         dateDebut.toString("dd/MM/yyyy"));
    addInfoRow("Commentaire",        commentaire.isEmpty() ? "—" : commentaire);
    addInfoRow("Étapes",             QString("%1 / %2 terminée(s)").arg(nbTermine).arg(nbTotal));
    addInfoRow("Étape actuelle",     etapeAct, "#6B3D1E");
    addInfoRow("Prochaine étape",    prochaine, "#6B3D1E");
    addInfoRow("Temps estimé total", QString::number(totalEstime, 'f', 1) + " h");
    addInfoRow("Temps réel total",   QString::number(totalReel,   'f', 1) + " h");
    addInfoRow("Écart",
               (totalReel - totalEstime >= 0 ? "+" : "") + QString::number(totalReel - totalEstime, 'f', 1) + " h",
               "#6B3D1E");
    addInfoRow("Statut global",      statutGlobal,
               "#6B3D1E");
    addInfoRow("Complexité",         complexite, couleurComplexiteBadge(complexite));

    // ── Barre de progression ─────────────────────────────────
    QLabel *lblProg = new QLabel(QString("Progression globale : %1%").arg(progression, 0, 'f', 1));
    lblProg->setStyleSheet("color: #2E1A00; font-size: 10px; background: transparent; margin-top: 6px;");
    mainVL->addWidget(lblProg);

    QProgressBar *pb = new QProgressBar();
    pb->setRange(0, 100);
    pb->setValue((int)progression);
    pb->setFormat(QString::number(progression, 'f', 0) + "%");
    pb->setFixedHeight(18);
    pb->setStyleSheet(
        "QProgressBar { background:#F0E4CD; border:1px solid #B8956A; border-radius:9px; color:#2E1A00; font-size:9px; }"
        "QProgressBar::chunk { background:#C8B89A; border-radius:9px; }");
    mainVL->addWidget(pb);

    // ── Liste des étapes ─────────────────────────────────────
    QFrame *sepH = new QFrame();
    sepH->setFrameShape(QFrame::HLine);
    sepH->setStyleSheet("color: #D4B896; margin-top: 6px;");
    mainVL->addWidget(sepH);

    QLabel *lblEtapes = new QLabel("Liste des etapes");
    lblEtapes->setStyleSheet("color: #7F4129; font-size: 11px; font-weight: bold; background: transparent;");
    mainVL->addWidget(lblEtapes);

    QSqlQuery qE;
    qE.prepare(
        "SELECT E.NOMETAPE, E.ORDRE, E.TEMPSESTIME, E.TEMPSREEL, "
        "       E.DATEDEBUT, E.DATEFIN, E.CIN "
        "FROM ETAPE E "
        "WHERE E.IDFABRICATION = :id "
        "ORDER BY E.ORDRE");
    qE.bindValue(":id", fabId);

    if (qE.exec()) {
        while (qE.next()) {
            QString nomEtape   = qE.value(0).toString();
            int     ordre      = qE.value(1).toInt();
            double  teEstime   = qE.value(2).toDouble();
            double  teReel     = qE.value(3).toDouble();
            QDate   dd         = qE.value(4).toDate();
            QDate   df         = qE.value(5).toDate();
            QString cin        = qE.value(6).toString();

            QString statutEtape = calculerStatutEtape(dd, df, teEstime);
            QFrame *etapeFrame = new QFrame();
            etapeFrame->setStyleSheet(
                "QFrame { background: #FDFAF4; border: 1px solid #D4B896; border-radius: 6px; margin: 2px 0; }");

            QHBoxLayout *eHL = new QHBoxLayout(etapeFrame);
            eHL->setContentsMargins(8, 4, 8, 4);

            QLabel *eLabel = new QLabel(
                QString("[%1] %2  |  Est: %3h  Réel: %4h  |  CIN: %5  |  %6")
                    .arg(ordre).arg(nomEtape)
                    .arg(teEstime, 0, 'f', 1).arg(teReel, 0, 'f', 1)
                    .arg(cin.isEmpty() ? "—" : cin)
                    .arg(statutEtape));
            eLabel->setStyleSheet("color: #2E1A00; font-size: 9px; background: transparent;");
            eLabel->setWordWrap(true);

            QLabel *eBadge = new QLabel("•");
            eBadge->setStyleSheet("color:#8D6E63; font-size: 12px; background: transparent;");
            eBadge->setFixedWidth(20);

            eHL->addWidget(eBadge);
            eHL->addWidget(eLabel);
            mainVL->addWidget(etapeFrame);
        }
    }

    mainVL->addStretch();
}

// ═════════════════════════════════════════════════════════════
//  PARTIE 4 — TIMELINE AMÉLIORÉE
// ═════════════════════════════════════════════════════════════

FabricationSuivi MainWindow::getFabricationSuivi(int fabId)
{
    FabricationSuivi fab;
    fab.id       = fabId;
    fab.quantite = 0;

    QSqlQuery q;
    q.prepare(
        "SELECT F.IDFABRICATION, M.NOM, F.QUANTITE_A_PRODUIRE, "
        "F.QUALITE, F.DATE_DEBUT "
        "FROM FABRICATION F "
        "LEFT JOIN MODELE M ON (TRIM(TO_CHAR(M.IDMODELE)) = TRIM(TO_CHAR(F.IDMODELE)) "
        "                     OR UPPER(TRIM(M.NOM)) = UPPER(TRIM(TO_CHAR(F.IDMODELE)))) "
        "WHERE F.IDFABRICATION = :id");
    q.bindValue(":id", fabId);

    if (q.exec() && q.next()) {
        fab.modele    = q.value(1).toString();
        fab.quantite  = q.value(2).toInt();
        fab.qualite   = q.value(3).toString();
        fab.dateDebut = q.value(4).toDate();
    }

    /*
     * Requête timeline (Oracle) :
     * - Chargement des étapes triées par ORDRE
     * - Calcul SQL du retard pour éviter des ambiguïtés côté C++
     *
     * SELECT IDETAPE, NOMETAPE, ORDRE, TEMPSESTIME, TEMPSREEL,
     *        DATEDEBUT, DATEFIN, E.CIN, P.NOM,
     *        CASE
     *          WHEN DATEFIN IS NOT NULL AND NVL(TEMPSREEL,0) > NVL(TEMPSESTIME,0) THEN 1
     *          WHEN DATEFIN IS NULL AND DATEDEBUT IS NOT NULL
     *               AND NVL(TEMPSESTIME,0) > 0
     *               AND ((SYSDATE - DATEDEBUT) * 24 * 60) > NVL(TEMPSESTIME,0) THEN 1
     *          ELSE 0
     *        END AS EST_RETARD
     * FROM ETAPE E
     * LEFT JOIN PERSONNEL P ON E.CIN = P.CIN
     * WHERE E.IDFABRICATION = :id
     * ORDER BY E.ORDRE ASC, E.IDETAPE ASC;
     */
    QSqlQuery qe;
    qe.prepare(
        "SELECT IDETAPE, NOMETAPE, ORDRE, TEMPSESTIME, TEMPSREEL, "
        "DATEDEBUT, DATEFIN, E.CIN, P.NOM, "
        "CASE "
        "  WHEN DATEFIN IS NOT NULL AND NVL(TEMPSREEL,0) > NVL(TEMPSESTIME,0) THEN 1 "
        "  WHEN DATEFIN IS NULL AND DATEDEBUT IS NOT NULL "
        "       AND NVL(TEMPSESTIME,0) > 0 "
        "       AND ((SYSDATE - DATEDEBUT) * 24 * 60) > NVL(TEMPSESTIME,0) THEN 1 "
        "  ELSE 0 "
        "END AS EST_RETARD "
        "FROM ETAPE E "
        "LEFT JOIN PERSONNEL P ON E.CIN = P.CIN "
        "WHERE IDFABRICATION = :id "
        "ORDER BY ORDRE ASC, IDETAPE ASC");
    qe.bindValue(":id", fabId);

    if (qe.exec()) {
        while (qe.next()) {
            EtapeSuivi e;
            e.id           = qe.value(0).toInt();
            e.nom          = qe.value(1).toString();
            e.ordre        = qe.value(2).toInt();
            e.tempsEstime  = qe.value(3).toDouble();
            e.tempsReel    = qe.value(4).toDouble();
            e.dateDebut    = qe.value(5).toDate();
            e.dateFin      = qe.value(6).toDate();
            e.cin          = qe.value(7).toString();
            e.personnelNom = qe.value(8).toString();
            e.estRetard    = qe.value(9).toInt() == 1;

            // Statut initial, sera remplacé par l'état timeline via computeTimelineStates().
            e.statut = e.dateFin.isValid() ? "TERMINEE" : "A_VENIR";

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

    if (fab.etapes.isEmpty())
        return;

    int completeCount = 0;
    int idxCurrent = -1;
    int idxFirstNotFinished = -1;

    // 1) Marquage de base: TERMINEE si DATEFIN renseignée, sinon A_VENIR.
    for (int i = 0; i < fab.etapes.size(); ++i) {
        EtapeSuivi &e = fab.etapes[i];
        const bool isFinished = e.dateFin.isValid();

        if (isFinished)
            ++completeCount;

        if (!isFinished && idxFirstNotFinished < 0)
            idxFirstNotFinished = i;

        // RETARD terminé: temps réel > temps estimé
        if (isFinished && e.estRetard) {
            e.statut = "RETARD";
        } else if (isFinished) {
            e.statut = "TERMINEE";
        } else {
            e.statut = "A_VENIR";
        }
    }

    // 2) Étape ACTUELLE = première étape non finie déjà démarrée.
    for (int i = 0; i < fab.etapes.size(); ++i) {
        const EtapeSuivi &e = fab.etapes[i];
        if (!e.dateFin.isValid() && e.dateDebut.isValid()) {
            idxCurrent = i;
            break;
        }
    }

    if (idxCurrent >= 0) {
        // Si l'étape actuelle est en retard, son état final devient RETARD.
        fab.etapes[idxCurrent].statut = fab.etapes[idxCurrent].estRetard ? "RETARD" : "ACTUELLE";
    }

    // 3) SUIVANTE = première étape non démarrée après l'étape actuelle.
    int idxNext = -1;
    if (idxCurrent >= 0) {
        for (int i = idxCurrent + 1; i < fab.etapes.size(); ++i) {
            const EtapeSuivi &e = fab.etapes[i];
            if (!e.dateDebut.isValid() && !e.dateFin.isValid()) {
                idxNext = i;
                break;
            }
        }
    } else if (idxFirstNotFinished >= 0) {
        // Aucune étape en cours: la première non finie devient la suivante.
        idxNext = idxFirstNotFinished;
    }

    if (idxNext >= 0 && fab.etapes[idxNext].statut == "A_VENIR")
        fab.etapes[idxNext].statut = "SUIVANTE";

    // 4) A_VENIR = tout ce qui reste après la suivante (non démarré / non terminé).
    for (int i = 0; i < fab.etapes.size(); ++i) {
        EtapeSuivi &e = fab.etapes[i];
        if (e.statut == "TERMINEE" || e.statut == "RETARD" || e.statut == "ACTUELLE" || e.statut == "SUIVANTE")
            continue;
        e.statut = "A_VENIR";
    }

    if (completedSteps)
        *completedSteps = completeCount;
    if (currentIndex)
        *currentIndex = idxCurrent;
    if (nextIndex)
        *nextIndex = idxNext;
    if (progression)
        *progression = fab.etapes.isEmpty() ? 0.0 : (100.0 * completeCount / fab.etapes.size());
}

void MainWindow::clearTimeline()
{
    QLayout *layout = ui->hl_timeline_steps;
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != nullptr) {
        if (item->widget() && item->widget() != ui->lbl_timeline_vide)
            item->widget()->deleteLater();
        delete item;
    }
}

QWidget* MainWindow::createStepBlock(const EtapeSuivi &etape)
{
    QColor col = QColor("#8D6E63");
    QString badgeText = etape.statut;

    if (etape.statut == "TERMINEE") col = QColor("#27AE60");
    else if (etape.statut == "ACTUELLE") col = QColor("#E67E22");
    else if (etape.statut == "SUIVANTE") col = QColor("#2980B9");
    else if (etape.statut == "A_VENIR") col = QColor("#8D6E63");
    else if (etape.statut == "RETARD") col = QColor("#C0392B");

    QString hexC = col.name();
    QString hexDark = col.darker(145).name();
    QString chipBg = col.lighter(130).name();
    QString chipBorder = col.lighter(160).name();
    const bool isCurrent = (etape.statut == "ACTUELLE" || etape.statut == "RETARD") && etape.dateFin.isNull();
    const bool isLateCurrent = (etape.statut == "RETARD") && etape.dateFin.isNull();

    QString stateDescription = "En attente de demarrage";
    if (etape.statut == "TERMINEE") stateDescription = "Operation finalisee";
    else if (etape.statut == "ACTUELLE") stateDescription = "Operation en cours";
    else if (etape.statut == "SUIVANTE") stateDescription = "Prochaine operation";
    else if (etape.statut == "RETARD") stateDescription = "Depassement detecte";

    TimelineStepCard *block = new TimelineStepCard(etape.id);
    block->setFixedWidth(210);
    block->setMinimumHeight(190);
    const QString borderStyle = isLateCurrent
        ? "3px solid #FF3B30"
        : (isCurrent ? "3px solid #F1C40F" : "2px solid " + col.lighter(130).name());

    const QString normalStyle = QString(
        "QFrame {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1," 
        "    stop:0 %1, stop:1 %2);"
        "  border-radius: 10px;"
        "  border: %3;"
        "}")
        .arg(hexC)
        .arg(hexDark)
        .arg(borderStyle);

    const QString hoverStyle = QString(
        "QFrame {"
        "  background: qlineargradient(x1:0,y1:0,x2:0,y2:1," 
        "    stop:0 %1, stop:1 %2);"
        "  border-radius: 10px;"
        "  border: %3;"
        "}")
        .arg(col.lighter(108).name())
        .arg(hexDark)
        .arg(borderStyle);

    block->setNormalStyle(normalStyle);
    block->setHoverStyle(hoverStyle);

    // Ouvre le popup détail de l'étape associée à la carte.
    block->onClicked = [this](int idEtape) {
        afficherDetailsEtape(idEtape);
    };

    QVBoxLayout *vl = new QVBoxLayout(block);
    vl->setContentsMargins(10, 8, 10, 8);
    vl->setSpacing(3);

    auto makeLabel = [](const QString &text, int fontSize, bool bold,
                        const QString &colorHex, bool wrap = true) -> QLabel* {
        QLabel *lbl = new QLabel(text);
        QString style = QString("color:%1; font-size:%2px; background:transparent;")
                            .arg(colorHex).arg(fontSize);
        if (bold) style += " font-weight:bold;";
        lbl->setStyleSheet(style);
        lbl->setWordWrap(wrap);
        return lbl;
    };

    // Badge visuel de l'etat timeline
    QFrame *stateChip = new QFrame(block);
    stateChip->setStyleSheet(QString(
        "QFrame {"
        "  background:%1;"
        "  border:1px solid %2;"
        "  border-radius:8px;"
        "}").arg(chipBg).arg(chipBorder));
    QHBoxLayout *chipLayout = new QHBoxLayout(stateChip);
    chipLayout->setContentsMargins(8, 2, 8, 2);
    chipLayout->setSpacing(6);

    QLabel *dot = new QLabel("o", stateChip);
    dot->setStyleSheet("color:#FFFFFF; font-size:10px; font-weight:bold; background:transparent;");
    QLabel *chipText = new QLabel("ETAT: " + badgeText, stateChip);
    chipText->setStyleSheet("color:#FFFFFF; font-size:9px; font-weight:bold; background:transparent;");
    chipLayout->addWidget(dot);
    chipLayout->addWidget(chipText);
    chipLayout->addStretch();
    vl->addWidget(stateChip);
    vl->addWidget(makeLabel(stateDescription, 8, false, "rgba(255,255,255,0.86)"));

    // Indicateur étape active / retard
    if (isLateCurrent) {
        QLabel *actif = new QLabel("RETARD - ETAPE ACTUELLE");
        actif->setStyleSheet(
            "color:#FFDDDD; font-size:8px; font-weight:bold; background:transparent;");
        vl->addWidget(actif);
    } else if (isCurrent) {
        QLabel *actif = new QLabel("ETAPE ACTUELLE");
        actif->setStyleSheet(
            "color:#F1C40F; font-size:8px; font-weight:bold; background:transparent;");
        vl->addWidget(actif);
    }

    vl->addWidget(makeLabel(
        QString("Etape %1 - %2").arg(etape.ordre).arg(etape.nom), 11, true, "#FFFFFF"));

    // Dates
    QString dates;
    if (etape.dateDebut.isValid())
        dates += "Début: " + etape.dateDebut.toString("dd/MM/yy");
    if (etape.dateFin.isValid()) {
        if (!dates.isEmpty()) dates += "\n";
        dates += "Fin: " + etape.dateFin.toString("dd/MM/yy");
    }
    if (!dates.isEmpty())
        vl->addWidget(makeLabel(dates, 8, false, "rgba(255,255,255,0.80)"));

    // Temps estimé / réel
    vl->addWidget(makeLabel(
        QString("Est: %1h  /  Réel: %2h")
            .arg(etape.tempsEstime, 0, 'f', 1)
            .arg(etape.tempsReel,   0, 'f', 1),
        8, false, "rgba(255,255,255,0.75)"));

    // Durée écoulée (simple)
    if (etape.dateDebut.isValid()) {
        const QDate finRef = etape.dateFin.isValid() ? etape.dateFin : QDate::currentDate();
        const int dureeJours = qMax(0, etape.dateDebut.daysTo(finRef));
        vl->addWidget(makeLabel(QString("Duree: %1 jour(s)").arg(dureeJours), 8, false, "rgba(255,255,255,0.75)"));
    }

    // Personnel
    if (!etape.personnelNom.isEmpty()) {
        vl->addWidget(makeLabel("👤 " + etape.personnelNom, 8, false, "rgba(255,255,255,0.80)"));
    } else if (!etape.cin.isEmpty()) {
        vl->addWidget(makeLabel("CIN: " + etape.cin, 8, false, "rgba(255,255,255,0.70)"));
    }

    return block;
}

QString MainWindow::calculerEtatEtape(const QVariant &dateDebut,
                                      const QVariant &dateFin,
                                      int tempsEstime,
                                      int tempsReel)
{
    const bool hasDateDebut = dateDebut.isValid() && !dateDebut.isNull() && dateDebut.toDate().isValid();
    const bool hasDateFin = dateFin.isValid() && !dateFin.isNull() && dateFin.toDate().isValid();
    const int tempsReelSafe = qMax(0, tempsReel);
    const int tempsEstimeSafe = qMax(0, tempsEstime);

    if (!hasDateDebut)
        return "NON_PLANIFIEE";

    if (hasDateFin && tempsReelSafe <= tempsEstimeSafe)
        return "TERMINEE";

    if (hasDateFin && tempsReelSafe > tempsEstimeSafe)
        return "EN_RETARD";

    if (hasDateDebut && !hasDateFin) {
        const QDate dateDebutValeur = dateDebut.toDate();
        if (tempsReelSafe <= 0 && dateDebutValeur.isValid() && dateDebutValeur > QDate::currentDate())
            return "PLANIFIEE";
        return "EN_COURS";
    }

    return "INCONNUE";
}

QString MainWindow::formaterDate(const QVariant &value)
{
    if (!value.isValid() || value.isNull())
        return "-";

    const QDateTime dateTime = value.toDateTime();
    if (dateTime.isValid())
        return dateTime.toString("dd/MM/yyyy HH:mm");

    const QDate date = value.toDate();
    return date.isValid() ? date.toString("dd/MM/yyyy") : "-";
}

QString MainWindow::formaterTexte(const QVariant &value)
{
    if (!value.isValid() || value.isNull())
        return "-";

    const QString txt = value.toString().trimmed();
    return txt.isEmpty() ? "-" : txt;
}

int MainWindow::cinPersonnelParDefaut() const
{
    return 12345677;
}

QVariantMap MainWindow::chargerDonneesEtape(int idEtape)
{
    QVariantMap data;
    if (idEtape <= 0)
        return data;

    QSqlDatabase db = Connexion::createInstance().getDatabase();
    if (!db.isOpen())
        return data;

    QSqlQuery q(db);
    q.prepare(
        "SELECT "
        "E.IDETAPE, E.CIN, E.IDFABRICATION, E.NOMETAPE, E.ORDRE, "
        "E.TEMPSESTIME, E.TEMPSREEL, E.DATEDEBUT, E.DATEFIN, "
        "P.CIN AS PERSONNEL_CIN, "
        "P.NOM AS PERSONNEL_NOM, "
        "P.PRENOM AS PERSONNEL_PRENOM, "
        "P.POSTE AS PERSONNEL_POSTE "
        "FROM ETAPE E "
        "LEFT JOIN PERSONNEL P ON E.CIN = P.CIN "
        "WHERE E.IDETAPE = :idEtape");
    q.bindValue(":idEtape", idEtape);

    qDebug() << "ID Etape:" << idEtape;

    if (!q.exec()) {
        qDebug() << "chargerDonneesEtape erreur SQL:" << q.lastError().text();
        return data;
    }
    if (!q.next()) {
        qDebug() << "chargerDonneesEtape: aucune ligne trouvee pour l'etape" << idEtape;
        return data;
    }

    data.insert("IDETAPE", q.value("IDETAPE"));
    data.insert("CIN", q.value("CIN"));
    data.insert("IDFABRICATION", q.value("IDFABRICATION"));
    data.insert("NOMETAPE", q.value("NOMETAPE"));
    data.insert("ORDRE", q.value("ORDRE"));
    data.insert("TEMPSESTIME", q.value("TEMPSESTIME"));
    data.insert("TEMPSREEL", q.value("TEMPSREEL"));
    data.insert("DATEDEBUT", q.value("DATEDEBUT"));
    data.insert("DATEFIN", q.value("DATEFIN"));
    data.insert("PERSONNEL_CIN", q.value("PERSONNEL_CIN"));
    data.insert("PERSONNEL_NOM", q.value("PERSONNEL_NOM"));
    data.insert("PERSONNEL_PRENOM", q.value("PERSONNEL_PRENOM"));
    data.insert("PERSONNEL_POSTE", q.value("PERSONNEL_POSTE"));

    data.insert("NOM", q.value("PERSONNEL_NOM"));
    data.insert("PRENOM", q.value("PERSONNEL_PRENOM"));
    data.insert("POSTE", q.value("PERSONNEL_POSTE"));

    const QVariant cinEtape = q.value("CIN");
    const QString cinEtapeTexte = cinEtape.toString().trimmed();
    const bool cinVideOuNul = cinEtape.isNull() || cinEtapeTexte.isEmpty() || cinEtapeTexte == "0";

    if (cinVideOuNul) {
        // Si ETAPE.CIN est vide ou nul, on charge le personnel par défaut au lieu d'afficher "-".
        QSqlQuery personnel(db);
        personnel.prepare(
            "SELECT CIN, NOM, PRENOM, POSTE "
            "FROM PERSONNEL "
            "WHERE CIN = :cin");
        personnel.bindValue(":cin", cinPersonnelParDefaut());

        if (personnel.exec() && personnel.next()) {
            data.insert("CIN", personnel.value("CIN"));
            data.insert("PERSONNEL_CIN", personnel.value("CIN"));
            data.insert("PERSONNEL_NOM", personnel.value("NOM"));
            data.insert("PERSONNEL_PRENOM", personnel.value("PRENOM"));
            data.insert("PERSONNEL_POSTE", personnel.value("POSTE"));
            data.insert("NOM", personnel.value("NOM"));
            data.insert("PRENOM", personnel.value("PRENOM"));
            data.insert("POSTE", personnel.value("POSTE"));
        } else {
            qDebug() << "chargerDonneesEtape: personnel par defaut introuvable pour CIN" << cinPersonnelParDefaut();
        }
    }

    qDebug() << "ID Etape:" << idEtape;
    qDebug() << "CIN ETAPE =" << data.value("CIN").toString();
    qDebug() << "Nom personnel =" << data.value("PERSONNEL_NOM").toString();
    qDebug() << "Prénom =" << data.value("PERSONNEL_PRENOM").toString();
    qDebug() << "Poste =" << data.value("PERSONNEL_POSTE").toString();

    const int tempsEstime = qRound(q.value("TEMPSESTIME").toDouble());
    const int tempsReel = qRound(q.value("TEMPSREEL").toDouble());
    data.insert("ETAT", calculerEtatEtape(q.value("DATEDEBUT"), q.value("DATEFIN"), tempsEstime, tempsReel));
    data.insert("RETARD", qMax(0, tempsReel - tempsEstime));
    return data;
}

bool MainWindow::existeEtapeActiveDansFabrication(int idFabrication, int idEtapeCourante)
{
    if (idFabrication <= 0)
        return false;

    QSqlDatabase db = Connexion::createInstance().getDatabase();
    if (!db.isOpen())
        return false;

    QSqlQuery q(db);
    QString sql =
        "SELECT COUNT(*) "
        "FROM ETAPE "
        "WHERE IDFABRICATION = :idFabrication "
        "  AND DATEDEBUT IS NOT NULL "
        "  AND DATEFIN IS NULL ";
    if (idEtapeCourante > 0)
        sql += " AND IDETAPE <> :idEtapeCourante";

    q.prepare(sql);
    q.bindValue(":idFabrication", idFabrication);
    if (idEtapeCourante > 0)
        q.bindValue(":idEtapeCourante", idEtapeCourante);

    if (!q.exec() || !q.next())
        return false;

    return q.value(0).toInt() > 0;
}

bool MainWindow::peutDemarrerEtape(int idEtape)
{
    const QVariantMap data = chargerDonneesEtape(idEtape);
    if (data.isEmpty())
        return false;

    const int idFabrication = data.value("IDFABRICATION").toInt();
    const int ordre = data.value("ORDRE").toInt();
    const QVariant dateDebut = data.value("DATEDEBUT");
    const QVariant dateFin = data.value("DATEFIN");
    if (dateDebut.isValid() && !dateDebut.isNull())
        return false;
    if (dateFin.isValid() && !dateFin.isNull())
        return false;

    QSqlDatabase db = Connexion::createInstance().getDatabase();
    if (!db.isOpen())
        return false;

    QSqlQuery qPrev(db);
    qPrev.prepare(
        "SELECT COUNT(*) "
        "FROM ETAPE "
        "WHERE IDFABRICATION = :idFabrication "
        "  AND ORDRE < :ordre "
        "  AND DATEFIN IS NULL");
    qPrev.bindValue(":idFabrication", idFabrication);
    qPrev.bindValue(":ordre", ordre);

    if (!qPrev.exec() || !qPrev.next())
        return false;
    if (qPrev.value(0).toInt() > 0)
        return false;

    if (existeEtapeActiveDansFabrication(idFabrication, idEtape))
        return false;

    return true;
}

QString MainWindow::couleurEtatEtapeBadge(const QString &etat) const
{
    if (etat == "NON_PLANIFIEE") return "#7F8C8D";
    if (etat == "PLANIFIEE")     return "#2980B9";
    if (etat == "EN_COURS")      return "#E67E22";
    if (etat == "TERMINEE")      return "#27AE60";
    if (etat == "EN_RETARD")     return "#C0392B";
    return "#8D6E63";
}

void MainWindow::afficherDetailsEtape(int idEtape)
{
    if (idEtape <= 0)
        return;

    // Mémorise l'ID étape porté par ce popup pour éviter tout accès à une sélection stale.
    m_idEtape = idEtape;

    QDialog dialog(this);
    dialog.setWindowTitle("Détails de l'étape");
    dialog.setModal(true);
    dialog.resize(640, 520);
    dialog.setStyleSheet(
        "QDialog { background: #F9F5EC; }"
        "QLabel#titreEtape { color: #2E1A00; font-size: 18px; font-weight: 700; }"
        "QLabel#champ { color: #5E4630; font-size: 12px; font-weight: 600; }"
        "QLabel#valeur { color: #2E1A00; font-size: 12px; }"
        "QFrame#toolbarDetail { background: #F2E8D7; border: 1px solid #D5C1A4; border-radius: 10px; }"
        "QFrame#carteDetail { background: #FDFAF4; border: 1px solid #D5C1A4; border-radius: 10px; }"
        "QPushButton { background: #7F4129; color: white; border: none; border-radius: 8px; padding: 8px 16px; }"
        "QPushButton:hover { background: #93553A; }"
        "QPushButton:disabled { background: #B8A38A; color: #EEE5D9; }");

    QVBoxLayout *root = new QVBoxLayout(&dialog);
    root->setContentsMargins(16, 16, 16, 16);
    root->setSpacing(12);

    QLabel *titre = new QLabel("Détails de l'étape");
    titre->setObjectName("titreEtape");
    root->addWidget(titre);

    // Barre d'actions en haut du popup.
    QFrame *toolbar = new QFrame();
    toolbar->setObjectName("toolbarDetail");
    QHBoxLayout *toolbarLayout = new QHBoxLayout(toolbar);
    toolbarLayout->setContentsMargins(10, 8, 10, 8);
    toolbarLayout->setSpacing(10);

    QPushButton *btnDemarrerEtape = new QPushButton("Démarrer étape");
    QPushButton *btnTerminerEtape = new QPushButton("Terminer étape");
    toolbarLayout->addWidget(btnDemarrerEtape);
    toolbarLayout->addWidget(btnTerminerEtape);
    toolbarLayout->addStretch();
    root->addWidget(toolbar);

    QFrame *carte = new QFrame();
    carte->setObjectName("carteDetail");
    QVBoxLayout *carteLayout = new QVBoxLayout(carte);
    carteLayout->setContentsMargins(14, 14, 14, 14);
    carteLayout->setSpacing(10);

    QLabel *badgeEtat = new QLabel("-");
    badgeEtat->setAlignment(Qt::AlignCenter);
    badgeEtat->setFixedHeight(28);
    carteLayout->addWidget(badgeEtat, 0, Qt::AlignLeft);

    QLabel *lblInfoEtape = new QLabel("");
    lblInfoEtape->setWordWrap(true);
    lblInfoEtape->setStyleSheet("color:#6B4F2A; font-style:italic;");
    lblInfoEtape->setVisible(false);
    carteLayout->addWidget(lblInfoEtape);

    QGridLayout *grille = new QGridLayout();
    grille->setHorizontalSpacing(18);
    grille->setVerticalSpacing(8);

    auto addLigne = [grille](int row, const QString &champ, const QString &valeur) {
        QLabel *lblChamp = new QLabel(champ);
        lblChamp->setObjectName("champ");
        QLabel *lblValeur = new QLabel(valeur);
        lblValeur->setObjectName("valeur");
        lblValeur->setWordWrap(true);
        lblValeur->setTextInteractionFlags(Qt::TextSelectableByMouse);
        grille->addWidget(lblChamp, row, 0, Qt::AlignTop);
        grille->addWidget(lblValeur, row, 1, Qt::AlignTop);
    };

    QLabel *valIdEtape = new QLabel("-");
    QLabel *valIdFab = new QLabel("-");
    QLabel *valComplexite = new QLabel("-");
    QLabel *valNomEtape = new QLabel("-");
    QLabel *valOrdre = new QLabel("-");
    QLabel *valCin = new QLabel("-");
    QLabel *valNomPers = new QLabel("-");
    QLabel *valPrenomPers = new QLabel("-");
    QLabel *valPostePers = new QLabel("-");
    QLabel *valDateDebut = new QLabel("-");
    QLabel *valDateFin = new QLabel("-");
    QLabel *valTempsEstime = new QLabel("-");
    QLabel *valTempsReel = new QLabel("-");
    QLabel *valEtat = new QLabel("-");
    QLabel *valRetard = new QLabel("-");

    auto addLigneDyn = [grille](int row, const QString &champ, QLabel *valueLabel) {
        QLabel *lblChamp = new QLabel(champ);
        lblChamp->setObjectName("champ");
        valueLabel->setObjectName("valeur");
        valueLabel->setWordWrap(true);
        valueLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
        grille->addWidget(lblChamp, row, 0, Qt::AlignTop);
        grille->addWidget(valueLabel, row, 1, Qt::AlignTop);
    };

    addLigneDyn(0, "ID Étape", valIdEtape);
    addLigneDyn(1, "ID Fabrication", valIdFab);
    addLigneDyn(2, "Complexité fabrication", valComplexite);
    addLigneDyn(3, "Nom Étape", valNomEtape);
    addLigneDyn(4, "Ordre", valOrdre);
    addLigneDyn(5, "CIN", valCin);
    addLigneDyn(6, "Nom du personnel", valNomPers);
    addLigneDyn(7, "Prénom du personnel", valPrenomPers);
    addLigneDyn(8, "Poste", valPostePers);
    addLigneDyn(9, "Date début", valDateDebut);
    addLigneDyn(10, "Date fin", valDateFin);
    addLigneDyn(11, "Temps estimé", valTempsEstime);
    addLigneDyn(12, "Temps réel", valTempsReel);
    addLigneDyn(13, "État calculé", valEtat);
    addLigneDyn(14, "Retard", valRetard);

    carteLayout->addLayout(grille);
    root->addWidget(carte);

    QHBoxLayout *actions = new QHBoxLayout();
    actions->addStretch();
    QPushButton *btnFermer = new QPushButton("Fermer");
    actions->addWidget(btnFermer);
    root->addLayout(actions);

    // Charge les données complètes ETAPE + PERSONNEL et rafraîchit le dialog.
    auto rafraichirDialog = [&, this]() {
        const QVariantMap data = chargerDonneesEtape(idEtape);
        if (data.isEmpty()) {
            showAnimatedMessageBox(QMessageBox::Warning,
                                   "Détails de l'étape",
                                   QString("Impossible de charger les informations pour l'étape #%1.").arg(idEtape));
            return;
        }

        const QVariant vIdEtape = data.value("IDETAPE");
        const QVariant vCin = data.value("CIN");
        const QVariant vIdFab = data.value("IDFABRICATION");
        const QVariant vNom = data.value("NOMETAPE");
        const QVariant vOrdre = data.value("ORDRE");
        const QVariant vTempsEstime = data.value("TEMPSESTIME");
        const QVariant vTempsReel = data.value("TEMPSREEL");
        const QVariant vDateDebut = data.value("DATEDEBUT");
        const QVariant vDateFin = data.value("DATEFIN");
        const QVariant vPersonnelCin = data.value("PERSONNEL_CIN");
        const QVariant vNomPers = data.value("PERSONNEL_NOM");
        const QVariant vPrenomPers = data.value("PERSONNEL_PRENOM");
        const QVariant vPostePers = data.value("PERSONNEL_POSTE");

        const int tempsEstime = vTempsEstime.isNull() ? 0 : qRound(vTempsEstime.toDouble());
        const int tempsReel = vTempsReel.isNull() ? 0 : qRound(vTempsReel.toDouble());
        QString etat = data.value("ETAT").toString();
        if (etat.isEmpty())
            etat = calculerEtatEtape(vDateDebut, vDateFin, tempsEstime, tempsReel);

        const int retard = qMax(0, tempsReel - tempsEstime);

        valIdEtape->setText(formaterTexte(vIdEtape));
        valIdFab->setText(formaterTexte(vIdFab));
        const int idFab = vIdFab.toInt();
        const QString complexiteFab = calculerComplexiteFabrication(idFab);

        valComplexite->setText(complexiteFab);
        valComplexite->setStyleSheet(
            QString("color:%1; font-weight:700;").arg(couleurComplexiteBadge(complexiteFab)));

        valNomEtape->setText(formaterTexte(vNom));
        valOrdre->setText(formaterTexte(vOrdre));
        // Si ETAPE.CIN vaut 0, vide ou une valeur non numérique, on affiche "-".
        const bool personnelTrouve = vPersonnelCin.isValid()
                         && !vPersonnelCin.isNull()
                         && !vPersonnelCin.toString().trimmed().isEmpty()
                         && vPersonnelCin.toString().trimmed() != "0";
        valCin->setText(personnelTrouve ? formaterTexte(vCin) : "-");
        valNomPers->setText(formaterTexte(vNomPers));
        valPrenomPers->setText(formaterTexte(vPrenomPers));
        valPostePers->setText(formaterTexte(vPostePers));
        valDateDebut->setText(formaterDate(vDateDebut));
        valDateFin->setText(formaterDate(vDateFin));
        valTempsEstime->setText(QString::number(tempsEstime));
        valTempsReel->setText(QString::number(tempsReel));
        valEtat->setText(etat);
        valRetard->setText(formaterRetardMinutes(retard));

        badgeEtat->setText(etat);
        badgeEtat->setStyleSheet(QString(
            "QLabel { background:%1; color:white; border-radius:14px; padding:4px 12px; font-weight:700; }")
            .arg(couleurEtatEtapeBadge(etat)));

        qDebug() << "Dialogue ETAPE" << idEtape
                 << "CIN=" << vCin.toString()
                 << "PERSONNEL_CIN=" << vPersonnelCin.toString()
                 << "PERSONNEL_NOM=" << vNomPers.toString()
                 << "PERSONNEL_PRENOM=" << vPrenomPers.toString()
                 << "PERSONNEL_POSTE=" << vPostePers.toString();

        QString info;
        const bool bloqueeParPrecedente = !peutDemarrerEtape(idEtape) && etat == "NON_PLANIFIEE";
        if (etat == "NON_PLANIFIEE") {
            if (existeEtapeActiveDansFabrication(idFab, idEtape))
                info = "Impossible de démarrer cette étape : une autre étape est déjà en cours pour cette fabrication.";
            else if (bloqueeParPrecedente)
                info = "Impossible de démarrer cette étape : une étape précédente n'est pas encore terminée.";
            else
                info = "Étape prête à démarrer.";
        } else if (etat == "PLANIFIEE") {
            info = "Cette étape a déjà été planifiée.";
        } else if (etat == "EN_COURS") {
            info = "Cette étape est en cours. Vous pouvez la terminer.";
        } else if (etat == "TERMINEE") {
            info = "Cette étape est terminée.";
        } else if (etat == "EN_RETARD") {
            info = "Cette étape est terminée avec retard.";
        }

        lblInfoEtape->setText(info);
        lblInfoEtape->setVisible(!info.isEmpty());

        const bool peutDemarrer = (etat == "NON_PLANIFIEE") && peutDemarrerEtape(idEtape);
        const bool peutTerminer = (etat == "EN_COURS");
        btnDemarrerEtape->setEnabled(peutDemarrer);
        btnTerminerEtape->setEnabled(peutTerminer);
    };

    QPointer<QDialog> safeDialog(&dialog);
    QPointer<QPushButton> safeBtnDemarrer(btnDemarrerEtape);
    QPointer<QPushButton> safeBtnTerminer(btnTerminerEtape);

    connect(btnDemarrerEtape, &QPushButton::clicked, this, [this, safeDialog, safeBtnDemarrer, &rafraichirDialog]() {
        qDebug() << "[popup] clic Démarrer étape";
        if (!safeDialog || !safeBtnDemarrer) {
            qDebug() << "[popup] dialog/bouton démarrer invalide -> stop";
            return;
        }
        if (m_idEtape <= 0) {
            qDebug() << "[popup] m_idEtape invalide pour démarrer:" << m_idEtape;
            return;
        }

        demarrerEtapeDepuisDialog(m_idEtape);

        if (!safeDialog) {
            qDebug() << "[popup] dialog détruit après démarrage -> pas de refresh dialog";
            return;
        }
        rafraichirDialog();
    });

    connect(btnTerminerEtape, &QPushButton::clicked, this, [this, safeDialog, safeBtnTerminer, &rafraichirDialog]() {
        qDebug() << "[popup] clic Terminer étape";
        if (!safeDialog || !safeBtnTerminer) {
            qDebug() << "[popup] dialog/bouton terminer invalide -> stop";
            return;
        }
        if (m_idEtape <= 0) {
            qDebug() << "[popup] m_idEtape invalide pour terminer:" << m_idEtape;
            return;
        }

        // Utilise uniquement l'ID passé au popup (pas de getSelectedEtapeId ici).
        terminerEtapeDepuisDialog(m_idEtape);

        if (!safeDialog) {
            qDebug() << "[popup] dialog détruit après terminaison -> pas de refresh dialog";
            return;
        }
        rafraichirDialog();
    });

    connect(btnFermer, &QPushButton::clicked, &dialog, &QDialog::accept);
    rafraichirDialog();
    dialog.exec();
}

void MainWindow::demarrerEtapeDepuisDialog(int idEtape)
{
    if (idEtape <= 0)
        return;

    QSqlDatabase db = Connexion::createInstance().getDatabase();
    if (!db.isOpen()) {
        QMessageBox::critical(this, "Démarrer étape", "Connexion Oracle indisponible.");
        return;
    }

    QSqlQuery qLoad(db);
    qLoad.prepare(
        "SELECT "
        "E.IDETAPE, E.IDFABRICATION, E.NOMETAPE, E.TEMPSESTIME, E.TEMPSREEL, E.DATEDEBUT, E.DATEFIN, "
        "P.NOM, P.PRENOM "
        "FROM ETAPE E "
        "LEFT JOIN PERSONNEL P ON E.CIN = P.CIN "
        "WHERE E.IDETAPE = :idEtape");
    qLoad.bindValue(":idEtape", idEtape);

    if (!qLoad.exec() || !qLoad.next()) {
        QMessageBox::warning(this, "Démarrer étape", "Étape introuvable.");
        return;
    }

    const int idFab = qLoad.value(1).toInt();
    const QString nomEtape = formaterTexte(qLoad.value(2));
    const int tempsEstime = qRound(qLoad.value(3).toDouble());
    const int tempsReel = qRound(qLoad.value(4).toDouble());
    const QVariant dateDebut = qLoad.value(5);
    const QVariant dateFin = qLoad.value(6);
    const QString nomPers = formaterTexte(qLoad.value(7));
    const QString prenomPers = formaterTexte(qLoad.value(8));

    QString personnel = (nomPers == "-" && prenomPers == "-") ? "-" : (nomPers + " " + prenomPers).trimmed();
    if (dateFin.isValid() && !dateFin.isNull()) {
        QMessageBox::warning(this, "Démarrer étape", "Impossible de démarrer cette étape : elle est déjà terminée.");
        return;
    }
    if (dateDebut.isValid() && !dateDebut.isNull()) {
        QMessageBox::warning(this, "Démarrer étape", "Impossible de démarrer cette étape : elle a déjà démarré.");
        return;
    }

    if (existeEtapeActiveDansFabrication(idFab, idEtape)) {
        QMessageBox::warning(this, "Démarrer étape", "Impossible de démarrer cette étape : une autre étape est déjà en cours pour cette fabrication.");
        return;
    }

    QSqlQuery qPrev(db);
    qPrev.prepare(
        "SELECT COUNT(*) "
        "FROM ETAPE "
        "WHERE IDFABRICATION = :idFabrication "
        "  AND ORDRE < (SELECT ORDRE FROM ETAPE WHERE IDETAPE = :idEtape) "
        "  AND DATEFIN IS NULL");
    qPrev.bindValue(":idFabrication", idFab);
    qPrev.bindValue(":idEtape", idEtape);

    if (!qPrev.exec() || !qPrev.next() || qPrev.value(0).toInt() > 0) {
        QMessageBox::warning(this, "Démarrer étape", "Impossible de démarrer cette étape : une étape précédente n'est pas encore terminée.");
        return;
    }

    if (!peutDemarrerEtape(idEtape)) {
        QMessageBox::warning(this, "Démarrer étape", "Impossible de démarrer cette étape.");
        return;
    }

    QSqlQuery qUpdate(db);
    qUpdate.prepare(
        "UPDATE ETAPE "
        "SET DATEDEBUT = SYSDATE, TEMPSREEL = 0, DATEFIN = NULL "
        "WHERE IDETAPE = :idEtape "
        "AND DATEDEBUT IS NULL "
        "AND DATEFIN IS NULL");
    qUpdate.bindValue(":idEtape", idEtape);

    if (!qUpdate.exec()) {
        QMessageBox::critical(this,
                              "Démarrer étape",
                              QString("Erreur Oracle:\n%1").arg(qUpdate.lastError().text()));
        return;
    }

    if (qUpdate.numRowsAffected() <= 0) {
        QMessageBox::warning(this,
                             "Démarrer étape",
                             "Impossible de démarrer cette étape : son état a changé.");
        return;
    }

    QSqlQuery qDate(db);
    qDate.prepare("SELECT DATEDEBUT FROM ETAPE WHERE IDETAPE = :idEtape");
    qDate.bindValue(":idEtape", idEtape);
    QString dateDemarrage = "-";
    if (qDate.exec() && qDate.next())
        dateDemarrage = formaterDate(qDate.value(0));

    QMessageBox::information(this,
                             "Démarrer étape",
                             QString("Étape '%1' démarrée le %2 par %3.")
                                 .arg(nomEtape)
                                 .arg(dateDemarrage)
                                 .arg(personnel));

    refreshSuiviAfterChanges(idFab);
    chargerEtapes(idFab);
}

void MainWindow::terminerEtapeDepuisDialog(int idEtape)
{
    // =====================================================================
    // MODE DE TEST DEBUG SIGSEGV
    // 1 = TEST 1 (MessageBox uniquement, sans SQL, sans refresh)
    // 2 = TEST 2 (SQL UPDATE, sans refresh)
    // 3 = TEST 3 (SQL UPDATE + refresh sécurisé)
    // =====================================================================
    constexpr int kTerminerEtapeTestMode = 3;

    qDebug() << "debut function";
    qDebug() << "idEtape =" << idEtape;
    qDebug() << "[terminerEtapeDepuisDialog] mode test =" << kTerminerEtapeTestMode;

    if (kTerminerEtapeTestMode == 1) {
        qDebug() << "[TEST 1] clic Terminer étape -> MessageBox uniquement";
        QMessageBox::information(this,
                                 "TEST 1",
                                 QString("Bouton Terminer étape cliqué (idEtape=%1).\nAucun SQL, aucun refresh.").arg(idEtape));
        return;
    }

    // 1) Validation stricte de l'identifiant d'étape
    if (idEtape <= 0) {
        qDebug() << "[terminerEtapeDepuisDialog] idEtape invalide -> arrêt.";
        QMessageBox::warning(this, "Terminer étape", "ID étape invalide.");
        return;
    }

    // 2) Validation de la connexion DB
    qDebug() << "[terminerEtapeDepuisDialog] Récupération connexion Oracle...";
    QSqlDatabase db = Connexion::createInstance().getDatabase();
    if (!db.isValid()) {
        qDebug() << "[terminerEtapeDepuisDialog] QSqlDatabase invalide.";
        QMessageBox::critical(this, "Terminer étape", "Connexion Oracle invalide.");
        return;
    }
    if (!db.isOpen()) {
        qDebug() << "[terminerEtapeDepuisDialog] Connexion Oracle fermée.";
        QMessageBox::critical(this, "Terminer étape", "Connexion Oracle indisponible.");
        return;
    }
    qDebug() << "[terminerEtapeDepuisDialog] Connexion Oracle OK.";

    // 3) Chargement sécurisé des données ETAPE
    qDebug() << "avant select";
    qDebug() << "[terminerEtapeDepuisDialog] Chargement données ETAPE...";
    QSqlQuery qLoad(db);
    const QString sqlLoad =
        "SELECT "
        "E.IDETAPE, E.IDFABRICATION, E.NOMETAPE, E.TEMPSESTIME, E.TEMPSREEL, E.DATEDEBUT, E.DATEFIN, "
        "P.NOM, P.PRENOM "
        "FROM ETAPE E "
        "LEFT JOIN PERSONNEL P ON E.CIN = P.CIN "
        "WHERE E.IDETAPE = :idEtape";

    if (!qLoad.prepare(sqlLoad)) {
        qDebug() << "[terminerEtapeDepuisDialog] Échec prepare qLoad:" << qLoad.lastError().text();
        QMessageBox::critical(this,
                              "Terminer étape",
                              QString("Erreur SQL (prepare):\n%1").arg(qLoad.lastError().text()));
        return;
    }

    qLoad.bindValue(":idEtape", idEtape);
    if (!qLoad.exec()) {
        qDebug() << "[terminerEtapeDepuisDialog] Échec exec qLoad:" << qLoad.lastError().text();
        QMessageBox::critical(this,
                              "Terminer étape",
                              QString("Erreur SQL (chargement):\n%1").arg(qLoad.lastError().text()));
        return;
    }
    if (!qLoad.next()) {
        qDebug() << "[terminerEtapeDepuisDialog] qLoad.next() == false pour idEtape=" << idEtape;
        QMessageBox::warning(this, "Terminer étape", "Étape introuvable.");
        return;
    }

    const int idFab = qLoad.value(1).toInt();
    const QString nomEtape = formaterTexte(qLoad.value(2));
    const int tempsEstime = qRound(qLoad.value(3).toDouble());
    const int tempsReelCourant = qRound(qLoad.value(4).toDouble());
    const QVariant dateDebut = qLoad.value(5);
    const QVariant dateFin = qLoad.value(6);
    const QString nomPers = formaterTexte(qLoad.value(7));
    const QString prenomPers = formaterTexte(qLoad.value(8));

    qDebug() << "[terminerEtapeDepuisDialog] Données chargées -"
             << "idFab=" << idFab
             << "nomEtape=" << nomEtape
             << "tempsEstime=" << tempsEstime
             << "tempsReelCourant=" << tempsReelCourant
             << "dateDebutValid=" << dateDebut.isValid()
             << "dateFinValid=" << dateFin.isValid();

    if (idFab <= 0) {
        qDebug() << "[terminerEtapeDepuisDialog] idFab invalide -> arrêt.";
        QMessageBox::warning(this, "Terminer étape", "Fabrication associée invalide.");
        return;
    }

    const QString personnel = (nomPers == "-" && prenomPers == "-")
        ? QStringLiteral("-")
        : (nomPers + " " + prenomPers).trimmed();

    // 4) Validation de l'état métier avant terminaison
    if (!dateDebut.isValid() || dateDebut.isNull()) {
        qDebug() << "[terminerEtapeDepuisDialog] DATEDEBUT NULL -> étape non démarrée.";
        QMessageBox::warning(this,
                             "Terminer étape",
                             "Impossible de terminer cette étape : elle n'a pas encore démarré.");
        return;
    }

    if (dateFin.isValid() && !dateFin.isNull()) {
        qDebug() << "[terminerEtapeDepuisDialog] DATEFIN déjà valorisée -> déjà terminée.";
        QMessageBox::information(this, "Terminer étape", "Cette étape est déjà terminée.");
        return;
    }

    const QString etat = calculerEtatEtape(dateDebut, dateFin, tempsEstime, tempsReelCourant);
    qDebug() << "[terminerEtapeDepuisDialog] État calculé=" << etat;
    if (etat != "EN_COURS") {
        qDebug() << "[terminerEtapeDepuisDialog] État non EN_COURS -> arrêt.";
        QMessageBox::warning(this, "Terminer étape", "Impossible de terminer cette étape.");
        return;
    }

    // 5) Saisie sécurisée du temps réel
    qDebug() << "[terminerEtapeDepuisDialog] Demande saisie TEMPSREEL...";
    bool ok = false;
    const int tempsReel = QInputDialog::getInt(this,
                                               "Terminer étape",
                                               "Saisir TEMPSREEL :",
                                               qMax(0, tempsReelCourant),
                                               0,
                                               100000,
                                               1,
                                               &ok);
    if (!ok) {
        qDebug() << "[terminerEtapeDepuisDialog] Saisie annulée par l'utilisateur.";
        return;
    }
    qDebug() << "[terminerEtapeDepuisDialog] TEMPSREEL saisi=" << tempsReel;

    // 6) Mise à jour SQL défensive
    qDebug() << "avant update";
    qDebug() << "[terminerEtapeDepuisDialog] Mise à jour ETAPE...";
    QSqlQuery qUpdate(db);
    const QString sqlUpdate =
        "UPDATE ETAPE "
        "SET DATEFIN = SYSDATE, "
        "    TEMPSREEL = :tempsReel "
        "WHERE IDETAPE = :idEtape "
        "  AND DATEDEBUT IS NOT NULL "
        "  AND DATEFIN IS NULL";

    if (!qUpdate.prepare(sqlUpdate)) {
        qDebug() << "[terminerEtapeDepuisDialog] Échec prepare qUpdate:" << qUpdate.lastError().text();
        QMessageBox::critical(this,
                              "Terminer étape",
                              QString("Erreur SQL (prepare update):\n%1").arg(qUpdate.lastError().text()));
        return;
    }

    qUpdate.bindValue(":tempsReel", tempsReel);
    qUpdate.bindValue(":idEtape", idEtape);

    if (!qUpdate.exec()) {
        qDebug() << "[terminerEtapeDepuisDialog] Échec exec qUpdate:" << qUpdate.lastError().text();
        QMessageBox::critical(this,
                              "Terminer étape",
                              QString("Erreur Oracle:\n%1").arg(qUpdate.lastError().text()));
        return;
    }

    const qint64 rows = qUpdate.numRowsAffected();
    qDebug() << "[terminerEtapeDepuisDialog] qUpdate rowsAffected=" << rows;
    if (rows <= 0) {
        QMessageBox::warning(this,
                             "Terminer étape",
                             "Impossible de terminer cette étape : elle n'est plus en cours.");
        return;
    }
    qDebug() << "update ok";

    // 7) Relecture date de fin (optionnelle, sécurisée)
    qDebug() << "[terminerEtapeDepuisDialog] Relecture DATEFIN...";
    QString dateFinText = "-";
    QSqlQuery qDate(db);
    if (!qDate.prepare("SELECT DATEFIN FROM ETAPE WHERE IDETAPE = :idEtape")) {
        qDebug() << "[terminerEtapeDepuisDialog] Échec prepare qDate:" << qDate.lastError().text();
    } else {
        qDate.bindValue(":idEtape", idEtape);
        if (!qDate.exec()) {
            qDebug() << "[terminerEtapeDepuisDialog] Échec exec qDate:" << qDate.lastError().text();
        } else if (!qDate.next()) {
            qDebug() << "[terminerEtapeDepuisDialog] qDate.next() == false (DATEFIN non relue).";
        } else {
            dateFinText = formaterDate(qDate.value(0));
            qDebug() << "[terminerEtapeDepuisDialog] DATEFIN relue=" << dateFinText;
        }
    }

    // 8) Notification utilisateur
    const int retard = qMax(0, tempsReel - tempsEstime);
    const QString retardText = (retard > 0)
        ? QString(" avec retard de %1").arg(formaterRetardMinutes(retard))
        : QString();

    QMessageBox::information(this,
                             "Terminer étape",
                             QString("Étape '%1' terminée%2 le %3 par %4.")
                                 .arg(nomEtape)
                                 .arg(retardText)
                                 .arg(dateFinText)
                                 .arg(personnel));

    if (kTerminerEtapeTestMode == 2) {
        qDebug() << "[TEST 2] SQL terminé, refresh désactivé pour isolation crash.";
        return;
    }

    // 9) Rafraîchissement sécurisé pour éviter tout accès invalide UI
    qDebug() << "avant refresh";
    qDebug() << "[terminerEtapeDepuisDialog] Début rafraîchissement UI - idFab=" << idFab;
    if (!ui) {
        qDebug() << "[terminerEtapeDepuisDialog] ui == nullptr -> refresh ignoré (sécurité).";
        return;
    }

    // TEST 3 : réactivation progressive et sécurisée du refresh.
    refreshSuiviAfterChanges(idFab);

    // Vérification supplémentaire après refresh (destruction potentielle en cours de fermeture)
    if (!ui) {
        qDebug() << "[terminerEtapeDepuisDialog] ui == nullptr après refresh -> fin sécurisée.";
        return;
    }

    // Les appels ci-dessous restent volontairement commentés pour isoler le SIGSEGV.
    // Décommentez un par un pendant l'investigation.
    chargerEtapes(idFab);
    // buildTimeline(getFabricationSuivi(idFab));
    // afficherDetailsEtape(idEtape);

    qDebug() << "[terminerEtapeDepuisDialog] Fin OK.";
}

void MainWindow::buildTimeline(const FabricationSuivi &fab)
{
    clearTimeline();

    QHBoxLayout *layout = ui->hl_timeline_steps;

    FabricationSuivi timelineFab = fab;
    int completed = 0;
    int currentIdx = -1;
    int nextIdx = -1;
    double progression = 0.0;
    computeTimelineStates(timelineFab, &completed, &currentIdx, &nextIdx, &progression);

    // En-tête de la timeline
    QFrame *headerBlock = new QFrame();
    headerBlock->setFixedWidth(150);
    headerBlock->setMinimumHeight(130);
    headerBlock->setStyleSheet(
        "QFrame { background: #2C3E50; border-radius: 10px; border: 2px solid #34495E; }");

    QVBoxLayout *hVL = new QVBoxLayout(headerBlock);
    hVL->setContentsMargins(10, 8, 10, 8);
    hVL->setSpacing(2);

    auto hLabel = [](const QString &text, int fs, bool bold, const QString &col) -> QLabel* {
        QLabel *l = new QLabel(text);
        l->setStyleSheet(QString("color:%1;font-size:%2px;background:transparent;%3")
                             .arg(col).arg(fs).arg(bold ? "font-weight:bold;" : ""));
        l->setWordWrap(true);
        return l;
    };

    hVL->addWidget(hLabel("🏭 FABRICATION", 9, true, "#3498DB"));
    hVL->addWidget(hLabel(QString("FAB-%1").arg(fab.id), 14, true, "#ECF0F1"));
    if (!timelineFab.modele.isEmpty())
        hVL->addWidget(hLabel("Modele: " + timelineFab.modele, 8, false, "#BDC3C7"));
    hVL->addWidget(hLabel(QString("Qte: %1").arg(timelineFab.quantite), 8, false, "#BDC3C7"));
    hVL->addWidget(hLabel("Qualite: " + timelineFab.qualite, 8, false, "#BDC3C7"));
    if (timelineFab.dateDebut.isValid())
        hVL->addWidget(hLabel("Debut: " + timelineFab.dateDebut.toString("dd/MM/yyyy"), 8, false, "#BDC3C7"));

    const QString currentStep = (currentIdx >= 0) ? timelineFab.etapes[currentIdx].nom : "Aucune";
    const QString nextStep = (nextIdx >= 0) ? timelineFab.etapes[nextIdx].nom : "Aucune";
    hVL->addWidget(hLabel(QString("Progression: %1%")
                          .arg(QString::number(progression, 'f', 1)), 8, true, "#F1C40F"));
    hVL->addWidget(hLabel(QString("Terminees: %1/%2").arg(completed).arg(timelineFab.etapes.size()), 8, false, "#BDC3C7"));
    hVL->addWidget(hLabel("Etape actuelle: " + currentStep, 8, true,
                          (currentIdx >= 0 && timelineFab.etapes[currentIdx].statut == "RETARD") ? "#FF6B6B" : "#F39C12"));
    hVL->addWidget(hLabel("Prochaine: " + nextStep, 8, false, "#3498DB"));

    // Complexité de la fabrication dans le header
    const QString complexite = calculerComplexiteFabrication(timelineFab.id);
    hVL->addWidget(hLabel(QString("Complexité: %1 %2")
                              .arg(getPredictionIcon(complexite))
                              .arg(complexite),
                          9,
                          true,
                          couleurComplexiteBadge(complexite)));

    layout->addWidget(headerBlock);

    // Flèche de départ
    QLabel *startArrow = new QLabel("▶");
    startArrow->setStyleSheet(
        "color:#3498DB; font-size:18px; background:transparent; padding:0 4px;");
    startArrow->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    layout->addWidget(startArrow);

    if (timelineFab.etapes.isEmpty()) {
        QLabel *lbl = new QLabel(
            QString("Aucune étape trouvée pour FAB-%1.\n"
                    "Ajoutez des étapes dans la table ETAPE.").arg(timelineFab.id));
        lbl->setStyleSheet(
            "color:#E67E22; font-size:12px; font-weight:bold; background:transparent;");
        lbl->setAlignment(Qt::AlignCenter);
        lbl->setWordWrap(true);
        layout->addWidget(lbl);
        layout->addStretch();
        ui->lbl_timeline_vide->setVisible(false);
        return;
    }

    ui->lbl_timeline_vide->setVisible(false);

    // Étapes avec flèches
    for (int i = 0; i < timelineFab.etapes.size(); i++) {
        if (i > 0) {
            // Connecteur entre étapes
            QVBoxLayout *connVL = new QVBoxLayout();
            connVL->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

            QLabel *line = new QLabel();
            line->setFixedSize(30, 2);
            line->setStyleSheet("background: #34495E;");

            QLabel *arrow = new QLabel("▶");
            arrow->setStyleSheet(
                "color:#7F8C8D; font-size:16px; background:transparent; padding:0 2px;");
            arrow->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);

            layout->addWidget(line);
            layout->addWidget(arrow);
        }
        layout->addWidget(createStepBlock(timelineFab.etapes[i]));
    }

    // Indicateur de fin
    QLabel *endLabel = new QLabel("🏁");
    endLabel->setStyleSheet("font-size:22px; background:transparent; padding:0 6px;");
    endLabel->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    layout->addWidget(endLabel);

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

void MainWindow::on_table_fabrications_etapes_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    if (row < 0 || row >= ui->table_fabrications_etapes->rowCount()) return;

    QTableWidgetItem *item = ui->table_fabrications_etapes->item(row, 0);
    if (!item) return;

    int fabId = item->text().toInt();
    if (fabId <= 0) return;

    int idx = m_allFabIds.indexOf(fabId);
    if (idx >= 0) m_currentFabIndex = idx;

    m_selectedFab = getFabricationSuivi(fabId);
    m_fabSelected = true;

    buildTimeline(m_selectedFab);
    chargerEtapes(fabId);

    if (ui->te_ai_result) {
        ui->te_ai_result->setVisible(false);
        ui->te_ai_result->clear();
    }
}

// ═════════════════════════════════════════════════════════════
//  PARTIE 5 — ALERTES DYNAMIQUES
// ═════════════════════════════════════════════════════════════

/*
 * Requête alertes (Oracle 11g) :
 * SELECT E.IDFABRICATION, E.NOMETAPE, E.TEMPSREEL, E.TEMPSESTIME,
 *        (E.TEMPSREEL - E.TEMPSESTIME) as ECART
 * FROM ETAPE E
 * WHERE E.TEMPSREEL > E.TEMPSESTIME
 *   AND E.TEMPSESTIME > 0
 * ORDER BY (E.TEMPSREEL - E.TEMPSESTIME) DESC
 */
void MainWindow::verifierAlertes()
{
    QStringList alertes;

    QSqlQuery q;
    q.prepare(
        "SELECT E.IDFABRICATION, E.NOMETAPE, E.TEMPSREEL, E.TEMPSESTIME, "
        "       (E.TEMPSREEL - E.TEMPSESTIME) as ECART "
        "FROM ETAPE E "
        "WHERE E.TEMPSREEL > E.TEMPSESTIME AND E.TEMPSESTIME > 0 "
        "ORDER BY (E.TEMPSREEL - E.TEMPSESTIME) DESC");

    if (q.exec()) {
        while (q.next()) {
            int    fabId    = q.value(0).toInt();
            QString nomE    = q.value(1).toString();
            double  reel    = q.value(2).toDouble();
            double  estime  = q.value(3).toDouble();
            double  ecart   = q.value(4).toDouble();
            double  ratio   = reel / estime;

            if (ratio > 1.3) {
                alertes << QString("🔴 FAB-%1 | %2 : +%3h de retard (%4%)")
                               .arg(fabId).arg(nomE)
                               .arg(ecart, 0, 'f', 1)
                               .arg(ratio * 100, 0, 'f', 0);
            } else if (ratio > 1.1) {
                alertes << QString("🟠 FAB-%1 | %2 : léger dépassement +%3h")
                               .arg(fabId).arg(nomE)
                               .arg(ecart, 0, 'f', 1);
            }
        }
    }

    if (!alertes.isEmpty()) {
        QString msg = "⚠️ Alertes détectées :\n\n" + alertes.join("\n");
        showAnimatedMessageBox(QMessageBox::Warning, "Alertes Production", msg);
    }
}

void MainWindow::calculateAndDisplayFabricationPrediction(int fabId)
{
    QSqlQuery q;
    q.prepare(
        "SELECT SUM(TEMPSREEL) as total_reel, SUM(TEMPSESTIME) as total_estime "
        "FROM ETAPE WHERE IDFABRICATION = :id");
    q.bindValue(":id", fabId);

    double totalReel   = 0.0;
    double totalEstime = 0.0;

    if (q.exec() && q.next()) {
        totalReel   = q.value(0).toDouble();
        totalEstime = q.value(1).toDouble();
    }

    QString complexite   = calculerComplexiteFabrication(fabId);
    QString statutGlobal = calculerStatutFabrication(fabId);
    double  progression  = calculerProgression(fabId);
    double  delay        = totalReel - totalEstime;

    QString message;
    message += getPredictionIcon(complexite) + " Complexité : " + complexite + "\n\n";
    message += QString("Statut global       : %1\n").arg(statutGlobal);
    message += QString("Progression         : %1%\n").arg(progression, 0, 'f', 1);
    message += QString("Temps estimé total  : %1 heures\n").arg(totalEstime, 0, 'f', 1);
    message += QString("Temps réel total    : %1 heures\n").arg(totalReel,   0, 'f', 1);

    if (delay > 0)
        message += QString("\n⚠️  Retard de %1 heures").arg(delay, 0, 'f', 1);
    else if (delay < 0)
        message += QString("\n✅ Avance de %1 heures").arg(-delay, 0, 'f', 1);
    else
        message += "\n✅ Dans les délais";

    if (complexite == QStringLiteral("Critique")) {
        showAnimatedMessageBox(QMessageBox::Warning, "⚠️ Alerte Retard", message);
    } else {
        showAnimatedMessageBox(QMessageBox::Information,
                               QString("Statut FAB-%1").arg(fabId), message);
    }
}

// ═════════════════════════════════════════════════════════════
//  PARTIE 6 — CATALOGUE ÉTAPES
// ═════════════════════════════════════════════════════════════
void MainWindow::loadEtapesCatalogue()
{
    ui->table_etapes_catalogue->clearContents();
    ui->table_etapes_catalogue->setRowCount(0);
    ui->table_etapes_catalogue->setColumnCount(3);

    QStringList headers = {"ID Etape", "Nom Etape", "Temps Estimé"};
    ui->table_etapes_catalogue->setHorizontalHeaderLabels(headers);

    QSqlQuery query;
    query.prepare("SELECT IDETAPE, NOMETAPE, TEMPSESTIME FROM ETAPE ORDER BY IDETAPE");

    if (!query.exec()) {
        qDebug() << "Catalogue etapes error:" << query.lastError().text();
        return;
    }

    int row = 0;
    while (query.next()) {
        ui->table_etapes_catalogue->insertRow(row);

        auto *idItem    = new QTableWidgetItem(query.value(0).toString());
        auto *nomItem   = new QTableWidgetItem(query.value(1).toString());
        auto *tempsItem = new QTableWidgetItem(query.value(2).toString());

        idItem->setTextAlignment(Qt::AlignCenter);
        tempsItem->setTextAlignment(Qt::AlignCenter);

        idItem->setFlags(idItem->flags()       & ~Qt::ItemIsEditable);
        nomItem->setFlags(nomItem->flags()     & ~Qt::ItemIsEditable);
        tempsItem->setFlags(tempsItem->flags() & ~Qt::ItemIsEditable);

        ui->table_etapes_catalogue->setItem(row, 0, idItem);
        ui->table_etapes_catalogue->setItem(row, 1, nomItem);
        ui->table_etapes_catalogue->setItem(row, 2, tempsItem);
        row++;
    }

    ui->table_etapes_catalogue->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->table_etapes_catalogue->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->table_etapes_catalogue->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);

    qDebug() << "Catalogue:" << row << " etapes chargees";
}

// ═════════════════════════════════════════════════════════════
//  PARTIE 7 — CHATBOT LOCAL (10 questions prédéfinies + logique)
// ═════════════════════════════════════════════════════════════

/*
 * Chatbot 100% local : requêtes SQL SELECT + logique C++
 * Aucun service externe. Aucune modification de base.
 */

// ── Fonction centrale du chatbot ─────────────────────────────
QString MainWindow::repondreQuestion(const QString &question, int fabId)
{
    QString q = question.toLower().trimmed();

    // Q1 : Statut global de la production
    if (q.contains("statut") && q.contains("production")) {
        return getChatStatusGlobal();
    }
    // Q2 : Fabrications en retard
    if (q.contains("retard")) {
        return getChatRetard();
    }
    // Q3 : Ratio de performance / étape la plus lente
    if (q.contains("ratio") || q.contains("lent")) {
        return fabId > 0 ? getSlowestStepMessage(fabId) : getChatRatioGlobal();
    }
    // Q4 : Étapes terminées aujourd'hui
    if (q.contains("terminée") && q.contains("aujourd")) {
        return fabId > 0 ? getTermineesTodayMessage(fabId) : "Sélectionnez une fabrication d'abord.";
    }
    // Q5 : Étapes en cours
    if (q.contains("en cours")) {
        return fabId > 0 ? getCurrentStepMessage(fabId) : getChatEtapesEnCours();
    }
    // Q6 : Temps prévu
    if (q.contains("temps") && (q.contains("prévu") || q.contains("estimé"))) {
        return fabId > 0 ? getTotalEstimatedMessage(fabId) : "Sélectionnez une fabrication d'abord.";
    }
    // Q7 : Temps réel
    if (q.contains("temps") && q.contains("réel")) {
        return fabId > 0 ? getTotalRealMessage(fabId) : "Sélectionnez une fabrication d'abord.";
    }
    // Q8 : Fabrications avec ratio < 1
    if (q.contains("ratio") && q.contains("1")) {
        return fabId > 0 ? getRatioInfMessage(fabId) : getChatRatioInfGlobal();
    }
    // Q9 : Étapes planifiées demain
    if (q.contains("demain")) {
        return fabId > 0 ? getDemainMessage(fabId) : getChatDemainGlobal();
    }
    // Q10 : Statut d'une fabrication
    if (q.contains("statut") && (q.contains("fabrication") || fabId > 0)) {
        return fabId > 0 ? getFabricationStatusMessage(fabId) : getChatStatusGlobal();
    }
    // Q bonus : qui travaille sur la fabrication
    if (q.contains("qui") && (q.contains("travaille") || q.contains("personnel"))) {
        return fabId > 0 ? getChatPersonnel(fabId) : "Sélectionnez une fabrication d'abord.";
    }
    // Q bonus : fabrication la plus risquée
    if (q.contains("risq")) {
        return getChatPlusRisquee();
    }
    // Q bonus : plus grand écart
    if (q.contains("écart") || q.contains("ecart")) {
        return getChatPlusGrandEcart();
    }
    // Q bonus : progression la plus faible
    if (q.contains("progression") && q.contains("faible")) {
        return getChatProgressionFaible();
    }
    // Q bonus : combien terminées
    if (q.contains("combien") && q.contains("termin")) {
        return getChatNbTerminees();
    }

    return "❓ Je n'ai pas compris votre question.\n"
           "Essayez : 'Quelles fabrications sont en retard ?'\n"
           "ou utilisez le menu des questions prédéfinies.";
}

// ── Implémentations des réponses ─────────────────────────────

QString MainWindow::getChatStatusGlobal()
{
    int total = 0, encours = 0, termine = 0, retard = 0, nonComm = 0;

    QSqlQuery q;
    if (q.exec("SELECT IDFABRICATION FROM FABRICATION")) {
        while (q.next()) {
            int fid = q.value(0).toInt();
            QString s = calculerStatutFabrication(fid);
            total++;
            if (s == "TERMINE")      termine++;
            else if (s == "RETARD")  retard++;
            else if (s == "EN_COURS") encours++;
            else                      nonComm++;
        }
    }

    QString res;
    res += QString("🏭 STATUT GLOBAL DE LA PRODUCTION\n");
    res += QString("━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    res += QString("Total fabrications   : %1\n").arg(total);
    res += QString("✅ Terminées          : %1\n").arg(termine);
    res += QString("⚙️  En cours           : %1\n").arg(encours);
    res += QString("🔴 En retard          : %1\n").arg(retard);
    res += QString("⬜ Non commencées     : %1\n").arg(nonComm);

    if (total > 0) {
        double pct = (double)(termine + encours) / total * 100.0;
        res += QString("📊 Avancement global  : %1%").arg(pct, 0, 'f', 1);
    }
    return res;
}

QString MainWindow::getChatRetard()
{
    QString response;
    bool found = false;

    QSqlQuery q;
    if (q.exec("SELECT IDFABRICATION FROM FABRICATION ORDER BY IDFABRICATION")) {
        while (q.next()) {
            int fid = q.value(0).toInt();
            if (calculerStatutFabrication(fid) == "RETARD") {
                if (!found) {
                    response = "⚠️ Fabrications en RETARD :\n━━━━━━━━━━━━━━━━\n";
                    found = true;
                }
                double prog = calculerProgression(fid);
                response += QString("  🔴 FAB-%1 | Progression: %2%\n")
                                .arg(fid).arg(prog, 0, 'f', 1);
            }
        }
    }
    if (!found) response = "✅ Aucune fabrication en retard détectée !";
    return response;
}

QString MainWindow::getChatRatioGlobal()
{
    QSqlQuery q;
    q.prepare(
        "SELECT E.IDFABRICATION, E.NOMETAPE, E.TEMPSREEL, E.TEMPSESTIME "
        "FROM ETAPE E "
        "WHERE E.TEMPSESTIME > 0 AND E.TEMPSREEL > 0 "
        "ORDER BY (E.TEMPSREEL / E.TEMPSESTIME) DESC");

    if (q.exec() && q.next()) {
        int    fid    = q.value(0).toInt();
        QString nom   = q.value(1).toString();
        double  reel  = q.value(2).toDouble();
        double  estime= q.value(3).toDouble();
        double  ratio = reel / estime;
        return QString("🐢 Étape la plus lente (toutes fabrications) :\n"
                       "  FAB-%1 | %2\n"
                       "  Temps réel: %3h | Estimé: %4h\n"
                       "  Ratio: %5%")
            .arg(fid).arg(nom)
            .arg(reel,   0,'f',1)
            .arg(estime, 0,'f',1)
            .arg(ratio*100, 0,'f',0);
    }
    return "❌ Aucune données de temps disponibles.";
}

QString MainWindow::getChatEtapesEnCours()
{
    QString response;
    bool found = false;

    QSqlQuery q;
    q.prepare(
        "SELECT E.IDFABRICATION, E.NOMETAPE, E.DATEDEBUT "
        "FROM ETAPE E "
        "WHERE E.DATEDEBUT IS NOT NULL AND E.DATEFIN IS NULL "
        "ORDER BY E.IDFABRICATION, E.ORDRE");

    if (q.exec()) {
        while (q.next()) {
            if (!found) {
                response = "⚙️ Étapes actuellement en cours :\n━━━━━━━━━━━━━━━━\n";
                found = true;
            }
            response += QString("  FAB-%1 | %2 | Depuis: %3\n")
                            .arg(q.value(0).toInt())
                            .arg(q.value(1).toString())
                            .arg(q.value(2).toDate().toString("dd/MM/yyyy"));
        }
    }
    if (!found) response = "✅ Aucune étape en cours actuellement.";
    return response;
}

QString MainWindow::getChatDemainGlobal()
{
    QSqlQuery q;
    q.prepare(
        "SELECT COUNT(*), COUNT(DISTINCT IDFABRICATION) FROM ETAPE "
        "WHERE TRUNC(DATEDEBUT) = TRUNC(SYSDATE + 1)");

    if (q.exec() && q.next()) {
        int nbEtapes = q.value(0).toInt();
        int nbFabs   = q.value(1).toInt();
        if (nbEtapes > 0)
            return QString("📅 Demain : %1 étape(s) planifiée(s) dans %2 fabrication(s)")
                .arg(nbEtapes).arg(nbFabs);
        return "📅 Aucune étape planifiée pour demain.";
    }
    return "❌ Erreur lors de la requête.";
}

QString MainWindow::getChatRatioInfGlobal()
{
    int count = 0;
    QString response;

    QSqlQuery q;
    q.prepare(
        "SELECT E.IDFABRICATION, E.NOMETAPE, E.TEMPSREEL, E.TEMPSESTIME "
        "FROM ETAPE E "
        "WHERE E.TEMPSESTIME > 0 AND E.TEMPSREEL > 0 AND E.TEMPSREEL < E.TEMPSESTIME "
        "ORDER BY (E.TEMPSREEL / E.TEMPSESTIME) ASC");

    if (q.exec()) {
        while (q.next()) {
            if (count == 0)
                response = "📈 Étapes plus rapides que prévu (ratio < 1) :\n━━━━━━━━━━━━━━━━\n";
            double ratio = q.value(2).toDouble() / q.value(3).toDouble();
            response += QString("  ✅ FAB-%1 | %2 | %3% du temps estimé\n")
                            .arg(q.value(0).toInt())
                            .arg(q.value(1).toString())
                            .arg(ratio * 100, 0, 'f', 0);
            count++;
        }
    }
    if (count == 0)
        response = "📊 Toutes les étapes respectent ou dépassent le temps estimé.";
    return response;
}

QString MainWindow::getChatPersonnel(int fabId)
{
    QString response;
    bool found = false;

    QSqlQuery q;
    q.prepare(
        "SELECT E.NOMETAPE, E.CIN, P.NOM "
        "FROM ETAPE E "
        "LEFT JOIN PERSONNEL P ON E.CIN = P.CIN "
        "WHERE E.IDFABRICATION = :id AND E.CIN IS NOT NULL "
        "ORDER BY E.ORDRE");
    q.bindValue(":id", fabId);

    if (q.exec()) {
        while (q.next()) {
            if (!found) {
                response = QString("👥 Personnel — FAB-%1 :\n━━━━━━━━━━━━━━━━\n").arg(fabId);
                found = true;
            }
            QString nom = q.value(2).toString();
            response += QString("  %1 : %2 (CIN: %3)\n")
                            .arg(q.value(0).toString())
                            .arg(nom.isEmpty() ? "Inconnu" : nom)
                            .arg(q.value(1).toString());
        }
    }
    if (!found)
        response = QString("❌ Aucun personnel assigné à FAB-%1.").arg(fabId);
    return response;
}

QString MainWindow::getChatPlusRisquee()
{
    // Fabrication avec le plus grand ratio TEMPSREEL/TEMPSESTIME
    QSqlQuery q;
    q.prepare(
        "SELECT IDFABRICATION, SUM(NVL(TEMPSREEL,0)), SUM(NVL(TEMPSESTIME,0)) "
        "FROM ETAPE "
        "WHERE TEMPSESTIME > 0 "
        "GROUP BY IDFABRICATION "
        "ORDER BY (SUM(NVL(TEMPSREEL,0)) / SUM(NVL(TEMPSESTIME,0))) DESC");

    if (q.exec() && q.next()) {
        int    fid    = q.value(0).toInt();
        double reel   = q.value(1).toDouble();
        double estime = q.value(2).toDouble();
        double ratio  = estime > 0 ? reel / estime : 0;
        return QString("⚠️ Fabrication la plus risquée : FAB-%1\n"
                       "  Temps réel: %2h | Estimé: %3h\n"
                       "  Ratio: %4%")
            .arg(fid)
            .arg(reel,   0,'f',1)
            .arg(estime, 0,'f',1)
            .arg(ratio * 100, 0,'f',0);
    }
    return "❌ Données insuffisantes.";
}

QString MainWindow::getChatPlusGrandEcart()
{
    QSqlQuery q;
    q.prepare(
        "SELECT IDFABRICATION, "
        "  SUM(NVL(TEMPSREEL,0)) - SUM(NVL(TEMPSESTIME,0)) as ECART "
        "FROM ETAPE "
        "GROUP BY IDFABRICATION "
        "ORDER BY ABS(SUM(NVL(TEMPSREEL,0)) - SUM(NVL(TEMPSESTIME,0))) DESC");

    if (q.exec() && q.next()) {
        int    fid   = q.value(0).toInt();
        double ecart = q.value(1).toDouble();
        return QString("📊 Fabrication avec le plus grand écart : FAB-%1\n"
                       "  Écart : %2%3 heures")
            .arg(fid)
            .arg(ecart >= 0 ? "+" : "")
            .arg(ecart, 0, 'f', 1);
    }
    return "❌ Données insuffisantes.";
}

QString MainWindow::getChatProgressionFaible()
{
    double minProg = 101.0;
    int    minFab  = -1;

    QSqlQuery q;
    if (q.exec("SELECT IDFABRICATION FROM FABRICATION")) {
        while (q.next()) {
            int fid = q.value(0).toInt();
            double prog = calculerProgression(fid);
            if (prog < minProg) {
                minProg = prog;
                minFab  = fid;
            }
        }
    }
    if (minFab > 0)
        return QString("📉 Fabrication avec la progression la plus faible : FAB-%1\n"
                       "  Progression : %2%")
            .arg(minFab).arg(minProg, 0, 'f', 1);
    return "❌ Aucune fabrication trouvée.";
}

QString MainWindow::getChatNbTerminees()
{
    int count = 0;
    QSqlQuery q;
    if (q.exec("SELECT IDFABRICATION FROM FABRICATION")) {
        while (q.next()) {
            if (calculerStatutFabrication(q.value(0).toInt()) == "TERMINE")
                count++;
        }
    }
    return QString("✅ Nombre de fabrications terminées : %1").arg(count);
}

// ── Méthodes héritées utilisées par le chatbot ───────────────

void MainWindow::appendChatMessage(const QString &message)
{
    if (ui->te_ai_result) {
        ui->te_ai_result->setVisible(true);
        QString timestamp = QDateTime::currentDateTime().toString("HH:mm:ss");
        ui->te_ai_result->append(QString("[%1] %2\n").arg(timestamp, message));
    }
}

QString MainWindow::getFabricationStatusMessage(int fabId)
{
    QString statut = calculerStatutFabrication(fabId);
    double  prog   = calculerProgression(fabId);
    QString complexite = calculerComplexiteFabrication(fabId);

    return QString("📋 FAB-%1\n"
                   "Statut     : %2\n"
                   "Progression: %3%\n"
                   "Complexité : %4 %5")
        .arg(fabId).arg(statut)
        .arg(prog, 0, 'f', 1)
        .arg(getPredictionIcon(complexite)).arg(complexite);
}

QString MainWindow::getDelayMessage(int fabId)
{
    QSqlQuery q;
    q.prepare("SELECT NOMETAPE, TEMPSREEL, TEMPSESTIME FROM ETAPE WHERE IDFABRICATION = :id");
    q.bindValue(":id", fabId);

    QString response;
    bool hasDelay = false;

    if (q.exec()) {
        while (q.next()) {
            double reel   = q.value(1).toDouble();
            double estime = q.value(2).toDouble();
            if (estime > 0 && reel / estime > 1.2) {
                if (!hasDelay) { response = "⚠️ Retards détectés :\n"; hasDelay = true; }
                response += QString("  - %1: +%2h (%3%)\n")
                                .arg(q.value(0).toString())
                                .arg(reel - estime, 0,'f',1)
                                .arg(reel/estime*100, 0,'f',0);
            }
        }
    }
    return hasDelay ? response : "✅ Aucun retard détecté !";
}

QString MainWindow::getSlowestStepMessage(int fabId)
{
    QSqlQuery q;
    q.prepare(
        "SELECT NOMETAPE, TEMPSREEL FROM ETAPE "
        "WHERE IDFABRICATION = :id AND TEMPSREEL IS NOT NULL "
        "ORDER BY TEMPSREEL DESC");
    q.bindValue(":id", fabId);

    if (q.exec() && q.next())
        return QString("🐢 Étape la plus lente : %1 (%2 heures)")
            .arg(q.value(0).toString()).arg(q.value(1).toDouble(), 0,'f',1);
    return "❌ Aucune étape trouvée.";
}

QString MainWindow::getTotalEstimatedMessage(int fabId)
{
    QSqlQuery q;
    q.prepare("SELECT SUM(NVL(TEMPSESTIME,0)) FROM ETAPE WHERE IDFABRICATION = :id");
    q.bindValue(":id", fabId);

    if (q.exec() && q.next())
        return QString("📊 Durée totale estimée : %1 heures")
            .arg(q.value(0).toDouble(), 0,'f',1);
    return "❌ Impossible de calculer.";
}

QString MainWindow::getTotalRealMessage(int fabId)
{
    QSqlQuery q;
    q.prepare("SELECT SUM(NVL(TEMPSREEL,0)) FROM ETAPE WHERE IDFABRICATION = :id");
    q.bindValue(":id", fabId);

    if (q.exec() && q.next())
        return QString("⏱️ Durée réelle totale : %1 heures")
            .arg(q.value(0).toDouble(), 0,'f',1);
    return "❌ Impossible de calculer.";
}

QString MainWindow::getCurrentStepMessage(int fabId)
{
    QSqlQuery q;
    q.prepare(
        "SELECT NOMETAPE, DATEDEBUT, DATEFIN FROM ETAPE "
        "WHERE IDFABRICATION = :id "
        "  AND DATEDEBUT IS NOT NULL AND DATEFIN IS NULL "
        "ORDER BY ORDRE ASC");
    q.bindValue(":id", fabId);

    if (q.exec() && q.next()) {
        return QString("⚙️ Étape en cours : %1\n  Début: %2")
            .arg(q.value(0).toString())
            .arg(q.value(1).toDate().toString("dd/MM/yyyy"));
    }
    return "✅ Aucune étape en cours.";
}

QString MainWindow::getTermineesTodayMessage(int fabId)
{
    QSqlQuery q;
    q.prepare(
        "SELECT COUNT(*) FROM ETAPE "
        "WHERE IDFABRICATION = :id AND TRUNC(DATEFIN) = TRUNC(SYSDATE)");
    q.bindValue(":id", fabId);

    if (q.exec() && q.next()) {
        int cnt = q.value(0).toInt();
        return cnt > 0
                   ? QString("✅ %1 étape(s) terminée(s) aujourd'hui").arg(cnt)
                   : "📅 Aucune étape terminée aujourd'hui.";
    }
    return "❌ Erreur lors de la requête.";
}

QString MainWindow::getRatioInfMessage(int fabId)
{
    int count = 0;
    QString response;

    QSqlQuery q;
    q.prepare(
        "SELECT NOMETAPE, TEMPSREEL, TEMPSESTIME FROM ETAPE "
        "WHERE IDFABRICATION = :id AND TEMPSESTIME > 0 AND TEMPSREEL < TEMPSESTIME");
    q.bindValue(":id", fabId);

    if (q.exec()) {
        while (q.next()) {
            if (count == 0) response = "📈 Étapes plus rapides que prévu :\n";
            double ratio = q.value(1).toDouble() / q.value(2).toDouble();
            response += QString("  ✅ %1 (%2% du temps estimé)\n")
                            .arg(q.value(0).toString()).arg(ratio*100, 0,'f',0);
            count++;
        }
    }
    return count == 0 ? "📊 Toutes les étapes dépassent ou égalent le temps estimé." : response;
}

QString MainWindow::getDemainMessage(int fabId)
{
    QSqlQuery q;
    q.prepare(
        "SELECT COUNT(*) FROM ETAPE "
        "WHERE IDFABRICATION = :id AND TRUNC(DATEDEBUT) = TRUNC(SYSDATE + 1)");
    q.bindValue(":id", fabId);

    if (q.exec() && q.next()) {
        int cnt = q.value(0).toInt();
        return cnt > 0
                   ? QString("📅 %1 étape(s) planifiée(s) pour demain").arg(cnt)
                   : "📅 Aucune étape planifiée pour demain.";
    }
    return "❌ Erreur lors de la requête.";
}

// ── Menu chatbot ─────────────────────────────────────────────
void MainWindow::on_btn_chatbot_clicked()
{
    QDialog *chatDialog = new QDialog(this);
    chatDialog->setWindowTitle("💬 Chatbot Production");
    chatDialog->setModal(true);
    chatDialog->resize(480, 560);
    chatDialog->setStyleSheet(
        "QDialog { background: #1E2A35; border-radius: 10px; }"
        "QLabel { color: #ECF0F1; background: transparent; }"
        "QPushButton { background: #2C3E50; color: #ECF0F1; border: 1px solid #34495E;"
        "              border-radius: 6px; padding: 8px 12px; text-align: left;"
        "              font-size: 10px; margin: 2px 0; }"
        "QPushButton:hover { background: #2980B9; border-color: #2980B9; }"
        "QPushButton#btnClose { background: #7F8C8D; text-align: center; }");

    QVBoxLayout *layout = new QVBoxLayout(chatDialog);
    layout->setContentsMargins(16, 16, 16, 16);
    layout->setSpacing(4);

    QLabel *title = new QLabel("🤖  Chatbot Production — Questions Prédéfinies");
    title->setStyleSheet(
        "font-weight: bold; font-size: 13px; color: #3498DB; margin-bottom: 8px;");
    layout->addWidget(title);

    if (m_fabSelected && m_selectedFab.id > 0) {
        QLabel *context = new QLabel(
            QString("📌 Contexte actuel : FAB-%1").arg(m_selectedFab.id));
        context->setStyleSheet("font-size: 10px; color: #27AE60;");
        layout->addWidget(context);
    }

    QFrame *sep = new QFrame();
    sep->setFrameShape(QFrame::HLine);
    sep->setStyleSheet("color: #2C3E50; margin: 4px 0;");
    layout->addWidget(sep);

    struct QuestionDef {
        QString label;
        std::function<void()> action;
    };

    QList<QuestionDef> qdefs = {
                                { "🏭 Quel est le statut global de la production ?",
                                 [this]() { appendChatMessage(getChatStatusGlobal()); } },
                                { "🔴 Quelles fabrications sont en retard ?",
                                 [this]() { appendChatMessage(getChatRetard()); } },
                                { "🐢 Quel est le ratio de performance moyen ?",
                                 [this]() { appendChatMessage(m_fabSelected ? getSlowestStepMessage(m_selectedFab.id) : getChatRatioGlobal()); } },
                                { "✅ Combien d'étapes sont terminées aujourd'hui ?",
                                 [this]() {
                                     if (m_fabSelected) appendChatMessage(getTermineesTodayMessage(m_selectedFab.id));
                                     else appendChatMessage("⚠️ Sélectionnez d'abord une fabrication."); } },
                                { "⚙️  Quelles étapes sont en cours ?",
                                 [this]() { appendChatMessage(m_fabSelected ? getCurrentStepMessage(m_selectedFab.id) : getChatEtapesEnCours()); } },
                                { "📊 Quel est le temps total prévu pour cette fabrication ?",
                                 [this]() {
                                     if (m_fabSelected) appendChatMessage(getTotalEstimatedMessage(m_selectedFab.id));
                                     else appendChatMessage("⚠️ Sélectionnez d'abord une fabrication."); } },
                                { "⏱️  Quel est le temps réel total pour cette fabrication ?",
                                 [this]() {
                                     if (m_fabSelected) appendChatMessage(getTotalRealMessage(m_selectedFab.id));
                                     else appendChatMessage("⚠️ Sélectionnez d'abord une fabrication."); } },
                                { "📈 Quelles fabrications ont un ratio de performance < 1 ?",
                                 [this]() { appendChatMessage(m_fabSelected ? getRatioInfMessage(m_selectedFab.id) : getChatRatioInfGlobal()); } },
                                { "📅 Combien d'étapes sont planifiées pour demain ?",
                                 [this]() { appendChatMessage(m_fabSelected ? getDemainMessage(m_selectedFab.id) : getChatDemainGlobal()); } },
                                { "📋 Quel est le statut de cette fabrication ?",
                                 [this]() {
                                     if (m_fabSelected) appendChatMessage(getFabricationStatusMessage(m_selectedFab.id));
                                     else appendChatMessage("⚠️ Sélectionnez d'abord une fabrication."); } },
                                { "👥 Qui travaille sur cette fabrication ?",
                                 [this]() {
                                     if (m_fabSelected) appendChatMessage(getChatPersonnel(m_selectedFab.id));
                                     else appendChatMessage("⚠️ Sélectionnez d'abord une fabrication."); } },
                                { "⚠️  Quelle fabrication est la plus risquée ?",
                                 [this]() { appendChatMessage(getChatPlusRisquee()); } },
                                { "📉 Quelle fabrication a la progression la plus faible ?",
                                 [this]() { appendChatMessage(getChatProgressionFaible()); } },
                                { "📊 Quelle fabrication a le plus grand écart ?",
                                 [this]() { appendChatMessage(getChatPlusGrandEcart()); } },
                                { "🔢 Combien de fabrications sont terminées ?",
                                 [this]() { appendChatMessage(getChatNbTerminees()); } },
                                };

    for (const QuestionDef &qd : qdefs) {
        QPushButton *btn = new QPushButton(qd.label);
        connect(btn, &QPushButton::clicked, chatDialog, [qd, chatDialog]() {
            qd.action();
            chatDialog->accept();
        });
        layout->addWidget(btn);
    }

    layout->addSpacing(8);
    QPushButton *closeBtn = new QPushButton("✖  Fermer");
    closeBtn->setObjectName("btnClose");
    connect(closeBtn, &QPushButton::clicked, chatDialog, &QDialog::accept);
    layout->addWidget(closeBtn);

    chatDialog->exec();
    delete chatDialog;
}

void MainWindow::on_chat_status_clicked()      { appendChatMessage(getChatStatusGlobal()); }
void MainWindow::on_chat_retard_clicked()      { appendChatMessage(getChatRetard()); }
void MainWindow::on_chat_ratio_clicked()       { if(m_fabSelected) appendChatMessage(getSlowestStepMessage(m_selectedFab.id)); }
void MainWindow::on_chat_terminees_clicked()   { if(m_fabSelected) appendChatMessage(getTermineesTodayMessage(m_selectedFab.id)); }
void MainWindow::on_chat_encours_clicked()     { if(m_fabSelected) appendChatMessage(getCurrentStepMessage(m_selectedFab.id)); }
void MainWindow::on_chat_temps_prevu_clicked() { if(m_fabSelected) appendChatMessage(getTotalEstimatedMessage(m_selectedFab.id)); }
void MainWindow::on_chat_temps_reel_clicked()  { if(m_fabSelected) appendChatMessage(getTotalRealMessage(m_selectedFab.id)); }
void MainWindow::on_chat_ratio_inf_clicked()   { if(m_fabSelected) appendChatMessage(getRatioInfMessage(m_selectedFab.id)); }
void MainWindow::on_chat_demain_clicked()      { if(m_fabSelected) appendChatMessage(getDemainMessage(m_selectedFab.id)); }
void MainWindow::on_chat_statut_fab_clicked()  { if(m_fabSelected) appendChatMessage(getFabricationStatusMessage(m_selectedFab.id)); }

// ═════════════════════════════════════════════════════════════
//  STREAMING SSE (Anthropic API)
// ═════════════════════════════════════════════════════════════
void MainWindow::onAIReplyReadyRead()
{
    if (!m_currentReply) return;

    QByteArray raw  = m_currentReply->readAll();
    QString    data(raw);

    for (const QString &line : data.split('\n')) {
        if (!line.startsWith("data: ")) continue;
        QString payload = line.mid(6).trimmed();
        if (payload == "[DONE]") continue;

        QJsonDocument doc = QJsonDocument::fromJson(payload.toUtf8());
        if (doc.isNull()) continue;

        QJsonObject obj = doc.object();
        if (obj["type"].toString() == "content_block_delta") {
            QJsonObject delta = obj["delta"].toObject();
            if (delta["type"].toString() == "text_delta") {
                m_aiAccumulated += delta["text"].toString();
                ui->te_ai_result->setPlainText(m_aiAccumulated);
                QTextCursor cursor = ui->te_ai_result->textCursor();
                cursor.movePosition(QTextCursor::End);
                ui->te_ai_result->setTextCursor(cursor);
            }
        }
    }
}

void MainWindow::onAIReplyFinished()
{
    if (!m_currentReply) return;

    int httpCode = m_currentReply->attribute(
                                     QNetworkRequest::HttpStatusCodeAttribute).toInt();

    if (httpCode == 200 || m_currentReply->error() == QNetworkReply::NoError) {
        ui->te_ai_result->setStyleSheet(
            "background-color:#F0FEF4; border:2px solid #27AE60;"
            "border-radius:8px; padding:6px 10px; color:#1A3A1A; font-size:10pt;");
    } else {
        ui->te_ai_result->setStyleSheet(
            "background-color:#FEF0F0; border:2px solid #C0392B;"
            "border-radius:8px; padding:6px 10px; color:#3A1A1A; font-size:10pt;");
    }

    m_currentReply->deleteLater();
    m_currentReply = nullptr;
}

void MainWindow::onAIReplyError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error);
    QString errMsg = m_currentReply ? m_currentReply->errorString() : "Erreur inconnue.";

    if (errMsg.contains("authentication", Qt::CaseInsensitive) ||
        errMsg.contains("API key", Qt::CaseInsensitive)) {
        errMsg = "Erreur d'authentification API Anthropic.\n\n"
                 "Remplacez 'sk-ant-VOTRE_CLE_ICI' par votre vraie clé API.\n\n"
                 "Obtenez une clé sur : https://console.anthropic.com/\n\n"
                 "Détail: " + errMsg;
    }

    ui->te_ai_result->setPlainText("Erreur: " + errMsg);
    ui->te_ai_result->setStyleSheet(
        "background-color:#FEF0F0; border:2px solid #C0392B;"
        "border-radius:8px; padding:6px 10px; color:#3A1A1A; font-size:10pt;");
    ui->te_ai_result->setVisible(true);
}

// ═════════════════════════════════════════════════════════════
//  NAVIGATION
// ═════════════════════════════════════════════════════════════
void MainWindow::on_btn_personnel_clicked()   { ui->stackedWidget->setCurrentIndex(4); }
void MainWindow::on_btn_modele_clicked()      { ui->stackedWidget->setCurrentIndex(3); }
void MainWindow::on_btn_bois_clicked()        { ui->stackedWidget->setCurrentIndex(2); }
void MainWindow::on_btn_etape_clicked()       { ui->stackedWidget->setCurrentIndex(0); }
void MainWindow::on_btn_fabrication_clicked() { ui->stackedWidget->setCurrentIndex(1); }

// ═════════════════════════════════════════════════════════════
//  FABRICATION — CRUD
// ═════════════════════════════════════════════════════════════
int MainWindow::generateId()
{
    QSqlQuery query;
    if (!query.exec("SELECT MAX(IDFABRICATION) FROM FABRICATION")) {
        qDebug() << "generateId error:" << query.lastError().text();
        return 1;
    }
    if (query.next()) {
        int maxId = query.value(0).toInt();
        if (maxId > 0) return maxId + 1;
    }
    return 1;
}

void MainWindow::generateFabricationId()
{
    ui->aff_id_fab->setText(QString::number(generateId()));
    ui->aff_id_fab->setReadOnly(true);
}

void MainWindow::clearFabricationFields()
{
    generateFabricationId();
    ui->aff_id_modele->clear();
    ui->aff_date_deb->setDate(QDate::currentDate());
    ui->aff_quantite->setValue(1);
    ui->aff_qualite->setCurrentIndex(0);
    ui->aff_commentaire->clear();
}

bool MainWindow::validateFabricationFields()
{
    const QString idModeleTexte = ui->aff_id_modele->text().trimmed();

    if (idModeleTexte.isEmpty()) {
        showAnimatedMessageBox(QMessageBox::Warning, "Champ vide",
                               "L'ID du Modele est obligatoire.");
        ui->aff_id_modele->setFocus();
        return false;
    }

    bool ok = false;
    int idModele = idModeleTexte.toInt(&ok);
    if (!ok) {
        showAnimatedMessageBox(QMessageBox::Warning, "Erreur de format",
                               "L'ID du modele doit etre un nombre (1, 2, 3, etc.)");
        ui->aff_id_modele->setFocus();
        return false;
    }

    // Vérification robuste: supporte IDMODELE numérique ou texte dans Oracle.
    // On compare la valeur saisie ET sa forme normalisée (ex: 01 -> 1).
    const QString idModeleNormalise = QString::number(idModele);

    QSqlQuery checkQuery;
    checkQuery.prepare(
        "SELECT COUNT(*) "
        "FROM MODELE "
        "WHERE TRIM(TO_CHAR(IDMODELE)) = :idSaisi "
        "   OR TRIM(TO_CHAR(IDMODELE)) = :idNormalise");
    checkQuery.bindValue(":idSaisi", idModeleTexte);
    checkQuery.bindValue(":idNormalise", idModeleNormalise);

    if (checkQuery.exec() && checkQuery.next()) {
        if (checkQuery.value(0).toInt() == 0) {
            showAnimatedMessageBox(QMessageBox::Warning, "Erreur",
                                   QString("L'ID Modele '%1' n'existe pas.").arg(idModeleTexte));
            ui->aff_id_modele->setFocus();
            return false;
        }
    } else {
        showAnimatedMessageBox(QMessageBox::Warning, "Erreur",
                               "Impossible de verifier l'existence du modele.");
        return false;
    }

    if (ui->aff_date_deb->date() < QDate::currentDate()) {
        showAnimatedMessageBox(QMessageBox::Warning, "Date invalide",
                               "La date de debut ne peut pas etre anterieure a aujourd'hui.");
        ui->aff_date_deb->setFocus();
        return false;
    }

    if (ui->aff_commentaire->toPlainText().trimmed().isEmpty()) {
        showAnimatedMessageBox(QMessageBox::Warning, "Champ vide",
                               "Le champ Commentaire est obligatoire.");
        ui->aff_commentaire->setFocus();
        return false;
    }

    return true;
}

void MainWindow::loadFabrications()
{
    refreshFabricationsTable(m_currentFabricationSearchText);
}

void MainWindow::filterFabrications(const QString &searchText)
{
    m_currentFabricationSearchText = searchText;
    refreshFabricationsTable(m_currentFabricationSearchText);
}

void MainWindow::exportToPDF()
{
    if (ui->table_modeles_2->rowCount() == 0) {
        showAnimatedMessageBox(QMessageBox::Warning, "Aucune donnee", "Rien a exporter.");
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, "Exporter PDF", "", "*.pdf");
    if (fileName.isEmpty()) return;

    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    QPainter painter;
    if (!painter.begin(&printer)) {
        showAnimatedMessageBox(QMessageBox::Warning, "Erreur",
                               "Impossible de creer le PDF.");
        return;
    }

    int yPos = 40;
    painter.setFont(QFont("Arial", 16, QFont::Bold));
    painter.drawText(100, yPos, "Rapport des Fabrications - WoodPilot");
    yPos += 40;

    int     xPos[]    = {20, 120, 250, 400, 500, 650};
    QString headers[] = {"ID", "Modele", "Date Debut", "Quantite", "Qualite", "Commentaire"};

    painter.setFont(QFont("Arial", 10, QFont::Bold));
    for (int i = 0; i < 6; i++) painter.drawText(xPos[i], yPos, headers[i]);
    yPos += 20;

    painter.setFont(QFont("Arial", 9));
    for (int r = 0; r < ui->table_modeles_2->rowCount(); r++) {
        if (yPos > printer.height() - 50) {
            printer.newPage(); yPos = 40;
            painter.setFont(QFont("Arial", 10, QFont::Bold));
            for (int i = 0; i < 6; i++) painter.drawText(xPos[i], yPos, headers[i]);
            yPos += 20;
            painter.setFont(QFont("Arial", 9));
        }
        for (int c = 0; c < 6; c++) {
            auto *it = ui->table_modeles_2->item(r, c);
            if (it) {
                QString t = it->text();
                if (c == 5 && t.length() > 30) t = t.left(27) + "...";
                painter.drawText(xPos[c], yPos, t);
            }
        }
        yPos += 20;
    }

    painter.end();
    showAnimatedMessageBox(QMessageBox::Information, "Succes", "PDF exporte avec succes !");
}

void MainWindow::on_btn_ajouter_2_clicked()
{
    if (!validateFabricationFields()) return;

    QSqlQuery query;
    query.prepare(
        "INSERT INTO FABRICATION (IDFABRICATION, IDMODELE, DATE_DEBUT, "
        "QUANTITE_A_PRODUIRE, QUALITE, COMMENTAIRE) "
        "VALUES (:id, :idm, :dat, :qty, :qal, :com)");
    query.bindValue(":id",  ui->aff_id_fab->text().toInt());
    query.bindValue(":idm", ui->aff_id_modele->text().trimmed().toInt());
    query.bindValue(":dat", ui->aff_date_deb->date());
    query.bindValue(":qty", ui->aff_quantite->value());
    query.bindValue(":qal", ui->aff_qualite->currentText());
    query.bindValue(":com", ui->aff_commentaire->toPlainText().trimmed());

    if (query.exec()) {
        showAnimatedMessageBox(QMessageBox::Information, "Succes",
                               "Fabrication ajoutee avec succes !");
        loadFabrications();
        loadFabricationsSuivi();
        reloadAllFabIds();
        buildKPICards();
        clearFabricationFields();
    } else {
        showAnimatedMessageBox(QMessageBox::Critical, "Erreur",
                               "Impossible d'ajouter:\n" + query.lastError().text());
    }
}

void MainWindow::on_btn_modifier_clicked()
{
    if (ui->aff_id_fab->text().isEmpty()) {
        showAnimatedMessageBox(QMessageBox::Warning, "Erreur",
                               "Veuillez selectionner une fabrication a modifier.");
        return;
    }
    if (!validateFabricationFields()) return;

    QSqlQuery query;
    query.prepare(
        "UPDATE FABRICATION SET IDMODELE=:idm, DATE_DEBUT=:dat, "
        "QUANTITE_A_PRODUIRE=:qty, QUALITE=:qal, COMMENTAIRE=:com "
        "WHERE IDFABRICATION=:id");
    query.bindValue(":id",  ui->aff_id_fab->text().toInt());
    query.bindValue(":idm", ui->aff_id_modele->text().trimmed().toInt());
    query.bindValue(":dat", ui->aff_date_deb->date());
    query.bindValue(":qty", ui->aff_quantite->value());
    query.bindValue(":qal", ui->aff_qualite->currentText());
    query.bindValue(":com", ui->aff_commentaire->toPlainText().trimmed());

    if (query.exec()) {
        showAnimatedMessageBox(QMessageBox::Information, "Succes",
                               "Fabrication modifiee avec succes !");
        loadFabrications();
        loadFabricationsSuivi();
        reloadAllFabIds();
        buildKPICards();
        clearFabricationFields();
    } else {
        showAnimatedMessageBox(QMessageBox::Critical, "Erreur",
                               "Impossible de modifier:\n" + query.lastError().text());
    }
}

void MainWindow::on_btn_supprimer_clicked()
{
    int fabId = ui->aff_id_fab->text().trimmed().toInt();

    // Fallback: récupérer l'ID depuis la ligne sélectionnée si le champ est vide.
    if (fabId <= 0 && ui->table_modeles_2) {
        const int row = ui->table_modeles_2->currentRow();
        if (row >= 0) {
            if (QTableWidgetItem *idItem = ui->table_modeles_2->item(row, 0))
                fabId = idItem->text().toInt();
        }
    }

    if (fabId <= 0) {
        showAnimatedMessageBox(QMessageBox::Warning, "Erreur",
                               "Veuillez selectionner une fabrication a supprimer.");
        return;
    }

    if (!showAnimatedQuestionBox("Confirmation",
                                 "Etes-vous sur de vouloir supprimer cette fabrication ?"))
        return;

    QSqlDatabase db = Connexion::createInstance().getDatabase();
    if (!db.isOpen()) {
        showAnimatedMessageBox(QMessageBox::Critical, "Erreur",
                               "Connexion Oracle indisponible.");
        return;
    }

    if (!db.transaction()) {
        showAnimatedMessageBox(QMessageBox::Critical, "Erreur",
                               "Impossible de demarrer la transaction de suppression.");
        return;
    }

    QSqlQuery deleteEtapes(db);
    deleteEtapes.prepare("DELETE FROM ETAPE WHERE IDFABRICATION = :id");
    deleteEtapes.bindValue(":id", fabId);

    if (!deleteEtapes.exec()) {
        db.rollback();
        showAnimatedMessageBox(QMessageBox::Critical, "Erreur",
                               "Impossible de supprimer les etapes liees:\n" + deleteEtapes.lastError().text());
        return;
    }

    const int nbEtapesSupprimees = deleteEtapes.numRowsAffected();

    QSqlQuery deleteFab(db);
    deleteFab.prepare("DELETE FROM FABRICATION WHERE IDFABRICATION = :id");
    deleteFab.bindValue(":id", fabId);

    if (!deleteFab.exec()) {
        db.rollback();
        showAnimatedMessageBox(QMessageBox::Critical, "Erreur",
                               "Impossible de supprimer la fabrication:\n" + deleteFab.lastError().text());
        return;
    }

    if (deleteFab.numRowsAffected() <= 0) {
        db.rollback();
        showAnimatedMessageBox(QMessageBox::Warning, "Suppression",
                               "Aucune fabrication correspondante n'a ete trouvee.");
        return;
    }

    if (!db.commit()) {
        db.rollback();
        showAnimatedMessageBox(QMessageBox::Critical, "Erreur",
                               "Echec de validation de la suppression.");
        return;
    }

    {
        showAnimatedMessageBox(QMessageBox::Information, "Succes",
                               QString("Fabrication supprimee avec succes !\nEtapes supprimees: %1")
                                   .arg(qMax(0, nbEtapesSupprimees)));
        loadFabrications();
        loadFabricationsSuivi();
        reloadAllFabIds();
        buildKPICards();
        clearFabricationFields();

        m_selectedFab = FabricationSuivi();
        m_fabSelected = false;
        clearTimeline();
        if (ui->table_etapes_catalogue) {
            ui->table_etapes_catalogue->clearContents();
            ui->table_etapes_catalogue->setRowCount(0);
        }
    }
}

void MainWindow::on_rech_7_textChanged(const QString &text)   { filterFabrications(text); }
void MainWindow::on_btn_rechercher_6_clicked()                { filterFabrications(ui->rech_7->text()); }
void MainWindow::on_btn_tire_2_clicked()                      { trierFabrications(ui->cb_critere_recherche_modele_2->currentText()); }
void MainWindow::on_btn_export_pdf_modele_2_clicked()         { exportToPDF(); }

void MainWindow::on_table_modeles_2_cellClicked(int row, int column)
{
    Q_UNUSED(column);
    if (row < 0 || row >= ui->table_modeles_2->rowCount()) return;

    auto get = [&](int col) -> QTableWidgetItem* {
        return ui->table_modeles_2->item(row, col);
    };

    if (get(0)) ui->aff_id_fab->setText(get(0)->text());
    if (get(1)) {
        QVariant idModele = get(1)->data(Qt::UserRole);
        ui->aff_id_modele->setText(idModele.isValid() ? idModele.toString() : get(1)->text());
    }
    if (get(2)) {
        QDate d = QDate::fromString(get(2)->text(), "dd/MM/yyyy");
        if (d.isValid()) ui->aff_date_deb->setDate(d);
    }
    if (get(3)) ui->aff_quantite->setValue(get(3)->text().toInt());
    if (get(4)) {
        int idx = ui->aff_qualite->findText(get(4)->text());
        if (idx >= 0) ui->aff_qualite->setCurrentIndex(idx);
    }
    if (get(5)) ui->aff_commentaire->setText(get(5)->text());
}

void MainWindow::applyAIOptimisedTimeline(const QString &aiText)
{
    Q_UNUSED(aiText);
}

// ═════════════════════════════════════════════════════════════
//  MESSAGE BOXES ANIMÉES
// ═════════════════════════════════════════════════════════════
void MainWindow::showAnimatedMessageBox(QMessageBox::Icon icon,
                                        const QString &title,
                                        const QString &text,
                                        const QString &informativeText)
{
    QMessageBox msgBox;
    msgBox.setIcon(icon);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    if (!informativeText.isEmpty()) msgBox.setInformativeText(informativeText);

    msgBox.setStyleSheet(alertDialogStyleSheet());

    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
    msgBox.setGraphicsEffect(effect);

    QPropertyAnimation *anim = new QPropertyAnimation(effect, "opacity");
    anim->setDuration(300);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);

    QTimer::singleShot(0, [&msgBox, anim]() {
        anim->start();
        QPropertyAnimation *popAnim = new QPropertyAnimation(&msgBox, "geometry");
        popAnim->setDuration(200);
        popAnim->setEasingCurve(QEasingCurve::OutElastic);
        QRect orig  = msgBox.geometry();
        QRect start = QRect(orig.x()+50, orig.y()+30, orig.width()-100, orig.height()-60);
        popAnim->setStartValue(start);
        popAnim->setEndValue(orig);
        popAnim->start();
    });

    msgBox.exec();
    delete anim;
}

bool MainWindow::showAnimatedQuestionBox(const QString &title, const QString &text)
{
    QMessageBox msgBox;
    msgBox.setIcon(QMessageBox::Question);
    msgBox.setWindowTitle(title);
    msgBox.setText(text);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    msgBox.setDefaultButton(QMessageBox::No);

    msgBox.setStyleSheet(alertDialogStyleSheet());

    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect();
    msgBox.setGraphicsEffect(effect);

    QPropertyAnimation *anim = new QPropertyAnimation(effect, "opacity");
    anim->setDuration(300);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->setEasingCurve(QEasingCurve::OutCubic);
    anim->start();

    auto reply = static_cast<QMessageBox::StandardButton>(msgBox.exec());
    delete anim;
    return reply == QMessageBox::Yes;
}
