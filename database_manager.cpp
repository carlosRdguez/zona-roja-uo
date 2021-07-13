#include "database_manager.h"

bool cargarTablaPacientes(QStringList& row) {
    // alcance de QSqlDatabase
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("data.db");
        if(db.open())
        {
            QSqlQuery query;
            if(query.exec(""))
            {
                while(query.next())
                {
                    row << query.value(0).toString()
                        << query.value(1).toString()
                        << query.value(2).toString();
                }
            }
        }
        else
        {
            return false;
        }
        db.close();
        return true;
    }
}
