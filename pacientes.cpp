#include "pacientes.h"

bool Pacientes::eventFilter(QObject *watched, QEvent *event) {
    Q_UNUSED(watched);
    // se detecto una tecla que fue liberada
    QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
    if(keyEvent) {
        // se presiono la tecla delete
        if(keyEvent->key() == Qt::Key_Delete && keyEvent->type() == QKeyEvent::KeyRelease) {
            // adquiriendo seleccion de las filas
            QItemSelectionModel *selected = tabla->selectionModel();
            if(selected->hasSelection()) {
                // adquiriendo filas seleccionadas completamente
                QModelIndexList listIndex = selected->selectedRows();
                // eliminar todas las filas seleccionadas
                // variable para llevar cuenta de filas borradas para realizar un corrimiento en
                // los indices de las filas que se borraran
                int corrimiento = 0;
                // generando conjunto de filas seleccionadas
                QList<int> filasSeleccionadas;
                for(auto it : listIndex) {
                    if(it.row() != 0)
                        filasSeleccionadas << it.row();
                }
                std::sort(filasSeleccionadas.begin(), filasSeleccionadas.end());
                dbm->beginTransaction();
                for(auto it : filasSeleccionadas) {
                    if(it - corrimiento > numRows)
                        continue;
                    if(!dbm->ejecutarSQL("delete from Pacientes where rowid="+tabla->item(it - corrimiento, tabla->columnCount()-1)->text())) {
                        QMessageBox::critical(parentWidget,"Error al eliminar fila","La base de datos no pudo ejecutar la operacion correctamente.");
                    }
                    else {
                        disconnect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
                        tabla->removeRow(it - corrimiento);
                        tabla->setVerticalHeaderLabels(verticalHeaderLabels);
                        connect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
                        numRows--;
                        corrimiento++;
                    }
                }
                dbm->endTransaction();
            }
            return true;
        }
        // se presiono el shortcut de una accion de marcar a un paciente CTRL+P, CTRL+N, CTRL+R
        if((keyEvent->text() == "\u0010" ||
           keyEvent->text() == "\u000E" ||
           keyEvent->text() == "\u0012") && keyEvent->type() == QKeyEvent::KeyRelease)
        {
            QString marca;
            if(keyEvent->text() == "\u0010")
                marca = "POSITIVO COVID-19";
            else if(keyEvent->text() == "\u000E")
                marca = "NEGATIVO COVID-19";
            else if(keyEvent->text() == "\u0012")
                marca = "REPETIR MUESTRA";
            int pcr1Index = tabla->columnCount()-7,
                    pcr2Index = tabla->columnCount()-5,
                    pcr3Index = tabla->columnCount()-3;
            int resultado1Index = tabla->columnCount()-6,
                    resultado2Index = tabla->columnCount()-4,
                    resultado3Index = tabla->columnCount()-2;

            QModelIndexList indexList = tabla->selectionModel()->selection().indexes();
            QSet<int> filasSeleccionadas;
            for(auto it : indexList) {
                if(it.row() != 0)
                    filasSeleccionadas << it.row();
            }
            for(auto it : filasSeleccionadas) {
                if(it <= numRows) {
                    // determinando donde colocar resultado (se coloca de acuerdo al resultado que se esta esperando)
                    if(tabla->item(it, pcr1Index) && (tabla->item(it, pcr2Index) == 0 || (tabla->item(it, pcr2Index) && tabla->item(it, pcr2Index)->text() == "")) && (tabla->item(it, pcr3Index) == 0 || (tabla->item(it, pcr3Index) && tabla->item(it, pcr3Index)->text() == ""))) {
                        // verificar que no hayan otros resultados posterior a este, si los hay, informar de error
                        if((tabla->item(it, resultado2Index) && tabla->item(it, resultado2Index)->text() != "") || (tabla->item(it, resultado3Index) && tabla->item(it, resultado3Index)->text() != "")) {
                            QMessageBox::critical(parentWidget, "Conflicto entre datos.", "El paciente de la fila "+QString::number(it+1)+" tiene un resultado registrado en una posicion no esperada");
                            return true;
                        }
                        else {
                            tabla->setCurrentItem(tabla->item(it, resultado1Index));
                            tabla->setItem(it, resultado1Index, new QTableWidgetItem(marca));
                            tabla->horizontalScrollBar()->setValue(tabla->horizontalScrollBar()->maximum());
                        }
                        continue;
                    }
                    else if(tabla->item(it, pcr2Index) && (tabla->item(it, pcr3Index) == 0 || (tabla->item(it, pcr3Index) && tabla->item(it, pcr3Index)->text() == ""))) {
                        // verificar que no hayan otros resultados posterior a este, si los hay, informar de error
                        if(tabla->item(it, resultado3Index) && tabla->item(it, resultado3Index)->text() != "") {
                            QMessageBox::critical(parentWidget, "Conflicto entre datos.", "El paciente de la fila "+QString::number(it+1)+" tiene un resultado registrado en una posicion no esperada");
                            return true;
                        }
                        else {
                            tabla->setCurrentItem(tabla->item(it, resultado2Index));
                            tabla->setItem(it, resultado2Index, new QTableWidgetItem(marca));
                            tabla->horizontalScrollBar()->setValue(tabla->horizontalScrollBar()->maximum());
                        }
                        continue;
                    }
                    else if(tabla->item(it, pcr3Index)) {
                        tabla->setCurrentItem(tabla->item(it, resultado3Index));
                        tabla->setItem(it, resultado3Index, new QTableWidgetItem(marca));
                        tabla->horizontalScrollBar()->setValue(tabla->horizontalScrollBar()->maximum());
                        continue;
                    }
                    else {
                        if((tabla->item(it, resultado2Index) && tabla->item(it, resultado2Index)->text() != "") || (tabla->item(it, resultado3Index) && tabla->item(it, resultado3Index)->text() != "")) {
                            QMessageBox::critical(parentWidget, "Conflicto entre datos.", "El paciente de la fila "+QString::number(it+1)+" tiene un resultado registrado en una posicion no esperada");
                            return true;
                        }
                        else {
                            tabla->setCurrentItem(tabla->item(it, resultado1Index));
                            tabla->setItem(it, resultado1Index, new QTableWidgetItem(marca));
                            tabla->horizontalScrollBar()->setValue(tabla->horizontalScrollBar()->maximum());
                        }
                    }
                }
            }
        }
        // agregar paciente a la lista de egresos o remisiones CTRL+E, CTRL+D
        if((keyEvent->text() == "\u0005" ||
           keyEvent->text() == "\u0004") && keyEvent->type() == QKeyEvent::KeyRelease) {
            QModelIndexList listIndex = tabla->selectionModel()->selection().indexes();
            // generando conjunto de filas seleccionadas
            QList<int> filasSeleccionadas;
            for(auto it : listIndex) {
                if(it.row() != 0)
                    filasSeleccionadas << it.row();
            }
            std::sort(filasSeleccionadas.begin(), filasSeleccionadas.end());
            // controlador de base de datos correspondiente
            DataBaseManager* dbMove;
            if(keyEvent->text() == "\u0005") {
                dbMove = new DataBaseManager(this, DataBaseManager::DBEgresos);
            }
            else if(keyEvent->text() == "\u0004") {
                dbMove = new DataBaseManager(this, DataBaseManager::DBRemisiones);
            }
            int controlador = dbMove->getControlador();
            for(auto it : filasSeleccionadas) {
                // preparando instruccion sql
                // se asume que la fecha de egreso o remision es la actual del sistema en cuanto se hace la operacion
                QString values = " values (\'" + QDate::currentDate().toString("dd-MM-yyyy") + "\',";
                QStringList rowValues = valoresFila(it);
                for(auto iter = rowValues.begin(); iter != rowValues.end(); iter++) {
                    if((*iter).isNull()) {
                        values += "\'\'";
                    }
                    else {
                        values += "\'" + *iter + "\'";
                    }
                    if(iter+1 != rowValues.end()) {
                        values += ", ";
                    }
                }
                // agregar a base de datos
                if(controlador == DataBaseManager::DBEgresos)
                    if(!dbMove->ejecutarSQL("insert into Pacientes('fechaEgreso', 'nombre', 'bloque', 'cubiculo', 'hta', 'dm', 'ab', 'il', 'direccion', 'edad', 'raza', 'sexo', 'ci', 'municipio', 'provincia', 'area', 'contacto', 'fi', 'fu', 'pcr1', 'resultado1', 'pcr2', 'resultado2', 'pcr3', 'resultado3', 'rowid')"+
                                            values+");")) {
                        QMessageBox::critical(parentWidget,"Error al insertar fila","La base de datos no pudo ejecutar la operacion correctamente.");
                    }
                if(controlador == DataBaseManager::DBRemisiones)
                    if(!dbMove->ejecutarSQL("insert into Pacientes('fechaRemision', 'nombre', 'bloque', 'cubiculo', 'hta', 'dm', 'ab', 'il', 'direccion', 'edad', 'raza', 'sexo', 'ci', 'municipio', 'provincia', 'area', 'contacto', 'fi', 'fu', 'pcr1', 'resultado1', 'pcr2', 'resultado2', 'pcr3', 'resultado3', 'rowid')"+
                                                 values+");")) {
                         QMessageBox::critical(parentWidget,"Error al insertar fila","La base de datos no pudo ejecutar la operacion correctamente.");
                    }
            }
            marcarFilas();
            actionEliminar();
        }
        // se desea copiar al portapapeles CTRL+C
        if(keyEvent->text() == "\u0003" && keyEvent->type() == QKeyEvent::KeyRelease) {
            QModelIndexList indexList = tabla->selectionModel()->selection().indexes();
            // almacenando coordenadas
            QList<QPair<int, int>> coordenadas;
            for(auto it : indexList) {
                if(it.row() <= numRows && it.row() > 0 && it.column() < tabla->columnCount()-1){
                    coordenadas << QPair<int, int>(it.row(), it.column());
                }
            }
            std::sort(coordenadas.begin(), coordenadas.end());
            int currentRow = -1;
            QString copyText;
            for(auto it : coordenadas) {
                if(copyText.isEmpty()) {
                    copyText.append(tabla->item(it.first, it.second)->text());
                    currentRow = it.first;
                    continue;
                }
                else if(it.first != currentRow) {
                    copyText.append('\n');
                }
                else {
                    copyText.append('\t');
                }
                copyText.append(tabla->item(it.first, it.second)->text());
                currentRow = it.first;
            }
            QApplication::clipboard()->setText(copyText);
        }
        // se desea pegar del portapapeles CTRL+V
        if( keyEvent->text() == "\u0016" && keyEvent->type() == QKeyEvent::KeyRelease) {
            QModelIndexList indexList = tabla->selectionModel()->selection().indexes();
            auto min = std::min(indexList.begin(), indexList.end());
            QPair<int, int> index = QPair<int, int>((*min).row(), (*min).column());
            int firstColumn = index.second;
            if(index.first > numRows) {
                index.first = numRows+1;
            }
            QStringList rows = QApplication::clipboard()->text().split("\n");
            dbm->beginTransaction();
            lastRowidBeforeCommit = dbm->maxRowId();
            for(auto i : rows) {
                QStringList values = i.split("\t");
                for(auto j : values) {
                    desactivarRenderizado = true;
                    tabla->setCurrentCell(index.first, index.second);
                    tabla->setItem(index.first, index.second, new QTableWidgetItem(j));
                    desactivarRenderizado = false;
                    index.second++;
                }
                index.second = firstColumn;
                index.first++;
            }
            lastRowidBeforeCommit = -1;
            dbm->endTransaction();
            // CARGAR TABLA PACIENTES DEL CENTRO
            cargarTabla("select *, rowid from Pacientes");
        }
        return false;
    }
    if(event->type() == QEvent::ContextMenu) {
        // casteando
        QContextMenuEvent* menuEvent = dynamic_cast<QContextMenuEvent*>(event);
        if(tabla->selectionModel()->selection().indexes().empty())
            return false;
        // marcar todas las filas de la seleccion
        marcarFilas();
        // mostrando menu
        menu->exec(menuEvent->globalPos());
    }
    return false;
}

