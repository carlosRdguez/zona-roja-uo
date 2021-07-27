#include "actualizador.h"

Actualizador::Actualizador(QObject *parent, Modo _modo) : QObject(parent) {
    asignarModo(_modo);
}

void Actualizador::asignarModo(Actualizador::Modo _modo) {
    if(modo != _modo) {
        // cambiando a modo manual
        if(modo == ActualizacionAutomatica) {
            // detener los intervalos de intento de actualizacion
            timer->stop();
            // desconectar sennales asociadas
            disconnect(timer, SIGNAL(timeout()), this, SLOT(obtenerAdjuntosCorreosEntrantes()));
            // destruir timer
            delete timer;
        }
        // cambiando a modo automatico
        if(modo == ActualizacionManual) {
            timer = new QTimer(this);
            timer->setInterval(2000); // 180000 msec = 3 min
            connect(timer, SIGNAL(timeout()), this, SLOT(obtenerAdjuntosCorreosEntrantes()));
            timer->start();
        }
        modo = _modo;
    }
}

Actualizador::Modo Actualizador::obtenerModo() {
    return modo;
}

void Actualizador::obtenerAdjuntosCorreosEntrantes() {
    qDebug() << QDate::currentDate().toString("dd-MM-yyyy") << ", " << QTime::currentTime().toString("h:m:s ap") << " : intentando descargar correo";
}

void Actualizador::leerNuevaEntrada(QString ruta) {
    if(entradaValida(ruta)) {
        // emitiendo sennal de nueva entrada, para saber la ultima placa del dia
        Tabla datos = XLSFile::obtenerDatos(ruta);
        actualizarBasesDeDatos(datos);
    }
}

bool Actualizador::actualizarBasesDeDatos(Tabla &resultados) {
    bool resultadosDelCentro = false;
    // determinando si llegaron resultados del centro
    for(int i = 1; i < resultados.size(); i++) {
        if(resultados.at(i).at(16) == "Universidad de Oriente") {
            resultadosDelCentro = true;
            break;
        }
    }
    if(resultadosDelCentro) {
        // emitiendo sennal para detener acciones del usuario hasta terminar la actualizacion
        emit nuevosResultadosDelCentro();
        // aqui se guardan los datos de los pacientes que no se pudieron localizar en base de datos
        QList<int> resultadosNoLocalizados;
        // obteniendo ubicacion de pacientes en base de datos se pacientes
        for(int i = 1; i < resultados.size(); i++) {
            DataBaseManager *dbm = new DataBaseManager(this, DataBaseManager::DBPacientes);
            Tabla consulta = dbm->consultaSQL("select bloque, cubiculo, rowid from Pacientes where nombre=\'" + resultados.at(i).at(3) + "\' and ci='" + resultados.at(i).at(6) + "\'");
            resultados[i].insert(4, QString());
            resultados[i].insert(5, QString());
            if(consulta.size() == 0) {
                resultadosNoLocalizados << i;
            }
            else {
                // completando informacion que falta (bloque, cubiculo)
                resultados[i][4] = consulta.at(0).at(0);
                resultados[i][5] = consulta.at(0).at(1);
            }
            QString marca = resultados.at(i).at(19);
            if(marca == "POS COVID-19")
                marca = "POSITIVO COVID-19";
            else if(marca == "NEG COVID-19")
                marca = "NEGATIVO COVID-19";
            if(!dbm->marcar(consulta.at(0).at(2), marca)){
                emit actualizacionFinalizada();
                return false;
            }
        }
        // hubieron pacientes no localizados (puede deberse a que se cambio el nombre o ci; tambien puede deberse a que
        // no son resultados de personas provenientes del centro o sea trabajadores)
        if(resultadosNoLocalizados.size()) {
            QString resultadosMsg = "";
            for(int i = 0; i < resultadosNoLocalizados.size(); i++) {
                resultadosMsg += "<h4>" + resultados[resultadosNoLocalizados[i]][3] + ", " + resultados[resultadosNoLocalizados[i]][8] + ", " + resultados[resultadosNoLocalizados[i]][9] + "</h4>";
            }
            resultadosMsg += "<br><br>";
            QMessageBox::information(dynamic_cast<QWidget*>(parent()), "Pacientes no localizados", QString("<h3>La siguiente lista muestra los datos de las personas\n"
                                                                                                           "de la ultima placa obtenida que no pudieron ser localizados\n"
                                                                                                           "en el centro.</h3>") + resultadosMsg + QString("Esto pudiera deberse a que no son pacientes del centro o \n"
                                                                                                                                                           "que sus datos fueron modificados despues de mandar la base\n"
                                                                                                                                                           "de datos de PCR."));
        }
    }
    // actualizando base de datos de pcr
    for(int i = 1; i < resultados.size(); i++) {
        DataBaseManager *dbm = new DataBaseManager(this, DataBaseManager::DBResultadosPCR);
        QString insertarFila = convertirSQL(resultados.at(i));
        if(!dbm->ejecutarSQL("insert into ResultadosPCR values(" + insertarFila + ")")) {
            emit actualizacionFinalizada();
            return false;
        }
    }
    emit actualizacionFinalizada();
    return true;
}

bool Actualizador::entradaValida(QString ruta) {
    Q_UNUSED(ruta);
    // por implementar
    return true;
}

QString Actualizador::convertirSQL(QStringList fila) {
    QString ret = "\'";
    ret += fila.join("\', \'");
    ret += "\'";
    return ret;
}
