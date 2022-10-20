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

////////Datausers///////////
int Datausers::verifyUser(const String &user, const String &pass){
  sprintf(sql, "SELECT * FROM usuarios WHERE user='%s' AND pass='%s'", user.c_str(), pass.c_str());
  transaction();
  int res = rowsRetrieved ? CORRECT_DATA : (errType ? errType : INCORRECT_DATA);
  rowsRetrieved = 0;
  return res;
}

int Datausers::verifyPermission(const String &user, const char &type){
  sprintf(sql, "SELECT * FROM usuarios WHERE user='%s' AND %c=1", user.c_str(), type);
  transaction();
  int res = rowsRetrieved ? ALLOWED : (errType ? errType : DENIED);
  rowsRetrieved = 0;
  return res;
}
