#include "arduino.h"
#include <QDebug>

// ════════════════════════════════════════════════════════════
// CLASSE ARDUINO RFID
// ════════════════════════════════════════════════════════════
Arduino::Arduino(QObject *parent) : QObject(parent)
{
    serial = new QSerialPort(this);
    connect(serial, &QSerialPort::readyRead, this, &Arduino::lireDonnees);
}

Arduino::~Arduino()
{
    deconnecter();
}

bool Arduino::connecter(const QString &portFixe)
{
    qDebug() << "=== Connexion Arduino RFID ===";

    QString portTrouve = portFixe;

    // Si aucun port fixe, recherche automatique
    if (portTrouve.isEmpty()) {
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
            if (info.description().contains("Arduino", Qt::CaseInsensitive) ||
                info.manufacturer().contains("Arduino", Qt::CaseInsensitive) ||
                info.description().contains("CH340", Qt::CaseInsensitive) ||
                info.vendorIdentifier() == 0x2341 ||
                info.vendorIdentifier() == 0x1A86) {
                portTrouve = info.portName();
                break;
            }
        }
    }

    if (portTrouve.isEmpty()) {
        qDebug() << "ERREUR RFID: Aucun port disponible !";
        return false;
    }

    serial->setPortName(portTrouve);
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if (serial->open(QIODevice::ReadWrite)) {
        qDebug() << "Arduino RFID connecte sur" << portTrouve;
        return true;
    }
    qDebug() << "Echec ouverture" << portTrouve << ":" << serial->errorString();
    return false;
}

void Arduino::deconnecter()
{
    if (serial->isOpen()) serial->close();
}

bool Arduino::estConnecte() const
{
    return serial->isOpen();
}

void Arduino::envoyerCommande(const QString &cmd)
{
    if (serial->isOpen()) {
        serial->write((cmd + "\n").toUtf8());
        serial->flush();
    }
}

void Arduino::lireDonnees()
{
    buffer.append(serial->readAll());
    while (buffer.contains('\n')) {
        int idx = buffer.indexOf('\n');
        QString ligne = QString::fromUtf8(buffer.left(idx)).trimmed();
        buffer.remove(0, idx + 1);

        if (ligne.startsWith("UID:")) {
            QString uid = ligne.mid(4);
            emit uidRecu(uid);
        }
        else if (ligne == "DONE") {
            emit accesTermine();
        }
    }
}

// ════════════════════════════════════════════════════════════
// NOTIFIER LCD (port separe)
// ════════════════════════════════════════════════════════════
namespace {
QSerialPort *g_notifierPort = nullptr;
QString g_notifierPortFixe = "";

QString formatLine(const QString &line)
{
    return line.simplified().left(16);
}

bool ouvrirNotifier()
{
    if (g_notifierPort && g_notifierPort->isOpen())
        return true;

    if (!g_notifierPort)
        g_notifierPort = new QSerialPort();

    // Si port fixe defini, l'utiliser directement
    if (!g_notifierPortFixe.isEmpty()) {
        g_notifierPort->setPortName(g_notifierPortFixe);
        g_notifierPort->setBaudRate(QSerialPort::Baud9600);
        g_notifierPort->setDataBits(QSerialPort::Data8);
        g_notifierPort->setParity(QSerialPort::NoParity);
        g_notifierPort->setStopBits(QSerialPort::OneStop);
        g_notifierPort->setFlowControl(QSerialPort::NoFlowControl);

        if (g_notifierPort->open(QIODevice::ReadWrite)) {
            qDebug() << "Notifier LCD connecte sur" << g_notifierPortFixe;
            return true;
        }
        qDebug() << "Echec Notifier sur" << g_notifierPortFixe;
        return false;
    }

    // Sinon recherche auto
    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &info : ports) {
        g_notifierPort->setPort(info);
        g_notifierPort->setBaudRate(QSerialPort::Baud9600);
        g_notifierPort->setDataBits(QSerialPort::Data8);
        g_notifierPort->setParity(QSerialPort::NoParity);
        g_notifierPort->setStopBits(QSerialPort::OneStop);
        g_notifierPort->setFlowControl(QSerialPort::NoFlowControl);

        if (g_notifierPort->open(QIODevice::ReadWrite)) {
            qDebug() << "Notifier connecte sur" << info.portName();
            return true;
        }
    }
    return false;
}

void sendToNotifier(const QString &l1, const QString &l2)
{
    QString line1 = formatLine(l1);
    QString line2 = formatLine(l2);

    if (line1.isEmpty() && line2.isEmpty())
        return;

    if (!ouvrirNotifier()) {
        qDebug() << "Notifier non disponible";
        return;
    }

    QByteArray data = (line1 + "|" + line2 + "\n").toUtf8();
    g_notifierPort->write(data);
    g_notifierPort->flush();
}
}

void initArduinoNotifier(const QString &portFixe)
{
    g_notifierPortFixe = portFixe;
    ouvrirNotifier();
}

void envoyerArduino(const QString &message)
{
    if (message.trimmed().isEmpty()) return;
    QStringList parts = message.split('\n');
    envoyerArduinoLignes(parts.value(0), parts.value(1));
}

void envoyerArduinoLignes(const QString &ligne1, const QString &ligne2)
{
    sendToNotifier(ligne1, ligne2);
}
