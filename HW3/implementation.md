Implementation details
### Haris Corner detection

### Scale Space Blob detection

### SIFT
**1. Constructing scale space:**
- resize image: `cv::resize(inImg, outImg, cv::Size(), 0.75, 0.75);`

- After each octave, the Gaussian image is down-sampled by a factor of 2

**2. Use DoG to approximate LoG:**

**3. Keypoints localization:**
- Input: 
- Output: 
- "key point": local maxima or minima points among their neighbors
- Thresholding keypoints: 
    + Low contrast: reject points with value of *D(x, y, sigma) lower than 0.03*
    + Along edges: 

**4. Orientation assignment:**
- 
- 
**5. SIFT construction:**
- 
- 

### Test