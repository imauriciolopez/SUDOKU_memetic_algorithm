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

#include <typeinfo>

void hello_world_local();

//enum del tipo de representación
enum class representacion{
    filas,
    columnas,
    celdas
};

template<typename T>
struct individuo{
    //atributos individuales
    unsigned int n;
    unsigned int n_2;
    std::vector<std::vector<unsigned int> > tablero;
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

    //INICIALIZACIÓN LIBRE
    individuo(fitness_func fit, 
              unsigned int seed=0,
              unsigned int n=0, 
              std::vector<std::vector<unsigned int> > tablero_={{}}, 
              representacion rep=representacion::filas,
              std::vector<T> pesos={}):
              fit_func(fit),
              seed(seed), 
              n(n), 
              n_2(std::pow(n, 2)),
              tablero(tablero_), 
              rep(rep){
        rndm.seed(seed);
        if(tablero_[0].empty()) inicializar_rndm(pesos);
        //else fitness=(this->*fit_func)(pesos);
        fijos=std::vector<std::vector<bool> >(n_2, std::vector<bool>(n_2, false));
    }

    //INICIALIZACIÓN RESTRINGIDA
    individuo(fitness_func fit, 
              unsigned int seed=0,
              unsigned int n=0, 
              std::vector<std::vector<unsigned int> > tablero_={{}}, 
              std::vector<std::vector<bool> > fijos={{}}, 
              representacion rep=representacion::filas,
              std::vector<T> pesos={}):
              fit_func(fit),
              seed(seed), 
              n(n), 
              n_2(std::pow(n, 2)),
              tablero(tablero_), 
              rep(rep){
        rndm.seed(seed);
        if(tablero_[0].empty()) inicializar_rndm(pesos);
        else fitness=(this->*fit_func)(pesos);
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

        return true;
    }

