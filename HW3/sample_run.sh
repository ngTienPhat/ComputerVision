# uncomment this line if you want to re-compile the project
# make

# Run Harris algorithm on sunflower2.jpg with specific parameters
bin/1712122_BT03 detect_harris ../data/sunflower2.jpg 0.01 0.05

# Run Blob-LOG algorithm on sunflower2.jpg with specific parameters
bin/1712122_BT03 detect_blob ../data/sunflower2.jpg 1.0 10

# Run Blob-DOG algorithm on sunflower2.jpg with specific parameters
bin/1712122_BT03 detect_blob_dog ../data/sunflower2.jpg 1.0 8

# Run SIFT algorithm to detect keypoints on TestImages/01.jpg with specific parameters
bin/1712122_BT03 detect_sift ../data/TestImages/01.jpg 1.6 4 5

# Run SIFT keypoints to matching images (train_images/01_2.jpg and (train_images/01_3.jpg for example)
bin/1712122_BT03 matching_images ../data/training_images/02_2.jpg ../data/training_images/02_3.jpg

