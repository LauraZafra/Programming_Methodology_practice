/**
 * @file Image.cpp
 * @author MP
 */
#include <iostream>
#include <fstream>
#include <cstring>
#include "MPTools.h"
#include "Byte.h"
#include "Image.h"
#include "Histogram.h"
#include <cmath>
using namespace std;

Image::Image() {
    _width = 0;
    _height = 0;
}

Image::Image(int width, int height) {
    Byte b;
    _height = height;
    _width = width;
    _data = reservarM(_width, _height);
}

int Image::height() const {
    return _height;
}

int Image::width() const {
    return _width;
}

void Image::setPixel(int x, int y, Byte v) {
    if ((y < _height)&&(x < _width)&&(y >= 0)&&(x >= 0)) {
        _data[x][y] = v;
    } else {
        cerr << "ERROR: (" << y << "," << x << ")" << endl;
    }
}

int Image::getPixel(int x, int y) const {
    if ((y < _height)&&(x < _width)&&(y >= 0)&&(x >= 0)) {
        return (int) _data[x][y].getValue();
    } else
        return -1;
}

void Image::setPos(int i, Byte v) {
    if ((i < _height * _width)&&(i >= 0)) {
        _data[i % _height][i % _width] = v;
    } else {
        cerr << "ERROR: (" << i << ")" << endl;
    }
}

int Image::getPos(int i) const {
    if ((i < _height * _width)&&(i >= 0))
        return (int) _data[i % _height][i % _width].getValue();
    else
        return -1;
}

void Image::flatten(Byte b) {
    for (int y = 0; y < height(); y++) {
        for (int x = 0; x < width(); x++) {
            setPixel(x, y, b);
        }
    }
}

void Image::showInWindow(string title) const {
    string tempname = "data" + SLASH + ".hidden_" + title + ".pgm";
    mpSavePGMImage(tempname, (const unsigned char *) _data, _width, _height);
    mpDisplayImage(tempname, title);

}

string Image::inspect() const {
    string result = "";
    result = std::to_string(_width) + "x" + std::to_string(_height) + " " + std::to_string(mphash((const unsigned char*) _data, _height * _width));
    return result;
}

Histogram Image::getHistogram() const {
    Histogram result;
    for (int y = 0; y < height(); y++) {
        for (int x = 0; x < width(); x++) {
            result.setLevel(getPixel(x, y), result.getLevel(getPixel(x, y)) + 1);
        }
    }
    return result;
}

int Image::readFromFile(const char fichero[]) {

    // Aquí deberíamos poner un setsize y un clear

    clear();

    int h, w, aux;
    Byte b;
    ifstream fi;
    char c1, c2;

    cout << endl << "...Reading image from " << fichero << endl;

    fi.open(fichero);
    if (!fi) {
        return IMAGE_ERROR_OPEN;
    }
    fi >> c1 >> c2;
    if (c1 != 'P') {
        return IMAGE_ERROR_FORMAT;
    }
    if (c2 != '2') {
        return IMAGE_ERROR_FORMAT;
    }
    fi >> w >> h >> aux;
    if (w * h > IMAGE_MAX_SIZE) {
        return IMAGE_TOO_LARGE;
    }
    
    setSize(w, h);
    cout << w << "x" << h << endl;
    for (int y = 0; y < height(); y++) {
        for (int x = 0; x < width(); x++) {
            fi >> aux;
            this->setPixel(x, y, aux);
        }
    }
    if (!fi) {
        return IMAGE_ERROR_DATA;
    }
    fi.close();
    return IMAGE_DISK_OK;

}

int Image::saveToFile(const char fichero[]) const {
    ofstream fo;
    cout << endl << "...Saving image into " << fichero << endl;
    if (height() == 0 || width() == 0) {
        return IMAGE_DISK_OK;
    }
    fo.open(fichero);
    if (!fo) {
        return IMAGE_ERROR_OPEN;
    }
    fo << "P2" << endl;
    fo << width() << " " << height() << endl;
    fo << "255" << endl;
    for (int y = 0; y < height(); y++) {
        for (int x = 0; x < width(); x++) {
            fo << (int) this->getPixel(x, y) << " ";
        }
    }
    if (!fo) {
        return IMAGE_ERROR_DATA;
    }
    fo.close();
    return IMAGE_DISK_OK;
}

