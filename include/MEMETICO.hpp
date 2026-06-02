#include <iostream>

#ifndef MEMETICO_HPP
#define MEMETICO_HPP

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

void hello_world_MEMETICO();

template<typename T>
class MEMETICO{
public:
    static thread_local std::mt19937 rndm;

    static thread_local std::uniform_int_distribution<int>     rndm_int;
    static thread_local std::uniform_real_distribution<double> rndm_dbl;

    unsigned int n_inds;
    unsigned int n_its;
    std::vector<struct individuo<T>*> pob;
    std::vector<struct individuo<T>*> padres;
    std::vector<struct individuo<T>*> n_pob;

    int n_swaps;
    unsigned int n;
    int n_padres;
    int fit_func;
    int n_swaps_cambio;
    representacion rep;
    unsigned int seed;
    std::vector<T> pesos;

    MEMETICO(int fit_func=0, 
             unsigned int n=0,
             unsigned int seed=0, 
             representacion rep=representacion::filas, 
             std::vector<T> pesos=std::vector<T>{},
             int n_swaps=0,
             int n_swaps_cambio=0,
             int n_padres=0,
             int n_inds=0, 
             int n_its=0):
             n_swaps_cambio(n_swaps_cambio), 
             pesos(pesos),
             n_padres(n_padres),
             n_inds(n_inds),
             n_pob(n_pob), 
             n_its(n_its), 
             seed(seed), 
             n_swaps(n_swaps),
             n(n), 
             fit_func(fit_func),
             rep(rep){
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
            std::cout<<"HERE"<<std::endl;
            
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
    }

    nlohmann::json optimize(){
        padres=std::vector<struct individuo<T>*>(n_padres);
        n_pob=std::vector<struct individuo<T>*>(n_inds);
        for(int iter=0;iter<n_its;iter++){
            //ordenamos la población
            std::vector<T> fitnesses(n_inds);
            for(int i=0;i<n_inds;i++) fitnesses[i]=pob[i]->fitness;
            std::vector<int> ordenados=b_u_merge_sort<T>(fitnesses, true, false);
            std::vector<struct individuo<T>*> pob_copia(n_inds);
            for(int i=0;i<n_inds;i++) pob_copia[i]=pob[ordenados[i]];
            pob=pob_copia;

            //elegimos el pool de padres
            for(int i=0;i<n_padres;i++) padres[i]=pob[i];

            std::vector<int> lista_padres(n_padres);
            //creamos la nueva población como crossovers de padres
            for(int i=0; i<n_inds;i++){
                //elegimos al padre 1
                int padre_1=rndm_int(rndm)%n_padres;
                //elegimos al padre 2
                int padre_2=rndm_int(rndm)%n_padres;

                n_pob[i]=new struct individuo<T>(*padres[padre_1]);

                //alguna unidad debe quedar igual que el padre 1
                int igual = rndm_int(rndm)%(n*n);

                for(int j=0;j<(n*n);j++){
                    //crossover como lanzamiento de moneda
                    if(rndm_dbl(rndm)<0.5&&j!=igual){

                        std::vector<bool> usado((n*n), false);

                        //marcamos los valores que ya están fijos en el hijo
                        for(int k = 0; k < (n*n); k++){
                            if(n_pob[i]->fijos[j][k]){
                                usado[n_pob[i]->tablero[j][k]]=true;
                            }
                        }

                        //tomamos del padre 2, en orden, los valores que no estén usados
                        std::vector<unsigned int> disponibles;

                        for(int k=0; k<(n*n);k++){
                            unsigned int valor=padres[padre_2]->tablero[j][k];
                            if(!usado[valor]){
                                disponibles.push_back(valor);
                                usado[valor]=true;
                            }
                        }

                        //rellenamos las posiciones no fijas del hijo
                        int pos=0;

                        for(int k=0;k<(n*n);k++){
                            if(!n_pob[i]->fijos[j][k]){
                                n_pob[i]->tablero[j][k]=disponibles[pos];
                                pos++;
                            }
                        }
                    }
                }

                n_pob[i]->fitness=(n_pob[i]->*(n_pob[i]->fit_func))(pesos);
            }

            //hacemos algunos cambios
            for(int i=0; i<n_inds;i++) n_pob[i]->random_valid_swaps(n_swaps_cambio);

            //busqueda local stochastic hill climbing
            for(int i=0;i<n_inds;i++){
                int count_no_mejora=0;
                while(count_no_mejora<n_swaps){
                    struct individuo<T>* copia=new struct individuo<T>(*n_pob[i]);
                    n_pob[i]->random_valid_swaps(1);
                    T nuevo_fitness=(n_pob[i]->*(n_pob[i]->fit_func))(pesos);
                    if(nuevo_fitness<=n_pob[i]->fitness){
                        n_pob[i]->fitness=nuevo_fitness;
                        count_no_mejora=0;
                        delete copia;
                    }
                    else{
                        delete n_pob[i];
                        n_pob[i]=copia;
                        count_no_mejora++;
                    }
                }
            }

            //ordenamos la nueva población
            for(int i=0;i<n_inds;i++) fitnesses[i]=n_pob[i]->fitness;
            ordenados=b_u_merge_sort<T>(fitnesses, true, false);
            for(int i=0;i<n_inds;i++) pob_copia[i]=n_pob[ordenados[i]];
            n_pob=pob_copia;

            //torneo binario contra toda la población
            for(int i=0;i<n_inds;i++){
                struct individuo<T>* perdedor;
                if(n_pob[i]->fitness<=pob[i]->fitness){
                    perdedor=pob[i];
                    pob[i]=n_pob[i];
                    n_pob[i] = nullptr;
                }
                else{
                    perdedor=n_pob[i];
                    n_pob[i] = nullptr;
                }
                delete perdedor;
            }
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

template<typename T> thread_local std::mt19937 MEMETICO<T>::rndm{std::random_device{}()};

template<typename T> thread_local std::uniform_int_distribution<int> MEMETICO<T>::rndm_int{0, 10000000};

template<typename T> thread_local std::uniform_real_distribution<double> MEMETICO<T>::rndm_dbl{0.0, 1.0};

#endif