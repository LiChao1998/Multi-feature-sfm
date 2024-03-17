#include <iostream>

#include "feature_extractor.h"

FeatureExtractor::FeatureExtractor():opts(){}

FeatureExtractor::FeatureExtractor(const FeatureExtractionOptions& options, 
                    std::shared_ptr<WorkQueue<cv::Mat>> inp_image_queue,
                    std::shared_ptr<WorkQueue<std::vector<cv::KeyPoint>>> outp_kp_queue,
                    std::shared_ptr<WorkQueue<cv::Mat>> outp_desc_queue): opts(options)
{
    this->InpImageQueue = inp_image_queue;
    this->OutpKpQueue = outp_kp_queue;
    this->OutpDescQueue = outp_desc_queue;
}

void FeatureExtractor::Run()
{
    while(true)
    {
        cv::Mat img = InpImageQueue->Pop();
        if(img.empty()) break;
        bool extracted = Extract(img);
    }
}

bool FeatureExtractor::Extract(cv::Mat img)
{
    return false;
}