Image Image::depictsHistogram() const {
    int w = 256, h = 160, b = 1, c = 9, a = h - (b + c) - 2;
    int normal;
    Image result(w, h);
    Histogram histogram = getHistogram();
    int max = histogram.getMaxLevel();
    result.flatten(Byte::MAX_BYTE_VALUE);
    for (int x = 0; x < 256; x++) {
        for (int y = 0; y < c; y++) {
            result.setPixel(x, h - 1 - y, x);
        }
        normal = (histogram.getLevel(x) * a) / max;
        for (int y = 0; y <= normal; y++) {
            result.setPixel(x, h - c - b - y - 1, 0);
        }
    }
    return result;
}

void Image::extractObjects(Image imageSet [], int &nimages, int maximages, double tolerance) const {
    Image aux(width(), height());
    Histogram histogram = getHistogram();
    int start, end, max = histogram.getMaxLevel(), count;

    nimages = 0;
    start = -1;
    end = -1;
    count = 0;
    for (int i = 255; i >= -1; i--) {
        if (i >= 0 && histogram.getLevel(i) *1.0 / max >= tolerance) {
            if (end < 0) {
                end = i;
                count = histogram.getLevel(i);
            } else {
                count += histogram.getLevel(i);
            }
        } else if (nimages < maximages && end >= 0) { // && (i==0 || count *1.0/ (width() * height()) >= tolerance )) { //  
            start = i;
            aux.flatten(0);
            //            aux=dotted(width(),height());
            for (int y = 0; y < height(); y++) {
                for (int x = 0; x < width(); x++) {
                    if (start <= getPixel(x, y) && getPixel(x, y) <= end) {
                        aux.setPixel(x, y, getPixel(x, y));
                    }
                }
            }
            cout << "Found object " << nimages << " in [" << start << "," << end << "]" << endl;
            if (nimages < maximages) {
                imageSet[nimages] = aux;
                nimages++;
            }
            end = -1;
            count = 0;
        }
    }
}

Image Image::threshold(int t) const {
    if (t < 0) {
        t = getHistogram().getBalancedLevel();
    }
    Image aux(this->width(), this->height());
    cout << "Thresholding to level " << t << endl;
    for (int y = 0; y < aux.height(); y++) {
        for (int x = 0; x < aux.width(); x++) {
            if (getPixel(x, y) <= t)
                aux.setPixel(x, y, Byte::MIN_BYTE_VALUE);
            else
                aux.setPixel(x, y, Byte::MAX_BYTE_VALUE);
        }
    }
    return aux;
}

Image Image::copyArea(int x, int y, int w, int h) const {
    int tx, ty, tfx, tfy, tw, th;
    tx = std::min(std::max(x, 0), width());
    ty = std::min(std::max(y, 0), height());
    tfx = std::min(std::max(x + w, 0), width());
    tfy = std::min(std::max(y + h, 0), height());
    tw = tfx - tx;
    th = tfy - ty;
    Image aux(tw, th);
    aux.flatten(0);
    for (int y = 0; y < aux.height(); y++) {
        for (int x = 0; x < aux.width(); x++) {
            Byte b = getPixel(tx + x, ty + y);
            aux.setPixel(x, y, b);
        }
    }
    return aux;
}

void Image::pasteArea(int x, int y, const Image &from, int toneup, int merge) {
    int tx, ty, tfx, tfy, tw, th;
    Byte b, pixelto, pixelfrom;
    merge = merge % 101;
    tx = std::min(std::max(x, 0), width());
    ty = std::min(std::max(y, 0), height());
    tfx = std::min(std::max(tx + from.width(), 0), width());
    tfy = std::min(std::max(ty + from.height(), 0), height());
    tw = tfx - tx;
    th = tfy - ty;
    if (toneup >= 0) {
        b.setValue(toneup % 256);
    }
    for (int y = 0; y < th; y++) {
        for (int x = 0; x < tw; x++) {
            pixelto.setValue(getPixel(tx + x, ty + y));
            pixelfrom.setValue(from.getPixel(x, y));
            if (toneup >= 0) {
                if (from.getPixel(x, y) > b.getValue()) {
                    pixelto.mergeByte(pixelfrom, merge);
                    setPixel(tx + x, ty + y, pixelto);
                }
            } else {
                setPixel(tx + x, ty + y, pixelfrom);
            }
        }
    }
}

Image::~Image() {
    clear();
}

Image::Image(const Image &from) { //copia dura, no se comparte puntero
    //creamos una imagen con las dimensiones de from
    Image(from.width(), from.height());
    //copiamos los datos al nuevo puntero
    copyData(from);
}

