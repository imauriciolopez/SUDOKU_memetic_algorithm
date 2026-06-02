#include <iostream>

#ifndef RECOCIDO_HPP
#define RECOCIDO_HPP

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

void hello_world_RECOCIDO();

template<typename T>
class RECOCIDO{
public:
    static thread_local std::mt19937 rndm;

    static thread_local std::uniform_int_distribution<int>     rndm_int;
    static thread_local std::uniform_real_distribution<double> rndm_dbl;

    int n_swaps;
    unsigned int n;
    int fit_func;
    representacion rep;
    unsigned int seed;
    unsigned int n_inds;
    std::vector<T> pesos;
    std::vector<struct individuo<T>*> pob;

    double temp_inicial;
    double temp_final;
    double enfriamiento;

    RECOCIDO(int fit_func=0, 
             unsigned int n=0,
             unsigned int seed=0, 
             representacion rep=representacion::filas, 
             std::vector<T> pesos=std::vector<T>{},
             int n_swaps=0,
             int n_inds=0, 
             double temp_inicial=0.0, 
             double temp_final=0.0, 
             double enfriamiento=0.0):
             n_swaps(n_swaps), 
             fit_func(fit_func), 
             n(n),
             rep(rep), 
             pesos(pesos),
             seed(seed), 
             n_inds(n_inds), 
             temp_inicial(temp_inicial), 
             temp_final(temp_final), 
             enfriamiento(enfriamiento){
        rndm.seed(seed);
    }

    void liberar_poblacion(){
        if(!pob.empty()){
            for(int i=0;i<pob.size();i++){
                delete pob[i];
            }
        }
    }

    void inicializar_poblacion(){
        liberar_poblacion();
        pob=std::vector<struct individuo<T>*>(n_inds);
        for(int i=0;i<n_inds;i++){
            pob[i]=new struct individuo<T>((fit_func==0)?(&individuo<double>::faltantes_y_sobrantes):(&individuo<double>::QUBO), 
                                            seed+(unsigned int)i,
                                            n, 
                                            {{}}, 
                                            rep,
                                            pesos);
        }
    }
    
    void ver(){
        for(int i=0;i<n_inds;i++){
            pob[i]->ver();
        }
        std::cout<<"SUDOKU"<<std::endl;
    }

    nlohmann::json optimize(){
        if(pob.empty()){
            inicializar_poblacion();
        }

        for(int ind=0;ind<n_inds;ind++){
            struct individuo<T>* actual=pob[ind];
            struct individuo<T>* mejor=new struct individuo<T>(*actual);

            double T_actual=temp_inicial;

            while(T_actual>temp_final){

                struct individuo<T>* vecino=new struct individuo<T>(*actual);

                //Generar vecino
                vecino->random_valid_swaps(n_swaps);
                
                //Evaluar el nuevo vecino
                vecino->fitness=(vecino->*(vecino->fit_func))(pesos);

                T delta=vecino->fitness-actual->fitness;

                bool aceptar=false;

                //aceptamos con cierta probabilidad dada por la temperatura
                if(delta<=0){
                    aceptar=true;
                }
                else{
                    double prob=std::exp(-static_cast<double>(delta)/T_actual);
                    double r=rndm_dbl(rndm);

                    if(r<prob){
                        aceptar=true;
                    }
                }

                //Aceptación
                if(aceptar){
                    delete actual;
                    actual=vecino;

                    //vemos si es el mejor o no
                    if(actual->fitness<mejor->fitness){
                        delete mejor;
                        mejor=new struct individuo<T>(*actual);
                    }

                    //si ya está en el óptimo, terminamos ciclo
                    if(mejor->fitness==0){
                        break;
                    }
                }
                else{
                    delete vecino;
                }

                //Enfriamiento
                T_actual*=enfriamiento;
            }

            delete actual;
            pob[ind]=mejor;
        }

        std::vector<T> fitnesses(n_inds);
        for(int i=0;i<n_inds;i++) fitnesses[i]=pob[i]->fitness;
        std::vector<int> ordenados=b_u_merge_sort<T>(fitnesses, true, false);
        std::vector<struct individuo<T>*> pob_copia(n_inds);
        for(int i=0;i<n_inds;i++) pob_copia[i]=pob[ordenados[i]];
        pob=pob_copia;

        std::vector<double> m_d=mean_desv_est<double>(fitnesses);

        nlohmann::json res;
        res["Mejor fitness"]=pob[0]->fitness;
        res["Peor fitness"]=pob[n_inds-1]->fitness;
        res["Promedio"]=m_d[0];
        res["Desvest"]=m_d[1];

        return res;
    }
};

template<typename T> thread_local std::mt19937 RECOCIDO<T>::rndm{std::random_device{}()};

template<typename T> thread_local std::uniform_int_distribution<int> RECOCIDO<T>::rndm_int{0, 10000000};

template<typename T> thread_local std::uniform_real_distribution<double> RECOCIDO<T>::rndm_dbl{0.0, 1.0};


#endif