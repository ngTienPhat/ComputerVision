rm bin/hw2
make
bin/hw2 _ --help
bin/hw2 ../data/lena.jpg detect_sobel 3 1.0
bin/hw2 ../data/lena.jpg detect_prewitt 3 1.0
bin/hw2 ../data/lena.jpg detect_laplacian 3 1.0
bin/hw2 ../data/lena.jpg detect_canny 3 1.0