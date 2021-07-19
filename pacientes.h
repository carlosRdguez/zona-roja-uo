#ifndef PACIENTES_H
#define PACIENTES_H

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QMainWindow>
#include <QGroupBox>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QString>
#include <QStringList>

#include <QTableWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QColor>
#include <QHeaderView>
#include <QLineEdit>
#include <QScrollBar>

#include <QEvent>
#include <QKeyEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QModelIndexList>
#include <QItemSelectionModel>


#define ROJO "#a01127"
#define VERDE "#4f8e3a"
#define AZUL "#4a72b2"
#define GRIS "#87868a"



class Pacientes : public QObject
{
    Q_OBJECT
protected :
    bool eventFilter(QObject *watched, QEvent *event);
public:
    explicit Pacientes(QObject *parent = nullptr, QTableWidget* view = nullptr);
    // crea la base de datos y sus tablas si no estan creadas
    bool crearBaseDeDatos();
    // carga la tabla con el comando sql deseado
    void cargarTabla(QString sql);
    // retorna el nombre de la fila en base de datos correspondiente a index
    void darColor(int row = -1, int column = -1);
    QString getRow(int index);
    // retorna la cantidad de campos no vacios en una fila
    int getNotNull(int row);
    // filtra la informacion mostrada en tabla (todos los criterios son separados por operadores and)
    void filtrar();
    ~Pacientes();
signals:

private slots:
    void on_tabla_cellChanged(int row, int column);

    void on_tabla_cellClicked(int row, int column);

    // slots para desplegar cuadro de seleccion de segmentacion de datos
    void on_bloqueButton_clicked();

    void on_cubiculoButton_clicked();

    void on_resultadoButton_clicked();

    void on_fechaPCRButton_clicked();

    // slots para manejar comportamiento de botones de seleccion de segmentacion de datos
    void opcionSegmentacionBloqueSeleccionada();

    void opcionSegmentacionCubiculoSeleccionada();
    // filtra todos los pacientes que tienen un resultado actual, se determina por el color de la fila
    void opcionSegmentacionResultadoSeleccionada();
    // filtra todos los pacientes que tienen un resultado dado en el resultado1 || resultado2 || resultado3
    void opcionSegmentacionFechaPCRSeleccionada();

    // slots para limpiar filtros de segmentacion
    void limpiarSegmentacionBloque();

    void limpiarSegmentacionCubiculo();

    void limpiarSegmentacionResultado();

    void limpiarSegmentacionFechaPCR();
    // slots de los action del context menu
    void actionPositivo();

    void actionNegativo();

    void actionRepetirMuestra();

    void actionAgregarListaRemision();

    void actionAgregarListaEgresos();

    void actionEliminar();
    // slot para filtros interactivos
    void filtroInteractivo(QString text);
private:
    // funciones privadas
    void crearMenu();
    void crearEncabezadoFiltrosInteractivos();
    // atributos
    QString rutaPacientes = "E:/Projects/Qt projects/Zona_Roja_UO/Pacientes.db";
    QMainWindow* parentWidget = nullptr;
    QTableWidget* tabla;
    int numRows = 0;
    QString currentText = "";
    QFrame *frameBloque = nullptr, *frameCubiculo = nullptr, *frameResultado = nullptr, *frameFechaPCR = nullptr;
    QPushButton *bloqueButton, *cubiculoButton, *resultadoButton, *fechaPCRButton;
    QLabel *bloqueLabel, *cubiculoLabel, *resultadoLabel, *fechaPCRLabel;
    QPushButton* limpiarBloque, *limpiarCubiculo, *limpiarResultado, *limpiarFechaPCR;
    QString *criterios[24]; // criterios de filtrado (24 columnas)
    QString *textoSegmentacionResultado = nullptr;
    QString *textoSegmentacionFechaPCR = nullptr;
    QMenu* menu;
    int filtroActivo = 0;
    int scrollBarValue = 0;
};

#endif // PACIENTES_H
