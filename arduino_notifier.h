#ifndef ARDUINO_NOTIFIER_H
#define ARDUINO_NOTIFIER_H

#include <QString>

// 🔌 Initialisation Arduino
void initArduino();

// 📤 Envoi message (string وحدة تتقسم لسطرين)
void envoyerArduino(const QString &message);

// 📺 Envoi مباشر لسطرين (LCD 16x2)
void envoyerArduinoLignes(const QString &ligne1, const QString &ligne2);

#endif // ARDUINO_NOTIFIER_H
