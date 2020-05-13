rm bin/hw2
make
bin/hw2 ../data/lena.jpg detect_sobel
bin/hw2 ../data/lena.jpg detect_prewitt
bin/hw2 ../data/lena.jpg detect_laplacian 3 1.0
bin/hw2 ../data/lena.jpg detect_canny 3 1.0