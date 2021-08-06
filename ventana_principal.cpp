#include "ventana_principal.h"
#include "ui_ventana_principal.h"

void VentanaPrincipal::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) { // para que solo acepte archivos el evento drop
        e->acceptProposedAction();
    }
}

void VentanaPrincipal::dropEvent(QDropEvent *e)
{
    // Adquiriendo nombres de archivos y agregandolos a listaExe
    foreach (const QUrl &url, e->mimeData()->urls()) {
        // Archivo no ejecutable
        if(url.toLocalFile().contains(".xls", Qt::CaseInsensitive)) {
            actualizador->leerNuevaEntrada(url.toLocalFile());
        }
    }
}

VentanaPrincipal::VentanaPrincipal(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VentanaPrincipal)
{
    ui->setupUi(this);
    // INICIALIZAR EL ACTUALIZADOR
    actualizador = new Actualizador(this, Actualizador::ActualizacionManual);
    // CONECTAR SENNALES QUE DISPARA EL ACTUALIZADOR CON RESPECTIVOS SLOTS
    connect(actualizador, SIGNAL(nuevosResultadosDelCentro()), this, SLOT(nuevosResultadosDelCentro()));
    connect(actualizador, SIGNAL(actualizacionFinalizada()), this, SLOT(actualizacionFinalizada()));
    connect(actualizador, SIGNAL(descargaIniciada()), this, SLOT(descargaIniciada()));
    // CARGAR TABLA PACIENTES DEL CENTRO
    on_actionPacientes_triggered();
    setWindowState(Qt::WindowMaximized);
}

VentanaPrincipal::~VentanaPrincipal() {
    eliminarControladoresActivos();
    delete ui;
}

void VentanaPrincipal::eliminarControladoresActivos() {
    switch (controladorActivo) {
    case DataBaseManager::DBPacientes: {
        delete controladorPacientes;
        controladorPacientes = nullptr;
        break;
    }
    case DataBaseManager::DBEgresos: {
        delete controladorPacientes;
        controladorPacientes = nullptr;
        break;
    }
    case DataBaseManager::DBRemisiones: {
        delete controladorPacientes;
        controladorPacientes = nullptr;
        break;
    }
    case DataBaseManager::DBEnvioPCR: {

        break;
    }
    case DataBaseManager::DBResultadosPCR: {
        delete controladorResultadosPCR;
        controladorResultadosPCR= nullptr;
        break;
    }
    }
}

void VentanaPrincipal::mostrarUltimaPlaca() {
    DataBaseManager *dbm = new DataBaseManager(this, DataBaseManager::DBResultadosPCR);
    Tabla maxFecha = dbm->consultaSQL("select max(fechaResultado) from ResultadosPCR");
    Tabla consulta = dbm->consultaSQL("select max(numeroPlaca) from ResultadosPCR where fechaResultado=\'" + maxFecha.at(0).at(0) +"'");
    if(consulta.size())
        setWindowTitle(windowTitle() + " [ultima placa : " + consulta.at(0).at(0) + "]");
}

void VentanaPrincipal::nuevosResultadosDelCentro() {
    // MENSAJE QUE SE MUESTRA CUANDO SE ESTA ACTUALIZANDO LAS BASES DE DATOS
    blockingMsg = new QMessageBox(this);
    blockingMsg->setWindowTitle("Actualizacion en curso");
    blockingMsg->setText("<h2>Se han detectado nuevos resultados del centro</h2>"
                         ""
                         "Espere unos instantes...");
    blockingMsg->setStandardButtons(QMessageBox::NoButton);
    blockingMsg->show();
}

void VentanaPrincipal::actualizacionFinalizada() {
    blockingMsg->hide();
    delete blockingMsg;
    on_actionResultados_PCR_triggered();
}

void VentanaPrincipal::descargaIniciada() {
    qDebug() << "descarga iniciada no cerrar la app";
}



void VentanaPrincipal::on_actionPacientes_triggered() {
    setWindowTitle("Zona Roja Universidad de Oriente - Pacientes del centro");
    eliminarControladoresActivos();
    // Crear controlador para seccion Pacientes
    controladorPacientes = new Pacientes(this, ui->tabla, DataBaseManager::DBPacientes);
    controladorActivo = DataBaseManager::DBPacientes;
}

void VentanaPrincipal::on_actionParte_triggered() {
    // VISUALIZAR PARTE DEL CENTRO
    setWindowTitle("Zona Roja Universidad de Oriente - Parte del centro");
}

void VentanaPrincipal::on_actionEgresos_triggered() {
    setWindowTitle("Zona Roja Universidad de Oriente - Egresos del centro");
    // CARGAR TABLA EGRESOS DEL CENTRO
    eliminarControladoresActivos();
    // Crear controlador para seccion Egresos
    controladorPacientes = new Pacientes(this, ui->tabla, DataBaseManager::DBEgresos);
    controladorActivo = DataBaseManager::DBEgresos;
}

