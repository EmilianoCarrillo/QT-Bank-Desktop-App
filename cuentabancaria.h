#ifndef CUENTABANCARIA_H
#define CUENTABANCARIA_H

#include <QtCore>
#include <QStandardItemModel>
#include "cliente.h"

class CuentaBancaria{
private:
    QString num;
    int tipo;
    double saldo;
    QDate fechaApertura;
    Cliente *titular;

public:
    CuentaBancaria();
    CuentaBancaria(QString, int, double, QDate, Cliente*);
    void popularModel(QStandardItemModel* , int);
    friend QTextStream &operator <<(QTextStream &outStream, const CuentaBancaria  &cuenta);

    QString getNum() const;
    int getTipo() const;
    double getSaldo() const;
    QDate getFechaApertura() const;
    Cliente *getTitular() const;
    void setSaldo(double value);
};

#endif // CUENTABANCARIA_H
