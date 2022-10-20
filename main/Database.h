#ifndef DATABASE_H
#define DATABASE_H

#include <Arduino.h>
#include <sqlite3.h>

#define CORRECT_DATA 29
#define INCORRECT_DATA 30
#define DENIED 31
#define ALLOWED 32

class Database{
  protected:
    //Base de datos
    sqlite3 *db;
    //Nombre de la base de datos
    String filename;
    //Valor del error
    int errType;
    //Comando de la trasaccion
    char sql[200];
    //Variables necesarias (no se ocupan)
    char *zErrMsg = 0;
    const char* data;
    
    /**
     * Permite abrir la base de datos, utilizando filename
     */
    int opendb();
    
  public:
    //Numero de filas dadas por la transaccion (solo util en verify)
    static int rowsRetrieved;
    //Constructor
    Database(const String dbName);
    /**
     * Permite realizar la transaccion utilizando el atributo sql
     */
    void transaction();
    //Callback de la transaccion
    static int callback(void *data, int argc, char **argv, char **azColName);
};

class Dataphone : public Database{
  public:
    Dataphone(const String dbName) : Database(dbName){};
    /**
     * Verifica que el numero de telefono esta en la base de datos
     * @param phone numero de telefono a verificar
     */
    int verify(const String &phone);
};

class Datausers : public Database{
  public:
    Datausers(const String dbName) : Database(dbName){};
    /**
     * Verifica los datos del usuario
     * @param user Nombre de usuario
     * @param pass Clave del usuario
     */
     int verifyUser(const String &user, const String &pass);
    /**
     * Verifica que el usuario cuenta con los permisos correspondientes
     * @param user Nombre de usuario
     * @param type Tipo de operacion a realizar
     */
    int verifyPermission(const String &user, const char &type);
};
#endif
