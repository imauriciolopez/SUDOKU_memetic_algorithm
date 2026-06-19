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

void guardar_resultados(nlohmann::json exp, nlohmann::json res, int iter, int inst){
    std::string nombre_salida=exp["Archivo salida"].get<std::string>()+"_"+std::to_string(inst)+"_"+std::to_string(iter)+".json";
    std::ofstream salida(nombre_salida.c_str());
    if(!salida.is_open()){
        std::cerr << "No se pudo abrir el archivo." << std::endl;
        return;
    }

    salida<<res.dump(4); //4 espacios de indentación
    salida.close();
}

void experimento(nlohmann::json json){
    std::vector<std::string> instancias;
    if(json["Dataset"]=="General"){
        std::ifstream file("../datasets/instances/general/instances.json");
        if(!file){
            throw std::runtime_error("NO SE ENCONTRÓ EL JSON");
        }
        nlohmann::json json_;
        file>>json_;

        instancias=json_["Archivos"].get<std::vector<std::string> >();
    }
    else if(json["Dataset"]=="Solvable"){
        std::ifstream file("../datasets/instances/logic-solvable/instances.json");
        if(!file){
            throw std::runtime_error("NO SE ENCONTRÓ EL JSON");
        }
        nlohmann::json json_;
        file>>json_;

        instancias=json_["Archivos"].get<std::vector<std::string> >();
    }
    else if(json["Dataset"]=="SudokuPuzzles"){
        std::ifstream file("../datasets/SudokuPuzzles/instances.json");
        if(!file){
            throw std::runtime_error("NO SE ENCONTRÓ EL JSON");
        }
        nlohmann::json json_;
        file>>json_;

        instancias=json_["Archivos"].get<std::vector<std::string> >();
    }

    if(json["Optimizador"]=="Memetico"){
        for(int i=0;i<instancias.size();i++){
            int n_hilos=json["N hilos"];
            omp_set_num_threads(n_hilos);
            #pragma omp parallel for schedule(dynamic)
            for(int j=0;j<json["N experimentos"].get<int>();j++){
                std::tuple<std::vector<std::vector<bool> >, std::vector<std::vector<int> >, int > restricciones=leer_instancia(instancias[i], (json["Rep"]=="Filas")?(representacion::filas):((json["Rep"]=="Columnas")?(representacion::columnas):(representacion::celdas)));
    
                class MEMETICO<double> *ga=new MEMETICO<double>((json["Crossover"]=="Cut")?(&MEMETICO<double>::crossover_2cut):(&MEMETICO<double>::crossover_binario),
                                                                json["Fitness func"].get<int>(), //fitfunc
                                                                std::get<2>(restricciones), //n
                                                                json["Seed"].get<unsigned int>()+(unsigned int)j,//seed
                                                                (json["Rep"]=="Filas")?(representacion::filas):((json["Rep"]=="Columnas")?(representacion::columnas):(representacion::celdas)), //rep
                                                                json["Pesos"].get<std::vector<double> >(),//pesos
                                                                json["N swaps"].get<int>(),//n_swaps
                                                                json["N swaps cambio"].get<int>(), //n_swaps_cambio
                                                                json["N padres"].get<int>(),//n_padres
                                                                json["N inds"].get<int>(),//n_inds
                                                                json["N its"].get<int>(),
                                                                std::get<0>(restricciones),
                                                                std::get<1>(restricciones));//n_its*/


                ga->inicializar_poblacion();
                nlohmann::json res_=ga->optimize(json["Restriccion tiempo"].get<double>());
                guardar_resultados(json, res_, j, i);
                ga->liberar_poblacion();
                delete ga;
            }
        }
    }
    else{
        for(int i=0;i<instancias.size();i++){
            int n_hilos=json["N hilos"];
            omp_set_num_threads(n_hilos);
            #pragma omp parallel for schedule(dynamic)
            for(int j=0;j<json["N experimentos"].get<int>();j++){
                std::tuple<std::vector<std::vector<bool> >, std::vector<std::vector<int> >, int > restricciones=leer_instancia(instancias[i], (json["Rep"]=="Filas")?(representacion::filas):((json["Rep"]=="Columnas")?(representacion::columnas):(representacion::celdas)));
    
                class RECOCIDO<double> *ga=new RECOCIDO<double>(json["Fitness func"].get<int>(), //fitfunc
                                                                std::get<2>(restricciones), //n
                                                                json["Seed"].get<unsigned int>()+(unsigned int)j,//seed
                                                                (json["Rep"]=="Filas")?(representacion::filas):((json["Rep"]=="Columnas")?(representacion::columnas):(representacion::celdas)), //rep
                                                                json["Pesos"].get<std::vector<double> >(),//pesos
                                                                json["N swaps"].get<int>(),//n_swaps
                                                                json["N inds"].get<int>(), //n_inds
                                                                json["Temp inicial"].get<double>(),
                                                                json["Temp final"].get<double>(),
                                                                json["Cambio"].get<double>(),
                                                                std::get<0>(restricciones),
                                                                std::get<1>(restricciones));//n_its*/

                ga->inicializar_poblacion();
                nlohmann::json res_=ga->optimize(json["Restriccion tiempo"].get<double>());
                guardar_resultados(json, res_, j, i);
                ga->liberar_poblacion();
                delete ga;
            }
        }
    }
}


int main(int argc, char* argv[]){
    //cargar información
    std::ifstream file(argv[1]);
    if(!file){
        throw std::runtime_error("NO SE ENCONTRÓ EL JSON");
    }
    nlohmann::json json;
    file>>json;

    experimento(json);
}

    /*
    
    class RECOCIDO<double> *ga=new RECOCIDO<double>(1, //fitfunc
                                                    3, //n
                                                    27,//seed
                                                    representacion::filas, //rep
                                                    std::vector<double>{1.0, 1.0},//pesos
                                                    1,//n_swaps
                                                    1, //n_inds
                                                    10,
                                                    1e-5,//n_inds
                                                    0.99999,
                                                    fijos,
                                                    pistas);
    */


    /*
    std::string ruta="../datasets/SudokuPuzzles/SD2.txt";

    std::tuple<std::vector<std::vector<bool> >, std::vector<std::vector<int> > > restricciones=leer_instancia(ruta, representacion::filas);
    
    class MEMETICO<double> *ga=new MEMETICO<double>(&MEMETICO<double>::crossover_2cut,
                                                    0, //fitfunc
                                                    3, //n
                                                    27,//seed
                                                    representacion::filas, //rep
                                                    std::vector<double>{1.0, 1.0},//pesos
                                                    50,//n_swaps
                                                    1, //n_swaps_cambio
                                                    20,//n_padres
                                                    100,//n_inds
                                                    100,
                                                    std::get<0>(restricciones),
                                                    std::get<1>(restricciones));//n_its

    ga->inicializar_poblacion();
    ga->ver();
    std::cout<<"------------------------------------------------------------------------------------------------------------------"<<std::endl;
    nlohmann::json res_=ga->optimize();
    ga->ver();
    ga->liberar_poblacion();
    */