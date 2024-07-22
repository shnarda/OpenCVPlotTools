#include <gtest/gtest.h>
#include "histogram.h"


class GaussianMat : public testing::Test
{
public:
    void SetUp() override{
        data = cv::Mat::zeros(200, 100, CV_32F);
        cv::randn(data, 0, 20);
    };
    cv::Mat getMat()const {return data;};

private:
    cv::Mat data;
};

TEST(HistogramTest, BinsConstructorUnequalArgsTest)
{
    std::vector<size_t> testHist{1, 1, 5, 10};
    std::vector<double> bins{-5, 0, 5};
    ASSERT_ANY_THROW(Histogram(testHist, bins));
}

TEST(HistogramTest, BinsConstructorEqualArgsTest)
{
    std::vector<size_t> testHist{1, 1, 5, 10};
    std::vector<double> bins{-5, 0, 5, 10};
    ASSERT_NO_THROW(Histogram(testHist, bins));
}

TEST(HistogramTest, MatrixContructorEmptyMatrixTest)
{
    cv::Mat emptyMat;
    ASSERT_ANY_THROW(Histogram{emptyMat});
}

TEST_F(GaussianMat, MatrixConstructorDefaultArgsTest)
{
    ASSERT_NO_THROW(Histogram(getMat()));
}

TEST_F(GaussianMat, MatrixConstructorZeroBinsTest)
{
    ASSERT_THROW(Histogram(getMat(), 0), std::invalid_argument);
}

TEST_F(GaussianMat, MatrixConstructorSingleBinTest)
{
    ASSERT_NO_THROW(Histogram(getMat(), 1));
}

TEST_F(GaussianMat, MatrixConstructorBinsTest)
{
    constexpr size_t NUMBER_OF_BINS = 100;
    Histogram histogramBins(getMat(), NUMBER_OF_BINS);
    ASSERT_EQ(NUMBER_OF_BINS, histogramBins.getBins().size());
}

TEST_F(GaussianMat, MatrixConstructorBinsHistogramTest)
{
    constexpr size_t NUMBER_OF_BINS = 100;
    Histogram histogramBins(getMat(), NUMBER_OF_BINS);
    ASSERT_EQ(NUMBER_OF_BINS, histogramBins.getHistogram().size());
}

TEST_F(GaussianMat, MatrixConstructorBinStartTest)
{
    constexpr double BIN_START = -100.5;
    Histogram histogramBins(getMat(), {}, BIN_START, {});

    const auto bins = histogramBins.getBins();
    const double minimumBin = *std::min_element(bins.begin(), bins.end());
    EXPECT_FLOAT_EQ(BIN_START, minimumBin);
}

TEST_F(GaussianMat, MatrixConstructorBinEndTest)
{
    constexpr double BIN_END = 11.5;
    Histogram histogramBins(getMat(), {}, {}, BIN_END);

    const auto bins = histogramBins.getBins();
    const double maximumBin = *std::max_element(bins.begin(), bins.end());
    EXPECT_NEAR(BIN_END, maximumBin, 0.001);
}

TEST_F(GaussianMat, InvalidBoundsTest)
{
    constexpr double BIN_START = -100.5;
    constexpr double BIN_END = BIN_START - 1;

    ASSERT_ANY_THROW(Histogram(getMat(), {}, BIN_START, BIN_END));
}