Pacientes::Pacientes(QObject *parent, QTableWidget* view, DataBaseManager::Controlador cont) : QObject(parent)
{
    dbm = new DataBaseManager(this, cont);
    QMainWindow* widget = dynamic_cast<QMainWindow*>(parent);
    // casteo correcto
    if(widget != nullptr)
        parentWidget = widget;
    else
        return;
    // adquiriendo referencia de la tabla
    tabla = view;
    // instalando el event filter
    tabla->installEventFilter(this);
    // creando context menu
    crearMenu();
    // PREPARAR SECCION DE SEGMENTACION DE DATOS
    seccionSegmentacionDeDatos();
    // PREPARAR TABLA
    tabla->clearContents();
    // encabezados
    QStringList encabezados;
    if(cont == DataBaseManager::DBEgresos) {
        encabezados << "Fecha de Egreso";
        tabla->setColumnCount(26);
        // OCULTANDO ROWID DE LAS FILAS DE LA TABLA
        tabla->hideColumn(25);
        // MOSTRANDO ESTA COLUMNA QUE EN PACIENTES ESTABA OCULTA
        tabla->showColumn(24);
    }
    else if(cont == DataBaseManager::DBRemisiones) {
        encabezados << "Fecha de Remision";
        tabla->setColumnCount(26);
        // OCULTANDO ROWID DE LAS FILAS DE LA TABLA
        tabla->hideColumn(25);
        // MOSTRANDO ESTA COLUMNA QUE EN PACIENTES ESTABA OCULTA
        tabla->showColumn(24);
    }
    else if(cont == DataBaseManager::DBPacientes) {
        tabla->setColumnCount(25);
        // OCULTANDO ROWID DE LAS FILAS DE LA TABLA
        tabla->hideColumn(24);

    }
    encabezados << "Nombres y Apellidos"
                << "Bloque"
                << "Cubiculo"
                << "HTA"
                << "DM"
                << "AB"
                << "IL"
                << "Direccion"
                << "Edad"
                << "Raza"
                << "Sexo"
                << "CI"
                << "Municipio"
                << "Provincia"
                << "Area de salud"
                << "Contacto de"
                << "Fecha de ingreso"
                << "Fecha de ultimo contacto"
                << "Fecha 1ER PCR"
                << "Resultado 1ER PCR"
                << "Fecha 2DO PCR"
                << "Resultado 2DO PCR"
                << "Fecha 3ER PCR"
                << "Resultado 3ER PCR";
    tabla->setHorizontalHeaderLabels(encabezados);
    // CARGAR TABLA PACIENTES DEL CENTRO
    cargarTabla("select *, rowid from Pacientes");
    // conectando signals de la tabla con sus correspondientes slots
    connect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
    connect(tabla, SIGNAL(cellClicked(int,int)), this, SLOT(on_tabla_cellClicked(int,int)));
    // estableciendo criterios de segmentacion de datos a nulos
    criterios = new QString*[tabla->columnCount()-1];
    for(int i=0; i<tabla->columnCount()-1; i++) {
        criterios[i] = nullptr;
    }
    crearEncabezadoFiltrosInteractivos();
    tabla->resizeColumnsToContents();
    tabla->resizeRowsToContents();
    // encabezado de filtros interactivos
    // preparando vertical header
    verticalHeaderLabels << "";
    for(int i=1; i<=500; i++) {
        verticalHeaderLabels << QString::number(i);
    }
    tabla->setVerticalHeaderLabels(verticalHeaderLabels);
}

