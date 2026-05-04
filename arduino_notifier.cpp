#include "arduino_notifier.h"

#include <QDebug>
#include <QSerialPort>
#include <QSerialPortInfo>

namespace {

QSerialPort *g_arduinoPort = nullptr;


QString formatLine(const QString &line)
{
    return line.simplified().left(16); // LCD 16 char
}


bool ouvrirArduino()
{
    if (g_arduinoPort && g_arduinoPort->isOpen())
        return true;

    if (!g_arduinoPort)
        g_arduinoPort = new QSerialPort();

    const QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();

    for (const QSerialPortInfo &info : ports) {
        g_arduinoPort->setPort(info);
        g_arduinoPort->setBaudRate(QSerialPort::Baud9600);
        g_arduinoPort->setDataBits(QSerialPort::Data8);
        g_arduinoPort->setParity(QSerialPort::NoParity);
        g_arduinoPort->setStopBits(QSerialPort::OneStop);
        g_arduinoPort->setFlowControl(QSerialPort::NoFlowControl);

        if (g_arduinoPort->open(QIODevice::ReadWrite)) {
            qDebug() << "✅ Arduino connected on" << info.portName();
            return true;
        }
    }

    qDebug() << "❌ Arduino not found.";
    return false;
}

// إرسال فعلي
void sendToArduino(const QString &l1, const QString &l2)
{
    QString line1 = formatLine(l1);
    QString line2 = formatLine(l2);

    if (line1.isEmpty() && line2.isEmpty())
        return;

    if (!ouvrirArduino()) {
        qDebug() << "⚠️ Arduino not available";
        return;
    }

    QByteArray data = (line1 + "|" + line2 + "\n").toUtf8();

    g_arduinoPort->write(data);
    g_arduinoPort->flush();
}

}

//═══════════════════════════════════════════════
// 🔌 INIT
//═══════════════════════════════════════════════
void initArduino()
{
    ouvrirArduino();
}

//═══════════════════════════════════════════════
// 📤 MESSAGE SIMPLE
//═══════════════════════════════════════════════
void envoyerArduino(const QString &message)
{
    if (message.trimmed().isEmpty())
        return;

    QStringList parts = message.split('\n');
    envoyerArduinoLignes(parts.value(0), parts.value(1));
}

//═══════════════════════════════════════════════
// 📺 ENVOI 2 LIGNES
//═══════════════════════════════════════════════
void envoyerArduinoLignes(const QString &ligne1, const QString &ligne2)
{
    sendToArduino(ligne1, ligne2);
}
