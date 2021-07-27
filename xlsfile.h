#ifndef XLSFILE_H
#define XLSFILE_H

#include <QObject>
#include <QStringList>

typedef QList<QStringList> Tabla;

/**
 * @brief Clase XLSFile
 *
 * Responsable de la manipulacion de archivos Excel (*xls). Proporciona
 * facilidades para escritura y lectura de estos archivos
 */

class XLSFile : public QObject
{
    Q_OBJECT
public:
    explicit XLSFile(QObject *parent = nullptr);
    // retorna los datos del archivo xls
    static Tabla obtenerDatos(QString ruta);
    // escribe los datos en un archivo xls
    static bool escribirDatos(Tabla datos, QString ruta);

signals:

public slots:
};

#endif // XLSFILE_H