bool Pacientes::crearBaseDeDatos() {
    return dbm->ejecutarSQL("CREATE TABLE `Pacientes` ("
                            "`nombre`	TEXT,"
                            "`bloque`	TEXT,"
                            "`cubiculo`	TEXT,"
                            "`hta`	TEXT,"
                            "`dm`	TEXT,"
                            "`am`	TEXT,"
                            "`il`	TEXT,"
                            "`direccion`	TEXT,"
                            "`edad`	TEXT,"
                            "`raza`	TEXT,"
                            "`sexo`	TEXT,"
                            "`ci`	TEXT,"
                            "`municipio`	TEXT,"
                            "`provincia`	TEXT,"
                            "`area`	TEXT,"
                            "`contacto`	TEXT,"
                            "`fi`	TEXT,"
                            "`fu`	TEXT,"
                            "`pcr1`	TEXT,"
                            "`resultado1`	TEXT,"
                            "`pcr2`	TEXT,"
                            "`resultado2`	TEXT,"
                            "`pcr3`	TEXT,"
                            "`resultado3`	TEXT,"
                            "PRIMARY KEY(`ci`)");
}

void Pacientes::cargarTabla(QString sql) {
    disconnect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
    disconnect(tabla, SIGNAL(cellClicked(int,int)), this, SLOT(on_tabla_cellClicked(int,int)));
    Tabla resultadoConsulta = dbm->consultaSQL(sql);
    for(int i = 0; i < resultadoConsulta.size(); i++) {
        for(int j = 0; j < resultadoConsulta.at(i).size(); j++) {
            // adquiriendo item y asignandole algunos estilos
            QTableWidgetItem* item = new QTableWidgetItem(resultadoConsulta.at(i).at(j));
            item->setTextColor(QColor(BLANCO));
            tabla->setItem(i + 1, j, item);
        }
    }
    numRows = resultadoConsulta.size();
    // Estableciendo colores de la tabla
    darColor();
    // reconectando signals
    connect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
    connect(tabla, SIGNAL(cellClicked(int,int)), this, SLOT(on_tabla_cellClicked(int,int)));
}

