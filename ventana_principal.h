#ifndef VENTANA_PRINCIPAL_H
#define VENTANA_PRINCIPAL_H

#include <QMainWindow>
#include <QTableWidget>
#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QSqlTableModel>
#include <QSqlError>
#include <QMessageBox>
#include <QSet>
#include <QString>

namespace Ui {
class VentanaPrincipal;
}

class VentanaPrincipal : public QMainWindow
{
    Q_OBJECT

public:
    explicit VentanaPrincipal(QWidget *parent = 0);
    ~VentanaPrincipal();

    // crea la base de datos y sus tablas si no estan creadas
    bool crearBaseDeDatos();
    // retorna el nombre de la fila en base de datos correspondiente a index
    QString getRow(int index);
    // retorna la cantidad de campos no vacios en una fila
    int getNotNull(int row);

private slots:
    void on_actionPacientes_triggered();

    void on_actionParte_triggered();

    void on_actionEgresos_triggered();

    void on_actionRemisiones_triggered();

    void on_actionDisponibilidad_triggered();

    void on_actionPlanificacion_PCR_triggered();

    void on_actionResultados_PCR_triggered();

    void on_actionImprimir_triggered();

    void on_tabla_cellChanged(int row, int column);

    void on_tabla_cellClicked(int row, int column);

    void on_bloqueButton_clicked();

    void on_cubiculoButton_clicked();

    void on_resultadoButton_clicked();

    void on_fechaPCRButton_clicked();

    // este slot implementa la funcionalidad de los botones de segmentacion de datos
    // para bloque
    void bloqueSlot();

private:
    QString rutaPacientes = "E:/Projects/Qt projects/Zona_Roja_UO/Pacientes.db";
    Ui::VentanaPrincipal *ui;
    int numRows = 0;
    QString currentText = "";
    QFrame *frameBloque = nullptr, *frameCubiculo = nullptr, *frameResultado = nullptr, *frameFechaPCR = nullptr;
    bool toogleBloque=0, toogleCubiculo=0, toogleResultado=0, toogleFechaPCR=0;
};

#endif // VENTANA_PRINCIPAL_H
