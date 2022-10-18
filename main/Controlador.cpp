#include "Controlador.h"

////////STATE//////////////
void State::setControlador(Controlador *controlador){
  _controlador = controlador;
}

void State::off(){
  _controlador->changeState(new StateA);
}

void State::accepted(){}


///////////STATE A/////////////
void StateA::off(){}

void StateA::initState(){
  Serial.println("ESTADO A");
  _controlador->_vista->off();
  char empty[5] = {'\0','\0', '\0', '\0', '\0'};
  _controlador->updateUser(empty);
  _controlador->updatePass(empty);
}

void StateA::newInput(const char &in){
  Serial.println("EN ESTADO A: ");
  Serial.println(in);
  _controlador->changeState(new StateB);
}


//////////////STATE B/////////////////
void StateB::initState(){
  Serial.println("ESTADO B");
  _controlador->_vista->on();
  _controlador->_vista->changeHead("Usuario?(4 nums)");
}

void StateB::newInput(const char &in){
  Serial.println("EN ESTADO B: ");
  Serial.println(in);
  //Eliminar caracter
  if (in == '*'){
    if (puntero > 0){
      puntero--;
      user[puntero] = 0;
      _controlador->_vista->deleteChar();
    }
  //Volver
  } else if (in == 'B' || in == 'A'){
    _controlador->changeState(new StateA);
  //Aceptar
  } else if (in == '#'){
    accepted();
  //Ingresar caracter 
  } else if (puntero <= 3 && isdigit(in)){
    user[puntero] = in;
    puntero++;
    _controlador->_vista->newInput(in);
  }
   
}

void StateB::accepted(){
  if (puntero == 4) {
      _controlador->updateUser(user);
      _controlador->changeState(new StateC);
    }
}

//////////////STATE C/////////////////
void StateC::initState(){
  Serial.println("ESTADO C");
  _controlador->_vista->changeHead("Pass?(4 nums)");
}

void StateC::newInput(const char &in){
  Serial.println("EN ESTADO C: ");
  Serial.println(in);
  //Eliminar caracter
  if (in == '*'){
    if (puntero > 0){
      puntero--;
      pass[puntero] = 0;
      _controlador->_vista->deleteChar();
    }
  //Salir
  } else if (in == 'A'){
    _controlador->changeState(new StateA);
  //Volver
  } else if (in == 'B'){
    _controlador->changeState(new StateB);
  //Aceptar
  } else if (in == '#'){
    accepted();
  //Ingresar caracter 
  } else if (puntero <= 3 && isdigit(in)){
    pass[puntero] = in;
    puntero++;
    _controlador->_vista->newInput(in);
  }
   
}

void StateC::accepted(){
  if (puntero == 4) {
      _controlador->updatePass(pass);
      _controlador->changeState(new StateD);
    } 
}

//////////////STATE D/////////////////
void StateD::initState(){
  Serial.println("ESTADO D");
  _controlador->_vista->changeHead("Tipo?(1-8)");
}

void StateD::newInput(const char &in){
  Serial.println("EN ESTADO D: ");
  Serial.println(in);
  //Eliminar caracter
  if (in == '*'){
     _controlador->_vista->deleteChar();
     type = 0;
  //Salir
  } else if (in == 'A'){
    _controlador->changeState(new StateA);
  //Volver
  } else if (in == 'B'){
    _controlador->changeState(new StateC);
  //Aceptar
  } else if (in == '#'){
    accepted();
  //Ingresar caracter entre 1 y 8
  } else if (type == 0 && isdigit(in) && in >= 49 && in <= 56){
    type = in - '0';
    _controlador->_vista->newInput(in);
  }
   
}

void StateD::accepted(){
  if (type != 0) {
      _controlador->updateType(type);
    } 
}


///////////CONTROLADOR////////////////
Controlador::Controlador(State *state, Vista *vista){
  _state = state;
  //_state->setControlador(this);
  _vista = vista;
}

Controlador::~Controlador(){
  delete _state;
}

void Controlador::changeState(State *state){
  Serial.println("SE HA CAMBIADO EL ESTADO");
  if (_state){
    delete _state;
  }
  _state = state;
  _state->setControlador(this);
  _state->initState();
}

void Controlador::newInput(const char &in){
  _state->newInput(in);
}

void Controlador::off(){
  _state->off();
}

void Controlador::updateUser(const char *user){
  strcpy(_user, user);
}

void Controlador::updatePass(const char *pass){
  strcpy(_pass, pass);
}

void Controlador::updateType(char type){
  _type = type;
}
