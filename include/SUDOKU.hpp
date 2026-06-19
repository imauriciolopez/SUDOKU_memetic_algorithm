#include <iostream>

#ifndef LOCAL_HPP
#define LOCAL_HPP

#include <fstream>
#include <string>

#include <type_traits>
#include <../nlohmann/json.hpp>
#include <iostream>
#include <cmath>
#include <iterator>
#include <random>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <ctime>
#include <algorithm>

#include <typeinfo>

#include <omp.h>


void hello_world_local();

//enum del tipo de representación
enum class representacion{
    filas,
    columnas,
    celdas
};

//leer instancia
std::tuple<std::vector<std::vector<bool> >, std::vector<std::vector<int> >, int > leer_instancia(std::string archivo, representacion rep);

template<typename T>
struct individuo{
    //atributos individuales
    unsigned int n;
    unsigned int n_2;
    std::vector<std::vector<unsigned int> > tablero;
    //-1 ES LIBRE, !=-1 ES FIJO
    std::vector<std::vector<int> > pistas;
    //FALSE ES LIBRE, TRUE ES FIJO
    std::vector<std::vector<bool> > fijos;
    unsigned int seed;
    T fitness;
    representacion rep;

    //función de fitness
    using fitness_func=T(individuo<T>::*)(std::vector<T>&);
    fitness_func fit_func;

    static thread_local std::mt19937 rndm;
    static thread_local std::uniform_int_distribution<int>     rndm_int;
    static thread_local std::uniform_real_distribution<double> rndm_dbl;

    individuo(fitness_func fit, 
              unsigned int seed=0,
              unsigned int n=0, 
              std::vector<std::vector<unsigned int> > tablero_={{}}, 
              representacion rep=representacion::filas,
              std::vector<T> pesos={},
              std::vector<std::vector<bool> > fijos_={{}}, 
              std::vector<std::vector<int> > pistas_={{}}):
              pistas(pistas_), 
              fijos(fijos_),
              fit_func(fit),
              seed(seed), 
              n(n), 
              n_2(std::pow(n, 2)),
              rep(rep){

        
        rndm.seed(seed);
        tablero=std::vector<std::vector<unsigned int>>(n_2, std::vector<unsigned int>(n_2, 0));
        if(fijos_.empty()||fijos_[0].empty()){
            fijos=std::vector<std::vector<bool>>(n_2, std::vector<bool>(n_2, false));
        } 
        else {
            fijos=fijos_;
        }
        
        if(!(pistas_.empty()||pistas_[0].empty())){
            for(int i=0;i<n_2;i++){
                for(int j=0;j<n_2;j++){
                    if(fijos[i][j]){
                        tablero[i][j]=static_cast<unsigned int>(pistas_[i][j]);
                    }
                }
            }
        }
        
        if(!(tablero_.empty()||tablero_[0].empty())) {
            tablero=tablero_;
            fitness=(this->*fit_func)(pesos);
        }
        else inicializar_rndm(pesos);
    }

    individuo* operator+(const individuo* otro) const{

    }
    individuo* operator-(const individuo* otro) const{

    }
    individuo* operator*(const double fact) const{

    }
    void crossover(struct individuo *otro){

    }
    
    std::vector<unsigned int> permutacion_aleatoria(){
        std::vector<unsigned int> res(n_2);
        
        for(int i=0;i<n_2;i++) res[i]=i;

        //                        ┌número arbitrariamente grande
        int veces=rndm_int(rndm)%(30*(n_2)), a, b;

        for(int i=0;i<veces;i++){
            a=rndm_int(rndm)%n_2;
            while((b=rndm_int(rndm)%n_2)==a) b=rndm_int(rndm)%n_2;
            res[a]=res[a]^res[b];
            res[b]=res[a]^res[b];
            res[a]=res[a]^res[b];
        }
        return res;
    }

