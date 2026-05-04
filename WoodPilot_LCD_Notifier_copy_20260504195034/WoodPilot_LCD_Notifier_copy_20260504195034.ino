#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// 🟢 Address غالبا 0x27 (كان ما يخدمش جرّب 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

String message = "";

void setup() {
  Serial.begin(9600);

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WoodPilot");
  lcd.setCursor(0, 1);
  lcd.print("En attente...");
}

void loop() {

  // 📩 كي يجي message من Qt
  if (Serial.available() > 0) {

    message = Serial.readStringUntil('\n');
    message.trim();

    // 💡 format: ligne1|ligne2
    int separatorIndex = message.indexOf('|');

    String line1 = "";
    String line2 = "";

    if (separatorIndex != -1) {
      line1 = message.substring(0, separatorIndex);
      line2 = message.substring(separatorIndex + 1);
    } else {
      line1 = message;
      line2 = "";
    }

    // ✂️ نقصهم لـ 16 caractères
    line1 = line1.substring(0, 16);
    line2 = line2.substring(0, 16);

    // 🖥️ affichage
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print(line1);

    lcd.setCursor(0, 1);
    lcd.print(line2);
  }
}