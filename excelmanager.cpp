#include "excelmanager.h"

ExcelManager::ExcelManager(QObject *parent) : QObject(parent){

}

Tabla ExcelManager::leerDocumento(QString ruta) {
    QXlsx::Document* xlsxR = new QXlsx::Document(ruta, parent());
    Tabla resultados;
    if (xlsxR->load()) { // load excel file
        int row = 1;
        QStringList fila;
        do {
            fila.clear();
            QXlsx::Cell* cell = xlsxR->cellAt(row, 1);
            if(cell && !cell->readValue().toString().isEmpty())  {
                fila << cell->readValue().toString();
                for(size_t column = 2; column <= 22; column++) {
                    if(xlsxR->cellAt(row, column))
                        fila << xlsxR->cellAt(row, column)->readValue().toString();
                    else
                        fila << "";
                }
                resultados << fila;
            }
            else {
                bool vacio = true;
                for(size_t column = 1; column <= 22; column++) {
                    if(xlsxR->cellAt(row, column))
                        fila << xlsxR->cellAt(row, column)->readValue().toString();
                    else
                        fila << "";
                    if(!vacio && xlsxR->cellAt(row, column) && !xlsxR->cellAt(row, column)->readValue().toString().isEmpty())
                        vacio = false;
                }
                if(!vacio)
                    resultados << fila;
                else
                    break;
            }
            row++;
        } while(true);
    }
    return resultados;
}

bool ExcelManager::escribirDocumento(QString ruta, Tabla &tabla) {
    QXlsx::Document xlsxW;
    for(int row = 0; row < tabla.size(); row++) {
        for(int column = 0; column < tabla.size(); column++) {
            xlsxW.write(QString('A'+column)+QString::number(row+1), tabla.at(row).at(column));
        }
    }
    bool r = xlsxW.saveAs(ruta);
    xlsxW.deleteLater();
    return r;
}
