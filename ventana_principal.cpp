#include "ventana_principal.h"
#include "ui_ventana_principal.h"

VentanaPrincipal::VentanaPrincipal(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::VentanaPrincipal)
{
    ui->setupUi(this);
    // CARGAR TABLA PACIENTES DEL CENTRO
    on_actionPacientes_triggered();
    // OCULTANDO ROWID DE LAS FILAS DE LA TABLA
    ui->tabla->hideColumn(24);
}

VentanaPrincipal::~VentanaPrincipal() {
    delete controladorPacientes;
    delete ui;
}

void VentanaPrincipal::eliminarControladoresActivos() {
    if(controladorPacientes != nullptr) {
        delete controladorPacientes;
        controladorPacientes = nullptr;
    }
}

void VentanaPrincipal::on_actionPacientes_triggered() {
    eliminarControladoresActivos();
    // Crear controlador para seccion Pacientes
    controladorPacientes = new Pacientes(this, ui->tabla);
}

void VentanaPrincipal::on_actionParte_triggered() {
    // VISUALIZAR PARTE DEL CENTRO
}

void VentanaPrincipal::on_actionEgresos_triggered() {
    // CARGAR TABLA EGRESOS DEL CENTRO
}

void VentanaPrincipal::on_actionRemisiones_triggered() {
    // CARGAR TABLA REMISIONES DEL CENTRO
}

void VentanaPrincipal::on_actionDisponibilidad_triggered() {
    // VISUALIZAR DISPONIBILIDAD DEL CENTRO
}

void VentanaPrincipal::on_actionPlanificacion_PCR_triggered() {
    // VISUALIZAR PLANIFICACION DE PCR DEL CENTRO
}

void VentanaPrincipal::on_actionResultados_PCR_triggered() {
    // VISUALIZAR RESULTADOS DE PCR DEL CENTRO
}

void VentanaPrincipal::on_actionImprimir_triggered() {
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
    QTextTable *table = cursor.insertTable(100,10,tableFormat);





    QPrinter printer;
    printer.setOrientation(QPrinter::Landscape);
    printer.setPageMargins(10,10,10,50,QPrinter::Millimeter);
    QPrintDialog *dlg = new QPrintDialog(&printer, this);
    if (dlg->exec() != QDialog::Accepted)
        return;
    document->print(&printer);



}