void VentanaPrincipal::on_actionRemisiones_triggered() {
    setWindowTitle("Zona Roja Universidad de Oriente - Remisiones del centro");
    // CARGAR TABLA REMISIONES DEL CENTRO
    eliminarControladoresActivos();
    // Crear controlador para seccion Remisiones
    controladorPacientes = new Pacientes(this, ui->tabla, DataBaseManager::DBRemisiones);
    controladorActivo = DataBaseManager::DBRemisiones;
}

void VentanaPrincipal::on_actionDisponibilidad_triggered() {
    // VISUALIZAR DISPONIBILIDAD DEL CENTRO
    setWindowTitle("Zona Roja Universidad de Oriente - Disponibilidad del centro");
}

void VentanaPrincipal::on_actionPlanificacion_PCR_triggered() {
    // VISUALIZAR PLANIFICACION DE PCR DEL CENTRO
    setWindowTitle("Zona Roja Universidad de Oriente - Planificacion de toma de muestras del centro");
}

void VentanaPrincipal::on_actionResultados_PCR_triggered() {
    setWindowTitle("Zona Roja Universidad de Oriente - Resultados de muestras enviadas del centro");
    mostrarUltimaPlaca();
    // CARGAR TABLA RESULTADOS PCR DEL CENTRO
    eliminarControladoresActivos();
    // Crear controlador para seccion Remisiones
    controladorResultadosPCR = new ResultadosPCR(this, ui->tabla, DataBaseManager::DBResultadosPCR);
    controladorActivo = DataBaseManager::DBResultadosPCR;
}


void VentanaPrincipal::on_actionSeleccion_triggered() {
    // IMPRIMIR CONTENIDO DE LA TABLA
    QTextDocument *document=new QTextDocument(this);
    QTextCursor cursor(document);
    cursor.movePosition(QTextCursor::Start);

    QTextTableCellFormat cellFormat;
    cellFormat.setLeftPadding(7);
    cellFormat.setRightPadding(7);
    cellFormat.setTopPadding(2);

    QBrush blackBrush(Qt::SolidPattern);
    QTextTableFormat tableFormat;
    tableFormat.setAlignment(Qt::AlignLeft);
    tableFormat.setBorderBrush(blackBrush);
    tableFormat.setBorder(0.5);
    tableFormat.setCellSpacing(0);
    tableFormat.setBorderStyle(QTextFrameFormat::BorderStyle_Solid);
    tableFormat.setWidth(QTextLength(QTextLength::PercentageLength, 100));
//    QTextTable *table = cursor.insertTable(100,10,tableFormat);





    QPrinter printer;
    printer.setOrientation(QPrinter::Landscape);
    printer.setPageMargins(10,10,10,50,QPrinter::Millimeter);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() != QDialog::Accepted)
        return;
    document->print(&printer);
}

void VentanaPrincipal::on_actionPantalla_completa_triggered() {
    if(windowState() == Qt::WindowFullScreen) {
        setWindowState(Qt::WindowMaximized);
    }
    else {
        setWindowState(Qt::WindowFullScreen);
    }
}

void VentanaPrincipal::on_actionSegmentacion_de_datos_triggered() {
    ui->barraHerramientas1Widget->setVisible(!ui->barraHerramientas1Widget->isVisible());
}

void VentanaPrincipal::on_actionBarra_de_estado_triggered() {
    ui->statusBar->setVisible(!ui->statusBar->isVisible());
}

void VentanaPrincipal::on_actionPreferencias_triggered() {

}

void VentanaPrincipal::on_actionCerrar_triggered() {
    this->close();
}

void VentanaPrincipal::on_actionAyuda_triggered() {
    // abrir archivo pdf de ayuda

    // si no se puede abrir el archivo mostrarla en un qDialog
}

void VentanaPrincipal::on_actionContacto_triggered() {
    QString body = "<h2>Zona Roja UO</h2>"
                   "<p>Puede localizarnos mediante cualquiera de las vias a continuacion mostradas:</p>"
                   "telefono: +53 58022621"
                   "<br>"
                   "<a href=\"https://gitlab.uo.edu.cu/carlos.rodriguezlo/zona-roja-uo\">Link del proyecto en GitLab</a>"
                   "<br>"
                   "<a href=\"mailto:carlosarl1999@gmail.com\">Correo de soporte</a>";
    QMessageBox::information(this, "Contacto del programador", body);
}

void VentanaPrincipal::on_actionAcerca_de_triggered() {
    QString body = "<h2>Zona Roja UO 1.0.0</h2>"
                   "<p>Compilado el 25 de Julio de 2021</p>"
                   "<p>Codigo fuente disponible en <a href=\"https://gitlab.uo.edu.cu/carlos.rodriguezlo/zona-roja-uo\">https://gitlab.uo.edu.cu/carlos.rodriguezlo/zona-roja-uo</a></p>";
    QMessageBox::about(this, "Acerca de la aplicacion", body);

}

void VentanaPrincipal::on_actionDeshacer_triggered() {
    if(controladorActivo == DataBaseManager::DBPacientes) {
        controladorPacientes->deshacer();
    }
    else if(controladorActivo == DataBaseManager::DBResultadosPCR) {

    }
}
