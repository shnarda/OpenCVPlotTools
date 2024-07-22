#include <gtest/gtest.h>
#include "colormap.h"


class GradientMat : public testing::Test
{
public:
    void SetUp() override{
        target = cv::Mat::zeros(200, 100, CV_32F);
        for(int c = 0; c < target.cols; c++){
            target.col(c).setTo(c);
        }

    };
    cv::Mat getMat()const {return target;};

private:
    cv::Mat target;
};

TEST(ColormapTest, ContructorEmptyMatrixTest)
{
    cv::Mat emptyMat;
    ASSERT_ANY_THROW(Colormap{emptyMat});
}

TEST(ColormapTest, ConstructorDefaultLimitsTest)
{
    cv::Mat target(200, 100, CV_8U);
    for(int c = 0; c < target.cols; c++){
        target.col(c).setTo(c);
    }
    ASSERT_NO_THROW(Colormap(target, cv::ColormapTypes::COLORMAP_AUTUMN));
}

TEST(ColormapTest, ConstructorDefaultLimitsGenerateTest)
{
    cv::Mat target(200, 100, CV_8U);
    for(int c = 0; c < target.cols; c++){
        target.col(c).setTo(c);
    }
    Colormap cmap(target, cv::ColormapTypes::COLORMAP_AUTUMN);
    ASSERT_NO_THROW(cmap.generate());
}

TEST_F(GradientMat, ConstructorDefaultArgsTest)
{
    ASSERT_NO_THROW(Colormap(getMat()));
}

TEST_F(GradientMat, ConstructorDefaultMinimumTest)
{
    ASSERT_NO_THROW(Colormap(getMat(), {}, 150));
}

TEST_F(GradientMat, ConstructorDefaultMaximumTest)
{
    ASSERT_NO_THROW(Colormap(getMat(), 50, {}));
}

TEST_F(GradientMat, ConstructorWrongBoundsTest)
{
    ASSERT_THROW(Colormap(getMat(), 20, 10), std::runtime_error);
}

TEST_F(GradientMat, ConstructorOutsideLimitsTest)
{
    ASSERT_THROW(Colormap(getMat(), -200, -100), std::runtime_error);
}

TEST_F(GradientMat, ConstructorDefaultArgsGenerateTest)
{
    Colormap cmap(getMat());
    ASSERT_NO_THROW(cmap.generate());
}

TEST_F(GradientMat, ConstructorDefaultMinimumGenerateTest)
{
    Colormap cmap(getMat(), {}, 150);
    ASSERT_NO_THROW(cmap.generate());
}

TEST_F(GradientMat, ConstructorDefaultMaximumGenerateTest)
{
    Colormap cmap(getMat(), 50, {});
    ASSERT_NO_THROW(cmap.generate());
}
