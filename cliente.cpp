#include "cliente.h"
#include "cuentabancaria.h"

QString Cliente::getNombre() const
{
    return nombre;
}

QString Cliente::getTelefono() const
{
    return telefono;
}

QString Cliente::getDireccion() const
{
    return direccion;
}

bool Cliente::getSexo() const
{
    return sexo;
}

void Cliente::insertCuenta(QString num, CuentaBancaria &cuenta){
    cuentas.insert(num, cuenta);
}

void Cliente::eliminarCuenta(QString num){
    cuentas[num].~CuentaBancaria();
    cuentas.remove(num);
}

QMap<QString, CuentaBancaria> Cliente::getCuentas() const
{
    return cuentas;
}

void Cliente::setAll(QString nuevoNombre, QString nuevoTelefono, QString nuevaDireccion, bool nuevoSexo)
{
    nombre = nuevoNombre;
    telefono = nuevoTelefono;
    direccion = nuevaDireccion;
    sexo = nuevoSexo;
}

Cliente::Cliente(){}

Cliente::Cliente(QString nombre, QString telefono, QString direccion, bool sexo)
                 :nombre(nombre), telefono(telefono), direccion(direccion), sexo(sexo){}

QTextStream &operator <<(QTextStream &outStream, const Cliente &cliente)
{
   outStream << cliente.nombre << endl;
   outStream << cliente.telefono << endl;
   outStream << cliente.direccion << endl;
   outStream << cliente.sexo << endl;
   return outStream;
}
