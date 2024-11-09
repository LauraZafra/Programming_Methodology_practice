/**
 * @file Byte.cpp
 * @brief Operators for bit level
 * @note To be implemented by students 
 * @author MP-DGIM, MP-IADE, MP-II (grupo B)
 */
#include <iostream>
#include <string>
#include "MPTools.h"
#include "Byte.h"

using namespace std;

const Byte Byte::MAX_BYTE_VALUE(255);
const Byte Byte::MIN_BYTE_VALUE(0);


// enciende el bit pos del Byte b
void Byte::onBit( int pos)
{
    unsigned int mask;
    if ((pos>= Byte::MIN_BIT) && (pos < Byte::MAX_BIT))
    {
        // genero la mascara
        mask = 0x01 << pos;
        _data = _data | mask;
    }
}

// apaga el bit pos del Byte b

void Byte::offBit( int pos) {
    unsigned int mask;
    if ((pos >= Byte::MIN_BIT) && (pos < Byte::MAX_BIT)) {
        // genero la mascara
        mask = 0x01 << pos;
        // invierto todos los bits
        mask = ~mask;
        // hago un AND para dejar todo igual salvo el cero
        _data = _data & mask;
    }
}

// devuelve en la variable status el estodo del bit (encendido = true, apagado = false)
// asumimos 0 <= pos < 8

bool Byte::getBit(int pos) const {
    unsigned int mask;
    bool status;
    // genero la mascara
    mask = 0x01 << pos;
    // hago un AND y compruebo si el resultado es mayor que cero
    status = (_data & mask) > 0;

    return (status);
}


string Byte::to_string() const {
    int i;
    bool valor;
    char c;
    string s = "";

    for (i = Byte::MIN_BIT; i < Byte::MAX_BIT; i++) {
        valor = getBit(i);
        c = valor + '0';
        s = c + s;
    }
    return s;
}



// enciende todos los bits
void Byte::onByte()
{
    _data = 0xFF; // es el 1111 1111
}

// apaga todos los bits
void Byte::offByte()
{
    _data = 0x00; // es el 0000 0000
}

//enciende los bits según la configuracion de v
void Byte::encodeByte( const bool v[])
{
    // los apago todos
    offByte();
	// y enciendo algunos
    for(int i=Byte::MIN_BIT; i < Byte::MAX_BIT; i++)
    {
        if (v[i])
            onBit(7-i);
    }


}

//asigna en v el estado de cada bit
void Byte::decodeByte( bool v[]) const
{
    for(int i = Byte::MIN_BIT; i<Byte::MAX_BIT; i++)
    {
        v[i] = getBit(i);
    }
}

//asigna en v la posición de los bits encendidos
void Byte::decomposeByte( int posic[], int & cuantos) const
{
    cuantos = 0;
    bool valor;
    for(int i = MIN_BIT; i<Byte::MAX_BIT; i++)
    {
        valor = getBit(i);
        if(valor)
        {
            posic[cuantos] = i;
            cuantos++;
        }
    }
}

void Byte::shiftRByte( int n) {
    _data = _data >> n;
}

void Byte::shiftLByte( int n) {
    _data = _data << n;
}

void Byte::mergeByte(Byte merge, int percentage){
    percentage = percentage % 101;
    setValue((getValue()*(100-percentage)+merge.getValue()*percentage)/100);
}