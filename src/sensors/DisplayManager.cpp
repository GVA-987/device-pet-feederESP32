#include "sensors/DisplayManager.h"
#include "sensors/TemperatureIntService.h"

DisplayManager display;

byte bar1[8] = {B00000, B00000, B00000, B00000, B00000, B00000, B11111, B11111}; // Baja
byte bar2[8] = {B00000, B00000, B00000, B11111, B11111, B11111, B11111, B11111}; // Media
byte bar3[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111}; // Alta

void DisplayManager::begin()
{

    Wire.begin(sda_pin, scl_pin);
    lcd.init();
    lcd.backlight();

    lcd.createChar(1, bar1);
    lcd.createChar(2, bar2);
    lcd.createChar(3, bar3);

    lcd.clear();
    lcd.setCursor(2, 0);
    lcd.print("PET-FEEDER");
    lcd.setCursor(0, 1);
    lcd.print("INICIALIZANDO...");
}

void DisplayManager::update(float peso, bool isWifiOk, String hora, int rssi)
{
    // ---  PESO Y WIFI ---
    lcd.setCursor(0, 0);
    lcd.print("P:");
    lcd.print(peso, 1);
    lcd.print("g      ");

    lcd.setCursor(11, 0);
    if (!isWifiOk)
    {
        lcd.print("W:OFF");
    }
    else
    {
        lcd.print("W:");
        lcd.setCursor(13, 0);
        lcd.write(1);
        lcd.setCursor(14, 0);
        if (rssi > -80)
            lcd.write(2);
        else
            lcd.print(" ");
        lcd.setCursor(15, 0);
        if (rssi > -60)
            lcd.write(3);
        else
            lcd.print(" ");
    }

    // --- TEMPERATURA Y HORA ---
    lcd.setCursor(0, 1);
    float t = getInternalTemperature();
    lcd.print("T:");
    if (t > 10 && t < 90)
    {
        lcd.print((int)t);
    }
    else
    {
        lcd.print("--");
    }
    lcd.print("C   ");

    lcd.setCursor(8, 1);
    if (hora == "" || hora.length() < 5)
    {
        lcd.print("--:--:--");
    }
    else
    {
        // Mostramos HH:MM:SS
        lcd.print(hora.substring(0, 8));
        lcd.print(" ");
    }
}

void DisplayManager::showFeeding(float pesoActual, float pesoInicial, float gramosPedidos)
{
    float servido = pesoInicial - pesoActual;
    if (servido < 0)
        servido = 0;

    // 2. Calcular porcentaje de progreso
    int porcentaje = (int)((servido / gramosPedidos) * 100.0);
    if (porcentaje > 100)
        porcentaje = 100;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" DOSIFICANDO:  ");
    lcd.print((int)gramosPedidos);
    lcd.print("g  ");

    // Progreso numérico y barra
    lcd.setCursor(0, 1);
    lcd.print(servido, 1);
    lcd.print("g ");

    // Dibujar barra de progreso (5 segmentos) en el espacio restante
    lcd.setCursor(10, 1);
    int bloques = porcentaje / 20;
    for (int i = 0; i < 5; i++)
    {
        if (i < bloques)
            lcd.write(3); // Caracter de barra llena
        else
            lcd.print("."); // Punto para el resto
    }
    lcd.print(" ");
}

void DisplayManager::showFeedingStatus(float gramosPedidos)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" DOSIFICANDO...");
    lcd.setCursor(0, 1);
    lcd.print("OBJETIVO: ");
    lcd.print((int)gramosPedidos);
    lcd.print("g");
}

void DisplayManager::showDispenseResult(String status, float cantidad)
{
    lcd.clear();
    lcd.setCursor(0, 0);

    if (status == "completado")
    {
        lcd.print("   EXITO   ");
        lcd.setCursor(0, 1);
        lcd.print("CANT: ");
        lcd.print(cantidad, 1);
        lcd.print("g OK");
    }
    else if (status == "parcial")
    {
        lcd.print(" DOSIS PARCIAL! ");
        lcd.setCursor(0, 1);
        lcd.print("SOLO: ");
        lcd.print(cantidad, 1);
        lcd.print("g ");
    }
    else
    { // Error
        lcd.print(" ERROR: ATASCO  ");
        lcd.setCursor(0, 1);
        lcd.print("REVISAR TOLVA   ");
    }
    delay(3000); // Para que el usuario pueda leerlo
    lcd.clear();
}

void DisplayManager::showTaraStatus(bool start)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    if (start)
    {
        lcd.print("CALIBRANDO...");
        lcd.setCursor(0, 1);
        lcd.print("NO TOCAR PLATO");
    }
    else
    {
        lcd.print("TARA COMPLETA");
        lcd.setCursor(0, 1);
        lcd.print("MEMORIA OK");
        delay(1500); // Pausa breve para que el usuario vea el mensaje
    }
}
