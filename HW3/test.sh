rm bin/1712122_BT03
make
# bin/1712122_BT03 detect_harris ../data/TestImages/01.jpg 0.01 0.05
# bin/1712122_BT03 detect_blob ../data/sunflower.jpg 1.0 10
# bin/1712122_BT03 detect_blob_dog ../data/sunflower.jpg 1.0 8
# bin/1712122_BT03 detect_sift ../data/TestImages/01.jpg 1.6 4 5
bin/1712122_BT03 matching_images ../data/training_images/04_2.jpg ../data/training_images/04_3.jpg
