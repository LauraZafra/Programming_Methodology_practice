/**
 * @file main.cpp
 * @author MP
 */
#include <iostream>
#include <fstream>
#include <cstring>
#include "MPTools.h"
#include "Byte.h"
#include "Image.h"
#include "Histogram.h"

using namespace std;

static const int ERROR_ARGUMENTS = 100;

/**
 * @brief It waits until the key [INTRO] [RETURN] is pressed
 */
void pressReturnToContinue();

/**
 * @brief it shows help about the syntax of the command line
 */
void showHelp();

/**
 * @brief It breaks the program due to some error and shows a message
 * @param errorcode The type of error as declared in Image.h
 * @param errordata A message trying to describe the error
 */
void errorBreak(int errorcode, const string & errordata);

int main(int nargs, char *args[]) {
    string input = "", output = "", zoom = "0", copy = "", plane = "", aux = "";
    string hidetext = "", showtext = "", hideimage = "", showimage = "";
    Image im_copyfrom, im_input, im_output, im_bin, im_cut, im_histogram;
    Image im_collection[10], im_aux, im_hi, im_si;
    int res, n, x, y, w, h, z, k;

    //leemos los argumentos del array de arrays args
    for (int n = 1; n < nargs;) {
        aux = args[n];
        if (aux == "-i" && n < nargs) {
            n++;
            input = args[n++];
        } else if (aux == "-c" && n < nargs) {
            n++;
            copy = args[n++];
        } else if (aux == "-o" && n < nargs) {
            n++;
            output = args[n++];
        } else if (aux == "-z" && n < nargs) {
            n++;
            zoom = args[n++];
        } else if (aux == "-p" && n < nargs) {
            n++;
            plane = args[n++];
        } else if (aux == "-ht" && n < nargs) {
            n++;
            hidetext = args[n++];
        } else if (aux == "-st" && n < nargs) {
            n++;
            showtext = args[n++];
        } else if (aux == "-hi" && n < nargs) {
            n++;
            hideimage = args[n++];
        } else if (aux == "-si" && n < nargs) {
            n++;
            showimage = args[n++];
        } else
            errorBreak(ERROR_ARGUMENTS, " Unkown argument " + aux);
    }

    //comprobamos la validez de input
    if (input.length() < 1) {
        errorBreak(ERROR_ARGUMENTS, " Missing input file");
    }
    //llenamos la imagen con los datos del fichero
    res = im_input.readFromFile(input.c_str());

    if (res != Image::IMAGE_DISK_OK) {
        errorBreak(res, input); //si no está bien damos error
    }
    im_input.showInWindow("input");
    // llamar a inspect()(im_input);
    //im_input.inspect();

    //pasamos k de string a int
    k = std::stoi(plane, nullptr, 10);

    //si hay un valor para zoom, input debe ampliarse o reducirse: PARTE NUEVA
    z = std::stoi(zoom, nullptr, 10);
    if (z != 0) {
        if (z == -1) im_input.zoomOut();
        else im_input.zoomIn();
    }

    //    //comprobamos copy
    //    if (copy.length() > 0) { //si no está vacío
    //        //llenamos la imagen con los datos del fichero
    //        res = im_copyfrom.readFromFile(copy.c_str());
    //        if (res != Image::IMAGE_DISK_OK) {
    //            errorBreak(res, input);
    //        }
    //        im_copyfrom.showInWindow("copyfrom");
    //        // llamar a inspect()(im_copyfrom);
    //        im_copyfrom.inspect();
    //
    //        //inicializamos ancho y alto
    //        w = im_copyfrom.width() + 5;
    //        h = im_copyfrom.height() + 5;
    //
    //        //creamos imagen binarizada (solo 0 ó 255) a partir de copyfrom
    //        im_bin = im_copyfrom.threshold();
    //        im_bin.showInWindow("threshold");
    //        // llamar a inspect()(im_bin);
    //        im_bin.inspect();
    //
    //        x = 0; //decidimos el valor que queremos dar a x
    //        //copiamos copyfrom en input
    //        im_input.pasteArea(x, 0, im_copyfrom); //dejamos toneup y merge en sus valores por defecto
    //        //segmenta el histograma y guarda cada segmento como imagen en im_collection
    //        im_copyfrom.extractObjects(im_collection, n, 3); //im_collection es un vector de imágenes
    //        im_cut = im_collection[0];
    //        im_collection[0].showInWindow("segmented");
    //        // llamar a inspect()(im_collection[0]);
    //        im_collection[0].inspect();
    //
    //        x = x + w; //avanzamos de posición
    //        im_input.pasteArea(x, 0, im_cut); //copia el primer segmento
    //        im_input.pasteArea(x, h, im_bin); //copia la imagen binarizada
    //        x = x + w;
    //        //misma operación añadiendo parámetro toneup
    //        im_input.pasteArea(x, 0, im_cut, 64);
    //        im_input.pasteArea(x, h, im_bin, 64);
    //        //misma operación añadiendo parámetro merge
    //        x = x + w;
    //        im_input.pasteArea(x, 0, im_cut, 64, 50);
    //        im_input.pasteArea(x, h, im_bin, 64, 50);
    //    }

    //HIDETEXT
    if (hidetext.length() > 0) {

        char aux;
        ifstream fi;
        int i=0;
        char source[Byte::MAX_BYTE*Byte::MAX_BYTE];

        cout << endl << "...Reading text from " << hidetext << endl;

        fi.open(hidetext);
        while (aux != '\0') {
            fi >> aux;
            source[i]=aux;
            i++;
        }
        fi.close();
        im_input.hideText(source, k);
}
    
//SHOWTEXT
if (showtext.length() > 0) {
    char hidden[Byte::MAX_BYTE*Byte::MAX_BYTE];
    im_input.showText(hidden,Byte::MAX_BYTE*Byte::MAX_BYTE,k);
}
    
//HIDEIMAGE
if (hideimage.length() > 0) {
    im_hi.readFromFile(hideimage.c_str());
    im_input.hideImage(k, im_hi);
}
    
//SHOWIMAGE
if (showimage.length() > 0) {
    im_si.readFromFile(showimage.c_str());
    if (im_input.showImage(k, im_si)) im_si.showInWindow("oculta");
}

//gestionamos la salida de la imagen, el resultado de la ejecución
im_output = im_input;
if (output.length() > 0) { //si hay fichero output la imagen se guarda en este
    im_output.saveToFile(output.c_str());
}
im_output.showInWindow("output");
// llamar a inspect()(im_output);
//im_output.inspect();

pressReturnToContinue();
return 0;
}

