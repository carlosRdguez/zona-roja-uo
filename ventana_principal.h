#ifndef VENTANA_PRINCIPAL_H
#define VENTANA_PRINCIPAL_H

#include <QMainWindow>
#include <QStringList>
#include <QPrintDialog>
#include <QTextDocument>
#include <QPrinter>

#include "pacientes.h"

namespace Ui {
class VentanaPrincipal;
}

class VentanaPrincipal : public QMainWindow
{
    Q_OBJECT

public:
    explicit VentanaPrincipal(QWidget *parent = 0);
    ~VentanaPrincipal();
    // elimina todos los controladores activos
    void eliminarControladoresActivos();
private slots:
    void on_actionPacientes_triggered();

    void on_actionParte_triggered();

    void on_actionEgresos_triggered();

    void on_actionRemisiones_triggered();

    void on_actionDisponibilidad_triggered();

    void on_actionPlanificacion_PCR_triggered();

    void on_actionResultados_PCR_triggered();

    void on_actionImprimir_triggered();

private:

    Ui::VentanaPrincipal *ui;
    Pacientes* controladorPacientes = nullptr;

};

#endif // VENTANA_PRINCIPAL_H
