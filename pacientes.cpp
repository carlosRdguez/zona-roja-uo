#include "pacientes.h"

bool Pacientes::eventFilter(QObject *watched, QEvent *event) {
    Q_UNUSED(watched);
    // se detecto una tecla que fue liberada
    if(event->type() == QEvent::KeyRelease) {
        QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
        if(keyEvent) {
            // se presiono la tecla delete
            if(keyEvent->key() == Qt::Key_Delete) {
                // adquiriendo seleccion de las filas
                QItemSelectionModel *selected = tabla->selectionModel();
                if(selected->hasSelection()) {
                    // adquiriendo filas seleccionadas completamente
                    QModelIndexList listIndex = selected->selectedRows();
                    // eliminar todas las filas seleccionadas
                    // alcance de db
                    {
                        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
                        // aqui se le dice la direccion de la base de datos a crear o cargar
                        db.setDatabaseName(rutaPacientes);
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
                        for(auto it : filasSeleccionadas) {
                            if(it - corrimiento > numRows)
                                continue;
                            QSqlQuery query;
                            if(db.open())
                            {
                                // eliminando fila de la base de datos
                                if(!(query.exec("delete from Pacientes where rowid="+tabla->item(it - corrimiento, 24)->text()))) {
                                    QMessageBox::critical(parentWidget,"Error al eliminar fila","La base de datos no pudo ejecutar la operacion correctamente.");
                                   // return true;
                                }
                                // eliminando fila de la tabla
                                disconnect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
                                tabla->removeRow(it - corrimiento);
                                connect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
                                numRows--;
                                corrimiento++;
                            }
                        }

                    }
                    QSqlDatabase::removeDatabase("qt_sql_default_connection");
                    // cada vez que se elimina una fila se descuenta de la tabla, restableciendo tamanno de la tabla
                    tabla->setRowCount(500);
                }
                else {
                    qDebug() << "no hay seleccion";
                }
            }
            return true;
        }
    }
    else if(event->type() == QEvent::ContextMenu) {
        // casteando
        QContextMenuEvent* menuEvent = dynamic_cast<QContextMenuEvent*>(event);
        if(tabla->selectionModel()->selection().indexes().empty())
            return false;
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
        // mostrando menu
        menu->exec(menuEvent->globalPos());
    }
    return false;
}

Pacientes::Pacientes(QObject *parent, QTableWidget* view) : QObject(parent)
{
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

    QHBoxLayout* barraHerramientas1 = dynamic_cast<QHBoxLayout*>(parentWidget->centralWidget()->layout()->children().at(0));
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
    // PREPARAR TABLA
    tabla->clearContents();
    // CARGAR TABLA PACIENTES DEL CENTRO
    cargarTabla("select *, rowid from Pacientes");
    // conectando signals de la tabla con sus correspondientes slots
    connect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
    connect(tabla, SIGNAL(cellClicked(int,int)), this, SLOT(on_tabla_cellClicked(int,int)));
    // estableciendo criterios de segmentacion de datos a nulos
    for(int i=0; i<24; i++) {
        criterios[i] = nullptr;
    }
    crearEncabezadoFiltrosInteractivos();
    tabla->resizeColumnsToContents();
    // encabezado de filtros interactivos
    crearEncabezadoFiltrosInteractivos();
    // preparando vertical header
    QStringList verticalHeaderLabels;
    verticalHeaderLabels << "Filtros";
    for(int i=1; i<=500; i++) {
        verticalHeaderLabels << QString::number(i);
    }
    tabla->setVerticalHeaderLabels(verticalHeaderLabels);
}

bool Pacientes::crearBaseDeDatos() {
    // alcance de db
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        //aqui se le dice la direccion de la base de datos a crear o cargar
        db.setDatabaseName(rutaPacientes);
        QSqlQuery query;
        if(db.open())
        {
            //creando tablas de la base de datos si no estan definidas
            if(!(query.exec("CREATE TABLE `Pacientes` ("
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
                            "PRIMARY KEY(`ci`)"
                            ))){
                return false;
            }
        }
        else
            return false;
        return true;
    }
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    return true;   
}