Image& Image::operator=(const Image &rhs) {
    //liberamos _data de la imagen principal (la que llama a función)
    //copiamos rhs asignándolo a la imagen principal
    //redimensionando y creando nuevo puntero con los datos de rhs
    copy(rhs); // los 3 pasos los hace copy
    //se devuelve el objeto con el que hemos llamado a la función modificado
    return *this;
}

Image Image::zoomIn() const {
    //cargamos una imagen input y creamos zoom
    Image zoom(_width * 2, _height * 2);
    Byte b;
    b.offByte();
    zoom.flatten(b);
    //rellenamos las posiciones pares en filas y columnas
    for (int i = 0; i < _width * 2; i++) {
        for (int j = 0; j < _height * 2; j++) {
            if (i % 2 == 0 && j % 2 == 0) {
                zoom.setPixel(i, j, Image::getPixel(i / 2, j / 2));

            } else { //rellenamos las posiciones restantes con interpolación de puntos de alrededor
                Byte b = Image::copyArea(i / 2 - 1, j / 2 - 1, 3, 3).getHistogram().getBalancedLevel();
                zoom.setPixel(i, j, b);
            }
        }
    }
    return zoom;
}

Image Image::zoomOut() const {
    //cargamos una imagen input y creamos zoom
    Image zoom(_width / 2, _height / 2);
    Byte b;
    b.offByte();
    zoom.flatten(b);
    //rellenamos las posiciones pares en filas y columnas
    for (int i = 0; i < _width * 2; i++) {
        for (int j = 0; j < _height * 2; j++) {
            if (i % 2 == 0 && j % 2 == 0) {
                zoom.setPixel(i, j, Image::getPixel(i * 2, j * 2));
            } else { //rellenamos las posiciones restantes con interpolación de puntos de alrededor
                Byte b = Image::copyArea(i * 2 - 1, j * 2 - 1, 3, 3).getHistogram().getBalancedLevel();
                zoom.setPixel(i, j, b);
            }
        }
    }

    return zoom;
}

/**
 * @brief It seeks for a hidden image in plane k. In order to do that, it
 * must first check that the first letters match the cookieImage constant. 
 * If it does not, it must return false since it is not possible. Otherwise,
 * the next bytes contain the width and height of the image, values in [0..255]. Finally the following 
 * width*height bytes encode the content of the hidden image, returned as a result. Query method
 * @param k The plane. input param
 * @param result The found image. output param
 * @return True when a valid image is found, false otherwise
 */
bool Image::showImage(int k, Image &result) const {
    //= true, si en algún momento se incumplen los requisitos = false
    bool hidden = true;
    int pos_act = 0; //seguimiento de la lectura 

    //extraemos el identificador y lo guardamos en un vector
    Byte idImg[3];
    for (int l = 0; l < 3; l++) {
        Byte i (extractByte(pos_act, k));
        idImg[l] = i;
    }

    //si todo OK
    if (sameId(idImg[0], 'I', idImg[1], 'M', idImg[2], 'G')) { //si el identificador es IMG
        //inicializamos alto y ancho y comprobamos que la imagen "quepa"
        int w = std::stoi(extractString(pos_act, k),nullptr,2);
        int h = std::stoi(extractString(pos_act, k),nullptr,2);
        if (w < 0 || w > 255 || h < 0 || h > 255) hidden = false;
        else {
            int n = (w * h + 5) * Byte::NUM_BITS; //tamaño completo de la img oculta
            // 5 bytes = I,M,G,w,h
            if (n > _width * _height) hidden = false;
                //si cabe, leemos el resto y vamos guardando en result solo la img
            else {
                for (int i = 5; i < n / Byte::NUM_BITS; i++) {
                    Byte b = extractByte(pos_act, k);
                    result.setPixel((i - 5)%_height,(i - 5)%_width,b);
                }
                cout << "Hay una imagen oculta" << endl;
            }
        }
    }
    return hidden;
}

/**
 * @brief It hides the image @a from by encoding the sequence of values of its pixels
 * into the bits of plane @a k. In order to do that, it first hides the cookieImage constant
 * to identify this plane as containing an image. Then it hides the width and the height. Then
 * all the bytes from left to right and top down. The self image is modified. Modifier method.
 * @param k The plane. input param
 * @param from The image to hide. input param
 * @return  It return true if the process is successful, false otherwise. 
 */

