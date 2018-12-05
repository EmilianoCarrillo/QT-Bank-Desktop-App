#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QtCore>
#include"cuentabancaria.h"
#include"cliente.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    // Métodos útiles
    void setupUI(int);
    void borrarCamposAnadirCliente();
    // "Base de Datos"
    void actualizarClientesBDFile();
    void actualizarClientesList();
    void actualizarCuentasBDFile();
    void actualizarCuentasList();

private slots:
    // Buttons
    void on_anadirCuentaButton_clicked();
    void on_cancelarButton_clicked();
    void on_anadirClienteButton_clicked();
    void on_cancelarButton_2_clicked();
    void on_aceptarClienteButton_clicked();
    void on_aceptarCuentaButton_clicked();
    //Validar campos
    void validarAnadirCliente();
    void validarAnadirCuenta();
    void on_tipoCuentaComboBox_currentTextChanged(const QString &arg1);
    void validarCamposLogin();

    void on_cuentasTable_clicked();
    void on_regresarDetailsButton_clicked();
    void on_verSeleccionButton_clicked();
    void on_eliminarCuentaButton_clicked();
    void on_busqueda_textChanged(const QString &arg1);
    void on_eliminarTitularButton_clicked();
    void on_imprimirReporteButton_clicked();
    void on_ingresaButton_clicked();
    void on_cerrarSesionButton_clicked();

    void on_eliminarCuentaButtonPequeo_clicked();

    void on_editarDatosButton_clicked();

    void on_guardarCambiosButton_clicked();

    void on_cancelarEdicionButton_clicked();

    void on_editarSaldoButton_clicked();

    void on_cuentasTable_doubleClicked(const QModelIndex &index);


private:
    Ui::MainWindow *ui;
    QStandardItemModel *cuentasModel;
    QStringListModel *clientesModel;
    //QMap porque QHash no ordena, lo hace arbitrario.
    //QMap es un contenedor que automáticamente ordena ascendentemente por key.
    QMap<QString, CuentaBancaria> cuentas;
    QMap<QString, Cliente> clientes;


};

#endif // MAINWINDOW_H