void Pacientes::cargarTabla(QString sql) {
    disconnect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
    disconnect(tabla, SIGNAL(cellClicked(int,int)), this, SLOT(on_tabla_cellClicked(int,int)));
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(rutaPacientes);
        if(db.open())
        {
            QSqlQuery query;
            if(query.exec(sql))
            {
                numRows = query.at();
                while(query.next())
                {
                    QString nombre = query.value(0).toString();
                    QString bloque = query.value(1).toString();
                    QString cubiculo = query.value(2).toString();
                    QString hta = query.value(3).toString();
                    QString dm = query.value(4).toString();
                    QString ab = query.value(5).toString();
                    QString il = query.value(6).toString();
                    QString direccion = query.value(7).toString();
                    QString edad = query.value(8).toString();
                    QString raza = query.value(9).toString();
                    QString sexo = query.value(10).toString();
                    QString ci = query.value(11).toString();
                    QString municipio = query.value(12).toString();
                    QString provincia = query.value(13).toString();
                    QString area = query.value(14).toString();
                    QString contacto = query.value(15).toString();
                    QString fi = query.value(16).toString();
                    QString fu = query.value(17).toString();
                    QString pcr1 = query.value(18).toString();
                    QString resultado1 = query.value(19).toString();
                    QString pcr2 = query.value(20).toString();
                    QString resultado2 = query.value(21).toString();
                    QString pcr3 = query.value(22).toString();
                    QString resultado3 = query.value(23).toString();
                    QString rowid = query.value(24).toString();

                    QTableWidgetItem *nombreItem = new QTableWidgetItem(nombre);
                    QTableWidgetItem *bloqueItem = new QTableWidgetItem(bloque);
                    QTableWidgetItem *cubiculoItem = new QTableWidgetItem(cubiculo);
                    QTableWidgetItem *htaItem = new QTableWidgetItem(hta);
                    QTableWidgetItem *dmItem = new QTableWidgetItem(dm);
                    QTableWidgetItem *abItem = new QTableWidgetItem(ab);
                    QTableWidgetItem *ilItem = new QTableWidgetItem(il);
                    QTableWidgetItem *direccionItem = new QTableWidgetItem(direccion);
                    QTableWidgetItem *edadItem = new QTableWidgetItem(edad);
                    QTableWidgetItem *razaItem = new QTableWidgetItem(raza);
                    QTableWidgetItem *sexoItem = new QTableWidgetItem(sexo);
                    QTableWidgetItem *ciItem = new QTableWidgetItem(ci);
                    QTableWidgetItem *municipioItem = new QTableWidgetItem(municipio);
                    QTableWidgetItem *provinciaItem = new QTableWidgetItem(provincia);
                    QTableWidgetItem *areaItem = new QTableWidgetItem(area);
                    QTableWidgetItem *contactoItem = new QTableWidgetItem(contacto);
                    QTableWidgetItem *fiItem = new QTableWidgetItem(fi);
                    QTableWidgetItem *fuItem = new QTableWidgetItem(fu);
                    QTableWidgetItem *pcr1Item = new QTableWidgetItem(pcr1);
                    QTableWidgetItem *resultado1Item = new QTableWidgetItem(resultado1);
                    QTableWidgetItem *pcr2Item = new QTableWidgetItem(pcr2);
                    QTableWidgetItem *resultado2Item = new QTableWidgetItem(resultado2);
                    QTableWidgetItem *pcr3Item = new QTableWidgetItem(pcr3);
                    QTableWidgetItem *resultado3Item = new QTableWidgetItem(resultado3);
                    QTableWidgetItem *rowidItem = new QTableWidgetItem(rowid);

                    QColor* color = new QColor("#ffffff");

                    nombreItem->setTextColor(*color);
                    bloqueItem->setTextColor(*color);
                    cubiculoItem->setTextColor(*color);
                    htaItem->setTextColor(*color);
                    dmItem->setTextColor(*color);
                    abItem->setTextColor(*color);
                    ilItem->setTextColor(*color);
                    direccionItem->setTextColor(*color);
                    edadItem->setTextColor(*color);
                    razaItem->setTextColor(*color);
                    sexoItem->setTextColor(*color);
                    ciItem->setTextColor(*color);
                    municipioItem->setTextColor(*color);
                    provinciaItem->setTextColor(*color);
                    areaItem->setTextColor(*color);
                    contactoItem->setTextColor(*color);
                    fiItem->setTextColor(*color);
                    fuItem->setTextColor(*color);
                    pcr1Item->setTextColor(*color);
                    resultado1Item->setTextColor(*color);
                    pcr2Item->setTextColor(*color);
                    resultado2Item->setTextColor(*color);
                    pcr3Item->setTextColor(*color);
                    resultado3Item->setTextColor(*color);

                    tabla->setItem(query.at() + 1, 0, nombreItem);
                    tabla->setItem(query.at() + 1, 1, bloqueItem);
                    tabla->setItem(query.at() + 1, 2, cubiculoItem);
                    tabla->setItem(query.at() + 1, 3, htaItem);
                    tabla->setItem(query.at() + 1, 4, dmItem);
                    tabla->setItem(query.at() + 1, 5, abItem);
                    tabla->setItem(query.at() + 1, 6, ilItem);
                    tabla->setItem(query.at() + 1, 7, direccionItem);
                    tabla->setItem(query.at() + 1, 8, edadItem);
                    tabla->setItem(query.at() + 1, 9, razaItem);
                    tabla->setItem(query.at() + 1, 10, sexoItem);
                    tabla->setItem(query.at() + 1, 11, ciItem);
                    tabla->setItem(query.at() + 1, 12, municipioItem);
                    tabla->setItem(query.at() + 1, 13, provinciaItem);
                    tabla->setItem(query.at() + 1, 14, areaItem);
                    tabla->setItem(query.at() + 1, 15, contactoItem);
                    tabla->setItem(query.at() + 1, 16, fiItem);
                    tabla->setItem(query.at() + 1, 17, fuItem);
                    tabla->setItem(query.at() + 1, 18, pcr1Item);
                    tabla->setItem(query.at() + 1, 19, resultado1Item);
                    tabla->setItem(query.at() + 1, 20, pcr2Item);
                    tabla->setItem(query.at() + 1, 21, resultado2Item);
                    tabla->setItem(query.at() + 1, 22, pcr3Item);
                    tabla->setItem(query.at() + 1, 23, resultado3Item);
                    tabla->setItem(query.at() + 1, 24, rowidItem);
                    numRows = query.at();
                }
                if(numRows == -1 || numRows == -2) {
                    numRows = 0;
                }
                else {
                    numRows++;
                }
            }
            else {
                qDebug() << query.lastError();
            }
        }
        else {
            QMessageBox::critical(parentWidget,"Error al cargar base de datos","La base de datos no pudo ser cargada correctamente.");
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
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
        // determinando color de la fila
        QColor* color = new QColor(GRIS);
        if(tabla->item(row, 19) || tabla->item(row, 21) || tabla->item(row, 23)) {
            if(tabla->item(row, 19) && tabla->item(row, 19)->text() == "POSITIVO COVID-19" && (tabla->item(row, 20) == 0 || (tabla->item(row, 20) && tabla->item(row, 20)->text() == "")) && (tabla->item(row, 21) == 0 || (tabla->item(row, 21) && tabla->item(row, 21)->text() == "")) && (tabla->item(row, 22) == 0 || (tabla->item(row, 22) && tabla->item(row, 22)->text() == "")) && (tabla->item(row, 23) == 0 || (tabla->item(row, 23) && tabla->item(row, 23)->text() == ""))) {
                color = new QColor(ROJO);
            }
            else if(tabla->item(row, 19) && tabla->item(row, 19)->text() == "NEGATIVO COVID-19" && (tabla->item(row, 20) == 0 || (tabla->item(row, 20) && tabla->item(row, 20)->text() == "")) && (tabla->item(row, 21) == 0 || (tabla->item(row, 21) && tabla->item(row, 21)->text() == "")) && (tabla->item(row, 22) == 0 || (tabla->item(row, 22) && tabla->item(row, 22)->text() == "")) && (tabla->item(row, 23) == 0 || (tabla->item(row, 23) && tabla->item(row, 23)->text() == ""))) {
                color = new QColor(VERDE);
            }
            else if(tabla->item(row, 19) && tabla->item(row, 19)->text() == "REPETIR MUESTRA" && (tabla->item(row, 20) == 0 || (tabla->item(row, 20) && tabla->item(row, 20)->text() == "")) && (tabla->item(row, 21) == 0 || (tabla->item(row, 21) && tabla->item(row, 21)->text() == "")) && (tabla->item(row, 22) == 0 || (tabla->item(row, 22) && tabla->item(row, 22)->text() == "")) && (tabla->item(row, 23) == 0 || (tabla->item(row, 23) && tabla->item(row, 23)->text() == ""))) {
                color = new QColor(AZUL);
            }
            if(tabla->item(row, 21) && tabla->item(row, 21)->text() == "POSITIVO COVID-19" && (tabla->item(row, 22) == 0 || (tabla->item(row, 22) && tabla->item(row, 22)->text() == "")) && (tabla->item(row, 23) == 0 || (tabla->item(row, 23) && tabla->item(row, 23)->text() == ""))) {
                color = new QColor(ROJO);
            }
            else if(tabla->item(row, 21) && tabla->item(row, 21)->text() == "NEGATIVO COVID-19" && (tabla->item(row, 22) == 0 || (tabla->item(row, 22) && tabla->item(row, 22)->text() == "")) && (tabla->item(row, 23) == 0 || (tabla->item(row, 23) && tabla->item(row, 23)->text() == ""))) {
                color = new QColor(VERDE);
            }
            else if(tabla->item(row, 21) && tabla->item(row, 21)->text() == "REPETIR MUESTRA" && (tabla->item(row, 22) == 0 || (tabla->item(row, 22) && tabla->item(row, 22)->text() == "")) && (tabla->item(row, 23) == 0 || (tabla->item(row, 23) && tabla->item(row, 23)->text() == ""))) {
                color = new QColor(AZUL);
            }
            if(tabla->item(row, 23) && tabla->item(row, 23)->text() == "POSITIVO COVID-19") {
                color = new QColor(ROJO);
            }
            else if(tabla->item(row, 23) && tabla->item(row, 23)->text() == "NEGATIVO COVID-19") {
                color = new QColor(VERDE);
            }
            else if(tabla->item(row, 23) && tabla->item(row, 23)->text() == "REPETIR MUESTRA") {
                color = new QColor(AZUL);
            }
        }
        disconnect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
        for(int i=0; i<24; i++) {
            if(tabla->item(row, i))
                tabla->item(row, i)->setBackgroundColor(*color);
            else {
                QTableWidgetItem* emptyItem = new QTableWidgetItem();
                emptyItem->setBackgroundColor(*color);
                tabla->setItem(row, i, emptyItem);
            }
        }
        connect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
    }
}

