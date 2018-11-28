#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QtCore>
#include <QMessageBox>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setupUI();

    // CONNECTIONS
    connect(ui->nombreLineEdit, SIGNAL(textChanged(QString)), this, SLOT(validarAnadirCliente()));
    connect(ui->telefonoLineEdit, SIGNAL(textChanged(QString)), this, SLOT(validarAnadirCliente()));
    connect(ui->direccionLineEdit, SIGNAL(textChanged(QString)), this, SLOT(validarAnadirCliente()));
    connect(ui->hombreRadioButton, SIGNAL(toggled(bool)), this, SLOT(validarAnadirCliente()));
    connect(ui->mujerRadioButton, SIGNAL(toggled(bool)), this, SLOT(validarAnadirCliente()));
    connect(ui->numCuentaLineEdit, SIGNAL(textChanged(QString)), this, SLOT(validarAnadirCuenta()));

    // Llenar QMap de clientes -------------------------
    actualizarClientesList();
    actualizarCuentasList();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// NAVIGATION BUTTON SLOTS
void MainWindow::on_anadirCuentaButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    QString randomCuentaNum;
    for(int i = 0; i < 16; i++)
        randomCuentaNum += QString::number(qrand()%10);
    ui->numCuentaLineEdit->setText(randomCuentaNum);
}
void MainWindow::on_cancelarButton_clicked()
{
    ui->clientesComboBox->setCurrentIndex(0);
    ui->tipoCuentaComboBox->setCurrentIndex(0);
    ui->saldoLineEdit->text() = "";
    ui->dateEdit->setDate(QDate::currentDate());
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_anadirClienteButton_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}
void MainWindow::on_cancelarButton_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    borrarCamposAnadirCliente();
}
void MainWindow::on_regresarDetailsButton_clicked(){
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::on_verSeleccionButton_clicked(){
    QModelIndex index = ui->cuentasTable->currentIndex();
    int currentRow = index.row();
    QString titularNombreString = ui->cuentasTable->model()->data(ui->cuentasTable->model()->index(currentRow,1)).toString();
    QString cuentaNumeroString = ui->cuentasTable->model()->data(ui->cuentasTable->model()->index(currentRow,0)).toString();

    CuentaBancaria *cuentaSelccionada = &cuentas[cuentaNumeroString];
    Cliente *clienteSeleccionado = &clientes[titularNombreString];

    if (clienteSeleccionado->getSexo()){
        ui->sexoDetails->setText("Hombre");
    } else{
        ui->sexoDetails->setText("Mujer");
    }

    if(cuentaSelccionada->getTipo() == 1){
        ui->saldoDetails->show();
        ui->saldoLabelDetails->show();
        ui->tipoDetails->setText("Crédito");
    } else{
        if(cuentaSelccionada->getTipo() == 1)
            ui->tipoDetails->setText("Débito");
        else
            ui->tipoDetails->setText("Cheques");
        ui->saldoDetails->hide();
        ui->saldoLabelDetails->hide();
    }

    ui->numeroDetails->setText(cuentaSelccionada->getNum());
    ui->saldoDetails->setText("$" + QString::number(cuentaSelccionada->getSaldo()));
    ui->fechaDetails->setText(cuentaSelccionada->getFechaApertura().toString("dd 'de' MMMM yyyy"));
    ui->nombreDetails->setText(clienteSeleccionado->getNombre());
    ui->telefonoDetails->setText(clienteSeleccionado->getTelefono());
    ui->direccionDetails->setText(clienteSeleccionado->getDireccion());

    ui->stackedWidget->setCurrentIndex(3);
}

// ACEPTAR DIALOG SLOTS
void MainWindow::on_aceptarClienteButton_clicked()
{
    QString nombre = ui->nombreLineEdit->text();
    QString telefono = ui->telefonoLineEdit->text();
    QString direccion = ui->direccionLineEdit->text();
    bool sexo = (ui->hombreRadioButton->isChecked()? 1 : 0);
    QString sexoString = (sexo? "Hombre" : "Mujer");

    QMessageBox msgBox;
    msgBox.setText("Autorizar agregar a un nuevo cliente.");
    msgBox.setInformativeText(tr("<p>¿Seguro que desea agregar al nuevo cliente con la siguiente información?</p>")
                                 .append("<br><b>Nombre:</b> " + nombre)
                                 .append("<br><b>Telefono:</b> " + telefono)
                                 .append("<br><b>Direccion:</b> " + direccion)
                                 .append("<br><b>Sexo:</b> " + sexoString));

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes){
        Cliente clienteAAnadir(nombre, telefono, direccion, sexo);
        clientes.insert(nombre, clienteAAnadir);
        actualizarClientesBDFile();
        ui->clientesComboBox->setCurrentText(clientes[nombre].getNombre());
        ui->stackedWidget->setCurrentIndex(1);
        borrarCamposAnadirCliente();
    }
}
void MainWindow::borrarCamposAnadirCliente(){
    ui->nombreLineEdit->setText("");
    ui->telefonoLineEdit->setText("");
    ui->direccionLineEdit->setText("");
    ui->hombreRadioButton->setChecked(false);
    ui->mujerRadioButton->setChecked(false);
}
void MainWindow::on_aceptarCuentaButton_clicked()
{
    QString numero = ui->numCuentaLineEdit->text();
    Cliente cliente = clientes[ui->clientesComboBox->currentText()];
    int tipo = ui->tipoCuentaComboBox->currentIndex();
    double saldo = ui->saldoLineEdit->text().toDouble();
    QDate fecha = ui->dateEdit->date();

    QString tipoString;
    switch(tipo){
    case 0:
        tipoString = "Crédito";
        saldo = 0;
        break;
    case 1:
        tipoString = "Débito";
        break;
    case 2:
        tipoString = "Cheques";
        saldo = 0;
        break;
    }


    QMessageBox msgBox;
    msgBox.setText("Autorizar agregar nueva cuenta.");
    msgBox.setInformativeText(tr("<p>¿Seguro que desea agregar la nueva cuenta con la siguiente información?</p>")
                                 .append("<br><b>Número:</b> " + numero)
                                 .append("<br><b>Titular:</b> " + cliente.getNombre())
                                 .append("<br><b>Tipo:</b> " + tipoString)
                                 .append("<br><b>Saldo:</b> " + QString::number(saldo))
                                 .append("<br><b>Fecha de Apertura:</b> " + fecha.toString("dd 'de' MMMM yyyy")));

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes){
        CuentaBancaria cuentaAAnadir(numero, tipo, saldo, fecha, &cliente);
        cuentas.insert(numero, cuentaAAnadir);

        //TODO : añadir cuenta a QMap de cuentas del cliente
        actualizarCuentasBDFile();
        on_cancelarButton_clicked();
    }

}
void MainWindow::on_eliminarCuentaButton_clicked()
{
    QString numero = ui->numeroDetails->text();
    QString cliente = ui->nombreDetails->text();
    QString tipo = ui->tipoDetails->text();
    double saldo = ui->saldoDetails->text().toDouble();
    QString fecha = ui->fechaDetails->text();


    QMessageBox msgBox;
    msgBox.setText("Autorizar eliminación de cuenta.");
    msgBox.setInformativeText(tr("<p>¿Seguro que desea eliminar la cuenta con la siguiente información?</p>")
                                 .append("<br><b>Número:</b> " + numero)
                                 .append("<br><b>Titular:</b> " + cliente)
                                 .append("<br><b>Tipo:</b> " + tipo)
                                 .append("<br><b>Saldo:</b> " + QString::number(saldo))
                                 .append("<br><b>Fecha de Apertura:</b> " + fecha));

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes){
        cuentas.remove(numero);
        actualizarCuentasBDFile();
        actualizarCuentasList();
        ui->stackedWidget->setCurrentIndex(0);
    }
}
void MainWindow::on_eliminarTitularButton_clicked(){
    QString nombreCliente = ui->nombreDetails->text();
    QString cuentasString;
    QList<QString> cuentasAEliminar;

    foreach(CuentaBancaria cuenta, cuentas){
        if(cuenta.getTitular()->getNombre() == nombreCliente){
            cuentasString += ("<br>" + cuenta.getNum());
            cuentasAEliminar.append(cuenta.getNum());
        }
    }

    QMessageBox msgBox;
    msgBox.setText("Autorizar eliminación de cliente.");
    msgBox.setInformativeText(tr("<p>Al eliminar a este cliente se eliminarán todas las cuentas asociadas a él.<br>")
                         .append(   "¿Seguro que desea continuar?</p>")
                         .append("<b>Nombre:  </b>" + nombreCliente + "<br>")
                         .append("<b>Cuentas: </b>" + cuentasString)
                                 );

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes){
        clientes.remove(nombreCliente);
        foreach(QString nombreDeCuenta, cuentasAEliminar){
            cuentas.remove(nombreDeCuenta);
        }
        actualizarClientesBDFile();
        actualizarCuentasBDFile();
        actualizarClientesList();
        actualizarCuentasList();
        ui->stackedWidget->setCurrentIndex(0);
    }

}


