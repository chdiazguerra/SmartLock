#include "Database.h"

int Database::rowsRetrieved = 0;

Database::Database(const String dbName){
  filename = dbName;
}


int Database::opendb(){
  int rc1 = sqlite3_open(filename.c_str(), &db);
  if (rc1) {
      Serial.printf("Can't open database: %s\n", sqlite3_errmsg(db));
      return rc1;
  } else {
      Serial.printf("Opened database successfully\n");
  }
  return rc1;
}

void Database::transaction(){
  errType = opendb();
  if (errType){
    return;
  }
  Serial.println(sql);  
  errType = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
  
  if (errType != SQLITE_OK){
    Serial.println("ERROR");
    Serial.println(errType);
    Serial.println(zErrMsg);
    sqlite3_free(zErrMsg);
  } else {
    Serial.println("CORRECT");
    Serial.print("NUMBER: ");
    Serial.println(rowsRetrieved);
  }

  sqlite3_close(db);
}

int Database::callback(void *data, int argc, char **argv, char **azColName){
  rowsRetrieved++;
  return 0;
}


//////////Dataphone/////////
int Dataphone::verify(const String &phone){
  sprintf(sql, "SELECT * FROM phones WHERE phone='%s'", phone.c_str());
  transaction();
  int res = rowsRetrieved ? ALLOWED : (errType ? errType : DENIED);
  rowsRetrieved = 0;
  return res;
}

int Dataphone::addPhone(const char *newphone){
  sprintf(sql, "INSERT INTO phones VALUES ('%s')", newphone);
  transaction();
  return errType;
}

int Dataphone::deletePhone(const char *phone){
  sprintf(sql, "DELETE FROM phones WHERE phone='%s'", phone);
  transaction();
  return errType;
}

////////Datausers///////////
int Datausers::verifyUser(const char *user, const char *pass){
  sprintf(sql, "SELECT * FROM usuarios WHERE user='%s' AND pass='%s'", user, pass);
  transaction();
  int res = rowsRetrieved ? CORRECT_DATA : (errType ? errType : INCORRECT_DATA);
  rowsRetrieved = 0;
  return res;
}

int Datausers::verifyPermission(const char *user, const char &type){
  sprintf(sql, "SELECT * FROM usuarios WHERE user='%s' AND `%c`=1", user, type);
  transaction();
  int res = rowsRetrieved ? ALLOWED : (errType ? errType : DENIED);
  rowsRetrieved = 0;
  return res;
}

int Datausers::newUser(const char *newuser, const char *newpass){
  sprintf(sql, "INSERT INTO usuarios (user, pass) VALUES ('%s', '%s')", newuser, newpass);
  transaction();
  return errType;
}

int Datausers::changePass(const char *user, const char *newpass){
  sprintf(sql, "UPDATE usuarios SET pass='%s' WHERE user='%s'", newpass, user);
  transaction();
  return errType;
}

int Datausers::deleteUser(const char *user){
  sprintf(sql, "DELETE FROM usuarios WHERE user='%s'", user);
  transaction();
  return errType;
}

int Datausers::addPermission(const char *user, const char &type){
  sprintf(sql, "UPDATE usuarios SET `%c`=1 WHERE user='%s'", type, user);
  transaction();
  return errType;
}

int Datausers::revokePermission(const char *user, const char &type){
  sprintf(sql, "UPDATE usuarios SET `%c`=0 WHERE user='%s'", type, user);
  transaction();
  return errType;
}
