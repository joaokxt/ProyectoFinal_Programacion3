#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>

#include "HashMap/HashLista.h"
#include "HashMap/HashEntry.h"
#include "Lista/Lista.h"
#include "Arbol/ArbolBinarioAVL.h"
#include "Arbol/ArbolBinarioContenedor.h"
#include "Cola/Cola.h"
#include "structs.h"

using namespace std;

int i, j, nMin, nMax, *nDeposito, cantArticulosDiferentes = 0, cantArticulos = 0, cantDepositos, depos;
//  argumento indica la cantidad de argumentos que hay en total
bool argumento[7] = {false};
//  comandos es un arreglo de strings que contiene los comandos que se pueden ingresar
string comandos[5] = {"total_art_dif", "total_art", "min_stock", "stock", "max_stock"};

Cola<string> nombresBusqueda;
Cola<string> colaBusquedaDeposito;
Cola<int> colaDeposito;

unsigned int hashFuncString(string clave)
{
    int x = clave[0], suma = 0, final = clave.size() - 1;

    suma += clave[1] * pow(x, 3) + clave[2] * pow(x, 2) + clave[3] * x;
    suma += clave[final] * pow(x, 3) + clave[final - 1] * pow(x, 2) + clave[final - 2] * x;

    return suma;
}

int contarColumnasCSV()
{
    // Se crea objeto asociado al archivo de inventario
    ifstream archivoCSV("Inventariado Fisico.csv");

    char buffer[4096];
    // Se lee la primera linea del CSV (el encabezado)
    archivoCSV.getline(buffer, 4096);

    // Objeto stringstream con contenido del encabezado
    // El stringstream permite leer como si fuera un flujo de caracteres
    stringstream encabezado(buffer);
    string columna;
    int contadorColumnas = 0;

    // Se divide al encabezado en columnas, separadas por ';'
    while (getline(encabezado, columna, ';'))
    {
        contadorColumnas++;
    }

    archivoCSV.close();
    return contadorColumnas;
}