void Pacientes::darColor(int row, int column) {
    // colorear toda la tabla
    if(row == -1 && column == -1) {
        for(int i=1; i <= numRows; i++) {
            // dar color a la fila
            darColor(i, -1);
        }
    }
    // colorear fila
    else if(row != -1 && column == -1) {
        int pcr2Index = tabla->columnCount()-5,
            pcr3Index = tabla->columnCount()-3;
        int resultado1Index = tabla->columnCount()-6,
            resultado2Index = tabla->columnCount()-4,
            resultado3Index = tabla->columnCount()-2;
        // determinando color de la fila
        QColor* color = new QColor(GRIS);
        if(tabla->item(row, resultado1Index) || tabla->item(row, resultado2Index) || tabla->item(row, resultado3Index)) {
            if(tabla->item(row, resultado1Index) && tabla->item(row, resultado1Index)->text() == "POSITIVO COVID-19" && (tabla->item(row, pcr2Index) == 0 || (tabla->item(row, pcr2Index) && tabla->item(row, pcr2Index)->text() == "")) && (tabla->item(row, resultado2Index) == 0 || (tabla->item(row, resultado2Index) && tabla->item(row, resultado2Index)->text() == "")) && (tabla->item(row, pcr3Index) == 0 || (tabla->item(row, pcr3Index) && tabla->item(row, pcr3Index)->text() == "")) && (tabla->item(row, resultado3Index) == 0 || (tabla->item(row, resultado3Index) && tabla->item(row, resultado3Index)->text() == ""))) {
                color = new QColor(ROJO);
            }
            else if(tabla->item(row, resultado1Index) && tabla->item(row, resultado1Index)->text() == "NEGATIVO COVID-19" && (tabla->item(row, pcr2Index) == 0 || (tabla->item(row, pcr2Index) && tabla->item(row, pcr2Index)->text() == "")) && (tabla->item(row, resultado2Index) == 0 || (tabla->item(row, resultado2Index) && tabla->item(row, resultado2Index)->text() == "")) && (tabla->item(row, pcr3Index) == 0 || (tabla->item(row, pcr3Index) && tabla->item(row, pcr3Index)->text() == "")) && (tabla->item(row, resultado3Index) == 0 || (tabla->item(row, resultado3Index) && tabla->item(row, resultado3Index)->text() == ""))) {
                color = new QColor(VERDE);
            }
            else if(tabla->item(row, resultado1Index) && tabla->item(row, resultado1Index)->text() == "REPETIR MUESTRA" && (tabla->item(row, pcr2Index) == 0 || (tabla->item(row, pcr2Index) && tabla->item(row, pcr2Index)->text() == "")) && (tabla->item(row, resultado2Index) == 0 || (tabla->item(row, resultado2Index) && tabla->item(row, resultado2Index)->text() == "")) && (tabla->item(row, pcr3Index) == 0 || (tabla->item(row, pcr3Index) && tabla->item(row, pcr3Index)->text() == "")) && (tabla->item(row, resultado3Index) == 0 || (tabla->item(row, resultado3Index) && tabla->item(row, resultado3Index)->text() == ""))) {
                color = new QColor(AZUL);
            }
            if(tabla->item(row, resultado2Index) && tabla->item(row, resultado2Index)->text() == "POSITIVO COVID-19" && (tabla->item(row, pcr3Index) == 0 || (tabla->item(row, pcr3Index) && tabla->item(row, pcr3Index)->text() == "")) && (tabla->item(row, resultado3Index) == 0 || (tabla->item(row, resultado3Index) && tabla->item(row, resultado3Index)->text() == ""))) {
                color = new QColor(ROJO);
            }
            else if(tabla->item(row, resultado2Index) && tabla->item(row, resultado2Index)->text() == "NEGATIVO COVID-19" && (tabla->item(row, pcr3Index) == 0 || (tabla->item(row, pcr3Index) && tabla->item(row, pcr3Index)->text() == "")) && (tabla->item(row, resultado3Index) == 0 || (tabla->item(row, resultado3Index) && tabla->item(row, resultado3Index)->text() == ""))) {
                color = new QColor(VERDE);
            }
            else if(tabla->item(row, resultado2Index) && tabla->item(row, resultado2Index)->text() == "REPETIR MUESTRA" && (tabla->item(row, pcr3Index) == 0 || (tabla->item(row, pcr3Index) && tabla->item(row, pcr3Index)->text() == "")) && (tabla->item(row, resultado3Index) == 0 || (tabla->item(row, resultado3Index) && tabla->item(row, resultado3Index)->text() == ""))) {
                color = new QColor(AZUL);
            }
            if(tabla->item(row, resultado3Index) && tabla->item(row, resultado3Index)->text() == "POSITIVO COVID-19") {
                color = new QColor(ROJO);
            }
            else if(tabla->item(row, resultado3Index) && tabla->item(row, resultado3Index)->text() == "NEGATIVO COVID-19") {
                color = new QColor(VERDE);
            }
            else if(tabla->item(row, resultado3Index) && tabla->item(row, resultado3Index)->text() == "REPETIR MUESTRA") {
                color = new QColor(AZUL);
            }
        }
        disconnect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
        for(int i=0; i<tabla->columnCount()-1; i++) {
            if(tabla->item(row, i))
                tabla->item(row, i)->setBackgroundColor(*color);
            else {
                QTableWidgetItem* emptyItem = new QTableWidgetItem();
                emptyItem->setBackgroundColor(*color);
                tabla->setItem(row, i, emptyItem);
            }
            tabla->item(row, i)->setTextColor(QColor(BLANCO));
        }
        connect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
    }
}

QString Pacientes::getRow(int index) {
   if(dbm->getControlador() == DataBaseManager::DBPacientes) {
        switch (index) {
        case 0 : return QString("nombre");
        case 1 : return QString("bloque");
        case 2 : return QString("cubiculo");
        case 3 : return QString("hta");
        case 4 : return QString("dm");
        case 5 : return QString("ab");
        case 6 : return QString("il");
        case 7 : return QString("direccion");
        case 8 : return QString("edad");
        case 9 : return QString("raza");
        case 10 : return QString("sexo");
        case 11 : return QString("ci");
        case 12 : return QString("municipio");
        case 13 : return QString("provincia");
        case 14 : return QString("area");
        case 15 : return QString("contacto");
        case 16 : return QString("fi");
        case 17 : return QString("fu");
        case 18 : return QString("pcr1");
        case 19 : return QString("resultado1");
        case 20 : return QString("pcr2");
        case 21 : return QString("resultado2");
        case 22 : return QString("pcr3");
        case 23 : return QString("resultado3");
        }
    }
    if(dbm->getControlador() == DataBaseManager::DBEgresos || dbm->getControlador() == DataBaseManager::DBRemisiones) {
        if(index == 0 && dbm->getControlador() == DataBaseManager::DBEgresos) {
            return QString("fechaEgreso");
        }
        else if(index == 0 && dbm->getControlador() == DataBaseManager::DBRemisiones) {
            return QString("fechaRemision");
        }
        switch (index) {
        case 1 : return QString("nombre");
        case 2 : return QString("bloque");
        case 3 : return QString("cubiculo");
        case 4 : return QString("hta");
        case 5 : return QString("dm");
        case 6 : return QString("ab");
        case 7 : return QString("il");
        case 8 : return QString("direccion");
        case 9 : return QString("edad");
        case 10 : return QString("raza");
        case 11 : return QString("sexo");
        case 12 : return QString("ci");
        case 13 : return QString("municipio");
        case 14 : return QString("provincia");
        case 15 : return QString("area");
        case 16 : return QString("contacto");
        case 17 : return QString("fi");
        case 18 : return QString("fu");
        case 19 : return QString("pcr1");
        case 20 : return QString("resultado1");
        case 21 : return QString("pcr2");
        case 22 : return QString("resultado2");
        case 23 : return QString("pcr3");
        case 24 : return QString("resultado3");
        }
    }
    return QString("");
}

