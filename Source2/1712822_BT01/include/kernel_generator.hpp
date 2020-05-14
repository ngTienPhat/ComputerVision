#ifndef KERNEL_GENERATOR_HPP__
#define KERNEL_GENERATOR_HPP__

#include "common.hpp"

class KernelGenerator{


public:
	static Mat getGaussianBlur3x3() {
		return (Mat_<float>(3, 3) << 1.0 / 16, 2.0 / 16, 1.0 / 16, 
									 2.0 / 16, 4.0 / 16, 2.0 / 16, 
									 1.0 / 16, 2.0 / 16, 1.0 / 16);
	}
	static Mat getGaussianBlur5x5() {
		return (Mat_<float>(5, 5) << 2.0 / 159, 4.0 / 159, 5.0 / 159, 4.0 / 159, 2.0 / 159, 
			                         4.0 / 159, 9.0 / 159, 12.0 / 159, 9.0 / 159, 4.0 / 159,
									 5.0 / 159, 12.0 / 159, 15.0 / 159, 12.0 / 159, 5.0 / 159, 
									 4.0 / 159, 9.0 / 159, 12.0 / 159, 9.0 / 159, 4.0 / 159, 
									 2.0 / 159, 4.0 / 159, 5.0 / 159, 4.0 / 159, 2.0 / 159);
	}
	static Mat getSobelKernelGx(){
        return (Mat_<float>(3,3) << -1, 0, 1, 
								  -2, 0, 2, 
			                      -1, 0, 1);
    }

    static Mat getSobelKernelGy(){
        return (Mat_<float>(3,3) << -1, -2, -1, 
			                       0, 0, 0, 
			                       1, 2, 1);
    }

    static Mat getPrewittKernelGx(){
        return (Mat_<float>(3,3) << -1, 0, 1,
								  -1, 0, 1, 
			                      -1, 0, 1);
    }

    static Mat getPrewittKernelGy(){
        return (Mat_<float>(3,3) << -1, -1, -1, 
			                       0, 0, 0, 
			                       1, 1, 1);
    }

    static Mat getLaplaceKernel(){ //fix
        return (Mat_<float>(3,3) << 0, -1, 0, 
			                     -1, 4, -1, 
			                      0, -1, 0);
    }

};

#endif // KERNEL_GENERATOR_HPP__