    bool validez(){
        for(int i=0;i<n_2;i++){
            std::vector<int> valores(n_2, 0);
            for(int j=0;j<n_2;j++){
                if(rep==representacion::filas||rep==representacion::celdas){
                    valores[tablero[i][j]]++;
                }
                else if(rep==representacion::columnas){
                    valores[tablero[j][i]]++;
                }
            }
            for(int j=0;j<n_2;j++){
                if(valores[j]!=1){
                    return false;
                }
            }
        }

        if(!(pistas.empty()||pistas[0].empty())){
            for(int i=0;i<n_2;i++){
                for(int j=0;j<n_2;j++){
                    if(pistas[i][j]!=-1&&tablero[i][j]!=pistas[i][j]){
                        return false;
                    }
                }
            }
        }

        return true;
    }

    void inicializar_rndm(std::vector<T>& pesos){
        if(rep==representacion::filas||rep==representacion::celdas){
            for(int i=0;i<n_2;i++){
                //checamos qué numeros ya están usados
                std::vector<bool> usado(n_2, false);
                for(int j=0;j<n_2;j++){
                    if(fijos[i][j]){
                        usado[tablero[i][j]]=true;
                    }
                }

                //checamos cuáles faltan
                std::vector<unsigned int> faltantes;
                for(int v=0;v<n_2;v++){
                    if(!usado[v]){
                        faltantes.push_back(v);
                    }
                }

                std::shuffle(faltantes.begin(), faltantes.end(), rndm);

                //rellenamos solo posiciones libres
                int idx=0;
                for(int j=0;j<n_2;j++){
                    if(!fijos[i][j]){
                        tablero[i][j]=faltantes[idx++];
                    }
                }
            }
            fitness=(this->*fit_func)(pesos);
        }
        else if(rep==representacion::columnas){
            for(int i = 0; i < n_2; i++){
                //checamos qué numeros ya están usados
                std::vector<bool> usado(n_2, false);
                for(int j = 0; j < n_2; j++){
                    if(fijos[j][i]){
                        usado[tablero[j][i]]=true;
                    }
                }

                //checamos cuáles faltan
                std::vector<unsigned int> faltantes;
                for(int v=0;v<n_2;v++){
                    if(!usado[v]){
                        faltantes.push_back(v);
                    }
                }

                std::shuffle(faltantes.begin(), faltantes.end(), rndm);

                //rellenamos solo posiciones libres
                int idx=0;
                for(int j=0;j<n_2;j++){
                    if(!fijos[j][i]){
                        tablero[j][i]=faltantes[idx++];
                    }
                }
            }
            fitness=(this->*fit_func)(pesos);
        }
    }