// VALIDATION SLOTS
void MainWindow::validarAnadirCliente()
{
    if (   !ui->nombreLineEdit->text().isEmpty() &&
           !ui->telefonoLineEdit->text().isEmpty() &&
           !(ui->telefonoLineEdit->text().length() < 15) &&
           !ui->direccionLineEdit->text().isEmpty() &&
           (ui->hombreRadioButton->isChecked() ||
            ui->mujerRadioButton->isChecked())){
        ui->aceptarClienteButton->setEnabled(true);
    } else{
         ui->aceptarClienteButton->setEnabled(false);
    }
}
void MainWindow::on_tipoCuentaComboBox_currentTextChanged(const QString &arg1)
{
    if(arg1 == "Débito"){
        ui->saldoWidgetLayout->show();
    } else{
        ui->saldoWidgetLayout->hide();
    }
}
void MainWindow::validarAnadirCuenta(){
    if(ui->numCuentaLineEdit->text().length() < 19){
        ui->aceptarCuentaButton->setEnabled(false);
    }else{
        ui->aceptarCuentaButton->setEnabled(true);
    }
}

// Métodos útiles
void MainWindow::setupUI(){
    // Empezar la aplicación en el InicioPage ----------
    ui->stackedWidget->setCurrentIndex(0);
    // Input Validators and Masks
    ui->saldoLineEdit->setValidator(new QDoubleValidator(-999.0,999.0, 2, ui->saldoLineEdit));
    // Date Input
    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setMaximumDate(QDate::currentDate());
    // Saldo hide input
    ui->saldoWidgetLayout->hide();
}
void MainWindow::on_cuentasTable_clicked(){
    ui->verSeleccionButton->setEnabled(true);
}

