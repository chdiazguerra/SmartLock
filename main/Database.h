#ifndef DATABASE_H
#define DATABASE_H

#include <Arduino.h>
#include <sqlite3.h>

#define CORRECT_DATA 29
#define INCORRECT_DATA 30
#define DENIED 31
#define ALLOWED 32
#define NO_ADMIN 33

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

    /**
     * Agrega un nuevo numero de telefono a la base de datos
     */
    int addPhone(const char *newphone);

    /**
     * Elimina un numero de telefono de la base de datos
     */
    int deletePhone(const char *phone);
};

class Datausers : public Database{
  public:
    Datausers(const String dbName) : Database(dbName){};
    /**
    * Verifica los datos del usuario
    * @param user Nombre de usuario
    * @param pass Clave del usuario
    * @return CORRECT_DATA, INCORRECT_DATA or !SQL_LITE
    */
    int verifyUser(const char *user, const char *pass);
     
    /**
    * Verifica que el usuario cuenta con los permisos correspondientes
    * @param user Nombre de usuario
    * @param type Tipo de operacion a realizar
    * @return ALLOWED, DENIED or !SQL_LITE
    */
    int verifyPermission(const char *user, const char &type);

    /**
    * Agrega un nuevo usuario
    * @param newuser Nuevo usuario a agregar
    * @param newpass Clave del usuario a agregar
    * @return SQLITE_CONSTRAINT, SQLITE_OK or !SQLITE_OK
    */
    int newUser(const char *newuser, const char *newpass);

    /**
    * Cambia la clave a algun usuario
    * @param user Usuario al que se le cambia la clave
    * @param newpass Nueva clave del usuario
    * @return SQLITE_OK or !SQLITE_OK
    */
    int changePass(const char *user, const char *newpass);

    /**
    * Borra un usuario de la base de datos
    * @param user Usuario a borrar
    * @return SQLITE_OK or !SQLITE_OK
    */
    int deleteUser(const char *user);

    /**
    * Le da permiso de cierto tipo a un usuario
    * @param user
    * @param type
    * @return SQLITE_OK or !SQLITE_OK
    */
    int addPermission(const char *user, const char &type);

    /**
    * Revoca el permiso a un usuario
    * @param user
    * @param type Tipo del permiso revocado
    * @return SQLITE_OK or !SQLITE_OK
    */
    int revokePermission(const char *user, const char &type);   
       
};

#endif
