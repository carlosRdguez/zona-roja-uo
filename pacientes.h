#ifndef PACIENTES_H
#define PACIENTES_H

#include <QApplication>
#include <QClipboard>

#include <QObject>
#include <QWidget>
#include <QDebug>
#include <QMainWindow>
#include <QGroupBox>
#include <QDate>

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
#include <QKeySequence>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QModelIndex>
#include <QModelIndexList>
#include <QItemSelectionModel>

#include <QFontDatabase>
#include <QGraphicsDropShadowEffect>



#include <databasemanager.h>

typedef QList<QStringList> Tabla;

#define ROJO QColor(152, 22, 40, 80)
#define VERDE QColor(37, 132, 64, 100)
#define AZUL QColor(22, 69, 132, 50)
#define GRIS QColor(145, 145, 145, 80)


/**
 * @brief Clase Pacientes
 *
 * Controlador del comportamiento de la tabla que muestra la informacion de
 * las bases de datos Pacientes.db, Egresos.db y Remisiones.db
 */

class Pacientes : public QObject
{
    Q_OBJECT
protected :
    bool eventFilter(QObject *watched, QEvent *event);
public:
    explicit Pacientes(QObject *parent = nullptr, QTableWidget* view = nullptr, DataBaseManager::Controlador cont = DataBaseManager::DBPacientes);
    // crea la base de datos y sus tablas si no estan creadas
    bool crearBaseDeDatos();
    // carga la tabla con el comando sql deseado
    void cargarTabla(QString sql);
    // da color a un item de la tabla
    void darColor(int row = -1, int column = -1);
    // retorna el nombre de la fila en base de datos correspondiente a index
    QString getRow(int index);
    // retorna todos los campos de la fila
    QStringList valoresFila(int fila);
    // retorna la cantidad de campos no vacios en una fila (esta funcion se emplea para determinar si se esta eliminando
    // el ultimo campo con valores de una fila; con ello se procede a la eliminacion del registro de la base de datos)
    int getNotNull(int row);
    // filtra la informacion mostrada en tabla (todos los criterios son separados por operadores and)
    void filtrar();
    // marca completamente las filas de una seleccion de la tabla
    void marcarFilas();
    // deshacer cambio en base de datos
    void deshacer();
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
    // filtra los pacientes que estan en determinado cubiculo
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
    // slot de los action del context menu
    void actionMarcar();

    void actionAgregarListaRemision();

    void actionAgregarListaEgresos();

    void actionEliminar();
    // slot para filtros interactivos
    void filtroInteractivo(QString text);
private:
    // funciones privadas
    void crearMenu();
    void crearEncabezadoFiltrosInteractivos();
    void seccionSegmentacionDeDatos();
    DataBaseManager* dbm; // controlador de la base de datos
    QMainWindow* parentWidget = nullptr; // parent Widget, este se podria obtener mediante QObject::parent(), pero hay que castear muchas veces
    QTableWidget* tabla; // tabla que esta clase controla
    // elementos visuales
    QFrame *frameBloque = nullptr, *frameCubiculo = nullptr, *frameResultado = nullptr, *frameFechaPCR = nullptr;
    QPushButton *bloqueButton, *cubiculoButton, *resultadoButton, *fechaPCRButton;
    QLabel *bloqueLabel, *cubiculoLabel, *resultadoLabel, *fechaPCRLabel;
    QPushButton* limpiarBloque, *limpiarCubiculo, *limpiarResultado, *limpiarFechaPCR;
    // indicadores
    QString **criterios; // criterios para filtrar, puede ser un array de 24 o 25 elementos
    QString *textoSegmentacionResultado = nullptr;
    QString *textoSegmentacionFechaPCR = nullptr;
    QVBoxLayout *bloquelayOut, *cubiculolayOut, *resultadolayOut, *fechaPCRlayOut;
    QHBoxLayout *indicadorFiltroBloque, *indicadorFiltroCubiculo, *indicadorFiltroResultado, *indicadorFiltroFechaPCR;
    QMenu* menu;
    int filtroActivo = 0;
    int scrollBarValue = 0;
    int numRows = 0;
    QString currentText = "";
    QStringList verticalHeaderLabels;
    bool desactivarRenderizado = false;
    int lastRowidBeforeCommit;
};

#endif // PACIENTES_H
