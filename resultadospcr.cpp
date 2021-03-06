#include "resultadospcr.h"


bool ResultadosPCR::eventFilter(QObject *watched, QEvent *event) {
    Q_UNUSED(watched);
    // se detecto una tecla que fue liberada
    QKeyEvent* keyEvent = dynamic_cast<QKeyEvent*>(event);
    if(keyEvent) {
        // se desea copiar al portapapeles
        if(keyEvent->text() == "CTRL+C" || keyEvent->key() == 67) {
            QModelIndexList indexList = tabla->selectionModel()->selection().indexes();
            // almacenando coordenadas
            QList<QPair<int, int>> coordenadas;
            for(auto it : indexList) {
                if(it.row() < numRows && it.row() > 0 && it.column() < tabla->columnCount()-1){
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

ResultadosPCR::ResultadosPCR(QObject *parent, QTableWidget* view, DataBaseManager::Controlador cont) : QObject(parent)
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
    tabla->clear();
    tabla->setColumnCount(25);
    tabla->hideColumn(24);
    encabezados << "Codigo LBM"
                << "Codigo"
                << "Fecha de entrada"
                << "Nombres y Apellidos"
                << "Bloque"
                << "Cubiculo"
                << "Edad"
                << "Sexo"
                << "CI"
                << "Direccion"
                << "Area de salud"
                << "Municipio"
                << "Provincia"
                << "Condicion"
                << "Pais"
                << "Fecha de inicio de sintomas"
                << "Fecha toma de muestra"
                << "Tipo de muestra"
                << "Procedencia de la entrega"
                << "Resultado"
                << "Cual"
                << "CT"
                << "Numero Placa"
                << "Fecha de Resultado";
    tabla->setHorizontalHeaderLabels(encabezados);
    tabla->setRowCount(501);
    // DESHABILITANDO LA EDICION DE LOS CAMPOS DE LA TABLA
    tabla->setEditTriggers(QAbstractItemView::NoEditTriggers);
    // CARGAR TABLA PACIENTES DEL CENTRO
    // estableciendo criterios de segmentacion de datos a nulos
    criterios = new QString*[tabla->columnCount()-1];
    for(int i=0; i<tabla->columnCount()-1; i++) {
        criterios[i] = nullptr;
    }
    // criterios por defecto en esta ventana
    criterios[18] = new QString("Universidad de Oriente");
    crearEncabezadoFiltrosInteractivos();
    filtrar();
    tabla->resizeColumnsToContents();
    // tamanno de las columnas (por defecto)
    QList<int> columnWidth = QList<int>()
            << 130
            << 80
            << 160
            << 240
            << 80
            << 90
            << 80
            << 80
            << 110
            << 240;
    for(int i = 0; i < columnWidth.size(); i++) {
        tabla->setColumnWidth(i, columnWidth.at(i));
    }
    // encabezado de filtros interactivos
    // preparando vertical header
    QStringList verticalHeaderLabels;
    verticalHeaderLabels << "";
    for(int i=1; i<=500; i++) {
        verticalHeaderLabels << QString::number(i);
    }
    tabla->setVerticalHeaderLabels(verticalHeaderLabels);
}

bool ResultadosPCR::crearBaseDeDatos() {
    return dbm->ejecutarSQL("CREATE TABLE `ResultadosPCR` ("
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

void ResultadosPCR::cargarTabla(QString sql) {
    Tabla resultadoConsulta = dbm->consultaSQL(sql);
    for(int i = 0; i < resultadoConsulta.size(); i++) {
        for(int j = 0; j < resultadoConsulta.at(i).size(); j++) {
            // adquiriendo item y asignandole algunos estilos
            QTableWidgetItem* item = new QTableWidgetItem(resultadoConsulta.at(i).at(j));
            tabla->setItem(i + 1, j, item);
        }
    }
    numRows = resultadoConsulta.size();
    // Estableciendo colores de la tabla
    darColor();
}

void ResultadosPCR::darColor(int row, int column) {
    // colorear toda la tabla
    if(row == -1 && column == -1) {
        for(int i=1; i <= numRows; i++) {
            // dar color a la fila
            darColor(i, -1);
        }
    }
    // colorear fila
    else if(row != -1 && column == -1) {
        int resultado = tabla->columnCount()-6;
        // determinando color de la fila
        QColor* color = new QColor(GRIS);
        if(tabla->item(row, resultado)) {
            QString res = tabla->item(row, resultado)->text();
            if(res == "POS COVID-19" || res == "POS-COVID 19") {
                color = new ROJO;
            }
            else if(res == "NEG COVID-19" || res == "NEG-COVID 19") {
                color = new VERDE;
            }
            else if(res == "REPETIR MUESTRA") {
                color = new AZUL;
            }
        }
        for(int i=0; i<tabla->columnCount()-1; i++) {
            if(tabla->item(row, i))
                tabla->item(row, i)->setBackgroundColor(*color);
            else {
                QTableWidgetItem* emptyItem = new QTableWidgetItem();
                emptyItem->setBackgroundColor(*color);
                tabla->setItem(row, i, emptyItem);
            }
        }
    }
}

QString ResultadosPCR::getRow(int index) {
    switch (index) {
    case 0 : return QString("codigoLBM");
    case 1 : return QString("codigo");
    case 2 : return QString("fe");
    case 3 : return QString("nombre");
    case 4 : return QString("bloque");
    case 5 : return QString("cubiculo");
    case 6 : return QString("edad");
    case 7 : return QString("sexo");
    case 8 : return QString("ci");
    case 9 : return QString("direccion");
    case 10 : return QString("area");
    case 11 : return QString("municipio");
    case 12 : return QString("provincia");
    case 13 : return QString("condicion");
    case 14 : return QString("pais");
    case 15 : return QString("fechaInicioSintomas");
    case 16 : return QString("fechaTomaMuestra");
    case 17 : return QString("tipoMuestra");
    case 18 : return QString("procedenciaMuestra");
    case 19 : return QString("resultado");
    case 20 : return QString("cual");
    case 21 : return QString("ct");
    case 22 : return QString("numeroPlaca");
    case 23 : return QString("fechaResultado");
    }
    return QString("");
}

int ResultadosPCR::getNotNull(int row) {
    int r{0};
    for(int i{0}; i < tabla->columnCount()-1; i++)
        if(tabla->item(row, i) && !tabla->item(row, i)->text().isEmpty())
            r++;
    return r;
}

void ResultadosPCR::filtrar() {
    QString sql = "select *, rowid from ResultadosPCR ";
    bool algunCriterio = false;
    QStringList sqlCriterios;
    for(int i=0; i<tabla->columnCount()-1; i++) {
        if(criterios[i] != nullptr) {
            algunCriterio = true;
            // para garantizar que la busqueda en este campo sea solo a las cadenas que comiencen por...
            if(i != 5) {
                sqlCriterios << getRow(i) + " like '%" + *criterios[i] + "%'";
            }
            else {
                sqlCriterios << getRow(i) + " like '%" + *criterios[i] + "'";
            }
        }
    }
    if(algunCriterio)
        sql += " where ";
    sql += sqlCriterios.join(" and ");
    // se esta usando el filtro de todos los pacientes que estan marcados con un resultado, el mismo criterio
    // para colorear las filas
    sql += " order by bloque, cubiculo;";
    tabla->clearContents();
    crearEncabezadoFiltrosInteractivos();
    cargarTabla(sql);
}

void ResultadosPCR::marcarFilas() {
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

ResultadosPCR::~ResultadosPCR() {
    tabla->setEditTriggers(QAbstractItemView::DoubleClicked|
                           QAbstractItemView::SelectedClicked|
                           QAbstractItemView::EditKeyPressed|
                           QAbstractItemView::AnyKeyPressed);
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
    delete indicadorFiltroBloque,
    delete indicadorFiltroCubiculo,
    delete indicadorFiltroResultado,
    delete indicadorFiltroFechaPCR;
    delete bloquelayOut,
    delete cubiculolayOut,
    delete resultadolayOut,
    delete fechaPCRlayOut;
}



void ResultadosPCR::on_bloqueButton_clicked() {
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

        frameBloque->setGeometry(bloqueButton->x()+20, bloqueButton->y() + 63, bloqueButton->width()-20, filas*50);
        frameBloque->setStyleSheet("background-color : #f5f5f5;");
        QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
        shadowEffect->setBlurRadius(5);
        shadowEffect->setColor(QColor(145, 145, 145, 80));
        shadowEffect->setOffset(3, 3);
        frameBloque->setGraphicsEffect(shadowEffect);
        frameBloque->show();
    }
    else {
        frameBloque->hide();
        delete frameBloque;
        frameBloque = nullptr;
    }
}

void ResultadosPCR::on_cubiculoButton_clicked() {
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

        frameCubiculo->setGeometry(cubiculoButton->x()+20, cubiculoButton->y() + 63, cubiculoButton->width()-20, filas*50);
        frameCubiculo->setStyleSheet("background-color : #f5f5f5;");
        frameCubiculo->show();
        QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
        shadowEffect->setBlurRadius(5);
        shadowEffect->setColor(QColor(145, 145, 145, 80));
        shadowEffect->setOffset(3, 3);
        frameCubiculo->setGraphicsEffect(shadowEffect);
    }
    else {
        frameCubiculo->hide();
        delete frameCubiculo;
        frameCubiculo = nullptr;
    }
}

void ResultadosPCR::on_resultadoButton_clicked() {
    if(frameResultado == nullptr) {
        QSet<QString> resultadosSet = dbm->valoresColumna("resultado");
        // generando lista de cubiculos
        QStringList resultados;
        for(auto it : resultadosSet) {
            resultados << it;
        }
        // ordenando lista
        std::sort(resultados.begin(), resultados.end());
        frameResultado = new QFrame(parentWidget);
        QVBoxLayout* container = new QVBoxLayout();

        QHBoxLayout* layout;
        int i = 0;
        for(auto it = resultados.constBegin(); it != resultados.constEnd(); it++) {
            layout = new QHBoxLayout();
            container->addLayout(layout);
            QPushButton* button = new QPushButton(*it);
            connect(button, SIGNAL(clicked()), this, SLOT(opcionSegmentacionResultadoSeleccionada()));
            layout->addWidget(button);
            i++;
        }

        frameResultado->setLayout(container);

        frameResultado->setGeometry(resultadoButton->x()+20, resultadoButton->y() + 63, resultadoButton->width()-20, 150);
        frameResultado->setStyleSheet("background-color : #f5f5f5;");
        frameResultado->show();
        QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
        shadowEffect->setBlurRadius(5);
        shadowEffect->setColor(QColor(145, 145, 145, 80));
        shadowEffect->setOffset(3, 3);
        frameResultado->setGraphicsEffect(shadowEffect);
    }
    else {
        frameResultado->hide();
        delete frameResultado;
        frameResultado = nullptr;
    }
}

void ResultadosPCR::on_fechaPCRButton_clicked() {
    if(frameFechaPCR == nullptr) {
        // todas las fechas distintas de toma de muestra
        QSet<QString> fechasPCRSet = dbm->valoresColumna("fechaTomaMuestra");
        // generando lista de cubiculos
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

        frameFechaPCR->setGeometry(fechaPCRButton->x()+20, fechaPCRButton->y() + 63, fechaPCRButton->width()-20, fechasPCR.size()*50);
        frameFechaPCR->setStyleSheet("background-color : #f5f5f5;");
        frameFechaPCR->show();
        QGraphicsDropShadowEffect* shadowEffect = new QGraphicsDropShadowEffect(this);
        shadowEffect->setBlurRadius(5);
        shadowEffect->setColor(QColor(145, 145, 145, 80));
        shadowEffect->setOffset(3, 3);
        frameFechaPCR->setGraphicsEffect(shadowEffect);
    }
    else {
        frameFechaPCR->hide();
        delete frameFechaPCR;
        frameFechaPCR = nullptr;
    }
}

void ResultadosPCR::opcionSegmentacionBloqueSeleccionada() {
    QPushButton* boton = (QPushButton*)sender();
    criterios[4] = new QString(boton->text());
    filtrar();
    // ocultando cuadro de seleccion de opcion de segmentacion
    frameBloque->hide();
    delete frameBloque;
    frameBloque = nullptr;
    // Actualizando label indicador de la segmentacion que se esta usando
    bloqueLabel->setText(*criterios[4]);
    bloqueLabel->show();
    limpiarBloque->show();
}

void ResultadosPCR::opcionSegmentacionCubiculoSeleccionada() {
    QPushButton* boton = (QPushButton*)sender();
    criterios[5] = new QString(boton->text());
    filtrar();
    // ocultando cuadro de seleccion de opcion de segmentacion
    frameCubiculo->hide();
    delete frameCubiculo;
    frameCubiculo = nullptr;
    // Actualizando label indicador de la segmentacion que se esta usando
    cubiculoLabel->setText(*criterios[5]);
    cubiculoLabel->show();
    limpiarCubiculo->show();
}

void ResultadosPCR::opcionSegmentacionResultadoSeleccionada() {
    QPushButton* boton = (QPushButton*)sender();
    criterios[19] = new QString(boton->text());
    filtrar();
    // ocultando cuadro de seleccion de opcion de segmentacion
    frameResultado->hide();
    delete frameResultado;
    frameResultado = nullptr;
    // Actualizando label indicador de la segmentacion que se esta usando
    resultadoLabel->setText(*criterios[19]);
    resultadoLabel->show();
    limpiarResultado->show();
}

void ResultadosPCR::opcionSegmentacionFechaPCRSeleccionada() {
    QPushButton* boton = (QPushButton*)sender();
    criterios[16] = new QString(boton->text());
    filtrar();
    // ocultando cuadro de seleccion de opcion de segmentacion
    frameFechaPCR->hide();
    delete frameFechaPCR;
    frameFechaPCR = nullptr;
    // Actualizando label indicador de la segmentacion que se esta usando
    fechaPCRLabel->setText(*criterios[16]);
    fechaPCRLabel->show();
    limpiarFechaPCR->show();
}

void ResultadosPCR::limpiarSegmentacionBloque() {
    delete criterios[4], criterios[4] = nullptr;
    bloqueLabel->clear();
    filtrar();
    limpiarBloque->hide();
    bloqueLabel->hide();
}

void ResultadosPCR::limpiarSegmentacionCubiculo() {
    delete criterios[5], criterios[5] = nullptr;
    cubiculoLabel->clear();
    filtrar();
    limpiarCubiculo->hide();
    cubiculoLabel->hide();
}

void ResultadosPCR::limpiarSegmentacionResultado() {
    delete criterios[19], criterios[19] = nullptr;
    filtrar();
    resultadoLabel->clear();
    limpiarResultado->hide();
    resultadoLabel->hide();
}

void ResultadosPCR::limpiarSegmentacionFechaPCR() {
    delete criterios[16], criterios[16] = nullptr;
    filtrar();
    fechaPCRLabel->clear();
    limpiarFechaPCR->hide();
    fechaPCRLabel->hide();
}

void ResultadosPCR::actionAgregarListaRemision() {

}

void ResultadosPCR::actionAgregarListaEgresos() {

}


void ResultadosPCR::filtroInteractivo(QString text) {
    QWidget* widget = (QWidget*)sender();
    // localizando que filtro es
    int index = 0;
    for(int i=0; i < tabla->columnCount()-1; i++) {
        if(tabla->cellWidget(0, i) == widget) {
            index = i;
            break;
        }
    }switch (index) {
    case 4: {
        limpiarSegmentacionBloque();
        break;

    }
    case 5: {
        limpiarSegmentacionCubiculo();
        break;

    }
    case 19: {
        limpiarSegmentacionResultado();
        break;

    }
    case 23: {
        limpiarSegmentacionFechaPCR();
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

void ResultadosPCR::crearMenu() {
    menu = new QMenu();
    QAction* actionAgregarListaRemision = new QAction("AGREGAR A LISTA DE REMISIONES");
    QAction* actionAgregarListaEgresos = new QAction("AGREGAR A LISTA DE EGRESOS");
    menu->addSeparator();
    menu->addAction(actionAgregarListaRemision);
    menu->addAction(actionAgregarListaEgresos);
    connect(actionAgregarListaRemision, SIGNAL(triggered()), this, SLOT(actionAgregarListaRemision()));
    connect(actionAgregarListaEgresos, SIGNAL(triggered()), this, SLOT(actionAgregarListaEgresos()));
}

void ResultadosPCR::crearEncabezadoFiltrosInteractivos() {
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

void ResultadosPCR::seccionSegmentacionDeDatos() {
    // PREPARAR SECCION DE SEGMENTACION DE DATOS
    bloqueButton = new QPushButton("Bloque");
    cubiculoButton = new QPushButton("Cubiculo");
    resultadoButton = new QPushButton("Resultado");
    fechaPCRButton = new QPushButton("Fecha PCR");

    QString CSS = "QPushButton{"
                  "     border-radius: 15px;"
                  "     background-color: #ffffff;"
                  "     height: 30px;"
                  "}"
                  "QPushButton:hover{"
                  "     background-color: #f5f5f5;"
                  "}";

    bloqueButton->setStyleSheet(CSS);
    cubiculoButton->setStyleSheet(CSS);
    resultadoButton->setStyleSheet(CSS);
    fechaPCRButton->setStyleSheet(CSS);

    bloqueButton->setToolTip("Filtro de segmentacion de datos para la ubicacion por bloque del paciente.");
    cubiculoButton->setToolTip("Filtro de segmentacion de datos para la ubicacion por cubiculo del paciente.");
    resultadoButton->setToolTip("Filtro de segmentacion de datos para el resultado del ultimo PCR del paciente.");
    fechaPCRButton->setToolTip("Filtro de segmentacion de datos para la fecha de toma de muestra del paciente.");

    limpiarBloque = new QPushButton("x");
    limpiarCubiculo = new QPushButton("x");
    limpiarResultado = new QPushButton("x");
    limpiarFechaPCR = new QPushButton("x");

    QString limpiarCSS = "QPushButton{"
                         "      width: 20px;"
                         "      height: 20px;"
                         "      border-radius: 11px;"
                         "      background-color: none;"
                         "      border: 1px solid black;"
                         "      color: black;"
                         "      padding-bottom: 3px;"
                         "}"
                         "QPushButton:hover{"
                         "      color: rgb(152, 22, 40);"
                         "      border: 1px solid #981628;"
                         "}";

    limpiarBloque->setStyleSheet(limpiarCSS);
    limpiarCubiculo->setStyleSheet(limpiarCSS);
    limpiarResultado->setStyleSheet(limpiarCSS);
    limpiarFechaPCR->setStyleSheet(limpiarCSS);

    limpiarBloque->setToolTip("Limpiar filtro.");
    limpiarCubiculo->setToolTip("Limpiar filtro.");
    limpiarResultado->setToolTip("Limpiar filtro.");
    limpiarFechaPCR->setToolTip("Limpiar filtro.");

    limpiarBloque->hide();
    limpiarCubiculo->hide();
    limpiarResultado->hide();
    limpiarFechaPCR->hide();

    bloqueLabel = new QLabel("");
    cubiculoLabel = new QLabel("");
    resultadoLabel = new QLabel("");
    fechaPCRLabel = new QLabel("");

    QString labelCSS = "QLabel{"
                         "      font-size: 16pt;"
                         "}";

    bloqueLabel->setStyleSheet(labelCSS);
    cubiculoLabel->setStyleSheet(labelCSS);
    resultadoLabel->setStyleSheet(labelCSS);
    fechaPCRLabel->setStyleSheet(labelCSS);

    bloqueLabel->hide();
    cubiculoLabel->hide();
    resultadoLabel->hide();
    fechaPCRLabel->hide();

    bloquelayOut = new QVBoxLayout();
    cubiculolayOut = new QVBoxLayout();
    resultadolayOut = new QVBoxLayout();
    fechaPCRlayOut = new QVBoxLayout();

    indicadorFiltroBloque = new QHBoxLayout();
    indicadorFiltroCubiculo = new QHBoxLayout();
    indicadorFiltroResultado = new QHBoxLayout();
    indicadorFiltroFechaPCR = new QHBoxLayout();

    indicadorFiltroBloque->addStretch(1);
    indicadorFiltroBloque->addWidget(bloqueLabel);
    indicadorFiltroBloque->addWidget(limpiarBloque);
    indicadorFiltroBloque->addStretch(1);

    indicadorFiltroCubiculo->addStretch(1);
    indicadorFiltroCubiculo->addWidget(cubiculoLabel);
    indicadorFiltroCubiculo->addWidget(limpiarCubiculo);
    indicadorFiltroCubiculo->addStretch(1);

    indicadorFiltroResultado->addStretch(1);
    indicadorFiltroResultado->addWidget(resultadoLabel);
    indicadorFiltroResultado->addWidget(limpiarResultado);
    indicadorFiltroResultado->addStretch(1);

    indicadorFiltroFechaPCR->addStretch(1);
    indicadorFiltroFechaPCR->addWidget(fechaPCRLabel);
    indicadorFiltroFechaPCR->addWidget(limpiarFechaPCR);
    indicadorFiltroFechaPCR->addStretch(1);

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

