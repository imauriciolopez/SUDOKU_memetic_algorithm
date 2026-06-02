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
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <cmath>
#include <algorithm>
#include <cstdlib>

#include <omp.h>

int main(int argc, char* argv[]){
    //cargar información
    std::ifstream file(argv[1]);
    if(!file){
        throw std::runtime_error("NO SE ENCONTRÓ EL JSON");
    }
    nlohmann::json json;
    file>>json;

    class RECOCIDO<double> *ga=new RECOCIDO<double>(0, //fitfunc
                                                    6, //n
                                                    27,//seed
                                                    representacion::filas, //rep
                                                    std::vector<double>{1.0, 1.0},//pesos
                                                    1,//n_swaps
                                                    1, //n_inds
                                                    5,//n_padres
                                                    1e-4,//n_inds
                                                    0.99999);//n_its

    ga->inicializar_poblacion();
    ga->ver();
    std::cout<<"------------------------------------------------------------------------------------------------------------------"<<std::endl;
    nlohmann::json res=ga->optimize();
    ga->ver();
    ga->liberar_poblacion();
}

    /*
    class MEMETICO<double> *ga=new MEMETICO<double>(0, //fitfunc
                                                    3, //n
                                                    27,//seed
                                                    representacion::filas, //rep
                                                    std::vector<double>{1.0, 1.0},//pesos
                                                    50,//n_swaps
                                                    1, //n_Swaps_cambio
                                                    20,//n_padres
                                                    100,//n_inds
                                                    100);//n_its
    */