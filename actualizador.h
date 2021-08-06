#ifndef ACTUALIZADOR_H
#define ACTUALIZADOR_H

#include <QObject>
#include <QStringList>
#include <QTimer>
#include <QDate>
#include <QTime>
#include <QDebug>
#include <QMessageBox>
#include <QThread>
#include <QString>

#include "excelmanager.h"
#include "databasemanager.h"

typedef QList<QStringList> Tabla;

/**
 * @brief Clase Actualizador
 *
 * Esta clase se encarga de mantener actualizado todo el sistema. La actualizacion
 * tiene lugar cuando el sistema recibe los resultados de PCR. Se modifican los datos
 * de las bases de datos.
 *
 * Funcionamiento
 * 1. descargara todos los correos adjuntos de un correo entrante
 * alternativa -> arrastrando cada correo entrante a la app
 * 2. actualizara la base de datos ResultadosPCR.db para mantener todos los resultados de PCR
 * 3. todos los resultados que sean del centro en la base de datos ResultadosPCR.db, ira uno
 * por uno y actulizara el resultado del paciente en la base de datos Pacientes.db
 */
class Actualizador : public QObject
{
    Q_OBJECT
public:
    // modos en que puede funcionar el actualizador
    enum Modo{ActualizacionManual, ActualizacionAutomatica};
    explicit Actualizador(QObject* parent = nullptr, Modo _modo = ActualizacionManual);
    // cambia de modo actualizacion automatica o manual
    void asignarModo(Modo modo);
    // obtiene el modo actual
    Modo obtenerModo();
    // lee un documento xls de actualizacion del sistema
    void leerNuevaEntrada(QString ruta);
    // actualiza las bases de datos
    bool actualizarBasesDeDatos(Tabla &resultados);
    virtual ~Actualizador();
signals:
    void descargaIniciada(); // se emite cada vez que hay un correo descargandose
    void nuevosResultadosDelCentro(); // se emite cada vez que hay resultados del centro
    void actualizacionFinalizada();
    void sinConexion(); // se emite cada vez que no se pueda conectar a la red

    void leer(QString ruta);
public slots:
    // obtiene los documentos adjuntos del correo y los descarga
    void obtenerAdjuntosCorreosEntrantes();
private:
    // determina si una entrada es valida
    bool entradaValida(QString ruta);
    // convierte una fila de lectura en una porcion de instruccion sql necesaria para actualizacion
    QString convertirSQL(QStringList fila);
    Modo modo;
    QTimer* timer;

};

#endif // ACTUALIZADOR_H
