#include "Controlador.h"

////////STATE//////////////
void State::setControlador(Controlador *controlador){
  _controlador = controlador;
}

void State::off(){
  _controlador->changeState(new StateA);
}

void State::accepted(){}
void State::newInput(const char &in){}
void State::initState(){}

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
      int resOp = _controlador->_datausers->verifyUser(_controlador->_user, _controlador->_pass);
      _controlador->_resOp = resOp;
      if (resOp == CORRECT_DATA){
        _controlador->changeState(new StateD);
      } else {
        _controlador->changeState(new StateEnd);
      }      
    } 
}

//////////////STATE D/////////////////
void StateD::initState(){
  Serial.println("ESTADO D");
  _controlador->_vista->changeHead("Tipo?(0-8)");
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
  //Ingresar caracter entre 0 y 8
  } else if (type == 0 && isdigit(in) && in >= 48 && in <= 56){
    type = in;
    _controlador->_vista->newInput(in);
  }
   
}

void StateD::accepted(){
  if (type != 0) {
    _controlador->updateType(type);
    int resOp = _controlador->_datausers->verifyPermission(_controlador->_user, _controlador->_type);
    _controlador->_resOp = resOp;
    if (resOp==ALLOWED){
      if (type=='0'){
        _controlador->changeState(new State0);
      } else if (type=='1') {
        _controlador->changeState(new State1);
      } else if (type=='2'){
        _controlador->changeState(new State2);
      } else if (type=='3'){
        _controlador->changeState(new State3); 
      } else if (type=='4'){
        _controlador->changeState(new State4);
      } else if (type=='5'){
        _controlador->changeState(new State5);
      } else if (type=='6'){
        _controlador->changeState(new State6);
      } else if (type=='7' || type=='8'){
        _controlador->changeState(new State7);
      } else {
        _controlador->changeState(new StateEnd);
      }
    } else {
      _controlador->changeState(new StateEnd);
    }
  } 
}


/////////StateEnd//////////////
void StateEnd::initState(){
  Serial.println("ESTADO End");
  int &resOp = _controlador->_resOp;
  String head;
  if (resOp==INCORRECT_DATA){
    head = "Datos erroneos";
  } else if (resOp==DENIED){
    head = "Denegado";
  } else if (resOp==SQLITE_CONSTRAINT){
    head = "Ya existe";
  } else if (resOp==NO_ADMIN){
    head = "No puede hacerse";
  } else if (resOp != SQLITE_OK) {
    head = "Ocurrio un error";
  } else {
    head = "Hecho";
  }
  _controlador->_vista->changeHead(head);
}

void StateEnd::newInput(const char &in){
  if (in == '#'){
    accepted();
  } else if (in == 'A'){
    _controlador->changeState(new StateA);
  }
}

void StateEnd::accepted(){
  _controlador->changeState(new StateB);
}


////////State0///////////
void State0::initState(){
  _controlador->relayOn();
  _controlador->_resOp = 0;
  _controlador->changeState(new StateEnd);
}


/////////State1//////////
void State1::initState(){
  Serial.println("ESTADO 1");
  _controlador->_vista->changeHead("Nuevo usuario?");
}

void State1::newInput(const char &in){
  Serial.println("EN ESTADO 1: ");
  Serial.println(in);
  //Eliminar caracter
  if (in == '*'){
    if (puntero > 0){
      puntero--;
      if (state==0){
        newuser[puntero] = 0;
      } else {
        newpass[puntero] = 0;
      }
      _controlador->_vista->deleteChar();
    }
  //Salir
  } else if (in == 'A'){
    _controlador->changeState(new StateA);
  //Volver
  } else if (in == 'B'){
    if (state==0){
      _controlador->changeState(new StateD);
    } else {
      state = 0;
      puntero = 0;
      newuser[0] = '\0';
      newpass[0] = '\0';
      _controlador->_vista->changeHead("Nuevo usuario?");
    }
  //Aceptar
  } else if (in == '#'){
    accepted();
  //Ingresar caracter 
  } else if (puntero <= 3 && isdigit(in)){
    if (state==0){
      newuser[puntero] = in;
    } else {
      newpass[puntero] = in;
    }
    puntero++;
    _controlador->_vista->newInput(in);
  }
   
}

void State1::accepted(){
  if (puntero == 4) {
    if (state==0){
      state = 1;
      puntero = 0;
      _controlador->_vista->changeHead("Nueva clave?");
      return;
    }
    _controlador->_resOp = _controlador->_datausers->newUser(newuser, newpass);
    _controlador->changeState(new StateEnd);    
  }
}

