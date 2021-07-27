#include "databasemanager.h"

DataBaseManager::DataBaseManager(QObject *parent, int _cont) :
    QObject(parent),
    cont(_cont)
{
    // ruta de la base de datos se determina por la carpeta de instalacion
    QString rutaDeBasesDeDatos = QString("E:/Projects/Qt projects/Zona_Roja_UO/");
    switch (_cont) {
    case DBPacientes: {
        path = rutaDeBasesDeDatos + QString("Pacientes.db");
        break;
    }
    case DBEgresos: {
        path = rutaDeBasesDeDatos + QString("Egresos.db");
        break;
    }
    case DBRemisiones: {
        path = rutaDeBasesDeDatos + QString("Remisiones.db");
        break;
    }
    case DBResultadosPCR: {
        path = rutaDeBasesDeDatos + QString("ResultadosPCR.db");
        break;
    }
    case DBEnvioPCR: {
        path = rutaDeBasesDeDatos + QString("EnvioPCR.db");
        break;
    }
    }

    if(cont == DBResultadosPCR) {
        nombreTabla = "ResultadosPCR";
    }
    else if(cont == DBEnvioPCR) {
        nombreTabla = "EnvioPCR";
    }
    else {
        nombreTabla = "Pacientes";
    }

}


QList<QStringList> DataBaseManager::consultaSQL(QString sql) {
    QList<QStringList> tabla;
    // alcance de la base de datos, luego de salir del mismo todos los objetos se eliminan automaticamente
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(path);
        if(db.open()) {
            QSqlQuery query;
            if(!query.exec(sql)) {
                emit queryNotExec(query.lastError().text());
            }
            else {
                // query correctamente ejecutado
                int numColumns = query.record().count();
                while(query.next()) {
                    QStringList row;
                    for(int i=0; i < numColumns; i++) {
                        row << query.value(i).toString();
                    }
                    tabla << row;
                }
            }
        }
        else {
            emit dataBaseNotOpen(db.lastError().text());
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return tabla;
}

bool DataBaseManager::ejecutarSQL(QString sql) {
    if(usingTransaction) {
        transactionStatements << sql;
        return true;
    }
    bool r = true;
    // alcance de la base de datos, luego de salir del mismo todos los objetos se eliminan automaticamente
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(path);
        if(db.open()) {
            if(!usingTransaction && !transactionStatements.isEmpty()) {
                if(db.transaction()) {
                    QSqlQuery query(db);
                    for(int i = 0; i < transactionStatements.size(); i++) {
                        if(!query.exec(transactionStatements.at(i))) {
                            qDebug() << "query error : " << query.lastError().text();
                        }
                    }
                    if(!db.commit()) {
                        qDebug() << "commit error";
                        db.rollback();
                    }
                }
                else {
                   // transaction error
                }
            }
            else {
                QSqlQuery query;
                if(!query.exec(sql)) {
                    emit queryNotExec(query.lastError().text());
                    qDebug() << query.lastError();
                    qDebug() << "DATABASE>>>" <<db.databaseName();
                    qDebug() << "ALLOW TRANSACTION : " << db.driver()->hasFeature(QSqlDriver::Transactions);
                    qDebug() << "SQL STATEMENT : " << sql;
                    r = false;
                }
                else {
                    r = true;
                }
            }
        }
        else {
            emit dataBaseNotOpen(db.lastError().text());
            r = false;
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return r;
}

int DataBaseManager::maxRowId() {
    int maxRowId = -1;
    // alcance de db
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        //aqui se le dice la direccion de la base de datos a crear o cargar
        db.setDatabaseName(path);
        QSqlQuery query;
        if(db.open()) {
            if(!query.exec("select max(rowid) from " + nombreTabla + "")) {
                emit queryNotExec(query.lastError().text());
            }
            else {
                // ejecutado correctamente
                if(query.next()) {
                    maxRowId = query.value(0).toInt();
                }
            }
        }
        else {
            emit dataBaseNotOpen(db.lastError().text());
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return maxRowId;
}

QSet<QString> DataBaseManager::valoresColumna(QString columna) {
    QSet<QString> ret;
    // alcance de db
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        //aqui se le dice la direccion de la base de datos a crear o cargar
        db.setDatabaseName(path);
        QSqlQuery query;
        if(db.open()) {
            if(!query.exec("select distinct " + columna + " from " + nombreTabla + "")) {
                emit queryNotExec(query.lastError().text());
            }
            else {
                // ejecutado correctamente
                while(query.next()) {
                    ret << query.value(0).toString();
                }
            }
        }
        else {
            emit dataBaseNotOpen(db.lastError().text());
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return ret;
}

int DataBaseManager::getControlador() {
    return cont;
}

bool DataBaseManager::marcar(QString rowid, QString marca) {
    // alcance de db
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        //aqui se le dice la direccion de la base de datos a crear o cargar
        db.setDatabaseName(path);
        QSqlQuery query;
        if(db.open()) {
            if(!query.exec("select pcr1, pcr2, pcr3 from " + nombreTabla + " where rowid=\'" + rowid + "\'")) {
                emit queryNotExec(query.lastError().text());
                return false;
            }
            else {
                // logica de marcado
                if(query.next()) {
                    QStringList valores = (QStringList() << query.value(0).toString() <<
                                                        query.value(1).toString() <<
                                                        query.value(2).toString());

                    QString sql = "update Pacientes set ";
                    if(valores.at(2) != "") {
                        sql += "resultado3=\'" + marca + "\'" ;
                    }
                    else if(valores.at(1) != "") {
                        sql += "resultado2=\'" + marca + "\'" ;
                    }
                    else if(valores.at(0) != "") {
                        sql += "resultado1=\'" + marca + "\'" ;
                    }
                    else {
                        sql += "resultado1=\'" + marca + "\'" ;
                    }
                    query.exec(sql + "where rowid=" + rowid);

                }
            }
        }
        else {
            emit dataBaseNotOpen(db.lastError().text());
            return false;
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return true;
}

void DataBaseManager::beginTransaction() {
    usingTransaction = true;
}

void DataBaseManager::endTransaction() {
    usingTransaction = false;
    ejecutarSQL("empty");
    transactionStatements.clear();
}

void DataBaseManager::undo() {
    // alcance de db
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        //aqui se le dice la direccion de la base de datos a crear o cargar
        db.setDatabaseName(path);
        db.open();
        db.rollback();
    }
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
}


DataBaseManager::~DataBaseManager() {

}