QStringList Pacientes::valoresFila(int fila) {
    QStringList valores;
    for(int i = 0; i < tabla->columnCount(); i++) {
        if(tabla->item(fila, i)) {
            valores << tabla->item(fila, i)->text();
        }
        else {
            valores << QString();
        }
    }
    return valores;
}

int Pacientes::getNotNull(int row) {
    int r{0};
    for(int i{0}; i < tabla->columnCount()-1; i++)
        if(tabla->item(row, i) && !tabla->item(row, i)->text().isEmpty())
            r++;
    return r;
}

void Pacientes::filtrar() {
    QString sql = "select *, rowid from Pacientes ";
    bool algunCriterio = false;
    QStringList sqlCriterios;
    for(int i=0; i<tabla->columnCount()-1; i++) {
        if(criterios[i] != nullptr) {
            algunCriterio = true;
            // para garantizar que la busqueda en este campo sea solo a las cadenas que comiencen por...
            if(i != 2) {
                sqlCriterios << getRow(i) + " like '%" + *criterios[i] + "%'";
            }
            else {
                sqlCriterios << getRow(i) + " like '%" + *criterios[i] + "'";
            }
        }
    }
    if(algunCriterio || textoSegmentacionResultado || textoSegmentacionFechaPCR)
        sql += " where ";
    sql += sqlCriterios.join(" and ");
    // se esta usando el filtro de todos los pacientes que estan marcados con un resultado, el mismo criterio
    // para colorear las filas
    if(textoSegmentacionResultado) {
        if(algunCriterio)
            sql += " and ";
        sql += " ((resultado1='"+*textoSegmentacionResultado+"' and (pcr2 is null or (pcr2 not null and pcr2=''))) or (resultado2='"+*textoSegmentacionResultado+"' and (pcr3 is null or (pcr3 not null and pcr3=''))) or resultado3='"+*textoSegmentacionResultado+"')";
    }
    // se esta usando el filtro de todos los pacientes que tienen el ultimo pcr planificado para cierto dia
    if(textoSegmentacionFechaPCR) {
        if(algunCriterio)
            sql += " and ";
        sql += " ((pcr1='"+*textoSegmentacionFechaPCR+"' and ((pcr2 is null or (pcr2 is not null and pcr2='')) and (pcr3 is null or (pcr3 is not null and pcr3='')))) or (pcr2='"+*textoSegmentacionFechaPCR+"' and (pcr3 is null or (pcr3 is not null and pcr3=''))) or (pcr3='"+*textoSegmentacionFechaPCR+"'))";
    }
    sql += ";";
    tabla->clearContents();
    crearEncabezadoFiltrosInteractivos();
    cargarTabla(sql);
}

void Pacientes::marcarFilas() {
    // modificando seleccion para que parezca que se seleccionaron todas las columnas
    tabla->setSelectionMode(QAbstractItemView::MultiSelection);
    auto indexList = tabla->selectionModel()->selection().indexes();
    // generando conjunto de filas seleccionadas
    QSet<int> filasSeleccionadas;
    for(auto it : indexList) {
        filasSeleccionadas << it.row();
    }
    // marcando las filas seleccionadas
    for(auto it : filasSeleccionadas) {
        if(it <= numRows) {
            tabla->selectRow(it);
        }
    }
    tabla->setSelectionMode(QAbstractItemView::ExtendedSelection);
}

void Pacientes::deshacer() {
    dbm->undo();
    // CARGAR TABLA PACIENTES DEL CENTRO
    cargarTabla("select *, rowid from Pacientes");
}

Pacientes::~Pacientes() {
    delete bloqueButton, delete cubiculoButton, delete resultadoButton, delete fechaPCRButton;
    delete limpiarBloque, delete limpiarCubiculo, delete limpiarResultado, delete limpiarFechaPCR;
    delete bloqueLabel, delete cubiculoLabel, delete resultadoLabel, delete fechaPCRLabel;
    delete dbm;
    for(int i=0; i < tabla->columnCount()-1; i++) {
        delete criterios[i];
    }
    delete criterios;
    if(frameBloque)
        delete frameBloque;
    if(frameCubiculo)
        delete frameCubiculo;
    if(frameFechaPCR)
        delete frameFechaPCR;
    if(frameResultado)
        delete frameResultado;
    if(textoSegmentacionResultado)
        delete textoSegmentacionResultado;
}


