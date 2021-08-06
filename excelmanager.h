#ifndef PCRFILE_H
#define PCRFILE_H

#include "QXlsx/header/xlsxdocument.h"
#include "QXlsx/header/xlsxchartsheet.h"
#include "QXlsx/header/xlsxcellrange.h"
#include "QXlsx/header/xlsxchart.h"
#include "QXlsx/header/xlsxrichstring.h"
#include "QXlsx/header/xlsxworkbook.h"

#include <QObject>
#include <QDebug>
#include <QStringList>



typedef QList<QStringList> Tabla;


class ExcelManager : public QObject
{
    Q_OBJECT
public:
    explicit ExcelManager(QObject* parent = nullptr);
    Tabla leerDocumento(QString ruta);
    bool escribirDocumento(QString ruta, Tabla& tabla);
};

#endif // PCRFILE_H
