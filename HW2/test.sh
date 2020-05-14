#this bash file is used to test canny algorithm on variant arguments, thresholds

rm bin/1712122_1712822_BT02
make
bin/1712122_1712822_BT02 ../data/1.jpg detect_canny 3 1.0 10 30 1
bin/1712122_1712822_BT02 ../data/2.jpg detect_canny 3 1.0 10 30 1
bin/1712122_1712822_BT02 ../data/3.jpg detect_canny 3 1.0 10 30 1
bin/1712122_1712822_BT02 ../data/4.jpg detect_canny 3 1.0 10 30 1
bin/1712122_1712822_BT02 ../data/5.jpg detect_canny 3 1.0 10 30 1

bin/1712122_1712822_BT02 ../data/1.jpg detect_canny 3 1.0 20 60 0
bin/1712122_1712822_BT02 ../data/2.jpg detect_canny 3 1.0 20 60 0
bin/1712122_1712822_BT02 ../data/3.jpg detect_canny 3 1.0 20 60 0
bin/1712122_1712822_BT02 ../data/4.jpg detect_canny 3 1.0 20 60 0
bin/1712122_1712822_BT02 ../data/5.jpg detect_canny 3 1.0 20 60 0

bin/1712122_1712822_BT02 ../data/1.jpg detect_canny 5 1.0 10 30 1
bin/1712122_1712822_BT02 ../data/2.jpg detect_canny 5 1.0 10 30 1
bin/1712122_1712822_BT02 ../data/3.jpg detect_canny 5 1.0 10 30 1
bin/1712122_1712822_BT02 ../data/4.jpg detect_canny 5 1.0 10 30 1
bin/1712122_1712822_BT02 ../data/5.jpg detect_canny 5 1.0 10 30 1

bin/1712122_1712822_BT02 ../data/1.jpg detect_canny 5 1.0 20 60 0
bin/1712122_1712822_BT02 ../data/2.jpg detect_canny 5 1.0 20 60 0
bin/1712122_1712822_BT02 ../data/3.jpg detect_canny 5 1.0 20 60 0
bin/1712122_1712822_BT02 ../data/4.jpg detect_canny 5 1.0 20 60 0
bin/1712122_1712822_BT02 ../data/5.jpg detect_canny 5 1.0 20 60 0
