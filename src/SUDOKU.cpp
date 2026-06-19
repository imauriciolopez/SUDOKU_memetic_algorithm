#include "../include/utils.hpp"
#include "../include/SUDOKU.hpp"
#include "../include/TABU.hpp"
#include "../include/MEMETICO.hpp"
#include "../include/PSO.hpp"
#include "../include/RECOCIDO.hpp"

#include <../nlohmann/json.hpp>
#include <iostream>
#include <iomanip>
#include <random>
#include <ctime>
#include <cstring>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <cstdlib>
#include <omp.h>

std::tuple<std::vector<std::vector<bool> >, std::vector<std::vector<int> >, int > leer_instancia(std::string file, representacion rep){
    std::ifstream archivo(file.c_str());

    if (!archivo.is_open()) {
        throw std::runtime_error("No se pudo abrir el archivo:");
    }

    unsigned int n;
    archivo>>n;
    int n_2=std::pow(n, 2);

    std::vector<std::vector<bool> > fijos(n_2, std::vector<bool>(n_2, true));
    std::vector<std::vector<int> > pistas(n_2, std::vector<int>(n_2, 0));

    //tablero natural leído del archivo
    for(int i=0;i<n_2;i++){
        for(int j=0;j<n_2;j++){
            if(rep==representacion::filas||rep==representacion::celdas){
                archivo>>pistas[i][j];
                pistas[i][j]--;
            }
            else if(rep==representacion::columnas){
                archivo>>pistas[j][i];
                pistas[j][i]--;
            }
        }
    }

    for(int i=0;i<n_2;i++){
        for(int j=0;j<n_2;j++){
            if(rep==representacion::filas||rep==representacion::celdas){
                if(pistas[i][j]==-1){
                    fijos[i][j]=false;
                }
            }
            else if(rep==representacion::columnas){
                if(pistas[j][i]==-1){
                    fijos[j][i]=false;
                }
            }
        }
    }

    return std::make_tuple(fijos, pistas, n);
}

void hello_world_local(){
    std::cout<<"Hello, world! (from local)"<<std::endl;
}


