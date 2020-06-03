rm bin/1712122_BT03
make
# Run Harris
# bin/1712122_BT03 detect_harris ../data/sunflower2.jpg 0.01 0.05
# bin/1712122_BT03 detect_harris ../data/capen.jpg 0.01 0.05
# bin/1712122_BT03 detect_harris ../data/kitchen.jpg 0.01 0.05

# Run blob
# bin/1712122_BT03 detect_blob ../data/sunflower.jpg 1.0 10
# bin/1712122_BT03 detect_blob ../data/sunflower2.jpg 1.0 10
# bin/1712122_BT03 detect_blob ../data/butterfly.jpg 1.0 10

# Run blob - DOG
# bin/1712122_BT03 detect_blob_dog ../data/sunflower.jpg 1.0 8
# bin/1712122_BT03 detect_blob_dog ../data/sunflower2.jpg 1.0 10
# bin/1712122_BT03 detect_blob_dog ../data/butterfly.jpg 1.0 10


# Run sift
# bin/1712122_BT03 detect_sift ../data/TestImages/01.jpg 1.6 4 5
# bin/1712122_BT03 detect_sift ../data/TestImages/02.jpg 1.6 4 5
# bin/1712122_BT03 detect_sift ../data/TestImages/04.jpg 1.6 4 5

# Run matching keypoints
# bin/1712122_BT03 matching_images ../data/training_images/01_2.jpg ../data/training_images/01_3.jpg
# bin/1712122_BT03 matching_images ../data/training_images/02_2.jpg ../data/training_images/02_3.jpg
# bin/1712122_BT03 matching_images ../data/training_images/04_2.jpg ../data/training_images/04_3.jpg

bin/1712122_BT03 matching_images ../data/TestImages/01.jpg ../data/training_images/01_1.jpg
bin/1712122_BT03 matching_images ../data/TestImages/02.jpg ../data/training_images/03_1.jpg
bin/1712122_BT03 matching_images ../data/TestImages/04.jpg ../data/training_images/04_1.jpg

