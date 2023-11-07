#ifndef STRUCTS_H
#define STRUCTS_H

#include <iostream>
#include <string>
using namespace std;

class Articulo{
private:
    std::string grupo;
    std::string codigo;
    std::string nombreArticulo;
    int *deposito;
    int total;
public:
    Articulo(){
        this->grupo = "NULO";
        this->codigo = "NULO";
        this->nombreArticulo = "NULO";
        this->deposito = nullptr;
        this->total = -1;
    }
    Articulo(std::string grupo, std::string codigo, std::string nombreArtiuclo, int *deposito, int total){
        this->grupo = grupo;
        this->codigo = codigo;
        this->nombreArticulo = nombreArticulo;
        this->deposito = deposito;
        this->total = total;
    }
    ~Articulo(){
        delete deposito;
    }
    std::string getGrupo(){
        return this->grupo;
    }
    void print(){
        cout<<"GRUPO: "<<this->grupo<<endl;
        cout<<"CODIGO: "<<this->codigo<<endl;
        cout<<"NOMBRE: "<<this->nombreArticulo<<endl;
        for(int i=0; i<sizeof(deposito)-3; i++){
            cout<<"DEPOSTIO "<<i+1<<": "<<this->deposito[i]<<endl;
        }
        std::cout<<"STOCK TOTAL: "<<this->total<<std::endl;
    }
};

template <class T>
struct Contenedor{
    T info; //cant deposito
    Articulo *puntero;
};

#endif