//////////State2///////////
void State2::initState(){
  Serial.println("ESTADO 2");
  _controlador->_vista->changeHead("User a borrar?");
}

void State2::newInput(const char &in){
  Serial.println("EN ESTADO 2: ");
  Serial.println(in);
  //Eliminar caracter
  if (in == '*'){
    if (puntero > 0){
      puntero--;
      user[puntero] = 0;
      _controlador->_vista->deleteChar();
    }
  } else if (in == 'A'){
    _controlador->changeState(new StateA);
  //Volver
  } else if (in == 'B'){
    _controlador->changeState(new StateD);
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

void State2::accepted(){
  if (puntero == 4) {
    if (strcmp(user, "0000") != 0){
      _controlador->_resOp = _controlador->_datausers->deleteUser(user);
    } else {
      _controlador->_resOp = NO_ADMIN;
    }
    _controlador->changeState(new StateEnd);
  }
}

//////////State3//////////
void State3::initState(){
  Serial.println("ESTADO 3");
  _controlador->_vista->changeHead("Usuario?");
}

void State3::newInput(const char &in){
  Serial.println("EN ESTADO 3: ");
  Serial.println(in);
  //Eliminar caracter
  if (in == '*'){
    if (puntero > 0){
      puntero--;
      if (state==0){
        user[puntero] = 0;
      } else {
        newpass[puntero] = 0;
      }
      _controlador->_vista->deleteChar();
    }
  //Salir
  } else if (in == 'A'){
    _controlador->changeState(new StateA);
  //Volver
  } else if (in == 'B'){
    if (state==0){
      _controlador->changeState(new StateD);
    } else {
      state = 0;
      puntero = 0;
      user[0] = '\0';
      newpass[0] = '\0';
      _controlador->_vista->changeHead("Usuario?");
    }
  //Aceptar
  } else if (in == '#'){
    accepted();
  //Ingresar caracter 
  } else if (puntero <= 3 && isdigit(in)){
    if (state==0){
      user[puntero] = in;
    } else {
      newpass[puntero] = in;
    }
    puntero++;
    _controlador->_vista->newInput(in);
  }
   
}

void State3::accepted(){
  if (puntero == 4) {
    if (strcmp(user, "0000") == 0){
      _controlador->_resOp = NO_ADMIN;
      _controlador->changeState(new StateEnd);
      return;
    }
    if (state==0){
      state = 1;
      puntero = 0;
      _controlador->_vista->changeHead("Nueva clave?");
      return;
    }
    _controlador->_resOp = _controlador->_datausers->changePass(user, newpass);
    _controlador->changeState(new StateEnd);    
  }
}

/////////State4////////////
void State4::initState(){
  Serial.println("ESTADO 4");
  _controlador->_vista->changeHead("Nueva clave?");
}

void State4::newInput(const char &in){
  Serial.println("EN ESTADO 4: ");
  Serial.println(in);
  //Eliminar caracter
  if (in == '*'){
    if (puntero > 0){
      puntero--;
      newpass[puntero] = 0;
      _controlador->_vista->deleteChar();
    }
  } else if (in == 'A'){
    _controlador->changeState(new StateA);
  //Volver
  } else if (in == 'B'){
    _controlador->changeState(new StateD);
  //Aceptar
  } else if (in == '#'){
    accepted();
  //Ingresar caracter 
  } else if (puntero <= 3 && isdigit(in)){
    newpass[puntero] = in;
    puntero++;
    _controlador->_vista->newInput(in);
  }
   
}

void State4::accepted(){
  if (puntero == 4) {
     _controlador->_resOp = _controlador->_datausers->changePass(_controlador->_user, newpass);
    _controlador->changeState(new StateEnd);
  }
}

////////////State5/////////////
void State5::initState(){
  Serial.println("ESTADO 5");
  _controlador->_vista->changeHead("Usuario?");
}

void State5::newInput(const char &in){
  Serial.println("EN ESTADO 5: ");
  Serial.println(in);
  //Eliminar caracter
  if (in == '*'){
    if (puntero > 0){
      if (state==0){
        puntero--;
        user[puntero] = 0;
      } else {
        type = 0;
      }
      _controlador->_vista->deleteChar();
    }
  //Salir
  } else if (in == 'A'){
    _controlador->changeState(new StateA);
  //Volver
  } else if (in == 'B'){
    if (state==0){
      _controlador->changeState(new StateD);
    } else {
      state = 0;
      puntero = 0;
      user[0] = '\0';
      type = 0;
      _controlador->_vista->changeHead("Usuario?");
    }
  //Aceptar
  } else if (in == '#'){
    accepted();
  //Ingresar caracter 
  } else if (isdigit(in)){
    if (state==0 && puntero <= 3){
      user[puntero] = in;
      puntero++;
      _controlador->_vista->newInput(in);
    } else if (state==1 && type == 0 && in >= 48 && in <= 56){
      type = in;
      _controlador->_vista->newInput(in);
    }
  }
}

void State5::accepted(){
  if ((puntero == 4 && state==0) || (type!=0 && state==1)) {
    if (strcmp(user, "0000") == 0){
      _controlador->_resOp = NO_ADMIN;
      _controlador->changeState(new StateEnd);
      return;
    }
    if (state==0){
      state = 1;
      puntero = 0;
      _controlador->_vista->changeHead("Tipo?(0-8)");
      return;
    }
    _controlador->_resOp = _controlador->_datausers->addPermission(user, type);
    _controlador->changeState(new StateEnd);    
  }
}

/////////State 6/////////////
void State6::initState(){
  Serial.println("ESTADO 6");
  _controlador->_vista->changeHead("Usuario?");
}

void State6::newInput(const char &in){
  Serial.println("EN ESTADO 6: ");
  Serial.println(in);
  //Eliminar caracter
  if (in == '*'){
    if (puntero > 0){
      if (state==0){
        puntero--;
        user[puntero] = 0;
      } else {
        type = 0;
      }
      _controlador->_vista->deleteChar();
    }
  //Salir
  } else if (in == 'A'){
    _controlador->changeState(new StateA);
  //Volver
  } else if (in == 'B'){
    if (state==0){
      _controlador->changeState(new StateD);
    } else {
      state = 0;
      puntero = 0;
      user[0] = '\0';
      type = 0;
      _controlador->_vista->changeHead("Usuario?");
    }
  //Aceptar
  } else if (in == '#'){
    accepted();
  //Ingresar caracter 
  } else if (isdigit(in)){
    if (state==0 && puntero <= 3){
      user[puntero] = in;
      puntero++;
      _controlador->_vista->newInput(in);
    } else if (state==1 && type == 0 && in >= 48 && in <= 56){
      type = in;
      _controlador->_vista->newInput(in);
    }
  }
}

void State6::accepted(){
  if ((puntero == 4 && state==0) || (type!=0 && state==1)) {
    if (strcmp(user, "0000") == 0){
      _controlador->_resOp = NO_ADMIN;
      _controlador->changeState(new StateEnd);
      return;
    }
    if (state==0){
      state = 1;
      puntero = 0;
      _controlador->_vista->changeHead("Tipo?(0-8)");
      return;
    }
    _controlador->_resOp = _controlador->_datausers->revokePermission(user, type);
    _controlador->changeState(new StateEnd);    
  }
}

/////////State7///////////
void State7::initState(){
  Serial.println("ESTADO 7");
  _controlador->_vista->changeHead("Nuevo telefono?");
}

void State7::newInput(const char &in){
  Serial.println("EN ESTADO 7: ");
  Serial.println(in);
  //Eliminar caracter
  if (in == '*'){
    if (puntero > 0){
      puntero--;
      phone[puntero] = 0;
      _controlador->_vista->deleteChar();
    }
  //Salir
  } else if (in == 'A'){
    _controlador->changeState(new StateA);
  //Volver
  } else if (in == 'B'){
    _controlador->changeState(new StateD);
  //Aceptar
  } else if (in == '#'){
    accepted();
  //Ingresar caracter 
  } else if (isdigit(in)){
    phone[puntero] = in;
    puntero++;
    _controlador->_vista->newInput(in);
  }
   
}

void State7::accepted(){
  if (_controlador->_type=='7'){
    _controlador->_resOp = _controlador->_dataphone->addPhone(phone);
  } else if (_controlador->_type=='8'){
    _controlador->_resOp = _controlador->_dataphone->deletePhone(phone);
  }
  _controlador->changeState(new StateEnd);    
}


///////////CONTROLADOR////////////////
Controlador::Controlador(State *state, Vista *vista, Datausers *datausers, Dataphone *dataphone){
  _state = state;
  _vista = vista;
  _datausers = datausers;
  _dataphone = dataphone;
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

void Controlador::relayOn(){
  Serial.println("RELE ACTIVADO");
  delay(500);
  Serial.println("RELE DESACTIVADO");
}
