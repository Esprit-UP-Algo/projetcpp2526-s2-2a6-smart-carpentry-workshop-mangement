#ifndef ARDUINO_H
#define ARDUINO_H

#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QString>

// ════════════════════════════════════════════════════════════
// Classe Arduino RFID (Scenario 1 - lecture badge + moteur)
// ════════════════════════════════════════════════════════════
class Arduino : public QObject
{
    Q_OBJECT
public:
    explicit Arduino(QObject *parent = nullptr);
    ~Arduino();

    // Connexion sur un port specifique (ex: "COM9")
    bool connecter(const QString &portFixe = QString());
    void deconnecter();
    bool estConnecte() const;
    void envoyerCommande(const QString &cmd);
    QSerialPort* getSerial() { return serial; }

signals:
    void uidRecu(QString uid);
    void accesTermine();

private slots:
    void lireDonnees();

private:
    QSerialPort *serial;
    QByteArray buffer;
};

// ════════════════════════════════════════════════════════════
// Notifier LCD (Scenario 2 - affichage modifications)
// ════════════════════════════════════════════════════════════
void initArduinoNotifier(const QString &portFixe = QString());
void envoyerArduino(const QString &message);
void envoyerArduinoLignes(const QString &ligne1, const QString &ligne2);

#endif // ARDUINO_H