    void random_valid_swaps(int n_swaps){
        for(int i=0;i<n_swaps;i++){
            int a=rndm_int(rndm)%n_2; //elegimos celda/columna/fila para hacer swap
            int p1, p2;
            if(rep==representacion::filas||rep==representacion::celdas){
                //verificamos que en la fila o celda elegidas haya al menos 2 espacios no fijos para hacer swap
                while(true){
                    int count=0;
                    for(int i=0;i<n_2;i++){
                        if(fijos[a][i]){
                            count++;
                        }
                    }
                    if(count>n_2-2){
                        a=rndm_int(rndm)%n_2;
                    }
                    else{
                        break;
                    }
                }
                //verificamos que la primera posicion se pueda hacer swap
                while(true){
                    p1=rndm_int(rndm)%n_2;
                    if(!fijos[a][p1]){
                        break;
                    }
                }
                //verificamos que la segunda posicion se pueda hacer swap y sea diferente de la primera
                while(true){
                    p2=rndm_int(rndm)%n_2;
                    if(!fijos[a][p2]&&p1!=p2){
                        break;
                    }
                }
            
            }
            else{
                //verificamos que en la fila o celda elegidas haya al menos 2 espacios no fijos para hacer swap
                while(true){
                    int count=0;
                    for(int i=0;i<n_2;i++){
                        if(fijos[i][a]){
                            count++;
                        }
                    }
                    if(count>n_2-2){
                        a=rndm_int(rndm)%n_2;
                    }
                    else{
                        break;
                    }
                }
                //verificamos que la primera posicion se pueda hacer swap
                while(true){
                    p1=rndm_int(rndm)%n_2;
                    if(!fijos[p1][a]){
                        break;
                    }
                }
                //verificamos que la segunda posicion se pueda hacer swap y sea diferente de la primera
                while(true){
                    p2=rndm_int(rndm)%n_2;
                    if(!fijos[p2][a]&&p1!=p2){
                        break;
                    }
                }                  
            }
            
            if(rep==representacion::filas||rep==representacion::celdas){
                //random swap dentro de una fila o celda
                tablero[a][p1]=tablero[a][p1]^tablero[a][p2];
                tablero[a][p2]=tablero[a][p1]^tablero[a][p2];
                tablero[a][p1]=tablero[a][p1]^tablero[a][p2];
            }
            else{
                //random swap dentro de una fila o celda
                tablero[p1][a]=tablero[p1][a]^tablero[p2][a];
                tablero[p2][a]=tablero[p1][a]^tablero[p2][a];
                tablero[p1][a]=tablero[p1][a]^tablero[p2][a];
            }
        }
    }

    //FUNCIONES DE FITNESS
    // Evalúa filas y columnas
    // Memoria: O(n_2)
    // Tiempo:  O(n_2^2)
    T faltantes_y_sobrantes_filas_columnas(std::vector<T>& pesos){
        T suma = 0;

        for(int i = 0; i < n_2; i++){
            std::vector<int> conteo_fila(n_2, 0);
            std::vector<int> conteo_columna(n_2, 0);

            for(int j = 0; j < n_2; j++){
                int val_fila = tablero[i][j];
                int val_col  = tablero[j][i];

                conteo_fila[val_fila]++;
                conteo_columna[val_col]++;
            }

            for(int j = 0; j < n_2; j++){
                if(conteo_fila[j] == 0){
                    suma += pesos[0];
                }
                else if(conteo_fila[j] > 1){
                    suma += pesos[1] * conteo_fila[j];
                }

                if(conteo_columna[j] == 0){
                    suma += pesos[0];
                }
                else if(conteo_columna[j] > 1){
                    suma += pesos[1] * conteo_columna[j];
                }
            }
        }

        return suma;
    }
    // Evalúa los bloques/cajas normales del Sudoku
    // Memoria: O(n_2)
    // Tiempo:  O(n_2^2)
    T faltantes_y_sobrantes_bloques(std::vector<T>& pesos){
        T suma = 0;

        for(int bloque = 0; bloque < n_2; bloque++){
            std::vector<int> conteo(n_2, 0);

            int bloque_fila = bloque / n;
            int bloque_columna = bloque % n;

            for(int i = 0; i < n; i++){
                for(int j = 0; j < n; j++){
                    int fila = bloque_fila * n + i;
                    int columna = bloque_columna * n + j;

                    int val = tablero[fila][columna];
                    conteo[val]++;
                }
            }

            for(int j = 0; j < n_2; j++){
                if(conteo[j] == 0){
                    suma += pesos[0];
                }
                else if(conteo[j] > 1){
                    suma += pesos[1] * conteo[j];
                }
            }
        }

        return suma;
    }
    // Fitness total del Sudoku
    // Debe ser 0 solamente si filas, columnas y bloques están correctos
    T faltantes_y_sobrantes(std::vector<T>& pesos){
        return faltantes_y_sobrantes_filas_columnas(pesos)+faltantes_y_sobrantes_bloques(pesos);
    }

