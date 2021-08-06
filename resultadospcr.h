#ifndef RESULTADOSPCR_H
#define RESULTADOSPCR_H

#include <QApplication>
#include <QClipboard>

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
#include <QKeySequence>
#include <QContextMenuEvent>
#include <QMenu>
#include <QAction>
#include <QModelIndex>
#include <QModelIndexList>
#include <QItemSelectionModel>
#include <QPair>

#include <QGraphicsDropShadowEffect>


#include <databasemanager.h>

typedef QList<QStringList> Tabla;


#define ROJO QColor(152, 22, 40, 80)
#define VERDE QColor(37, 132, 64, 100)
#define AZUL QColor(22, 69, 132, 50)
#define GRIS QColor(145, 145, 145, 80)

/**
 * @brief Clase ResultadosPCR
 *
 * Controlador del comportamiento de la tabla que muestra la informacion de
 * las bases de datos ResultadosPCR.db
 */

class ResultadosPCR : public QObject
{
    Q_OBJECT
protected :
    bool eventFilter(QObject *watched, QEvent *event);
public:
    explicit ResultadosPCR(QObject *parent = nullptr, QTableWidget* view = nullptr, DataBaseManager::Controlador cont = DataBaseManager::DBResultadosPCR);
    // crea la base de datos y sus tablas si no estan creadas
    bool crearBaseDeDatos();
    // carga la tabla con el comando sql deseado
    void cargarTabla(QString sql);
    // da color a un item de la tabla
    void darColor(int row = -1, int column = -1);
    // retorna el nombre de la fila en base de datos correspondiente a index
    QString getRow(int index);
    // retorna la cantidad de campos no vacios en una fila (esta funcion se emplea para determinar si se esta eliminando
    // el ultimo campo con valores de una fila; con ello se procede a la eliminacion del registro de la base de datos)
    int getNotNull(int row);
    // filtra la informacion mostrada en tabla (todos los criterios son separados por operadores and)
    void filtrar();
    // marca completamente las filas de una seleccion de la tabla
    void marcarFilas();
    ~ResultadosPCR();
signals:

private slots:

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
    // slot de los action del context menu

    void actionAgregarListaRemision();

    void actionAgregarListaEgresos();

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
    QVBoxLayout *bloquelayOut, *cubiculolayOut, *resultadolayOut, *fechaPCRlayOut;
    QHBoxLayout *indicadorFiltroBloque, *indicadorFiltroCubiculo, *indicadorFiltroResultado, *indicadorFiltroFechaPCR;
    // indicadores
    QString **criterios; // criterios para filtrar, puede ser un array de 24 o 25 elementos
    QMenu* menu;
    int filtroActivo = 0;
    int scrollBarValue = 0;
    int numRows = 0;
    QString currentText = "";
    QList<QStringList> resultados;
};

#endif // RESULTADOSPCR_H