    void inicializar_rndm(std::vector<T>& pesos){
        if(rep==representacion::filas){
            do{
                tablero=std::vector<std::vector<unsigned int> >(n_2, std::vector<unsigned int>(n_2));
                for(int i=0;i<n_2;i++){
                    tablero[i]=permutacion_aleatoria();
                }
                fitness=(this->*fit_func)(pesos);
            }while(!validez());
        }
        else if(rep==representacion::columnas){
            do{
                tablero=std::vector<std::vector<unsigned int> >(n_2, std::vector<unsigned int>(n_2));
                for(int i=0;i<n_2;i++){
                    std::vector<unsigned int> perm=permutacion_aleatoria();
                    for(int j=0;j<n_2;j++){
                        tablero[j][i]=perm[j];
                    }
                }
                fitness=(this->*fit_func)(pesos);
            }while(!validez());
        }
        else{
            do{
                tablero=std::vector<std::vector<unsigned int> >(n_2, std::vector<unsigned int>(n_2));
                for(int i=0;i<n_2;i++){
                    std::vector<unsigned int> perm=permutacion_aleatoria();
                    for(int j=0;j<n_2;j++){
                        tablero[i][j]=perm[j];
                    }
                }
                fitness=(this->*fit_func)(pesos);
            }while(!validez());
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

    //costo en memoria: O(n_2)
    //costo en tiempo:  O(n_2^2)
    T faltantes_y_sobrantes_filas(std::vector<T>& pesos){
        T suma=0;
        //iteramos sobre filas
        
        for(int i=0;i<n_2;i++){
            std::vector<int> conteo_1(n_2, 0);
            std::vector<int> conteo_2(n_2, 0);

            
            for(int j=0;j<n_2;j++){
                conteo_1[tablero[i][j]]++;
                conteo_2[tablero[j][i]]++;
            }
            
            for(int j=0;j<n_2;j++){
                
                if(conteo_1[j]==0){
                    suma=suma+pesos[0];
                }
                else if(conteo_1[j]>1){
                    suma=suma+pesos[1]*conteo_1[j];
                }

                if(conteo_2[j]==0){
                    suma=suma+pesos[0];
                }
                else if(conteo_2[j]>1){
                    suma=suma+pesos[1]*conteo_1[j];
                }
                    
            }
        }
        return suma;
    }
    //costo en memoria: O(n_2^2)
    //costo en tiempo:  O(n_2^2)
    T faltantes_y_sobrantes_celdas(std::vector<T>& pesos){
        std::vector<std::vector<int> > conteo_1(n_2, std::vector<int>(n_2, 0));//en cada fila tiene un conteo de qué números faltan por fila
        std::vector<std::vector<int> > conteo_2(n_2, std::vector<int>(n_2, 0));//en cada fila tiene un conteo de qué números faltan por columna
        
        //iteramos sobre las celdas
        for(int i=0;i<n_2;i++){
            for(int j=0;j<n_2;j++){
                int fila=(int)(std::floor((double)j/(double)n))+(int)(std::floor((double)i/(double)n)*n);
                int columna=j%n+(i%n)*n;

                conteo_1[fila][tablero[i][j]]++;
                conteo_2[columna][tablero[i][j]]++;
            }
        }

        //hacemos el conteo de faltantes
        T suma=0;
        for(int i=0;i<n_2;i++){
            for(int j=0;j<n_2;j++){
                if(conteo_1[i][j]==0){
                    suma=suma+pesos[0];
                }
                else if(conteo_1[i][j]>1){
                    suma=suma+conteo_1[i][j]*pesos[1];
                }
                if(conteo_2[i][j]==0){
                    suma=suma+pesos[0];
                }
                else if(conteo_2[i][j]>1){
                    suma=suma+conteo_2[i][j]*pesos[1];
                }
            }
        }
        return suma;
    }
    T faltantes_y_sobrantes(std::vector<T>& pesos){
        if(rep==representacion::filas||rep==representacion::columnas) return faltantes_y_sobrantes_filas(pesos);
        else return faltantes_y_sobrantes_celdas(pesos);
    }

    //costo en memoria: O(n_2^3)
    //costo en tiempo:  O(n_2^3)
    T QUBO_filas(std::vector<T>& pesos){
        //mapeamos casilla por casilla qué número tiene (si la casilla 2, 3 tiene un 5 por ejemplo, entonces conteo[2][3][5]=true, y los demás en conteo[2][3]=false)
        std::vector<std::vector<std::vector<bool> > > conteo(n_2, std::vector<std::vector<bool> >(n_2, std::vector<bool>(n_2, false)));
        for(int i=0;i<n_2;i++) for(int j=0;j<n_2;j++) conteo[i][j][tablero[i][j]]=true;

        
        T pen_celdas=0;
        for(int i=0;i<n;i++){
            for(int j=0;j<n;j++){//iteramos sobre cada celda
                T suma=0;

                for(int k=0;k<n_2;k++){//iteramos sobre cada posible valor dentro de la casilla
                    for(int fila=0;fila<n;fila++){//iteramos sobre casilla elemento de la celda actual
                        for(int columna=0;columna<n;columna++){
                            suma=suma+conteo[i*n+fila][j*n+columna][k];
                        }
                    }
                }

                pen_celdas=pen_celdas+(suma-1)*(suma-1);
            }
        }
        

        T pen_filas=0;
        for(int i=0;i<n_2;i++){
            for(int k=0;k<n_2;k++){
                T suma=0;
                for(int j=0;j<n_2;j++){
                    suma=suma+conteo[i][j][k];
                }
                pen_filas=pen_filas+((suma-1)*(suma-1));
            }
        }

        T pen_columnas=0;
        for(int j=0;j<n_2;j++){
            for(int k=0;k<n_2;k++){
                T suma=0;
                for(int i=0;i<n_2;i++){
                    suma=suma+conteo[i][j][k];
                }
                pen_columnas=pen_columnas+((suma-1)*(suma-1));
            }
        }

        return pen_celdas+pen_filas+pen_columnas;
    }
    //costo en memoria: O(n_2^3)
    //costo en tiempo:  O(n_2^3)
    T QUBO_celdas(std::vector<T>& pesos){
        std::vector<std::vector<std::vector<bool> > > conteo(n_2, std::vector<std::vector<bool> >(n_2, std::vector<bool>(n_2, false)));
        //iteramos sobre las celdas
        for(int i=0;i<n_2;i++){
            for(int j=0;j<n_2;j++){
                int fila=(int)(std::floor((double)j/(double)n))+(int)(std::floor((double)i/(double)n)*n);
                int columna=j%n+(i%n)*n;

                conteo[fila][columna][tablero[i][j]]=true;
            }
        }
        //mapeamos casilla por casilla qué número tiene (si la casilla 2, 3 tiene un 5 por ejemplo, entonces conteo[2][3][5]=true, y los demás en conteo[2][3]=false)
        
        T pen_filas=0;
        for(int i=0;i<n_2;i++){
            for(int k=0;k<n_2;k++){
                T suma=0;
                for(int j=0;j<n_2;j++){
                    suma=suma+conteo[i][j][k];
                }
                pen_filas=pen_filas+((suma-1)*(suma-1));
            }
        }

        T pen_columnas=0;
        for(int j=0;j<n_2;j++){
            for(int k=0;k<n_2;k++){
                T suma=0;
                for(int i=0;i<n_2;i++){
                    suma=suma+conteo[i][j][k];
                }
                pen_columnas=pen_columnas+((suma-1)*(suma-1));
            }
        }

        return pen_filas+pen_columnas;
    }
    T QUBO(std::vector<T>& pesos){
        if(rep==representacion::filas||rep==representacion::columnas) return QUBO_filas(pesos);
        else return QUBO_celdas(pesos);
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

        /*
        if(rep==representacion::columnas||rep==representacion::filas){
            for(int i=0;i<tablero.size();i++){
                for(int j=0;j<tablero[i].size();j++){
                    std::cout<<tablero[i][j]<<", ";
                }
                std::cout<<std::endl;
            }
        }
        else{
            //iteramos sobre las celdas
            std::vector<std::vector<int> > n_tab(n_2, std::vector<int>(n_2));
            for(int i=0;i<n_2;i++){
                for(int j=0;j<n_2;j++){
                    int fila=(int)(std::floor((double)j/(double)n))+(int)(std::floor((double)i/(double)n)*n);
                    int columna=j%n+(i%n)*n;

                    n_tab[fila][columna]=tablero[i][j];
                }
            }

            for(int i=0;i<n_tab.size();i++){
                for(int j=0;j<n_tab[i].size();j++){
                    std::cout<<n_tab[i][j]<<", ";
                }
                std::cout<<std::endl;
            }
        }*/
        
        std::cout<<"FITNESS: "<<fitness<<std::endl;
    }
};

template<typename T> thread_local std::mt19937 individuo<T>::rndm{std::random_device{}()};

template<typename T> thread_local std::uniform_int_distribution<int> individuo<T>::rndm_int{0, 10000000};

template<typename T> thread_local std::uniform_real_distribution<double> individuo<T>::rndm_dbl{0.0, 1.0};

template<typename T> nlohmann::json experimento(nlohmann::json json){
    nlohmann::json j_son;
    j_son["value1"]="void";

    return j_son;
}

template<typename T> void guardar_resultados(nlohmann::json exp, nlohmann::json res){
    
}






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