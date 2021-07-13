#include "ventana_principal.h"
#include "ui_ventana_principal.h"

VentanaPrincipal::VentanaPrincipal(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VentanaPrincipal)
{
    ui->setupUi(this);


    // CARGAR TABLA PACIENTES DEL CENTRO
    on_actionPacientes_triggered();
}

VentanaPrincipal::~VentanaPrincipal()
{
    delete ui;
}

bool VentanaPrincipal::crearBaseDeDatos()
{
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
                            "`edad`	INTEGER,"
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

QString VentanaPrincipal::getRow(int index) {
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

int VentanaPrincipal::getNotNull(int row) {
    int r{0};
    for(int i{0}; i<ui->tabla->columnCount(); i++)
        if(!ui->tabla->item(row, i)->text().isEmpty())
            r++;
    return r;
}

void VentanaPrincipal::on_actionPacientes_triggered()
{
    ui->tabla->clear();
    // se insertaran datos en la tabla y no se desea que se dispare esta sennal
    disconnect(ui->tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
    // CARGAR TABLA PACIENTES DEL CENTRO
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName(rutaPacientes);
        if(db.open())
        {
            QSqlQuery query;
            if(query.exec("select * from Pacientes"))
            {
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


                    ui->tabla->setItem(query.at(), 0, nombreItem);
                    ui->tabla->setItem(query.at(), 1, bloqueItem);
                    ui->tabla->setItem(query.at(), 2, cubiculoItem);
                    ui->tabla->setItem(query.at(), 3, htaItem);
                    ui->tabla->setItem(query.at(), 4, dmItem);
                    ui->tabla->setItem(query.at(), 5, abItem);
                    ui->tabla->setItem(query.at(), 6, ilItem);
                    ui->tabla->setItem(query.at(), 7, direccionItem);
                    ui->tabla->setItem(query.at(), 8, edadItem);
                    ui->tabla->setItem(query.at(), 9, razaItem);
                    ui->tabla->setItem(query.at(), 10, sexoItem);
                    ui->tabla->setItem(query.at(), 11, ciItem);
                    ui->tabla->setItem(query.at(), 12, municipioItem);
                    ui->tabla->setItem(query.at(), 13, provinciaItem);
                    ui->tabla->setItem(query.at(), 14, areaItem);
                    ui->tabla->setItem(query.at(), 15, contactoItem);
                    ui->tabla->setItem(query.at(), 16, fiItem);
                    ui->tabla->setItem(query.at(), 17, fuItem);
                    ui->tabla->setItem(query.at(), 18, pcr1Item);
                    ui->tabla->setItem(query.at(), 19, resultado1Item);
                    ui->tabla->setItem(query.at(), 20, pcr2Item);
                    ui->tabla->setItem(query.at(), 21, resultado2Item);
                    ui->tabla->setItem(query.at(), 22, pcr3Item);
                    ui->tabla->setItem(query.at(), 23, resultado3Item);
                    numRows = query.at();
                }
                numRows++;
            }
        }
        else
        {
            QMessageBox::critical(this,"Error al cargar base de datos","La base de datos no pudo ser cargada correctamente.");
        }
        db.close();
    }
    QSqlDatabase::removeDatabase("qt_sql_default_connection");
    // reconectando la sennal de la tabla
    connect(ui->tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
}

void VentanaPrincipal::on_actionParte_triggered()
{
    // VISUALIZAR PARTE DEL CENTRO
}

void VentanaPrincipal::on_actionEgresos_triggered()
{
    // CARGAR TABLA EGRESOS DEL CENTRO
}

void VentanaPrincipal::on_actionRemisiones_triggered()
{
    // CARGAR TABLA REMISIONES DEL CENTRO
}

void VentanaPrincipal::on_actionDisponibilidad_triggered()
{
    // VISUALIZAR DISPONIBILIDAD DEL CENTRO
}

void VentanaPrincipal::on_actionPlanificacion_PCR_triggered()
{
    // VISUALIZAR PLANIFICACION DE PCR DEL CENTRO
}

void VentanaPrincipal::on_actionResultados_PCR_triggered()
{
    // VISUALIZAR RESULTADOS DE PCR DEL CENTRO
}

void VentanaPrincipal::on_actionImprimir_triggered()
{
    // IMPRIMIR CONTENIDO DE LA TABLA
}

void VentanaPrincipal::on_tabla_cellChanged(int row, int column)
{
//    qDebug() << "cell [" << row << ", " << column << "] changed";
    // si la fila modificada es mayor a la cantidad de pacientes, el evento corresponde al
    // registro de un nuevo paciente, si no, se esta modificando un paciente existente
    if(row+1 > numRows) {
        // ingresando dato a la base de datos
        {
            // si no se ingreso nada
            QString newText = ui->tabla->currentItem()->text();
            if(newText.isEmpty())
                return ;
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(rutaPacientes);
            if(db.open()) {
                QSqlQuery query;
                if(!query.exec("insert into Pacientes('" + getRow(column) + "') values('" + newText + "')")) {
                    QMessageBox::critical(this,"Error al insertar dato",query.lastError().text());
                }
                // desconectando sennal momentaneamente para no rellamar este slot
                disconnect(ui->tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
                ui->tabla->currentItem()->setText("");
                ui->tabla->setItem(numRows, column, new QTableWidgetItem(newText));
                numRows++;
                connect(ui->tabla, SIGNAL(cellChanged(int,int)), this, SLOT(on_tabla_cellChanged(int,int)));
            }
            else {

            }
            db.close();
        }
        QSqlDatabase::removeDatabase("qt_sql_default_connection");
    }
    // se esta modificando un paciente ya registrado
    else {
        {
            // si no se hizo ninguna modificacion
            QString newText = ui->tabla->currentItem()->text();
            if(newText == currentText)
                return ;
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(rutaPacientes);
            if(db.open()) {
                QSqlQuery query;
                int camposNoVacios = getNotNull(row);
                // se esta eliminando el unico campo no vacio de una fila, por lo que se borra
                if(camposNoVacios == 0 && newText.isEmpty()) {
                    if(!query.exec("delete from Pacientes where rowid=" + QString::number(row+1))) {
                        QMessageBox::critical(this,"Error al insertar dato",query.lastError().text());
                    }
                    numRows--;
                }
                // actualizar
                else {
                    if(!query.exec("update Pacientes set " + getRow(column) + "='" + newText + "' where rowid=" + QString::number(row+1))) {
                        QMessageBox::critical(this,"Error al insertar dato",query.lastError().text());
                    }
                }

            }
            else {
                QMessageBox::critical(this,"Error al abrir base de datos","La base de datos no pudo ser abierta correctamente.");
            }
            db.close();
        }
        QSqlDatabase::removeDatabase("qt_sql_default_connection");
    }

}

void VentanaPrincipal::on_tabla_cellClicked(int row, int column) {
    if(row+1 <= numRows)
        currentText = ui->tabla->item(row, column)->text();
}

void VentanaPrincipal::on_bloqueButton_clicked() {

    if(frameBloque == nullptr) {
        frameBloque = new QFrame(this);
        QVBoxLayout* container = new QVBoxLayout();
        QHBoxLayout* row1 = new QHBoxLayout();
        QHBoxLayout* row2 = new QHBoxLayout();
        QHBoxLayout* row3 = new QHBoxLayout();
        QHBoxLayout* row4 = new QHBoxLayout();
        QHBoxLayout* row5 = new QHBoxLayout();

        row1->addWidget(new QPushButton("A1"));
        row1->addWidget(new QPushButton("A2"));
        row1->addWidget(new QPushButton("A3"));

        row2->addWidget(new QPushButton("B1"));
        row2->addWidget(new QPushButton("B2"));
        row2->addWidget(new QPushButton("B3"));

        row3->addWidget(new QPushButton("C1"));
        row3->addWidget(new QPushButton("C2"));
        row3->addWidget(new QPushButton("C3"));

        row4->addWidget(new QPushButton("E1"));
        row4->addWidget(new QPushButton("E2"));
        row4->addWidget(new QPushButton("E3"));

        row5->addWidget(new QPushButton("H1"));
        row5->addWidget(new QPushButton("H2"));
        row5->addWidget(new QPushButton("H3"));

        container->addLayout(row1);
        container->addLayout(row2);
        container->addLayout(row3);
        container->addLayout(row4);
        container->addLayout(row5);

        frameBloque->setLayout(container);

        frameBloque->setGeometry(ui->bloqueButton->x(), ui->bloqueButton->y() + 50, 180, 250);
        frameBloque->setStyleSheet("background-color : #e2e2e2;");
        frameBloque->show();
    }
    else {
        frameBloque->hide();
        delete frameBloque;
        frameBloque = nullptr;
    }
}

void VentanaPrincipal::on_cubiculoButton_clicked() {
    if(frameCubiculo == nullptr) {
        frameCubiculo = new QFrame(this);
        QVBoxLayout* container = new QVBoxLayout();
        QHBoxLayout* row1 = new QHBoxLayout();
        QHBoxLayout* row2 = new QHBoxLayout();
        QHBoxLayout* row3 = new QHBoxLayout();
        QHBoxLayout* row4 = new QHBoxLayout();

        row1->addWidget(new QPushButton("C1"));
        row1->addWidget(new QPushButton("C2"));
        row1->addWidget(new QPushButton("C3"));

        row2->addWidget(new QPushButton("C4"));
        row2->addWidget(new QPushButton("C5"));
        row2->addWidget(new QPushButton("C6"));

        row3->addWidget(new QPushButton("C7"));
        row3->addWidget(new QPushButton("C8"));
        row3->addWidget(new QPushButton("C9"));

        row4->addWidget(new QPushButton("C10"));
        row4->addWidget(new QPushButton("C11"));
        row4->addWidget(new QPushButton("C12"));

        container->addLayout(row1);
        container->addLayout(row2);
        container->addLayout(row3);
        container->addLayout(row4);

        frameCubiculo->setLayout(container);

        frameCubiculo->setGeometry(ui->cubiculoButton->x(), ui->cubiculoButton->y() + 50, 180, 200);
        frameCubiculo->setStyleSheet("background-color : #e2e2e2;");
        frameCubiculo->show();
    }
    else {
        frameCubiculo->hide();
        delete frameCubiculo;
        frameCubiculo = nullptr;
    }
}

void VentanaPrincipal::on_resultadoButton_clicked() {
    if(frameResultado == nullptr) {
        frameResultado = new QFrame(this);
        QVBoxLayout* container = new QVBoxLayout();

        container->addWidget(new QPushButton("POSITIVO COVID-19"));
        container->addWidget(new QPushButton("NEGATIVO COVID-19"));
        container->addWidget(new QPushButton("REPETIR MUESTRA"));

        frameResultado->setLayout(container);

        frameResultado->setGeometry(ui->resultadoButton->x(), ui->resultadoButton->y() + 50, 180, 200);
        frameResultado->setStyleSheet("background-color : #e2e2e2;");
        frameResultado->show();
    }
    else {
        frameResultado->hide();
        delete frameResultado;
        frameResultado = nullptr;
    }
}

void VentanaPrincipal::on_fechaPCRButton_clicked() {
    if(frameFechaPCR == nullptr) {
        QSet<QString> fechasPCR;
        // realizar consulta en base de datos para obtener fechas disponibles
        {
            QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
            db.setDatabaseName(rutaPacientes);
            if(db.open()) {
                QSqlQuery query;
                if(!query.exec("select distinct pcr1 from Pacientes")) {
                    QMessageBox::critical(this,"Error al insertar dato",query.lastError().text());
                }
                else {
                    while(query.next()) {
                        fechasPCR << query.value(0).toString();
                    }
                }
                if(!query.exec("select distinct pcr2 from Pacientes")) {
                    QMessageBox::critical(this,"Error al insertar dato",query.lastError().text());
                }
                else {
                    while(query.next()) {
                        fechasPCR << query.value(0).toString();
                    }
                }
                if(!query.exec("select distinct pcr3 from Pacientes")) {
                    QMessageBox::critical(this,"Error al insertar dato",query.lastError().text());
                }
                else {
                    while(query.next()) {
                        fechasPCR << query.value(0).toString();
                    }
                }
            }
            db.close();
        }
        QSqlDatabase::removeDatabase("qt_sql_default_connection");

        frameFechaPCR = new QFrame(this);
        QVBoxLayout* container = new QVBoxLayout();

        for(auto it : fechasPCR) {
            if(it != "")
                container->addWidget(new QPushButton(it));
        }

        frameFechaPCR->setLayout(container);

        frameFechaPCR->setGeometry(ui->fechaPCRButton->x(), ui->fechaPCRButton->y() + 50, 180, fechasPCR.size()*30);
        frameFechaPCR->setStyleSheet("background-color : #e2e2e2;");
        frameFechaPCR->show();
    }
    else {
        frameFechaPCR->hide();
        delete frameFechaPCR;
        frameFechaPCR = nullptr;
    }
}

void VentanaPrincipal::bloqueSlot() {

}
