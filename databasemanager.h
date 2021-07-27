#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QStringList>
#include <QDebug>

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
#include <QSet>

/**
 * @brief Clase DataBaseManager
 *
 * Esta clase se encarga de la manipulacion directa de los datos del sistema mediante
 * el acceso a las base de datos SQLITE que posee el programa. Es la unica clase que
 * manipula los datos.
 */
class DataBaseManager : public QObject
{
    Q_OBJECT
public:
    // instancia un objeto de la clase
    explicit DataBaseManager(QObject *parent = nullptr, int _cont = DBPacientes);
    // realiza una consulta sql y devuelve el resultado
    QList<QStringList> consultaSQL(QString sql);
    // ejecuta la instruccion sql, retorna si fue exitosa la operacion
    bool ejecutarSQL(QString sql);
    // obtiene el ultimo id insertado
    int maxRowId();
    // obtiene los distintos valores que existen en una columna
    QSet<QString> valoresColumna(QString columna);
    // retorna el controlador
    int getControlador();
    // marca la fila con rowid dado a marca dada
    bool marcar(QString rowid, QString marca);
    void beginTransaction();
    void endTransaction();
    void undo();


    // destructor de la clase
    ~DataBaseManager();

    enum Controlador{DBPacientes,
          DBEgresos,
          DBRemisiones,
          DBEnvioPCR,
          DBResultadosPCR};
signals:
    void dataBaseNotOpen(QString error);
    void queryNotExec(QString error);
public slots:

private:
    QString path;
    QString nombreTabla;
    int cont;
    bool usingTransaction = false;
    QStringList transactionStatements;


};

#endif // DATABASEMANAGER_H