//"BASES DE DATOS"
void MainWindow::actualizarClientesBDFile()
{
    QFile clientesBDFile("/Users/emilianocarrillo/Desktop/COIN_bank/BD/Clientes.txt");
    if (!clientesBDFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
       qDebug() << "ERROR: el archivo de clientes no está abierto.";
    }else{
        QTextStream stream(&clientesBDFile);
        foreach(Cliente cliente, clientes){
            stream << cliente;
        }
    }
    clientesBDFile.close();
    actualizarClientesList();
}
void MainWindow::actualizarClientesList()
{
    QFile clientesBDFile("/Users/emilianocarrillo/Desktop/COIN_bank/BD/Clientes.txt");
    int lineCount = 0;
    int attributeCount = 0;
    QString data;
    QString nom; QString tel; QString dir; bool sex;

    if(!clientesBDFile.open(QIODevice::ReadOnly)){
        qDebug() << "ERROR: El archivo de clientes no se pudo abrir.";
    } else{
        QTextStream in(&clientesBDFile);
        while(!in.atEnd())
        {
            attributeCount = lineCount % 4;
            data = in.readLine();

            if(attributeCount == 0){
                nom = data;
            }else if(attributeCount == 1){
                tel = data;
            }else if(attributeCount == 2){
                dir = data;
            }else if(attributeCount == 3){
                sex = data.toInt();
                Cliente clienteAAgregar(nom, tel, dir, sex);
                clientes.insert(nom, clienteAAgregar);
            }
            lineCount++;
        }
    }
    clientesBDFile.close();

    // Actualizar modelo
    clientesModel = new QStringListModel();
    QStringList clientesList;
    foreach(Cliente cliente, clientes){
        clientesList << cliente.getNombre();
    }
    clientesModel->setStringList(clientesList);

    ui->clientesComboBox->setModel(clientesModel);
}

