#ifndef VISTA_H
#define VISTA_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

class Vista{
  private:
    //Pantalla LCD
    LiquidCrystal_I2C *lcd;
    //Lugar donde esta escribiendo
    char puntero = 0;

  public:
    Vista(LiquidCrystal_I2C *lcd_);

    /**
     * Imprime un nuevo caracter en pantalla
     */
    void newInput(const char &in);

    /**
     * Elimina el ultimo caracter ingresado
     */
    void deleteChar();

    /**
     * Cambiar la primera linea
     */
    void changeHead(const String &in);

    /**
     * Apaga la pantalla
     */
    void off();

    /**
     * Enciende la pantalla
     */
    void on();
};

#endif