void Pacientes::on_tabla_cellChanged(int row, int column)
{
//    qDebug() << "cell [" << row << ", " << column << "] changed, number of actual rows in table: " << numRows;
    // si la fila modificada es mayor a la cantidad de pacientes, el evento corresponde al
    // registro de un nuevo paciente, si no, se esta modificando un paciente existente
    if(row > numRows) {
        // ingresando dato a la base de datos
        // si no se ingreso nada
        QString newText;
        if(tabla->currentItem()) {
             newText = tabla->currentItem()->text();
        }
        if(newText.isEmpty())
            return ;
        if(!dbm->ejecutarSQL("insert into Pacientes('" + getRow(column) + "') values('" + newText + "');")) return; // error fatal
        int rowidInserted = dbm->maxRowId();
        if(rowidInserted == -1) return; // error fatal
        if(desactivarRenderizado) {
            lastRowidBeforeCommit++;
            numRows++;
            disconnect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
            tabla->setItem(numRows, tabla->columnCount()-1, new QTableWidgetItem(QString::number(lastRowidBeforeCommit)));
            connect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
            return;
        }
        // desconectando sennal momentaneamente para no rellamar este slot
        disconnect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
        tabla->currentItem()->setText("");
        tabla->setItem(++numRows, column, new QTableWidgetItem(newText));
        tabla->item(numRows, column)->setTextColor(QColor(BLANCO));
        tabla->setItem(numRows, tabla->columnCount()-1, new QTableWidgetItem(QString::number(rowidInserted)));
        // Estableciendo color de la fila
        darColor(numRows, -1);
        connect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));

    }
    // se esta modificando un paciente ya registrado
    else {
        // si no se hizo ninguna modificacion
        QString newText = tabla->currentItem()->text();
        int camposNoVacios = getNotNull(row);
        // se esta eliminando el unico campo no vacio de una fila, por lo que se borra
        if(camposNoVacios == 0 && newText.isEmpty()) {
            if(!dbm->ejecutarSQL("delete from Pacientes where rowid=" + tabla->item(row, tabla->columnCount()-1)->text())) return; // error fatal
            // Estableciendo color de la fila
            tabla->removeRow(row);
            tabla->setVerticalHeaderLabels(verticalHeaderLabels);
            numRows--;
        }
        // actualizar
        else {
            if(!dbm->ejecutarSQL("update Pacientes set " + getRow(column) + "='" + newText + "' where rowid=\'" + tabla->item(row, tabla->columnCount()-1)->text() + "\'; ")) return; // error fatal
            if(desactivarRenderizado) return;
            // Estableciendo color de la fila
            darColor(row, -1);
        }
    }
}

void Pacientes::on_tabla_cellClicked(int row, int column) {
    if(tabla->item(row, column))
        currentText = tabla->item(row, column)->text();
    else
        currentText = "";
}

void Pacientes::on_bloqueButton_clicked() {

    if(frameBloque == nullptr) {
        QSet<QString> bloquesSet = dbm->valoresColumna("bloque");
        // generando lista de bloques
        QStringList bloques;
        for(auto it : bloquesSet) {
            bloques << it;
        }
        // ordenando lista
        std::sort(bloques.begin(), bloques.end());

        frameBloque = new QFrame(parentWidget);
        QVBoxLayout* container = new QVBoxLayout();

        QHBoxLayout* layout = new QHBoxLayout();
        container->addLayout(layout);
        int i = 0, filas = 1;
        for(auto it = bloques.constBegin(); it != bloques.constEnd(); it++) {
            // cada tres iteraciones realizar esta accion
            if(i!=0 && i%4==0) {
                layout = new QHBoxLayout();
                container->addLayout(layout);
                filas++;
            }
            QPushButton* button = new QPushButton(*it);
            connect(button, SIGNAL(clicked()), this, SLOT(opcionSegmentacionBloqueSeleccionada()));
            layout->addWidget(button);
            i++;
        }

        frameBloque->setLayout(container);

        frameBloque->setGeometry(bloqueButton->x(), bloqueButton->y() + 50, bloqueButton->width(), filas*50);
        frameBloque->setStyleSheet("background-color : #e2e2e2;");
        frameBloque->show();
    }
    else {
        frameBloque->hide();
        delete frameBloque;
        frameBloque = nullptr;
    }
}

void Pacientes::on_cubiculoButton_clicked() {
    if(frameCubiculo == nullptr) {
        QSet<QString> cubiculosSet = dbm->valoresColumna("cubiculo");
        // generando lista de cubiculos
        QStringList cubiculos;
        for(auto it : cubiculosSet) {
            cubiculos << it;
        }
        // ordenando lista
        std::sort(cubiculos.begin(), cubiculos.end());
        frameCubiculo = new QFrame(parentWidget);
        QVBoxLayout* container = new QVBoxLayout();

        QHBoxLayout* layout = new QHBoxLayout();
        container->addLayout(layout);
        int i = 0, filas = 1;
        for(auto it = cubiculos.constBegin(); it != cubiculos.constEnd(); it++) {
            // cada tres iteraciones realizar esta accion
            if(i!=0 && i%6==0) {
                layout = new QHBoxLayout();
                container->addLayout(layout);
                filas++;
            }
            QPushButton* button = new QPushButton(*it);
            connect(button, SIGNAL(clicked()), this, SLOT(opcionSegmentacionCubiculoSeleccionada()));
            layout->addWidget(button);
            i++;
        }

        frameCubiculo->setLayout(container);

        frameCubiculo->setGeometry(cubiculoButton->x(), cubiculoButton->y() + 50, cubiculoButton->width(), filas*50);
        frameCubiculo->setStyleSheet("background-color : #e2e2e2;");
        frameCubiculo->show();
    }
    else {
        frameCubiculo->hide();
        delete frameCubiculo;
        frameCubiculo = nullptr;
    }
}

void Pacientes::on_resultadoButton_clicked() {
    if(frameResultado == nullptr) {
        frameResultado = new QFrame(parentWidget);
        QVBoxLayout* container = new QVBoxLayout();

        QPushButton* resultado1 = new QPushButton("POSITIVO COVID-19");
        QPushButton* resultado2 = new QPushButton("NEGATIVO COVID-19");
        QPushButton* resultado3 = new QPushButton("REPETIR MUESTRA");

        connect(resultado1, SIGNAL(clicked()), this, SLOT(opcionSegmentacionResultadoSeleccionada()));
        connect(resultado2, SIGNAL(clicked()), this, SLOT(opcionSegmentacionResultadoSeleccionada()));
        connect(resultado3, SIGNAL(clicked()), this, SLOT(opcionSegmentacionResultadoSeleccionada()));

        container->addWidget(resultado1);
        container->addWidget(resultado2);
        container->addWidget(resultado3);

        frameResultado->setLayout(container);

        frameResultado->setGeometry(resultadoButton->x(),resultadoButton->y() + 50, resultadoButton->width(), 150);
        frameResultado->setStyleSheet("background-color : #e2e2e2;");
        frameResultado->show();
    }
    else {
        frameResultado->hide();
        delete frameResultado;
        frameResultado = nullptr;
    }
}

