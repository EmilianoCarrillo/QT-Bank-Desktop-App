#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include <QtCore>
#include <QMessageBox>
#include <QPrinter>
#include <QTextDocument>
#include <QInputDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this); 

    QWidget::setWindowTitle ("banco MX.");
    QPixmap logo(":/assets/assets/loginLogo.png");
    ui->logo->setPixmap(logo.scaled(90,90, Qt::KeepAspectRatio));
    QPixmap card(":/assets/assets/card.png");
    ui->cardImage->setPixmap(card.scaled(300,300, Qt::KeepAspectRatio));
    QPixmap logoEnInicio(":/assets/assets/logo.png");
    ui->logoInicio->setPixmap(logoEnInicio.scaled(90,90, Qt::KeepAspectRatio));

    ui->usuarioLineEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);
    ui->contrasenaLineEdit->setAttribute(Qt::WA_MacShowFocusRect, 0);

    // Empezar la aplicación en el InicioPage ----------
    ui->stackedWidget->setCurrentIndex(4);
    ui->errorLoginLabel->hide();

    // CONNECTIONS
    connect(ui->nombreLineEdit, SIGNAL(textChanged(QString)), this, SLOT(validarAnadirCliente()));
    connect(ui->telefonoLineEdit, SIGNAL(textChanged(QString)), this, SLOT(validarAnadirCliente()));
    connect(ui->direccionLineEdit, SIGNAL(textChanged(QString)), this, SLOT(validarAnadirCliente()));
    connect(ui->hombreRadioButton, SIGNAL(toggled(bool)), this, SLOT(validarAnadirCliente()));
    connect(ui->mujerRadioButton, SIGNAL(toggled(bool)), this, SLOT(validarAnadirCliente()));
    connect(ui->numCuentaLineEdit, SIGNAL(textChanged(QString)), this, SLOT(validarAnadirCuenta()));
    connect(ui->stackedWidget, SIGNAL(currentChanged(int)), this, SLOT(validarAnadirCuenta()));
    connect(ui->usuarioLineEdit, SIGNAL(textChanged(QString)), this, SLOT(validarCamposLogin()));
    connect(ui->contrasenaLineEdit, SIGNAL(textChanged(QString)), this, SLOT(validarCamposLogin()));

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

    if(cuentaSelccionada->getTipo() == 0){
        ui->saldoDetails->hide();
        ui->saldoLabelDetails->hide();
        ui->tipoDetails->setText("Crédito");
        ui->editarSaldoButton->hide();
    } else{
        if(cuentaSelccionada->getTipo() == 1)
            ui->tipoDetails->setText("Débito");
        else
            ui->tipoDetails->setText("Cheques");
        ui->saldoDetails->show();
        ui->saldoLabelDetails->show();
        ui->editarSaldoButton->show();
    }

    ui->numeroDetails->setText(cuentaSelccionada->getNum());
    ui->numeroDetails_2->setText(cuentaSelccionada->getNum());
    ui->saldoDetails->setText("$" + QString::number(cuentaSelccionada->getSaldo()));
    ui->fechaDetails->setText(cuentaSelccionada->getFechaApertura().toString("dd 'de' MMMM yyyy"));
    ui->nombreDetails->setText(clienteSeleccionado->getNombre());
    ui->nombreDetails_2->setText(clienteSeleccionado->getNombre());
    ui->telefonoDetails->setText(clienteSeleccionado->getTelefono());
    ui->direccionDetails->setText(clienteSeleccionado->getDireccion());

    ui->stackedWidget->setCurrentIndex(3);
}
void MainWindow::on_imprimirReporteButton_clicked(){

    QMessageBox::information(this, "Operación exitosa", "Se le generó un archivo de Reportes en formato pdf de manera exitosa en el folder del proyecto.",
                                QMessageBox::Ok);

    QString cuentasString, clientesString;

    foreach(CuentaBancaria cuenta, cuentas){

        QString tipoString;
        QString saldoString;

        switch(cuenta.getTipo()){
        case 0:
            tipoString = "Crédito";
            saldoString = "--";
            break;
        case 1:
            tipoString = "Débito";
            saldoString = "$" + QString::number(cuenta.getSaldo());
            break;
        case 2:
            tipoString = "Cheques";
            saldoString = "--";
            break;
        }

        cuentasString += ""
                + cuenta.getNum() + " | "
                "<span style=\"color:grey\">"
                + cuenta.getTitular()->getNombre().remove(10,50) + "...  | "
                + tipoString + " |  "
                + cuenta.getFechaApertura().toString("dd/MM/yy") + "  | "
                + saldoString +
                "</span>"
                + "<br> <br>";
    }

    foreach(Cliente cliente, clientes){

        QString sexoString = (cliente.getSexo() ? "Hombre" : "Mujer");

        clientesString += ""
                "<h3>" + cliente.getNombre() + "</h3>"
                "Teléfono:    " + cliente.getTelefono() + "<br>"
                "Dirección:   " + cliente.getDireccion() + "<br>"
                "Sexo:           " + sexoString + "<br>"
                "<hr>";
    }

    QString html = ""
            "<div align=right>"
                "Reporte generado el " + QDate::currentDate().toString("dd 'de' MMMM 'del' yyyy") +
            "</div>"
            "<div>"
                "<img width=100 align=left src=\":/assets/assets/logo.png\" alt=\"COIN bank\"></img><br>"
                "<h2 align=center>REPORTE DE CUENTAS BANCARIAS</h2>"
                "<hr>"
                "<p style=\"white-space:pre\">"
                    "<span style=\"background:'#E8E8E8'\">NÚM. DE CUENTA     |     TITULAR    |   TIPO   |   FECHA   |   SALDO     </span><br><br>"
                    + cuentasString +
                "</p>"
                "<hr><br>"
                "Fecha de reporte: " + QDate::currentDate().toString("dd 'de' MMMM 'del' yyyy") +
            "</div>"

            "<div style=\"page-break-before:always\">"
                "<img width=100  align=left src=\":/assets/assets/logo.png\" alt=\"COIN bank\"></img><br>"
            "</div>"
            "<div>"
                "<h2 align=center>REPORTE DE CLIENTES</h2>"
                "<hr>"
                "<p style=\"white-space:pre\">"
                    + clientesString +
                "</p>"
                "<hr><br>"
                "<div align=right>"
                    "Reporte generado el " + QDate::currentDate().toString("dd 'de' MMMM 'del' yyyy") +
                "</div>"
            "</div>"
                   "";

    QTextDocument document;
    document.setHtml(html);

    QPrinter printer(QPrinter::PrinterResolution);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setPaperSize(QPrinter::A4);
    printer.setOutputFileName("/Users/emilianocarrillo/Desktop/COIN_bank/BD/Reporte.pdf");
    printer.setPageMargins(QMarginsF(15, 15, 15, 15));
    document.print(&printer);
    printer.newPage();
    document.print(&printer);


}
void MainWindow::on_cerrarSesionButton_clicked(){
    ui->stackedWidget->setCurrentIndex(4);
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
        auto clienteAAnadir = new Cliente(nombre, telefono, direccion, sexo);
        clientes.insert(nombre, *clienteAAnadir);
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
    Cliente *cliente = &clientes[ui->clientesComboBox->currentText()];
    int tipo = ui->tipoCuentaComboBox->currentIndex();
    double saldo = ui->saldoLineEdit->text().toDouble();
    QDate fecha = ui->dateEdit->date();
    QString saldoString = QString::number(saldo);

    QString tipoString;
    switch(tipo){
    case 0:
        tipoString = "Crédito";
        saldoString = "-";
        break;
    case 1:
        tipoString = "Débito";
        break;
    case 2:
        tipoString = "Cheques";
        break;
    }


    QMessageBox msgBox;
    msgBox.setText("Autorizar agregar nueva cuenta.");
    msgBox.setInformativeText(tr("<p>¿Seguro que desea agregar la nueva cuenta con la siguiente información?</p>")
                                 .append("<br><b>Número:</b> " + numero)
                                 .append("<br><b>Titular:</b> " + cliente->getNombre())
                                 .append("<br><b>Tipo:</b> " + tipoString)
                                 .append("<br><b>Saldo:</b> " + saldoString)
                                 .append("<br><b>Fecha de Apertura:</b> " + fecha.toString("dd 'de' MMMM yyyy")));

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes){
        auto cuentaAAnadir = new CuentaBancaria(numero, tipo, saldo, fecha, cliente);
        cliente->insertCuenta(numero, *cuentaAAnadir);
        cuentas.insert(numero, *cuentaAAnadir);

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
        clientes[cliente].eliminarCuenta(numero);
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
void MainWindow::on_ingresaButton_clicked(){

    int validarIngreso(QString, QString);
    QString usuario = ui->usuarioLineEdit->text();
    QString contraseña = ui->contrasenaLineEdit->text();
    ui->errorLoginLabel->hide();

    int exito = validarIngreso(usuario, contraseña);
    QString bienvenida = (exito == 1) ? "GERENTE. Bienvenido, Emiliano.": "";
    bienvenida = (exito == 2) ? "EMPLEADO. Bienvenido, Ángel.": bienvenida;


    if(!exito){
        ui->errorLoginLabel->show();
    } else{
        ui->bienvenidaLabel->setText(bienvenida);
        ui->stackedWidget->setCurrentIndex(0);
        setupUI(exito);
    }

}
void MainWindow::on_eliminarCuentaButtonPequeo_clicked()
{
    QModelIndex index = ui->cuentasTable->currentIndex();
    int currentRow = index.row();
    QString titularNombreString = ui->cuentasTable->model()->data(ui->cuentasTable->model()->index(currentRow,1)).toString();
    QString cuentaNumeroString = ui->cuentasTable->model()->data(ui->cuentasTable->model()->index(currentRow,0)).toString();
    QString tipoString = ui->cuentasTable->model()->data(ui->cuentasTable->model()->index(currentRow,2)).toString();
    QString saldoString = ui->cuentasTable->model()->data(ui->cuentasTable->model()->index(currentRow,4)).toString();
    QString fechaString = ui->cuentasTable->model()->data(ui->cuentasTable->model()->index(currentRow,3)).toString();

    QMessageBox msgBox;
    msgBox.setText("Autorizar eliminación de cuenta.");
    msgBox.setInformativeText(tr("<p>¿Seguro que desea eliminar la cuenta con la siguiente información?</p>")
                                 .append("<br><b>Número:</b> " + cuentaNumeroString)
                                 .append("<br><b>Titular:</b> " + titularNombreString)
                                 .append("<br><b>Tipo:</b> " + tipoString)
                                 .append("<br><b>Saldo:</b> " + saldoString)
                                 .append("<br><b>Fecha de Apertura:</b> " + fechaString));

    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Yes);
    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes){
        cuentas.remove(cuentaNumeroString);
        clientes[titularNombreString].eliminarCuenta(cuentaNumeroString);
        actualizarCuentasBDFile();
        actualizarCuentasList();
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
    if(arg1 == "Crédito"){
        ui->saldoWidgetLayout->hide();
    } else{
        ui->saldoWidgetLayout->show();
    }
}
void MainWindow::validarAnadirCuenta(){
    if(ui->numCuentaLineEdit->text().length() < 19 || ui->clientesComboBox->currentText() == ""){
        ui->aceptarCuentaButton->setEnabled(false);
    }else{
        ui->aceptarCuentaButton->setEnabled(true);
    }
}
void MainWindow::validarCamposLogin(){
    if(ui->usuarioLineEdit->text() != ""){
        ui->usuarioLineEdit->setStyleSheet("#usuarioLineEdit{ border-bottom : 2px solid #BAD296; }");
    } else {
        ui->usuarioLineEdit->setStyleSheet("#usuarioLineEdit{ border-bottom : 2px solid #E3EBF5; }");
    }

    if(ui->contrasenaLineEdit->text() != ""){
        ui->contrasenaLineEdit->setStyleSheet("#contrasenaLineEdit{ border-bottom : 2px solid #BAD296; }");
    } else {
        ui->contrasenaLineEdit->setStyleSheet("#contrasenaLineEdit{ border-bottom : 2px solid #E3EBF5; }");
    }

    if(ui->usuarioLineEdit->text() == "" || ui->contrasenaLineEdit->text() == ""){
        ui->ingresaButton->setEnabled(false);
    } else {
        ui->ingresaButton->setEnabled(true);
    }
}