int main(int argc, char *argv[])
{
    cantDepositos = contarColumnasCSV() - 3;

    nDeposito = new int[cantDepositos];
    for (i = 0; i < cantDepositos; i++)
    {
        nDeposito[i] = -1;
    }

    //  Se leen los argumentos ingresados en consola
    bool check;
    for (i = 0; i < argc; i++)
    {
        // Se compara cada string ingresado por consola con uno de los comandos de comandos[].
        // Si alguno verifica, se activa el argumento.

        check = false;
        if (argv[i] == comandos[0])
        {
            argumento[0] = true;
        }
        else if (argv[i] == comandos[1])
        {
            argumento[1] = true;
        }
        else if (argv[i] == comandos[2])
        {
            if (argc <= i + 2)
            {
                argumento[2] = true;
                nMin = stoi(argv[i + 1]);
            }
            else
            {
                for (j = 0; j < 5; j++)
                {
                    if (argv[i + 2] == comandos[j])
                    {
                        check = true;
                        argumento[2] = true;
                        nMin = stoi(argv[i + 1]);
                        i++;
                    }
                }
                if (!check)
                {
                    argumento[3] = true;
                    nDeposito[stoi(argv[i + 2]) - 1] = stoi(argv[i + 1]);
                    depos = stoi(argv[i + 2]);
                    i += 2;
                }
            }
        }
        else if (argv[i] == comandos[3])
        {
            if (argc <= i + 2)
            {
                argumento[4] = true;
                nombresBusqueda.encolar(argv[i + 1]);
            }
            else
            {
                for (j = 0; j < 5; j++)
                {
                    if (argv[i + 2] == comandos[j])
                    {
                        check = true;
                        argumento[4] = true;
                        nombresBusqueda.encolar(argv[i + 1]);
                        i++;
                    }
                }
                if (!check)
                {
                    argumento[5] = true;
                    colaBusquedaDeposito.encolar(argv[i + 1]);
                    colaDeposito.encolar(stoi(argv[i + 2]));
                    i += 2;
                }
            }
        }
        else if (argv[i] == comandos[4])
        {
            argumento[6] = true;
            nMax = stoi(argv[i + 1]);
        }
    }

    // Se verifica si por lo menos se activo uno de los posibles comandos.

    check = false;
    for (i = 0; i < 7; i++)
    {
        if (argumento[i])
            check = true;
    }

    if (!check)
    {
        cout << "NO SE INGRESO NINGUN ARGUMENTO VALIDO!" << endl;
        return 0;
    }

    //  mapaArticulos es un HashMap que contiene todos los articulos, con su nombre como clave
    HashMapL<string, Articulo *> mapaArticulos(1024, *hashFuncString);

    //  listaArbolesDepositos es una lista de arboles, cada uno contiene los articulos de un deposito
    Lista<ArbolBinarioContenedor<int>> listaArbolesDepositos;

    //  arbolMinimo y arbolMaximo son arboles que contienen en su nodo raíz un objeto articulo nulo asociado con la cantidad n según la que queremos ordenar.
    ArbolBinarioContenedor<int> arbolMinimo(nMin, new Articulo());
    ArbolBinarioContenedor<int> arbolMaximo(nMax, new Articulo());

    for (i = 0; i < cantDepositos; i++)
    {
        ArbolBinarioContenedor<int> arbolDeposito(nDeposito[i], new Articulo());
        listaArbolesDepositos.insertarUltimo(arbolDeposito);
    }

    // Se abre el archivo a leer.
    ifstream archivoCSV("Inventariado Fisico.csv");

    // Se declaran variables necesarias para guardar datos de objetos.
    int total, *deposito = new int[cantDepositos];
    ;
    char separador = ';';
    string grupo, codigo, nombreArticulo, linea, *d;
    bool presente;
    Articulo *articuloActual;

    //  Se lee la primera linea del CSV, que contiene los nombres de los depositos
    getline(archivoCSV, linea);

    //  Se procede a realizar la lectura del CSV
    while (getline(archivoCSV, linea))
    {
        stringstream stream(linea);
        deposito = new int[cantDepositos];
        d = new string[cantDepositos];

        total = 0;
        presente = false;

        getline(stream, grupo, separador);
        getline(stream, codigo, separador);
        getline(stream, nombreArticulo, separador);
        for (i = 0; i < cantDepositos; i++)
        {   
            // Se toma el string del stock en cada depósito.
            getline(stream, d[i], separador);
            try
            {   
                // Se intenta pasar el string a int.
                deposito[i] = stoi(d[i]);
            }
            catch (std::invalid_argument)
            {   
                // Si el string da error, es una celda vacía. Stock 0.
                deposito[i] = 0;
            }
            total += deposito[i];
        }

        articuloActual = new Articulo(grupo, codigo, nombreArticulo, deposito, total);

        arbolMinimo.put(total, articuloActual);
        arbolMaximo.put(total, articuloActual);

        // Se verifica si el articuloActual ya está en el Hash, comparando nombre.
        Lista<HashEntry<string, Articulo *> *> *listaPrueba;
        try
        {
            listaPrueba = mapaArticulos.get(nombreArticulo);
            for (i = 0; i < listaPrueba->getTamanio(); i++)
            {
                if (listaPrueba->getDato(i)->getValor()->getNombre() == nombreArticulo)
                {
                    presente = true;
                }
            }
            if (!presente)
            {
                mapaArticulos.put(nombreArticulo, articuloActual);
                cantArticulos += total;
                cantArticulosDiferentes++;
            }
        }
        catch (int e)
        {
            // Si no está, y el espacio asignado por la función está vacío, tira error 404. Se maneja acá.
            mapaArticulos.put(nombreArticulo, articuloActual);
            cantArticulos += total;
            cantArticulosDiferentes++;
        }

        for (i = 0; i < cantDepositos; i++)
        {
            if(nDeposito[i] != -1)
                listaArbolesDepositos.getDato(i).put(deposito[i], articuloActual);
        }
    }
    archivoCSV.close();

    clock_t begin;
    std::cout << "Comenzando a medir Tiempo\n"
              << endl;
    begin = clock();

    //  Se procede a mostrar los argumentos ingresados
    if (argumento[0])
    {
        cout << " > La cantidad total de articulos diferentes es de: " << cantArticulosDiferentes << endl;
    }
    if (argumento[1])
    {
        cout << " > La cantidad total de articulos es de: " << cantArticulos << endl;
    }
    if (argumento[2])
    {
        cout << " > Mostrando codigos de productos con " << nMin << " o menos en stock: " << endl;
        arbolMinimo.min();
        cout << endl;
    }
    if (argumento[3])
    {
        for (i = 0; i < cantDepositos; i++)
        {
            if (nDeposito[i] != -1)
            {
                cout << " > Mostrando codigos de productos con " << nDeposito[i] << " o menos en el deposito " << i + 1 << ": " << endl;
                listaArbolesDepositos.getDato(i).min();
                cout << endl;
            }
        }
    }
    if (argumento[4])
    {
        Lista<HashEntry<string, Articulo *> *> *listaBusqueda;
        while(!nombresBusqueda.esVacia())
        {
            string nombreBuscar = nombresBusqueda.desencolar();

            try
            {
                listaBusqueda = mapaArticulos.get(nombreBuscar);

                for (j = 0; j < listaBusqueda->getTamanio(); j++)
                {
                    if (listaBusqueda->getDato(j)->getValor()->getNombre() == nombreBuscar)
                    {
                        listaBusqueda->getDato(j)->getValor()->printStock();
                    }
                }
            }
            catch (int e)
            {
                if (e == 404)
                {
                    cout << "ARTICULO [" << nombreBuscar << "] NO ENCONTRADO!" << endl;
                }
            }
        }
    }
    if (argumento[5])
    {
        Lista<HashEntry<string, Articulo *> *> *listaBusqueda;
        while (!colaBusquedaDeposito.esVacia())
        {
            string nombreBuscar = colaBusquedaDeposito.desencolar();
            int depositoBuscar = colaDeposito.desencolar();

            try
            {
                listaBusqueda = mapaArticulos.get(nombreBuscar);
                for (j = 0; j < listaBusqueda->getTamanio(); j++)
                {
                    if (listaBusqueda->getDato(j)->getValor()->getNombre() == nombreBuscar)
                    {
                        listaBusqueda->getDato(j)->getValor()->printDeposito(depositoBuscar);
                    }
                }
            }
            catch (int e)
            {
                if (e == 404)
                {
                    cout << "ARTICULO [" << nombreBuscar << "] NO ENCONTRADO!" << endl;
                }
            }
        }
    }
    if (argumento[6])
    {
        cout << " > Mostrando codigos de productos con " << nMax << " o mas en stock: " << endl
             << endl;
        arbolMaximo.max();
        cout << endl;
    }

    clock_t end = clock();
    double elapsed_secs = static_cast<double>(end - begin) / CLOCKS_PER_SEC;
    cout << endl;
    cout << "Tardo elapsed_secs " << elapsed_secs << "\n"
         << std::endl;
}