bool Image::hideImage(int k, const Image &from) {
    bool success = true;
    //comprobamos si se puede ocultar la imagen
    int n = (from.width() * from.height() + 5) * Byte::NUM_BITS; //tamaño completo de la imagen a ocultar
    if (n > _width * _height) success = false; //no cabe
    else {
        //copiamos el identificador en la imagen
        hideId('I', 'M', 'G', k);

        //pasamos alto y ancho a enter0
        int wa=3*Byte::NUM_BITS;
        int we=4*Byte::NUM_BITS;
        int w = std::stoi(extractString(wa, k),nullptr,2);
        int h = std::stoi(extractString(we, k),nullptr,2);
        //copiamos el tamaño en la imagen
        hideSize(3, w, k);
        hideSize(4, h, k);


        //ocultamos la imagen bit a bit
        int pos_act = 5 * Byte::NUM_BITS; //posición por la que vamos en este momento copiando en la principal
        for (int i = 0; i < from.width(); i++) { //el número filas de pixeles que hay que copiar desde from
            for (int j = 0; j < from.height(); j++) { //el número cols de pixeles que hay que copiar desde from
                for (int l = 0; l < Byte::NUM_BITS; l++) { //control de cada bit del pixel

                    setBit(from.getPixel(i, j), l, i % _height, i % _width, k);
                    pos_act++; //cada vez que ocultamos un bit pasamos al siguiente pixel de la principal
                }
            }
        }

        //añadimos al final la señal \0 para indicar el fin de la copia
        for (int i = 0; i < Byte::NUM_BITS; i++) {
            int tam_total = (5 + (from.width() * from.height())) * Byte::NUM_BITS + i;
            _data[tam_total % _height][tam_total % _width].offBit(k);
        }
    }
    return success;
    if (success == false) cout << "No se puede ocultar la imagen" << endl;
}

/**
 * @brief Steganography process.
 * It hides the cstring given within the k-plane especified. If no
 * plane is specified, the 0-plane is considered. If the cstring is too
 * large given the size of the original image, it must return false. Otherwise
 * the image hides the cstring and it returns true. In order to do that, it first
 * hides the cookieText constant, then the content of the cstring and finally,
 * the \0 delimiter converted in 00000000 encoded into the image. Modifier method.
 * @param the cstring to hide. input param
 * @param k the k-plane. input param
 * @return true if it has been possible, false otherwise
 */

bool Image::hideText(char source [], int k) {
    bool hidden = true;
    int pos_act = 3 * Byte::NUM_BITS, nsource = 0;

    //copiamos el identificador
    hideId('T', 'X', 'T', k);

    //copiamos el texto en sí
    bool pasando = true;
    while (pasando) {
        for (int i = 0; i < Byte::NUM_BITS; i++) { //procesamos cada bit del char
            int fila = pos_act / _width;
            int col = pos_act % _width;
            setBit(source[nsource], i, pos_act % _height, pos_act % _width, k);
            pos_act++; // avanzamos posición de la imagen principal
        }
        nsource++; //posición del vector por la que vamos

        //comprobamos si debe terminar el bucle
        if (source[nsource] == '\0') pasando = false; //terminamos si hemos llegado al final del texto
        if (pos_act > _width * _height) { //si se excede de tamaño
            pasando = false;
            cout << "No se puede ocultar la el texto porque es demasiado largo" << endl;
            hidden = false;
        }
    }
    return hidden;
}

/**
 * @brief It checks that the k-plane contains a cstring if the first bytes 
 * of the image contains the cookieText constant. Afterwards, all the bytes
 * encided in plane k are extracted and the final one must be the \0 delimiter
 * @param hidden cstring to return the hidden string, if any
 * @param max size of the cstring, without including the \0 delimiter
 * @param k The plane
 * @return True if success, false otherwise.
 */
bool Image::showText(char hidden [], const int max, int k) const {
    //= true, si en algún momento se incumplen los requisitos = false
    bool success = true;
    int pos_act = 0;

    //creamos el vector con el identificador "real"
    Byte letters[3];
    for (int j = 0; j < 3; j++) {
        letters[j] = extractByte(pos_act, k);
    }

    if (sameId(letters[0], 'T', letters[1], 'X', letters[2], 'T')) { //si el vector es TXT continuamos
        //vamos añadiendo caracteres al array hasta encontrar la marca \0
        int pos_act = Byte::NUM_BITS * 3; //seguimiento de la lectura
        do {
            string s = (extractString(pos_act, k));
            int i = std::stoi(s, nullptr, 2);
            hidden[(pos_act / Byte::NUM_BITS) - 3] = i;
        } while (hidden[(pos_act / Byte::NUM_BITS) - 3] != '\0');

        //imprimimos el texto en pantalla
        cout << "Hay un texto oculto:" << endl;
        for (int i = 0; i < max; i++) cout << hidden[i];
    } else {
        success = false;
    }

    return success;
}

