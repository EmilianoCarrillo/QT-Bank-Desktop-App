#include "cuentabancaria.h"

QString CuentaBancaria::getNum() const
{
    return num;
}

int CuentaBancaria::getTipo() const
{
    return tipo;
}

double CuentaBancaria::getSaldo() const
{
    return saldo;
}

QDate CuentaBancaria::getFechaApertura() const
{
    return fechaApertura;
}

Cliente *CuentaBancaria::getTitular() const
{
    return titular;
}

CuentaBancaria::CuentaBancaria(){}

CuentaBancaria::CuentaBancaria(QString num,  int tipo, double saldo, QDate fechaApertura, Cliente *titular):
    num(num), tipo(tipo), saldo(saldo), fechaApertura(fechaApertura){
    this->titular = titular;
}

void CuentaBancaria::popularModel(QStandardItemModel *model, int fila)
{
    QModelIndex index = model->index(fila, 0, QModelIndex());
    model->setData(index, QVariant::fromValue(num));
    index = model->index(fila, 1, QModelIndex());
    model->setData(index, QVariant::fromValue(titular->getNombre()));
    index = model->index(fila, 2, QModelIndex());
    model->setData(index, QVariant::fromValue(tipo));
    index = model->index(fila, 3, QModelIndex());
    model->setData(index, QVariant::fromValue(fechaApertura));
    index = model->index(fila, 4, QModelIndex());
    model->setData(index, QVariant::fromValue(saldo));

    QStandardItem *tipoItem = model->item(fila,2);
    QStandardItem *saldoItem = model->item(fila,4);

    switch(tipoItem->text().toInt()){
    case 0:
        tipoItem->setText("Crédito");
        saldoItem->setText("--");
        break;
    case 1:
        tipoItem->setText("Débito");
        saldoItem->setText("$" + saldoItem->text());
        break;
    case 2:
        tipoItem->setText("Cheques");
        saldoItem->setText("--");
        break;
    }
}


QTextStream &operator <<(QTextStream &outStream, const CuentaBancaria &cuenta)
{
   outStream << cuenta.num << endl;
   outStream << cuenta.titular->getNombre() << endl;
   outStream << cuenta.tipo << endl;
   outStream << cuenta.fechaApertura.toString("dd 'de' MMMM yyyy") << endl;
   outStream << cuenta.saldo << endl;
   return outStream;
}
