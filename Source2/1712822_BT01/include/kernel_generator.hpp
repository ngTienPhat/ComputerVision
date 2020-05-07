#ifndef KERNEL_GENERATOR_HPP__
#define KERNEL_GENERATOR_HPP__

#include "common.hpp"

class KernelGenerator{


public:
    static Mat getSobelKernelGx(){
        return (Mat_<int>(3,3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
    }

    static Mat getSobelKernelGy(){
        return (Mat_<int>(3,3) << -1, -2, -1, 0, 0, 0, 1, 2, 1);
    }

    static Mat getPrewittKernelGx(){
        return (Mat_<int>(3,3) << -1, 0, 1, -1, 0, 1, -1, 0, 1);
    }

    static Mat getPrewittKernelGy(){
        return (Mat_<int>(3,3) << -1, -1, -1, 0, 0, 0, 1, 1, 1);
    }

    static Mat getLaplaceKernelGx(){
        return (Mat_<int>(3,3) << 0, -1, 0, -1, 4, -1, 0, -1, 0);
    }

    static Mat getLaplaceKernelGy(){
        return (Mat_<int>(3,3) << -1, -1, -1, -1, 8, -1, -1, -1, -1);
    }
};

#endif // KERNEL_GENERATOR_HPP__
