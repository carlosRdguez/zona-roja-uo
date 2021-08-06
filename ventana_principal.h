#ifndef VENTANA_PRINCIPAL_H
#define VENTANA_PRINCIPAL_H

#include <QMainWindow>
#include <QStringList>
#include <QPrintDialog>
#include <QTextDocument>
#include <QPrinter>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QMimeData>

#include <customtable.h>

#include "pacientes.h"
#include "resultadospcr.h"
#include "actualizador.h"
#include "databasemanager.h"

namespace Ui {
class VentanaPrincipal;
}

/**
 * @brief Clase VentanaPrincipal
 *
 * Clase de interfaz que controla la interfaz del sistema
 */

class VentanaPrincipal : public QMainWindow
{
    Q_OBJECT
protected:
    void dragEnterEvent(QDragEnterEvent *e);
    void dropEvent(QDropEvent *e);
public:
    explicit VentanaPrincipal(QWidget *parent = 0);
    ~VentanaPrincipal();
    // elimina todos los controladores activos
    void eliminarControladoresActivos();
    // muestra placa actual del dia
    void mostrarUltimaPlaca();
private slots:
    // este slot se activa cuando se estan actualizando valores en la base de datos y bloquea la app
    // para evitar errores; se espera que termine la actualizacion y se devuelveel control al usuario
    void nuevosResultadosDelCentro();
    // dispara cuando se terminan de actualizar las bases de datos posteriormente de arribar nuevos resultados de PCR al sistema
    void actualizacionFinalizada();
    // este slot se activa al detectar un correo con resultados nuevo, se deberia imposibilitar al usuario cerrar la app
    void descargaIniciada();

    void on_actionPacientes_triggered();

    void on_actionParte_triggered();

    void on_actionEgresos_triggered();

    void on_actionRemisiones_triggered();

    void on_actionDisponibilidad_triggered();

    void on_actionPlanificacion_PCR_triggered();

    void on_actionResultados_PCR_triggered();

    void on_actionSeleccion_triggered();

    void on_actionPantalla_completa_triggered();

    void on_actionSegmentacion_de_datos_triggered();

    void on_actionBarra_de_estado_triggered();

    void on_actionPreferencias_triggered();

    void on_actionCerrar_triggered();

    void on_actionAyuda_triggered();

    void on_actionContacto_triggered();

    void on_actionAcerca_de_triggered();


    void on_actionDeshacer_triggered();

private:

    Ui::VentanaPrincipal *ui;

    Actualizador* actualizador;
    DataBaseManager::Controlador controladorActivo;
    Pacientes* controladorPacientes = nullptr;
    ResultadosPCR* controladorResultadosPCR = nullptr;
    QMessageBox* blockingMsg;
    CustomTable* tabla;
    QString CSS = "QTableWidget{"
                  "     font-size: 12pt;"
                  "     selection-color: black;" // color del texto del item que se selecciona
                  "     border: 1px solid white;"
                  "     color: #2b2f33;"
                  "     border-radius: 5px;"
                  "}"
                  "QHeaderView{"
                  "     border: 1px solid gray;"
                  "     border-style: none none solid none;"
                  "     color: #495057;"
                  "     background-color: white;"
                  "}"
                  "QHeaderView::section:checked{"
                  "     background-color: white;"
                  "     border: 1px solid gray;"
                  "     border-style: none none none none;"
                  "     padding: 4px;"
                  "}"
                  "QHeaderView::section:hover{"
                  "     background-color: #eaeaea;"
                  "     border: 1px solid gray;"
                  "     border-style: none none none none;"
                  "     padding: 4px;"
                  "}"
                  "QTableWidget::item:selected{"
                  "     background-color: #999999;"
                  "}"
                  "QTableView{"
                  "     background-color: white;"
                  "}";
};

#endif // VENTANA_PRINCIPAL_H