void MainWindow::actualizarCuentasBDFile(){
    QFile cuentasBDFile("/Users/emilianocarrillo/Desktop/COIN_bank/BD/CuentasBancarias.txt");
    if (!cuentasBDFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)){
       qDebug() << "ERROR: el archivo de clientes no está abierto.";
    }else{
        QTextStream stream(&cuentasBDFile);
        foreach(CuentaBancaria cuenta, cuentas){
            stream << cuenta;
        }
    }
    cuentasBDFile.close();
    actualizarCuentasList();

}
void MainWindow::actualizarCuentasList(){
    QFile cuentasBDFile("/Users/emilianocarrillo/Desktop/COIN_bank/BD/CuentasBancarias.txt");
    int lineCount = 0;
    int attributeCount = 0;
    QString data;
    QString num; int tipo; QDate fecha; double saldo;
    Cliente *cliente = new Cliente;

    if(!cuentasBDFile.open(QIODevice::ReadOnly)){
        qDebug() << "ERROR: El archivo de cuentas bancarias no se pudo abrir.";
    } else{
        QTextStream in(&cuentasBDFile);
        while(!in.atEnd())
        {
            attributeCount = lineCount % 5;
            data = in.readLine();

            if(attributeCount == 0){
                num = data;
            }else if(attributeCount == 1){
                cliente = &clientes[data];
            }else if(attributeCount == 2){
                tipo = data.toInt();
            }else if(attributeCount == 3){
                fecha = QDate::fromString(data,"dd 'de' MMMM yyyy");
            }else if(attributeCount == 4){
                saldo = data.toDouble();
                CuentaBancaria cuentaAAgregar(num, tipo, saldo, fecha, cliente);
                cuentas.insert(num, cuentaAAgregar);
            }
            lineCount++;
        }
    }
    cuentasBDFile.close();

    // Crear y popular modelo
    cuentasModel = new QStandardItemModel(this);
    cuentasModel->setColumnCount(5);
    cuentasModel->setRowCount(lineCount/5);
    cuentasModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Num. de cuenta"));
    cuentasModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Titular"));
    cuentasModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Tipo"));
    cuentasModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Fecha de apertura"));
    cuentasModel->setHeaderData(4, Qt::Horizontal, QObject::tr("Saldo"));

    int index = 0;
    foreach(CuentaBancaria cuenta, cuentas){
        cuenta.popularModel(cuentasModel, index);
        index++;
    }

    ui->cuentasTable->setModel(cuentasModel);
}

//Filtro
void MainWindow::on_busqueda_textChanged(const QString &inputText)
{
    if(inputText == ""){
        ui->cuentasTable->setModel(cuentasModel);
        return;
    }

    auto filtroModel = new QStandardItemModel(this);
    filtroModel->setColumnCount(cuentasModel->columnCount());
    filtroModel->setRowCount(cuentasModel->rowCount());
    filtroModel->setHeaderData(0, Qt::Horizontal, QObject::tr("Num. de cuenta"));
    filtroModel->setHeaderData(1, Qt::Horizontal, QObject::tr("Titular"));
    filtroModel->setHeaderData(2, Qt::Horizontal, QObject::tr("Tipo"));
    filtroModel->setHeaderData(3, Qt::Horizontal, QObject::tr("Fecha de apertura"));
    filtroModel->setHeaderData(4, Qt::Horizontal, QObject::tr("Saldo"));

    int numFil = 0;
    int numColAtributo = ui->filtroComboBox->currentIndex();
    int numElemFiltrados = 0;

    foreach(CuentaBancaria cuenta, cuentas){
        QString contenido = cuentasModel->index(numFil, numColAtributo).data().toString();
        if(contenido.contains(inputText)){
            cuenta.popularModel(filtroModel, numElemFiltrados);
            numElemFiltrados++;
        }
        numFil++;
    }

    ui->cuentasTable->setModel(filtroModel);

}




