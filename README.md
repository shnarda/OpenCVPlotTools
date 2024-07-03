
# OpenCVPlotTools

A C++ plotting library that generates basic plotting tools with the help of OpenCV library.



## Roadmap

This library is currently incomplete and exists on this platform for being a code sample.\
Here is the current state 0f the library:
- Histogram element ☑️
- Colormap element (work in progress)
- Plot element (work in progress)
- Subplots (work in progress)
- Table element (work in progress)

## Installation

Requirements of the library are as follows:\
-OpenCV\
-C++17 or later

While building the project, if the path of the OpenCV library hasn't been stored on the cmake cache, -DOpenCV_DIR=<PATH_TO_OPENCV_BUILD_DIRECTORY> should be added on the build command.
    
## Usage/Examples

The following example is based on the "Examples/Histogram_example.cpp" file.

```c++
    //Prepare the normal distributed histogram data and pass it on a Histogram object
    cv::Mat gaussian(200, 100, CV_32F);
    cv::randn(gaussian, 4, 20);
    Histogram hist{gaussian};

    //Set some of the available properties
    hist.setText(TextField::Title, "Histogram Example", 1, PainterConstants::blue);
    hist.setText(TextField::XAxis, "Gaussian Distribution X-Axis", 1, PainterConstants::red);
    hist.setCanvasSize(cv::Size{450, 450});

    //Generate the histogram canvas and retrieve it
    cv::Mat histogramCanvas = hist.generate();

    //Display the output canvas
    cv::imshow("Histogram Example", histogramCanvas);
    cv::waitKey(0);
}
```
The output canvas can be seen below:
(Examples/Histogram.png)




