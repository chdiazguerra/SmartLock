#ifndef CONTROLADOR_H
#define CONTROLADOR_H

#include <Arduino.h>
#include "Vista.h"

class State;

class Controlador{
  private:
    //Estado actual
    State *_state;
       
  public:
    //Vista para mostrar al usuario
    Vista *_vista;
    //Usuario
    char _user[5];
    //Password
    char _pass[5];
    //Tipo de operacion
    char _type;
  
    //Constructor
    Controlador(State *state, Vista *vista);
    //Destructor
    ~Controlador();

    /**
     * Cambia el estado actual
     * @param state el estado al cual cambiar
     */
    void changeState(State *state);

    /**
     * Maneja un nuevo caracter de entrada
     * @param in entrada ingresada
     */
    void newInput(const char &in);

    /**
     * Apaga la pantalla
     */
     void off();

     /**
      * Guarda el usuario ingresado
      */
      void updateUser(const char *user);

     /**
      * Guarda el password ingresado
      */
      void updatePass(const char *pass);

      /**
       * Guarda el tipo de operacion
       */
       void updateType(char type);
};

class State{
  protected:
    //Controlador asociado al estado
    Controlador *_controlador;

  private:
    /**
     * Se ha aceptado la entrada
     */
    virtual void accepted();

  public:
    /**
     * Maneja un nuevo caracter de entrada
     * @param in entrada ingresada
     */
    virtual void newInput(const char &in);
    
    /**
     * Fija el controlador asociado al estado
     * @param controlador
     */
    void setControlador(Controlador *controlador);

    /**
     * Apaga la pantalla
     */
     virtual void off();

     /**
      * Realiza procedimientos luego de iniciar el estado
      */
      virtual void initState();
};

/**
 * Estado inicial, con la pantalla apagada
 */
class StateA : public State{
  public:
    void newInput(const char &in) override;
    void off() override;
    void initState() override;
};

/**
 * Estado de prendido de pantalla y pregunta el usuario
 */
class StateB : public State{

  private:
    char user[5] = {'\0','\0', '\0', '\0', '\0'};
    char puntero = 0;

    void accepted() override;
    
  public:
    void newInput(const char &in) override;
    void initState() override;
};

/**
 * Pregunta el password
 */
class StateC : public State{    
  private:
    char pass[5] = {'\0','\0', '\0', '\0', '\0'};
    char puntero = 0;

     void accepted() override;
    
  public:
    void newInput(const char &in) override;
    void initState() override;
};

/**
 * Pregunta el tipo de operacion
 */
class StateD : public State{    
  private:
    char type = 0;
    
    void accepted() override;
    
  public:
    void newInput(const char &in) override;
    void initState() override;
};



#endif
