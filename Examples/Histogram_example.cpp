//#include <iostream>
//#include "plotelementbase.h"
#include "opencv2/highgui.hpp"
#include "histogram.h"
#include <paintertools.h>

int main()
{
    //Prepare the histogram data
    cv::Mat dummyData(200, 100, CV_32F);
    cv::randn(dummyData, 4, 20);

    Histogram test{dummyData};

    test.setText(TextField::Title, "Histogram Example", 1, PainterConstants::blue);
    test.setText(TextField::XAxis, "Gaussian Distribution X-Axis", 1, PainterConstants::red);
    //test.setRanges(AxisRange{0, 60}, {});
    cv::Mat out = test.generate();

    cv::imshow("test", out);
    cv::waitKey(0);
}
