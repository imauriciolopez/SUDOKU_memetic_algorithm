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
    int n_iters_renovacion;

    std::vector<std::vector<bool> > fijos;
    std::vector<std::vector<int> > pistas;

    //función de crossover
    using crossover_func=struct individuo<T>* (MEMETICO<T>::*)(int,int,std::vector<struct individuo<T>*>);
    crossover_func crossover;

    MEMETICO(crossover_func cross,
             int fit_func=0, 
             unsigned int n=0,
             unsigned int seed=0, 
             representacion rep=representacion::filas, 
             std::vector<T> pesos=std::vector<T>{},
             int n_swaps=0,
             int n_swaps_cambio=0,
             int n_padres=0,
             int n_inds=0, 
             int n_its=0,
             std::vector<std::vector<bool> > fijos_={{}},
             std::vector<std::vector<int> > pistas_={{}}):
             crossover(cross),
             fijos(fijos_),
             pistas(pistas_),
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
        n_iters_renovacion=20;
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
                                            pesos,
                                            fijos, 
                                            pistas);
        }
    }
    
    void ver(){
        for(int i=0;i<n_inds;i++){
            pob[i]->ver();
        }
    }

    struct individuo<T>* crossover_binario(int padre_1, int padre_2, std::vector<struct individuo<T>*>padres){
        struct individuo<T>* n_ind=new struct individuo<T>(*padres[padre_1]);

        //alguna unidad debe quedar igual que el padre 1
        int igual=rndm_int(rndm)%(n*n);

        for(int j=0;j<(n*n);j++){
            //crossover como lanzamiento de moneda
            if(rndm_dbl(rndm)<0.5&&j!=igual){

                std::vector<bool> usado((n*n), false);

                //marcamos los valores que ya están fijos en el hijo
                for(int k = 0; k < (n*n); k++){
                    if(n_ind->fijos[j][k]){
                        usado[n_ind->tablero[j][k]]=true;
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
                    if(!n_ind->fijos[j][k]){
                        n_ind->tablero[j][k]=disponibles[pos];
                        pos++;
                    }
                }
            }
        }
        return n_ind;
    }

    std::vector<unsigned int> two_point_v1(const std::vector<unsigned int>& perm1, const std::vector<unsigned int>& perm2){
        int n_nums = perm1.size();

        if(n_nums <= 1){
            return perm1;
        }

        int L = rndm_int(rndm) % n_nums;
        int R = rndm_int(rndm) % n_nums;

        while(L == R){
            R = rndm_int(rndm) % n_nums;
        }

        if(L > R){
            std::swap(L, R);
        }

        std::vector<bool> used_1(n * n, false);
        std::vector<bool> used_2(n * n, false);

        std::vector<unsigned int> nueva_1(n_nums, 0);
        std::vector<unsigned int> nueva_2(n_nums, 0);

        for(int i = 0; i < L; i++){
            nueva_1[i] = perm1[i];
            nueva_2[i] = perm2[i];

            used_1[perm1[i]] = true;
            used_2[perm2[i]] = true;
        }

        for(int i = R + 1; i < n_nums; i++){
            nueva_1[i] = perm1[i];
            nueva_2[i] = perm2[i];

            used_1[perm1[i]] = true;
            used_2[perm2[i]] = true;
        }

        int idx_1 = L;
        int idx_2 = L;

        for(int i = 0; i < n_nums; i++){
            if(!used_1[perm2[i]]){
                nueva_1[idx_1] = perm2[i];
                idx_1++;
            }

            if(!used_2[perm1[i]]){
                nueva_2[idx_2] = perm1[i];
                idx_2++;
            }
        }

        return (rndm_int(rndm) % 2 == 0) ? nueva_1 : nueva_2;
    }

    struct individuo<T>* crossover_2cut(int padre_1, int padre_2, std::vector<struct individuo<T>*>padres){
        struct individuo<T>* n_ind=new struct individuo<T>(*padres[padre_1]);

        //alguna unidad debe quedar igual que el padre 1
        int igual=rndm_int(rndm)%((n*n)-1);

        for(int i=0;i<(n*n);i++){
            if(i!=igual){
                std::vector<unsigned int> no_fijos_p_1;
                std::vector<unsigned int> no_fijos_p_2;

                if(padres[padre_1]->rep==representacion::filas||padres[padre_1]->rep==representacion::celdas){
                    for(int j=0;j<(n*n);j++){
                        //los fijos deben ser iguales porque resuelven la misma instancia
                        if(!padres[padre_1]->fijos[i][j]){
                            no_fijos_p_1.push_back(padres[padre_1]->tablero[i][j]);
                            no_fijos_p_2.push_back(padres[padre_2]->tablero[i][j]);
                        }
                    }
                }
                
                else if(padres[padre_1]->rep==representacion::columnas){
                    for(int j=0;j<(n*n);j++){
                        //los fijos deben ser iguales porque resuelven la misma instancia
                        if(!padres[padre_1]->fijos[j][i]){
                            no_fijos_p_1.push_back(padres[padre_1]->tablero[j][i]);
                            no_fijos_p_2.push_back(padres[padre_2]->tablero[j][i]);
                        }
                    }
                }
                
                std::vector<unsigned int> n_unidad=two_point_v1(no_fijos_p_1, no_fijos_p_2);
                
                if(padres[padre_1]->rep==representacion::filas||padres[padre_1]->rep==representacion::celdas){
                    int idx = 0;
                    for(int j = 0; j < (n*n); j++){
                        if(!n_ind->fijos[i][j]){
                            n_ind->tablero[i][j] = n_unidad[idx];
                            idx++;
                        }
                    }
                }
                
                else if(padres[padre_1]->rep==representacion::columnas){
                    int idx=0;
                    for(int j=0;j<(n*n);j++){
                        if(!n_ind->fijos[j][i]){
                            n_ind->tablero[j][i]=n_unidad[idx];
                            idx++;
                        }
                    }
                }
            }
        }

        return n_ind;
    }

    nlohmann::json optimize(double tiempo){
        unsigned t0=clock();

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

            //creamos la nueva población como crossovers de padres
            for(int i=0; i<n_inds;i++){
                //elegimos al padre 1
                int padre_1=rndm_int(rndm)%n_padres;
                //elegimos al padre 2
                int padre_2=rndm_int(rndm)%n_padres;
                
                n_pob[i]=(this->*crossover)(padre_1, padre_2, padres);
            }

            //hacemos algunos cambios
            for(int i=0; i<n_inds;i++) n_pob[i]->random_valid_swaps(n_swaps_cambio);

            for(int i=0; i<n_inds;i++) n_pob[i]->fitness=(n_pob[i]->*(n_pob[i]->fit_func))(pesos);

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

            //torneo binario contra individuo aleatorio
            for(int i=0;i<n_inds;i++){
                //intentar reemplazar aleatoriamente un individuo fuera de la elite
                int idx=n_padres+(rndm_int(rndm)%(n_inds-n_padres-1));
                //std::cout<<idx<<std::endl;
                struct individuo<T>* perdedor;
                if(n_pob[i]->fitness<=pob[idx]->fitness){
                    perdedor=pob[idx];
                    pob[idx]=n_pob[i];
                    n_pob[i]=nullptr;
                }
                else{
                    perdedor=n_pob[i];
                    n_pob[i]=nullptr;
                }
                delete perdedor;
            }

            
            //renovar
            if(iter>0&&iter%n_iters_renovacion==0){
                for(int i=0;i<n_inds;i++) fitnesses[i]=pob[i]->fitness;
                ordenados=b_u_merge_sort<T>(fitnesses, true, false);
                for(int i=0;i<n_inds;i++) pob_copia[i]=pob[ordenados[i]];
                pob=pob_copia;

                for(int i=(int)(n_inds*0.5);i<n_inds;i++){
                    delete pob[i];
                    pob[i] = new struct individuo<T>((fit_func==0)?(&individuo<double>::faltantes_y_sobrantes):(&individuo<double>::QUBO),
                                                      seed+static_cast<unsigned int>(iter*n_inds+i),
                                                      n,
                                                      {{}},
                                                      rep,
                                                      pesos,
                                                      fijos,
                                                      pistas);
                    pob[i]->fitness=(pob[i]->*(pob[i]->fit_func))(pesos);
                }
            }

            if((double(clock()-t0)/CLOCKS_PER_SEC)>tiempo){
                break;
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