QString Pacientes::getRow(int index) {
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
    return QString("");
}

int Pacientes::getNotNull(int row) {
    int r{0};
    for(int i{0}; i < tabla->columnCount(); i++)
        if(tabla->item(row, i) && !tabla->item(row, i)->text().isEmpty())
            r++;
    return r;
}

void Pacientes::filtrar() {
    QString sql = "select *, rowid from Pacientes ";
    bool algunCriterio = false;
    QStringList sqlCriterios;
    for(int i=0; i<24; i++) {
        if(criterios[i] != nullptr) {
            algunCriterio = true;
            sqlCriterios << getRow(i) + " like '" + *criterios[i] + "%'";

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

Pacientes::~Pacientes() {
    delete bloqueButton, delete cubiculoButton, delete resultadoButton, delete fechaPCRButton;
    delete limpiarBloque, delete limpiarCubiculo, delete limpiarResultado, delete limpiarFechaPCR;
    delete bloqueLabel, delete cubiculoLabel, delete resultadoLabel, delete fechaPCRLabel;
    for(int i=0; i < 24; i++) {
        delete criterios[i];
    }
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
        {
            // si no se ingreso nada
            QString newText = tabla->currentItem()->text();
            if(newText.isEmpty())
                return ;
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(rutaPacientes);
            if(db.open()) {
                QSqlQuery query;
                if(!query.exec("insert into Pacientes('" + getRow(column) + "') values('" + newText + "')")) {
                    QMessageBox::critical(parentWidget,"Error al insertar dato",query.lastError().text());
                }
                int rowidInserted;
                if(!query.exec("select max(rowid) from Pacientes")) {
                    QMessageBox::critical(parentWidget,"Error al determinar identificador de fila",query.lastError().text());
                }
                else {
                    if(query.next()) {
                        rowidInserted = query.value(0).toInt();
                    }
                    else {
                        QMessageBox::critical(parentWidget,"Error al determinar identificador de fila",query.lastError().text());
                    }
                }
                // desconectando sennal momentaneamente para no rellamar este slot
                disconnect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
                tabla->currentItem()->setText("");
                tabla->setItem(numRows+1, column, new QTableWidgetItem(newText));
                tabla->item(numRows+1, column)->setTextColor(QColor("#ffffff"));
                tabla->setItem(numRows+1, 24, new QTableWidgetItem(QString::number(rowidInserted)));
                // Estableciendo color de la fila
                darColor(numRows+1, -1);
                numRows++;
                connect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
            }
            else {
                QMessageBox::critical(parentWidget,"Error al cargar base de datos","La base de datos no pudo ser cargada correctamente.");
            }
            db.close();
        }
        QSqlDatabase::removeDatabase("qt_sql_default_connection");
    }
    // se esta modificando un paciente ya registrado
    else {
        {
            // si no se hizo ninguna modificacion
            QString newText = tabla->currentItem()->text();
            if(newText == currentText)
                return ;
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(rutaPacientes);
            if(db.open()) {
                QSqlQuery query;
                int camposNoVacios = getNotNull(row);
                // se esta eliminando el unico campo no vacio de una fila, por lo que se borra
                if(camposNoVacios == 0 && newText.isEmpty()) {
                    if(!query.exec("delete from Pacientes where rowid=" + tabla->item(row, 24)->text())) {
                        QMessageBox::critical(parentWidget,"Error al insertar dato",query.lastError().text());
                    }
                    // Estableciendo color de la fila
                    darColor(row, -1);
                    numRows--;
                }
                // actualizar
                else {
                    if(!query.exec("update Pacientes set " + getRow(column) + "='" + newText + "' where rowid=" + tabla->item(row, 24)->text())) {
                        QMessageBox::critical(parentWidget,"Error al insertar dato",query.lastError().text());
                    }
                    // Estableciendo color de la fila
                    darColor(row, -1);
                }

            }
            else {
                QMessageBox::critical(parentWidget,"Error al abrir base de datos","La base de datos no pudo ser abierta correctamente.");
            }
            db.close();
        }
        QSqlDatabase::removeDatabase("qt_sql_default_connection");
        // Estableciendo color de la fuente de la celda
        disconnect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
        tabla->item(row, column)->setTextColor(QColor("#ffffff"));
        connect(tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
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

        QSet<QString> bloquesSet;
        // realizar consulta en base de datos para obtener fechas disponibles
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(rutaPacientes);
            if(db.open()) {
                QSqlQuery query;
                if(!query.exec("select distinct bloque from Pacientes")) {
                    QMessageBox::critical(parentWidget,"Error al insertar dato",query.lastError().text());
                }
                else {
                    while(query.next()) {
                        bloquesSet << query.value(0).toString();
                    }
                }
            }
            else {
                QMessageBox::critical(parentWidget,"Error al cargar base de datos","La base de datos no pudo ser cargada correctamente.");
            }
            db.close();
        }
        QSqlDatabase::removeDatabase("qt_sql_default_connection");
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
            if(i!=0 && i%3==0) {
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

        frameBloque->setGeometry(bloqueButton->x(), bloqueButton->y() + 50, 180, filas*65);
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

        QSet<QString> cubiculosSet;
        // realizar consulta en base de datos para obtener fechas disponibles
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(rutaPacientes);
            if(db.open()) {
                QSqlQuery query;
                if(!query.exec("select distinct cubiculo from Pacientes")) {
                    QMessageBox::critical(parentWidget,"Error al insertar dato",query.lastError().text());
                }
                else {
                    while(query.next()) {
                        cubiculosSet << query.value(0).toString();
                    }
                }
            }
            else {
                QMessageBox::critical(parentWidget,"Error al cargar base de datos","La base de datos no pudo ser cargada correctamente.");
            }
            db.close();
        }
        QSqlDatabase::removeDatabase("qt_sql_default_connection");

        // generando lista de bloques
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
            if(i!=0 && i%3==0) {
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

        frameCubiculo->setGeometry(cubiculoButton->x(), cubiculoButton->y() + 50, 180, filas*65);
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

        frameResultado->setGeometry(resultadoButton->x(),resultadoButton->y() + 50, 180, 200);
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
        QSet<QString> fechasPCR;
        // realizar consulta en base de datos para obtener fechas disponibles
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(rutaPacientes);
            if(db.open()) {
                QSqlQuery query;
                if(!query.exec("select distinct pcr1 from Pacientes")) {
                    QMessageBox::critical(parentWidget,"Error al insertar dato",query.lastError().text());
                }
                else {
                    while(query.next()) {
                        fechasPCR << query.value(0).toString();
                    }
                }
                if(!query.exec("select distinct pcr2 from Pacientes")) {
                    QMessageBox::critical(parentWidget,"Error al insertar dato",query.lastError().text());
                }
                else {
                    while(query.next()) {
                        fechasPCR << query.value(0).toString();
                    }
                }
                if(!query.exec("select distinct pcr3 from Pacientes")) {
                    QMessageBox::critical(parentWidget,"Error al insertar dato",query.lastError().text());
                }
                else {
                    while(query.next()) {
                        fechasPCR << query.value(0).toString();
                    }
                }
            }
            else {
                QMessageBox::critical(parentWidget,"Error al cargar base de datos","La base de datos no pudo ser cargada correctamente.");
            }
            db.close();
        }
        QSqlDatabase::removeDatabase("qt_sql_default_connection");

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

        frameFechaPCR->setGeometry(fechaPCRButton->x(), fechaPCRButton->y() + 50, 180, fechasPCR.size()*30);
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

void Pacientes::actionPositivo() {
    QModelIndexList indexList = tabla->selectionModel()->selection().indexes();
    for(auto it : indexList) {
        if(it.row() <= numRows) {
            // solo se actualiza si el paciente no tiene resultado negativo
            if(tabla->item(it.row(), 0)->backgroundColor() != QColor(ROJO)) {
                // determinando donde colocar resultado (se coloca de acuerdo al resultado que se esta esperando)
                if(tabla->item(it.row(), 18) && (tabla->item(it.row(), 20) == 0 || (tabla->item(it.row(), 20) && tabla->item(it.row(), 20)->text() == "")) && (tabla->item(it.row(), 22) == 0 || (tabla->item(it.row(), 22) && tabla->item(it.row(), 22)->text() == ""))) {
                    // verificar que no hayan otros resultados posterior a este, si los hay, informar de error
                    if((tabla->item(it.row(), 21) && tabla->item(it.row(), 21)->text() != "") || (tabla->item(it.row(), 23) && tabla->item(it.row(), 23)->text() != "")) {
                        QMessageBox::critical(parentWidget, "Conflicto entre datos.", "El paciente de la fila "+QString::number(it.row()+1)+" tiene un resultado registrado en una posicion no esperada");
                        return;
                    }
                    else {
                        tabla->setCurrentItem(tabla->item(it.row(), 19));
                        tabla->setItem(it.row(), 19, new QTableWidgetItem("POSITIVO COVID-19"));
                        tabla->scrollTo(it);
                    }
                    continue;
                }
                else if(tabla->item(it.row(), 20) && (tabla->item(it.row(), 22) == 0 || (tabla->item(it.row(), 22) && tabla->item(it.row(), 22)->text() == ""))) {
                    // verificar que no hayan otros resultados posterior a este, si los hay, informar de error
                    if(tabla->item(it.row(), 23) && tabla->item(it.row(), 23)->text() != "") {
                        QMessageBox::critical(parentWidget, "Conflicto entre datos.", "El paciente de la fila "+QString::number(it.row()+1)+" tiene un resultado registrado en una posicion no esperada");
                        return;
                    }
                    else {
                        tabla->setCurrentItem(tabla->item(it.row(), 21));
                        tabla->setItem(it.row(), 21, new QTableWidgetItem("POSITIVO COVID-19"));
                        tabla->scrollTo(it);
                    }
                    continue;
                }
                else if(tabla->item(it.row(), 22)) {
                    tabla->setCurrentItem(tabla->item(it.row(), 23));
                    tabla->setItem(it.row(), 23, new QTableWidgetItem("POSITIVO COVID-19"));
                    tabla->scrollTo(it);
                    continue;
                }
                else {
                    if((tabla->item(it.row(), 21) && tabla->item(it.row(), 21)->text() != "") || (tabla->item(it.row(), 23) && tabla->item(it.row(), 23)->text() != "")) {
                        QMessageBox::critical(parentWidget, "Conflicto entre datos.", "El paciente de la fila "+QString::number(it.row()+1)+" tiene un resultado registrado en una posicion no esperada");
                        return;
                    }
                    else {
                        tabla->setCurrentItem(tabla->item(it.row(), 19));
                        tabla->setItem(it.row(), 19, new QTableWidgetItem("POSITIVO COVID-19"));
                        tabla->scrollTo(it);
                    }
                }
            }
        }
    }
}

void Pacientes::actionNegativo() {
    QModelIndexList indexList = tabla->selectionModel()->selection().indexes();
    for(auto it : indexList) {
        if(it.row() <= numRows) {
            // solo se actualiza si el paciente no tiene resultado negativo
            if(tabla->item(it.row(), 0)->backgroundColor() != QColor(VERDE)) {
                // determinando donde colocar resultado (se coloca de acuerdo al resultado que se esta esperando)
                if(tabla->item(it.row(), 18) && (tabla->item(it.row(), 20) == 0 || (tabla->item(it.row(), 20) && tabla->item(it.row(), 20)->text() == "")) && (tabla->item(it.row(), 22) == 0 || (tabla->item(it.row(), 22) && tabla->item(it.row(), 22)->text() == ""))) {
                    // verificar que no hayan otros resultados posterior a este, si los hay, informar de error
                    if((tabla->item(it.row(), 21) && tabla->item(it.row(), 21)->text() != "") || (tabla->item(it.row(), 23) && tabla->item(it.row(), 23)->text() != "")) {
                        QMessageBox::critical(parentWidget, "Conflicto entre datos.", "El paciente de la fila "+QString::number(it.row()+1)+" tiene un resultado registrado en una posicion no esperada");
                        return;
                    }
                    else {
                        tabla->setCurrentItem(tabla->item(it.row(), 19));
                        tabla->setItem(it.row(), 19, new QTableWidgetItem("NEGATIVO COVID-19"));
                        tabla->scrollTo(it);
                    }
                    continue;
                }
                else if(tabla->item(it.row(), 20) && (tabla->item(it.row(), 22) == 0 || (tabla->item(it.row(), 22) && tabla->item(it.row(), 22)->text() == ""))) {
                    // verificar que no hayan otros resultados posterior a este, si los hay, informar de error
                    if(tabla->item(it.row(), 23) && tabla->item(it.row(), 23)->text() != "") {
                        QMessageBox::critical(parentWidget, "Conflicto entre datos.", "El paciente de la fila "+QString::number(it.row()+1)+" tiene un resultado registrado en una posicion no esperada");
                        return;
                    }
                    else {
                        tabla->setCurrentItem(tabla->item(it.row(), 21));
                        tabla->setItem(it.row(), 21, new QTableWidgetItem("NEGATIVO COVID-19"));
                        tabla->scrollTo(it);
                    }
                    continue;
                }
                else if(tabla->item(it.row(), 22)) {
                    tabla->setCurrentItem(tabla->item(it.row(), 23));
                    tabla->setItem(it.row(), 23, new QTableWidgetItem("NEGATIVO COVID-19"));
                    tabla->scrollTo(it);
                    continue;
                }
                else {
                    if((tabla->item(it.row(), 21) && tabla->item(it.row(), 21)->text() != "") || (tabla->item(it.row(), 23) && tabla->item(it.row(), 23)->text() != "")) {
                        QMessageBox::critical(parentWidget, "Conflicto entre datos.", "El paciente de la fila "+QString::number(it.row()+1)+" tiene un resultado registrado en una posicion no esperada");
                        return;
                    }
                    else {
                        tabla->setCurrentItem(tabla->item(it.row(), 19));
                        tabla->setItem(it.row(), 19, new QTableWidgetItem("NEGATIVO COVID-19"));
                        tabla->scrollTo(it);
                    }
                }
            }
        }
    }
}

void Pacientes::actionRepetirMuestra() {
    QModelIndexList indexList = tabla->selectionModel()->selection().indexes();
    for(auto it : indexList) {
        if(it.row() <= numRows) {
            // solo se actualiza si el paciente no tiene resultado negativo
            if(tabla->item(it.row(), 0)->backgroundColor() != QColor(AZUL)) {
                // determinando donde colocar resultado (se coloca de acuerdo al resultado que se esta esperando)
                if(tabla->item(it.row(), 18) && (tabla->item(it.row(), 20) == 0 || (tabla->item(it.row(), 20) && tabla->item(it.row(), 20)->text() == "")) && (tabla->item(it.row(), 22) == 0 || (tabla->item(it.row(), 22) && tabla->item(it.row(), 22)->text() == ""))) {
                    // verificar que no hayan otros resultados posterior a este, si los hay, informar de error
                    if((tabla->item(it.row(), 21) && tabla->item(it.row(), 21)->text() != "") || (tabla->item(it.row(), 23) && tabla->item(it.row(), 23)->text() != "")) {
                        QMessageBox::critical(parentWidget, "Conflicto entre datos.", "El paciente de la fila "+QString::number(it.row()+1)+" tiene un resultado registrado en una posicion no esperada");
                        return;
                    }
                    else {
                        tabla->setCurrentItem(tabla->item(it.row(), 19));
                        tabla->setItem(it.row(), 19, new QTableWidgetItem("REPETIR MUESTRA"));
                        tabla->scrollTo(it);
                    }
                    continue;
                }
                else if(tabla->item(it.row(), 20) && (tabla->item(it.row(), 22) == 0 || (tabla->item(it.row(), 22) && tabla->item(it.row(), 22)->text() == ""))) {
                    // verificar que no hayan otros resultados posterior a este, si los hay, informar de error
                    if(tabla->item(it.row(), 23) && tabla->item(it.row(), 23)->text() != "") {
                        QMessageBox::critical(parentWidget, "Conflicto entre datos.", "El paciente de la fila "+QString::number(it.row()+1)+" tiene un resultado registrado en una posicion no esperada");
                        return;
                    }
                    else {
                        tabla->setCurrentItem(tabla->item(it.row(), 21));
                        tabla->setItem(it.row(), 21, new QTableWidgetItem("REPETIR MUESTRA"));
                        tabla->scrollTo(it);
                    }
                    continue;
                }
                else if(tabla->item(it.row(), 22)) {
                    tabla->setCurrentItem(tabla->item(it.row(), 23));
                    tabla->setItem(it.row(), 23, new QTableWidgetItem("REPETIR MUESTRA"));
                    tabla->scrollTo(it);
                    continue;
                }
                else {
                    if((tabla->item(it.row(), 21) && tabla->item(it.row(), 21)->text() != "") || (tabla->item(it.row(), 23) && tabla->item(it.row(), 23)->text() != "")) {
                        QMessageBox::critical(parentWidget, "Conflicto entre datos.", "El paciente de la fila "+QString::number(it.row()+1)+" tiene un resultado registrado en una posicion no esperada");
                        return;
                    }
                    else {
                        tabla->setCurrentItem(tabla->item(it.row(), 19));
                        tabla->setItem(it.row(), 19, new QTableWidgetItem("REPETIR MUESTRA"));
                        tabla->scrollTo(it);
                    }
                }
            }
        }
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
    for(int i=0; i < 24; i++) {
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

    QAction* actionPositivo = new QAction("POSITIVO COVID-19");
    QAction* actionNegativo = new QAction("NEGATIVO COVID-19");
    QAction* actionRepetirMuestra = new QAction("REPETIR MUESTRA");
    QAction* actionAgregarListaRemision = new QAction("AGREGAR A LISTA DE REMISIONES");
    QAction* actionAgregarListaEgresos = new QAction("AGREGAR A LISTA DE EGRESOS");
    QAction* actionEliminar = new QAction("ELIMINAR");

    menu->addAction(actionPositivo);
    menu->addAction(actionNegativo);
    menu->addAction(actionRepetirMuestra);
    menu->addSeparator();
    menu->addAction(actionAgregarListaRemision);
    menu->addAction(actionAgregarListaEgresos);
    menu->addSeparator();
    menu->addAction(actionEliminar);
    // conectando slots de estas actions
    connect(actionPositivo, SIGNAL(triggered()), this, SLOT(actionPositivo()));
    connect(actionNegativo, SIGNAL(triggered()), this, SLOT(actionNegativo()));
    connect(actionRepetirMuestra, SIGNAL(triggered()), this, SLOT(actionRepetirMuestra()));
    connect(actionAgregarListaRemision, SIGNAL(triggered()), this, SLOT(actionAgregarListaRemision()));
    connect(actionAgregarListaEgresos, SIGNAL(triggered()), this, SLOT(actionAgregarListaEgresos()));
    connect(actionEliminar, SIGNAL(triggered()), this, SLOT(actionEliminar()));
}

void Pacientes::crearEncabezadoFiltrosInteractivos() {
    // PREPARANDO FILTROS INTERACTIVOS DE LA TABLA
    QLineEdit* filtrosInteractivos[24];
    for(int i=0; i < 24; i++) {
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

