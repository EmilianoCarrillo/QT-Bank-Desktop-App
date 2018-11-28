#include "cliente.h"

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

Cliente::Cliente()
{}

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
