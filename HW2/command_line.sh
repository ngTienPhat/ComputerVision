# run examples command

rm bin/1712122_1712822_BT02
make
bin/1712122_1712822_BT02 ../data/1.jpg detect_sobel 3 1.0 100
bin/1712122_1712822_BT02 ../data/1.jpg detect_prewitt 3 1.0 100
bin/1712122_1712822_BT02 ../data/1.jpg detect_laplacian 3 1.0 0.2
bin/1712122_1712822_BT02 ../data/1.jpg detect_canny 5 1.0 20 60

bin/1712122_1712822_BT02 ../data/3.jpg detect_sobel 3 1.0 100
bin/1712122_1712822_BT02 ../data/3.jpg detect_prewitt 3 1.0 100
bin/1712122_1712822_BT02 ../data/3.jpg detect_laplacian 3 1.0 0.2
bin/1712122_1712822_BT02 ../data/3.jpg detect_canny 5 1.0 20 60