void pressReturnToContinue() {
    char aux[10];
    cout << "Press [RETURN] to continue ...";
    cin.getline(aux, 1, '\n');
}

void showHelp() {
    cout << "Please use: -i <imageCarrier> [" << "zoom" << " <-1|+1> " << "k";
    cout << " <k=0> " << "argHideText" << " <textSource> " << "argShowText" << " ";
    cout << "argHideImage" << " <imageSource> " << "argShowImage" << "]" << endl;
    cout << "\n\t-i <imageCarrier> \t\tImage to carry hiden objets" << endl;
    cout << "\n\t-o <output> \t\t\t(OPT) File to store the result" << endl;
    cout << "\n\t" << "argZoom" << " <-1|+1> " << "\t\t\t(OPT) zooming image in (>0) and out (<0). Default value is 0" << endl;
    cout << "\n\t" << "k" << " <k> " << " \t\t\t(OPT) bit-plane to deal with. Its default is 0" << endl;
    cout << "\n\t" << "argHideText" << " <textSource> " << " \t\t(OPT) Hide text comtained in the file " << endl;
    cout << "\n\t" << "argShowText" << "\t\t\t\t(OPT) Showing the text encoded in the file " << endl;
    cout << "\n\t" << "argHideImage" << " <imageSource> " << " \t\t(OPT) Hide image comtained in the file " << endl;
    cout << "\n\t" << "argShowImage" << "\t\t\t\t(OPT) Showing the image encoded in the file " << endl;
}

void errorBreak(int errorcode, const string & errordata) {
    switch (errorcode) {
        case ERROR_ARGUMENTS:
            cout << endl << "Error in call: " << errordata << endl;
            showHelp();
            break;
        case Image::IMAGE_ERROR_OPEN:
            cout << endl << "Error opening file " << errordata << endl;
            break;
        case Image::IMAGE_ERROR_DATA:
            cout << endl << "Data error in file " << errordata << endl;
            break;
        case Image::IMAGE_ERROR_FORMAT:
            cout << endl << "Unrecognized format in file " << errordata << endl;
            break;
        case Image::IMAGE_TOO_LARGE:
            cout << endl << "The image is too large and does not fit into memory " << errordata << endl;
            break;
    }
    std::exit(1);

}