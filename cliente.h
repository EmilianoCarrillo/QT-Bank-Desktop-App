#ifndef CLIENTE_H
#define CLIENTE_H

#include <QtCore>

class CuentaBancaria;
class Cliente
{
private:
    QString nombre;
    QString telefono;
    QString direccion;
    bool sexo;
    QMap<QString, CuentaBancaria> *cuentas;

public:
    Cliente();
    Cliente(QString, QString, QString, bool);
    friend QTextStream &operator <<(QTextStream &outStream, const Cliente  &cliente);
    QString getNombre() const;
    QString getTelefono() const;
    QString getDireccion() const;
    bool getSexo() const;
};

#endif // CLIENTE_H