void Pacientes::on_fechaPCRButton_clicked() {
    if(frameFechaPCR == nullptr) {
        // todas las fechas distintas de pcr1, pcr2 y pcr3
        QSet<QString> fechasPCRSet;
        for(int i = 1; i <= 3; i++) {
            QSet<QString> fechas = dbm->valoresColumna("pcr"+QString::number(i));
            for(auto it : fechas)
                fechasPCRSet << it;
        }
        QStringList fechasPCR;
        for(auto it : fechasPCRSet)
            fechasPCR << it;
        // ordenando lista
        std::sort(fechasPCR.begin(), fechasPCR.end());
        frameFechaPCR = new QFrame(parentWidget);
        QVBoxLayout* container = new QVBoxLayout();

        for(auto it : fechasPCR) {
            if(it != "") {
                QPushButton* fecha = new QPushButton(it);
                connect(fecha, SIGNAL(clicked()), this, SLOT(opcionSegmentacionFechaPCRSeleccionada()));
                container->addWidget(fecha);
            }
        }

        frameFechaPCR->setLayout(container);

        frameFechaPCR->setGeometry(fechaPCRButton->x(), fechaPCRButton->y() + 50, fechaPCRButton->width(), fechasPCR.size()*50);
        frameFechaPCR->setStyleSheet("background-color : #e2e2e2;");
        frameFechaPCR->show();
    }
    else {
        frameFechaPCR->hide();
        delete frameFechaPCR;
        frameFechaPCR = nullptr;
    }
}

void Pacientes::opcionSegmentacionBloqueSeleccionada() {
    QPushButton* boton = (QPushButton*)sender();
    criterios[1] = new QString(boton->text());
    filtrar();
    // ocultando cuadro de seleccion de opcion de segmentacion
    frameBloque->hide();
    delete frameBloque;
    frameBloque = nullptr;
    // Actualizando label indicador de la segmentacion que se esta usando
    bloqueLabel->setText(*criterios[1]);
}

void Pacientes::opcionSegmentacionCubiculoSeleccionada() {
    QPushButton* boton = (QPushButton*)sender();
    criterios[2] = new QString(boton->text());
    filtrar();
    // ocultando cuadro de seleccion de opcion de segmentacion
    frameCubiculo->hide();
    delete frameCubiculo;
    frameCubiculo = nullptr;
    // Actualizando label indicador de la segmentacion que se esta usando
    cubiculoLabel->setText(boton->text());
}

void Pacientes::opcionSegmentacionResultadoSeleccionada() {  
    textoSegmentacionResultado = new QString(((QPushButton*)sender())->text());
    filtrar();
    // ocultando cuadro de seleccion de opcion de segmentacion
    frameResultado->hide();
    delete frameResultado;
    frameResultado = nullptr;
    // Actualizando label indicador de la segmentacion que se esta usando
    resultadoLabel->setText(*textoSegmentacionResultado);
}

void Pacientes::opcionSegmentacionFechaPCRSeleccionada() {
    textoSegmentacionFechaPCR = new QString(((QPushButton*)sender())->text());
    filtrar();
    // ocultando cuadro de seleccion de opcion de segmentacion
    frameFechaPCR->hide();
    delete frameFechaPCR;
    frameFechaPCR = nullptr;
    // Actualizando label indicador de la segmentacion que se esta usando
    fechaPCRLabel->setText(*textoSegmentacionFechaPCR);
}

void Pacientes::limpiarSegmentacionBloque() {
    delete criterios[1], criterios[1] = nullptr;
    bloqueLabel->clear();
    filtrar();
}

void Pacientes::limpiarSegmentacionCubiculo() {
    delete criterios[2], criterios[2] = nullptr;
    cubiculoLabel->clear();
    filtrar();
}

void Pacientes::limpiarSegmentacionResultado() {
    delete textoSegmentacionResultado, textoSegmentacionResultado = nullptr;
    filtrar();
    resultadoLabel->clear();
}

void Pacientes::limpiarSegmentacionFechaPCR() {
    delete textoSegmentacionFechaPCR, textoSegmentacionFechaPCR = nullptr;
    filtrar();
    fechaPCRLabel->clear();
}

void Pacientes::actionMarcar() {
    QString marca = ((QAction*)sender())->text();
    if(marca == "POSITIVO COVID-19") {
        QKeyEvent* keyEvent = new QKeyEvent(QEvent::KeyRelease, 0, Qt::ControlModifier, "\u0010");
        eventFilter(nullptr, keyEvent);
    }
    else if(marca == "NEGATIVO COVID-19"){
        QKeyEvent* keyEvent = new QKeyEvent(QEvent::KeyRelease, 0, Qt::ControlModifier, "\u000E");
        eventFilter(nullptr, keyEvent);
    }
    else if(marca == "REPETIR MUESTRA"){
        QKeyEvent* keyEvent = new QKeyEvent(QEvent::KeyRelease, 0, Qt::ControlModifier, "\u0012");
        eventFilter(nullptr, keyEvent);
    }
}

void Pacientes::actionAgregarListaRemision() {

}

void Pacientes::actionAgregarListaEgresos() {

}

void Pacientes::actionEliminar() {
    QKeyEvent* event = new QKeyEvent(QEvent::KeyRelease, Qt::Key_Delete, Qt::NoModifier);
    eventFilter(nullptr, event);
}

void Pacientes::filtroInteractivo(QString text) {
    QWidget* widget = (QWidget*)sender();
    // localizando que filtro es
    int index = 0;
    for(int i=0; i < tabla->columnCount()-1; i++) {
        if(tabla->cellWidget(0, i) == widget) {
            index = i;
            break;
        }
    }
    if(text != "")
        criterios[index] = new QString(text);
    else {
        delete criterios[index];
        criterios[index] = nullptr;
    }
    filtroActivo = index;
    scrollBarValue = tabla->horizontalScrollBar()->value();
    filtrar();
}

