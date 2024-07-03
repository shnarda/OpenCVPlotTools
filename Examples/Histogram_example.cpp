#include "opencv2/highgui.hpp"
#include "histogram.h"

int main()
{
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
