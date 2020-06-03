# Implement local features detection algorithms
- Language: C++
- Library: OpenCV
- [Test data](https://drive.google.com/drive/folders/1Ircuey9VdRubQermWCBwKvmaow8DvmVd?usp=sharing)
- [Video demo](https://drive.google.com/file/d/1x1p62tP_QECXy-u9NVTk3fYMgf96yP6L/view?usp=sharing)

## Install
1. clone this repo and `cd` to right working directory
2. create folder `result` to save visualized results.
3. run `make` to recompile this project
4. the execute file is now in `bin` folder, see `sample_run.sh` to run it with right parameters.

## Project organization
| Algorithms      | Source code |
| ----------- | ----------- |
| Harris      | [header](include/corner_detector.hpp), [source](src/corner_detector.cpp)       |
| Blob-LOG   | [header](include/blob_detector.hpp), [source](src/blob_detector.cpp)        |
| Blob-DOG   | [header](include/corner_detector.hpp), [source](src/corner_detector.cpp)        |
| Sift  | [header](include/sift.hpp), [source](src/sift.cpp)        |
| Matching Sift-keypoints  | [header](include/keypoints_matcher.hpp), [source](src/keypoints_matcher.cpp)        |

