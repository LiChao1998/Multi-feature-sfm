#ifndef FEATURE_EXTRACTOR_H
#define FEATURE_EXTRACTOR_H

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <thread>
#include <vector>

#include "threading.h"
#include "work_queue.h"
#include "options.h"

class FeatureExtractor:public Thread
{
public:
    FeatureExtractor();
    FeatureExtractor(const FeatureExtractionOptions& options, 
                    std::shared_ptr<WorkQueue<cv::Mat>> inp_image_queue,
                    std::shared_ptr<WorkQueue<std::vector<cv::KeyPoint>>> outp_kp_queue,
                    std::shared_ptr<WorkQueue<cv::Mat>> outp_desc_queue);
    ~FeatureExtractor() = default;

protected:
    void Run() override;

private:
    virtual bool Extract(cv::Mat img);

    std::shared_ptr<WorkQueue<cv::Mat>> InpImageQueue;
    std::shared_ptr<WorkQueue<std::vector<cv::KeyPoint>>> OutpKpQueue;
    std::shared_ptr<WorkQueue<cv::Mat>> OutpDescQueue;

    const FeatureExtractionOptions opts;
};

class SiftExtractor:public FeatureExtractor
{
public:
    SiftExtractor();
    SiftExtractor(const FeatureExtractionOptions& options,
                std::shared_ptr<WorkQueue<cv::Mat>> inp_image_queue,
                std::shared_ptr<WorkQueue<std::vector<cv::KeyPoint>>> outp_kp_queue,
                std::shared_ptr<WorkQueue<cv::Mat>> outp_desc_queue);
    ~SiftExtractor();
protected:
    void Run() override;
private:
    cv::Ptr<cv::SIFT> detector;
};

class ORBExtractor:public FeatureExtractor
{
public:
    ORBExtractor();
    ORBExtractor(const FeatureExtractionOptions& options,
                std::shared_ptr<WorkQueue<cv::Mat>> inp_image_queue,
                std::shared_ptr<WorkQueue<std::vector<cv::KeyPoint>>> outp_kp_queue,
                std::shared_ptr<WorkQueue<cv::Mat>> outp_desc_queue);
    ~ORBExtractor();
protected:
    void Run() override;
private:
    cv::Ptr<cv::ORB> detector;
};
#endif