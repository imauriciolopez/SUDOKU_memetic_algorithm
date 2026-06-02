#include <iostream>

#ifndef TABU_HPP
#define TABU_HPP

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

void hello_world_TABU();

template<typename T>
class TABU{
public:
    static thread_local std::mt19937 rndm;

    static thread_local std::uniform_int_distribution<int>     rndm_int;
    static thread_local std::uniform_real_distribution<double> rndm_dbl;

    unsigned int n_pob;
    unsigned int n_its;
    std::vector<struct individuo<T>*> pob;

    void ver(){
        std::cout<<"SUDOKU"<<std::endl;
    }
};

template<typename T> thread_local std::mt19937 TABU<T>::rndm{std::random_device{}()};

template<typename T> thread_local std::uniform_int_distribution<int> TABU<T>::rndm_int{0, 10000000};

template<typename T> thread_local std::uniform_real_distribution<double> TABU<T>::rndm_dbl{0.0, 1.0};


#endif