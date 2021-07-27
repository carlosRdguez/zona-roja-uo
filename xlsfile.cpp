#include "xlsfile.h"

XLSFile::XLSFile(QObject *parent) : QObject(parent) {

}

Tabla XLSFile::obtenerDatos(QString ruta) {
    Q_UNUSED(ruta);
    Tabla datos;
    datos << (QStringList() << "Codigo LBM" << "Codigo" << "Fecha de entrada" << "Nombre y apellidos" << "Edad" << "Sexo"
             << "CI" << "Direccion" << "Area de salud" << "Municipio" << "Provincia" << "Condicion" << "Pais de procedencia"
             << "Fecha de inicio de sintomas" << "Fecha de toma de muestra" << "Tipo de muestra" << "Procedencia de la entrega"
             << "Resultado" << "Cual" << "CT" << "Numero de placa" << "Fecha de resultado")
          << (QStringList() << "432311" << "1" << "20-07-2021" << "Pedro Mune"
                            << "56" << "M" << "85457848584" << "Edif. 45 Bloque A Apto. 4 DJM" << "Jose Marti"
                            << "Santiago" << "Santiago" << "Sospechoso COVID-19" << "Cuba" << "20-07-2021"
                            << "25-07-2021" << "Nasal" << "Pol. MININT" << "POS COVID-19" << ""
                            << "20.07" << "23" << "25-07-2021")
          << (QStringList() << "432312" << "2" << "29-07-2021" << "Jorge del Rio"
                                      << "52" << "M" << "89120254871" << "Calle Houston No. 3454 North Caroline" << "Washington"
                                      << "Santiago" << "Santiago" << "Sospechoso COVID-19" << "EE. UU" << "20-07-2021"
                                      << "25-07-2021" << "Nasal" << "Universidad de Oriente" << "NEG COVID-19" << ""
                                      << "" << "24" << "25-07-2021")
          << (QStringList() << "432313" << "3" << "29-07-2021" << "Karel Guzman"
                                      << "25" << "M" << "96150712567" << "Ave. Libertadores No. 342 Suenno" << "Armando Garcia"
                                      << "Santiago" << "Santiago" << "Sospechoso COVID-19" << "Cuba" << "20-07-2021"
                                      << "25-07-2021" << "Nasal" << "Universidad de Oriente" << "POS COVID-19" << ""
                                      << "29.65" << "24" << "25-07-2021");


    return datos;
}

bool XLSFile::escribirDatos(Tabla datos, QString ruta) {
    Q_UNUSED(datos);
    Q_UNUSED(ruta);
    return true;
}