// Métodos útiles
void MainWindow::setupUI(int tipoUsuario){

    if(tipoUsuario == 2){
        ui->eliminarCuentaButtonPequeo->hide();
        ui->anadirCuentaButton->hide();
        ui->eliminarTitularButton->hide();
        ui->eliminarCuentaButton->hide();
        ui->editarSaldoButton->show();
        ui->editarDatosButton->hide();
    } else{
        ui->eliminarCuentaButtonPequeo->show();
        ui->anadirCuentaButton->show();
        ui->eliminarTitularButton->show();
        ui->eliminarCuentaButton->show();
        ui->editarSaldoButton->hide();
        ui->editarDatosButton->show();
    }

    // Login clear text
    ui->usuarioLineEdit->setText("");
    ui->contrasenaLineEdit->setText("");
    // Input Validators and Masks
    ui->saldoLineEdit->setValidator(new QDoubleValidator(-999.0,999.0, 2, ui->saldoLineEdit));
    // Date Input
    ui->dateEdit->setDate(QDate::currentDate());
    ui->dateEdit->setMinimumDate(QDate::currentDate());
    // Saldo hide input
    ui->saldoWidgetLayout->hide();
}
void MainWindow::on_cuentasTable_clicked(){
    ui->verSeleccionButton->setEnabled(true);
    ui->eliminarCuentaButtonPequeo->setEnabled(true);
}
int validarIngreso(QString usuario, QString contraseña){
    if(usuario == "emiliano.carrillo"){
        if(contraseña == "gerente"){
            return 1;
        }
    } else if(usuario == "angelGenis10"){
        if(contraseña == "empleado"){
            return 2;
        }
    }
    return 0;
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

void MainWindow::on_editarDatosButton_clicked()
{
    int fila = ui->cuentasTable->currentIndex().row();
    QString titularNombre = ui->cuentasTable->model()->data(ui->cuentasTable->model()->index(fila,1)).toString();
    QString cuentaNumero = ui->cuentasTable->model()->data(ui->cuentasTable->model()->index(fila,0)).toString();

    Cliente clienteAEditar = clientes[titularNombre];
    CuentaBancaria cuentaAEditar = cuentas[cuentaNumero];

    QString tipoString;

    switch(cuentaAEditar.getTipo()){
    case 0:
        tipoString = "Crédito";
        ui->saldoEdicion->hide();
        ui->saldoLabelEdit->hide();
        break;
    case 1:
        tipoString = "Débito";
        ui->saldoEdicion->show();
        ui->saldoLabelEdit->show();
        break;
    case 2:
        tipoString = "Cheques";
        ui->saldoEdicion->hide();
        ui->saldoLabelEdit->hide();
        break;
    }

    ui->numeroEdicion->setText(cuentaAEditar.getNum());
    ui->tipoEdicion->setText(tipoString);
    ui->saldoEdicion->setText(QString::number(cuentaAEditar.getSaldo()));
    ui->fechaEdicion->setDate(cuentaAEditar.getFechaApertura());
    ui->nombreEdicion->setText(clienteAEditar.getNombre());
    ui->telefonoEdicion->setText(clienteAEditar.getTelefono());
    ui->direccionEdicion->setText(clienteAEditar.getDireccion());
    ui->hombreEdicion->setChecked(clienteAEditar.getSexo());
    ui->mujerEdicion->setChecked(!clienteAEditar.getSexo());

    ui->stackedWidget->setCurrentIndex(5);

}

void MainWindow::on_guardarCambiosButton_clicked()
{
    int fila = ui->cuentasTable->currentIndex().row();
    QString titularNombre = ui->cuentasTable->model()->data(ui->cuentasTable->model()->index(fila,1)).toString();
    QString cuentaNumero = ui->cuentasTable->model()->data(ui->cuentasTable->model()->index(fila,0)).toString();

    Cliente clienteAEditar = clientes[titularNombre];
    CuentaBancaria cuentaAEditar = cuentas[cuentaNumero];

    bool esHombre = (ui->hombreEdicion->isChecked()) ?  true : false;

    clientes[titularNombre].setAll(ui->nombreEdicion->text(),ui->telefonoEdicion->text(),ui->direccionEdicion->text(), esHombre);

    CuentaBancaria *cuenta = new CuentaBancaria(cuentas[cuentaNumero].getNum(), cuentas[cuentaNumero].getTipo(), ui->saldoEdicion->text().toDouble(), ui->fechaEdicion->date(), &clientes[ui->nombreEdicion->text()]);


    cuentas.insert(cuentas[cuentaNumero].getNum(), *cuenta);

    actualizarClientesBDFile();
    actualizarCuentasBDFile();
    actualizarClientesList();
    actualizarCuentasList();

    ui->stackedWidget->setCurrentIndex(0);


}

void MainWindow::on_cancelarEdicionButton_clicked()
{
    int ret =  QMessageBox::warning(this, tr("Salir sin guardar"),
                       tr("¿Está seguro de que desea salir sin guardar?\n"),
                            QMessageBox::Cancel|QMessageBox::Yes);

    if(ret ==  QMessageBox::Yes){
        ui->stackedWidget->setCurrentIndex(3);
    }

}

void MainWindow::on_editarSaldoButton_clicked()
{
    bool ok;
    QString nuevoSaldo = QInputDialog::getText(this, tr("Editar saldo de cuenta"),
                                             "Saldo actual: " + ui->saldoDetails->text() + "\nNuevo saldo:", QLineEdit::Normal,
                                             "", &ok);
    if (ok && !nuevoSaldo.isEmpty()){
        cuentas[ui->numeroDetails->text()].setSaldo(nuevoSaldo.toDouble());
        ui->saldoDetails->setText(nuevoSaldo);
        actualizarCuentasBDFile();
        actualizarCuentasList();
    }
}

void MainWindow::on_cuentasTable_doubleClicked(const QModelIndex &index)
{
    on_verSeleccionButton_clicked();
}