    // QUBO para filas, columnas y bloques normales de Sudoku
    // Memoria: O(n_2^3)
    // Tiempo:  O(n_2^3)
    T QUBO(std::vector<T>& pesos){
        // conteo[i][j][k] = true si la casilla (i,j) contiene el valor k
        std::vector<std::vector<std::vector<bool>>> conteo(
            n_2,
            std::vector<std::vector<bool>>(
                n_2,
                std::vector<bool>(n_2, false)
            )
        );

        for(int i = 0; i < n_2; i++){
            for(int j = 0; j < n_2; j++){
                int val = tablero[i][j];

                // Asume valores de 0 a n_2 - 1
                conteo[i][j][val] = true;
            }
        }

        T pen_filas = 0;

        for(int i = 0; i < n_2; i++){
            for(int k = 0; k < n_2; k++){
                T suma = 0;

                for(int j = 0; j < n_2; j++){
                    suma += static_cast<T>(conteo[i][j][k]);
                }

                pen_filas += (suma - 1) * (suma - 1);
            }
        }

        T pen_columnas = 0;

        for(int j = 0; j < n_2; j++){
            for(int k = 0; k < n_2; k++){
                T suma = 0;

                for(int i = 0; i < n_2; i++){
                    suma += static_cast<T>(conteo[i][j][k]);
                }

                pen_columnas += (suma - 1) * (suma - 1);
            }
        }

        T pen_bloques = 0;

        for(int bloque = 0; bloque < n_2; bloque++){
            int bloque_fila = bloque / n;
            int bloque_columna = bloque % n;

            for(int k = 0; k < n_2; k++){
                T suma = 0;

                for(int i = 0; i < n; i++){
                    for(int j = 0; j < n; j++){
                        int fila = bloque_fila * n + i;
                        int columna = bloque_columna * n + j;

                        suma += static_cast<T>(conteo[fila][columna][k]);
                    }
                }

                pen_bloques += (suma - 1) * (suma - 1);
            }
        }

        return pen_filas + pen_columnas + pen_bloques;
    }

    void ver(){
        std::cout<<"REPRESENTACION: ";
        if(rep==representacion::filas){
            std::cout<<"FILAS"<<std::endl;
        }
        else if(rep==representacion::columnas){
            std::cout<<"COLUMNAS"<<std::endl;
        }
        else{
            std::cout<<"CELDAS"<<std::endl;
        }
        std::cout<<"N: "<<n<<std::endl;
        std::cout<<"N_2: "<<n_2<<std::endl;
        std::cout<<"SEED: "<<seed<<std::endl;

        for(int i=0;i<tablero.size();i++){
            for(int j=0;j<tablero[i].size();j++){
                std::cout<<tablero[i][j]<<", ";
            }
            std::cout<<std::endl;
        }

        std::cout<<"VALIDO: "<<validez()<<std::endl;

        std::cout<<"FITNESS: "<<fitness<<std::endl;
    }
};

template<typename T> thread_local std::mt19937 individuo<T>::rndm{std::random_device{}()};

template<typename T> thread_local std::uniform_int_distribution<int> individuo<T>::rndm_int{0, 10000000};

template<typename T> thread_local std::uniform_real_distribution<double> individuo<T>::rndm_dbl{0.0, 1.0};



/*
template<typename T>
class SUDOKU{
public:
    static thread_local std::mt19937 rndm;

    static thread_local std::uniform_int_distribution<int>     rndm_int;
    static thread_local std::uniform_real_distribution<double> rndm_dbl;

    T QUBO(struct individuo<T> *ind){
        if(ind->)
    }

    T eval(struct individuo<T> *ind){}

    void ver(){
        std::cout<<"SUDOKU"<<std::endl;
    }
};

template<typename T> thread_local std::mt19937 SUDOKU<T>::rndm{std::random_device{}()};

template<typename T> thread_local std::uniform_int_distribution<int> SUDOKU<T>::rndm_int{0, 10000000};

template<typename T> thread_local std::uniform_real_distribution<double> SUDOKU<T>::rndm_dbl{0.0, 1.0};

*/

#endif