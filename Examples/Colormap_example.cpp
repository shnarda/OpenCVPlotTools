#include "opencv2/highgui.hpp"
#include "colormap.h"

int main()
{
    //Prepare the normal distributed histogram data and pass it on a Histogram object
    cv::Mat target(100, 150, CV_8U);
    for(int c = 0; c < target.cols; c++){
        target.col(c).setTo(c);
    }

    Colormap cmap(target, cv::COLORMAP_JET);
    cmap.setText(TextField::Title, "Colormap Example", 1.3, PainterConstants::red);
    cmap.generate();

    //Display the output canvas
    cv::imshow("Colormap Example", cmap.canvas());
    cv::waitKey(0);
}