void Image::clear() {
    //destructor de puntero
    if (_data != nullptr) {
        for (int i = 0; i < _height; i++) {
            delete [] _data[i];
            _data[i]=nullptr;
        }
        delete [] _data;
        _data = nullptr;

        //dejamos el tamaño de la imagen vacío
        _width = 0;
        _height = 0;
    }
}

void Image::copy(const Image &rhs) { //rhs no se modifica
    //redimensionamos la imagen con las medidas de rhs
    setSize(rhs._width, rhs._height);
    //copiamos los datos a la imagen redimensionada
    copyData(rhs);
    //devolvemos la imagen con la que se llama al método modificada
}

void Image::setSize(const int w, const int h) {
    //borramos el puntero antiguo de datos
    clear();
    //asignamos nuevo tamaño
    _height = h;
    _width = w;
    //nuevo puntero con el nuevo tamaño
    _data = reservarM(w, h);

}

void Image::copyData(const Image &from) {
    for (int i = 0; i < _width; i++) {
        for (int j = 0; j < _height; j++) {
            Byte b = from.getPixel(i, j); //byte auxiliar
            setPixel(i, j, b);
        }
    }
}

Byte** Image::reservarM(int w, int h) {
    Byte * p = new Byte[w * h];
    _data = new Byte*[h];
    for (int i = 0; i < h; i++) {
        _data[i] = &p[w * i];
    }
    return _data;
}

/**
 @brief lee de la imagen un byte a través de 8 píxeles
 @param pos_act posición actual
 @param k plano de oculación
 @return byte en formato string
 */
Byte Image::extractByte(int &pos_act, int k) const{
    string s = "";
    for (int i = pos_act; i < pos_act + Byte::NUM_BITS; i++) {
        s += _data[i % _height][i % _width].getBit(k);
    }
    pos_act += Byte::NUM_BITS;
    int i = std::stoi(s, nullptr, 2);
    Byte b(i);
    return b;
}

std::string Image::extractString(int &pos_act, int k) const{
    string s = "";
    for (int i = pos_act; i < pos_act + Byte::NUM_BITS; i++) {
    s += _data[i % _height][i % _width].getBit(k);
    }
    pos_act += Byte::NUM_BITS;
    return s;
}

/**
 @brief oculta IMG o TXT en la imagen
 @param a,b,c caracteres
 @param k plano de ocultación
 */
void Image::hideId(char a, char b, char c, int k) {
    Byte s;
    for (int j = 0; j < 3; j++) {
        if (j == 0) s = a;
        if (j == 1) s = b;
        if (j == 2) s = c;
        for (int i = 0; i < Byte::NUM_BITS; i++) {
            int fil = (i + (Byte::NUM_BITS * j)) % _height;
            int col = (i + (Byte::NUM_BITS * j)) % _width;
            setBit(s, i, fil, col, k);
        }
    }
}

/**
 @brief oculta medidas de ancho y alto en una imagen
 @param desde_donde posición de la img donde se insertan las medidas
 @param byte medida a ocultar
 */
void Image::hideSize(int desde_donde, int byte, int k) {
    for (int i = desde_donde * Byte::NUM_BITS; i < (desde_donde + 1) * Byte::NUM_BITS; i++) { //empezamos a añadir después de los píxeles de ID
        setBit(byte, i, i % _height, i % _width, k);
    }
}

/**
 @brief oculta medidas de ancho y alto en una imagen, se debe usar 
 dentro de un bucle que recorra el byte entero
 @param desde_donde posición de la img donde se insertan las medidas
 @param byte medida a ocultar
 */
void Image::setBit(Byte b, int i, int fil, int col, int k) {
    if (b.Byte::getBit(i) == 1) _data[fil][col].onBit(k);
    else {
        _data[fil][col].offBit(k);
    }
}

//comprueba si coindicen el identificador que hay con el que debería haber

bool Image::sameId(Byte ab, char ac, Byte bb, char bc, Byte cb, char cc) const{
    bool same = true;
    //pasamos de byte a string y de string a int
    string as = ab.to_string();
    string bs = bb.to_string();
    string cs = cb.to_string();
    int ai = std::stoi(as,nullptr,2);
    int bi = std::stoi(bs,nullptr,2);
    int ci = std::stoi(cs,nullptr,2);
    
    //pasamos de char a int
    int a = ac - '\0';
    int b = bc - '\0';
    int c = cc - '\0';

    //comparamos int con int
    if (ai != a || bi != b || ci != c) same = false;
    return same;
}