void Pacientes::crearMenu() {
    menu = new QMenu();
    // estas acciones estaran disponibles solo en la base de datos de pacientes
    if(dbm->getControlador() == DataBaseManager::DBPacientes) {
        QAction* actionPositivo = new QAction("POSITIVO COVID-19");
        QAction* actionNegativo = new QAction("NEGATIVO COVID-19");
        QAction* actionRepetirMuestra = new QAction("REPETIR MUESTRA");
        QAction* actionAgregarListaRemision = new QAction("AGREGAR A LISTA DE REMISIONES");
        QAction* actionAgregarListaEgresos = new QAction("AGREGAR A LISTA DE EGRESOS");
        menu->addAction(actionPositivo);
        menu->addAction(actionNegativo);
        menu->addAction(actionRepetirMuestra);
        menu->addSeparator();
        menu->addAction(actionAgregarListaRemision);
        menu->addAction(actionAgregarListaEgresos);
        menu->addSeparator();
        connect(actionPositivo, SIGNAL(triggered()), this, SLOT(actionMarcar()));
        connect(actionNegativo, SIGNAL(triggered()), this, SLOT(actionMarcar()));
        connect(actionRepetirMuestra, SIGNAL(triggered()), this, SLOT(actionMarcar()));
        connect(actionAgregarListaRemision, SIGNAL(triggered()), this, SLOT(actionAgregarListaRemision()));
        connect(actionAgregarListaEgresos, SIGNAL(triggered()), this, SLOT(actionAgregarListaEgresos()));
    }
    // acciones predeterminadas
    QAction* actionEliminar = new QAction("ELIMINAR");
    menu->addAction(actionEliminar);
    // conectando slots de estas actions
    connect(actionEliminar, SIGNAL(triggered()), this, SLOT(actionEliminar()));
}

void Pacientes::crearEncabezadoFiltrosInteractivos() {
    // PREPARANDO FILTROS INTERACTIVOS DE LA TABLA
    QLineEdit* filtrosInteractivos[tabla->columnCount()-1];
    for(int i=0; i < tabla->columnCount()-1; i++) {
        if(criterios[i] != nullptr)
            filtrosInteractivos[i] = new QLineEdit(*criterios[i]);
        else
            filtrosInteractivos[i] = new QLineEdit();
        filtrosInteractivos[i]->setPlaceholderText("Filtro");
        tabla->setCellWidget(0, i, filtrosInteractivos[i]);
        connect(filtrosInteractivos[i], SIGNAL(textEdited(QString)), SLOT(filtroInteractivo(QString)));
    }
    filtrosInteractivos[filtroActivo]->setFocus();
    tabla->horizontalScrollBar()->setValue(scrollBarValue);
}

void Pacientes::seccionSegmentacionDeDatos() {
    // PREPARAR SECCION DE SEGMENTACION DE DATOS
    bloqueButton = new QPushButton("Bloque");
    cubiculoButton = new QPushButton("Cubiculo");
    resultadoButton = new QPushButton("Resultado");
    fechaPCRButton = new QPushButton("Fecha PCR");

    limpiarBloque = new QPushButton("x");
    limpiarCubiculo = new QPushButton("x");
    limpiarResultado = new QPushButton("x");
    limpiarFechaPCR = new QPushButton("x");

    bloqueLabel = new QLabel("");
    cubiculoLabel = new QLabel("");
    resultadoLabel = new QLabel("");
    fechaPCRLabel = new QLabel("");

    QVBoxLayout* bloquelayOut = new QVBoxLayout();
    QVBoxLayout* cubiculolayOut = new QVBoxLayout();
    QVBoxLayout* resultadolayOut = new QVBoxLayout();
    QVBoxLayout* fechaPCRlayOut = new QVBoxLayout();

    QHBoxLayout* indicadorFiltroBloque = new QHBoxLayout();
    QHBoxLayout* indicadorFiltroCubiculo = new QHBoxLayout();
    QHBoxLayout* indicadorFiltroResultado = new QHBoxLayout();
    QHBoxLayout* indicadorFiltroFechaPCR = new QHBoxLayout();

    indicadorFiltroBloque->addWidget(bloqueLabel);
    indicadorFiltroBloque->addWidget(limpiarBloque);
    indicadorFiltroCubiculo->addWidget(cubiculoLabel);
    indicadorFiltroCubiculo->addWidget(limpiarCubiculo);
    indicadorFiltroResultado->addWidget(resultadoLabel);
    indicadorFiltroResultado->addWidget(limpiarResultado);
    indicadorFiltroFechaPCR->addWidget(fechaPCRLabel);
    indicadorFiltroFechaPCR->addWidget(limpiarFechaPCR);

    bloquelayOut->addWidget(bloqueButton);
    bloquelayOut->addLayout(indicadorFiltroBloque);

    cubiculolayOut->addWidget(cubiculoButton);
    cubiculolayOut->addLayout(indicadorFiltroCubiculo);

    resultadolayOut->addWidget(resultadoButton);
    resultadolayOut->addLayout(indicadorFiltroResultado);

    fechaPCRlayOut->addWidget(fechaPCRButton);
    fechaPCRlayOut->addLayout(indicadorFiltroFechaPCR);

    QWidget* widget = dynamic_cast<QWidget*>(parentWidget->centralWidget()->children().at(1));
    QHBoxLayout* barraHerramientas1 = dynamic_cast<QHBoxLayout*>(widget->children().at(0));
    if(barraHerramientas1) {
        barraHerramientas1->addLayout(bloquelayOut);
        barraHerramientas1->addLayout(cubiculolayOut);
        barraHerramientas1->addLayout(resultadolayOut);
        barraHerramientas1->addLayout(fechaPCRlayOut);
    }
    // CONECTANDO BOTONES PARA DESPLEGAR OPCIONES DE SEGMENTACION
    connect(bloqueButton, SIGNAL(clicked()), this, SLOT(on_bloqueButton_clicked()));
    connect(cubiculoButton, SIGNAL(clicked()), this, SLOT(on_cubiculoButton_clicked()));
    connect(resultadoButton, SIGNAL(clicked()), this, SLOT(on_resultadoButton_clicked()));
    connect(fechaPCRButton, SIGNAL(clicked()), this, SLOT(on_fechaPCRButton_clicked()));
    // CONECTANDO BOTONES PARA ELIMINAR FILTROS
    connect(limpiarBloque, SIGNAL(clicked()), this, SLOT(limpiarSegmentacionBloque()));
    connect(limpiarCubiculo, SIGNAL(clicked()), this, SLOT(limpiarSegmentacionCubiculo()));
    connect(limpiarResultado, SIGNAL(clicked()), this, SLOT(limpiarSegmentacionResultado()));
    connect(limpiarFechaPCR, SIGNAL(clicked()), this, SLOT(limpiarSegmentacionFechaPCR()));
}

