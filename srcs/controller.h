#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include <string>
#include <opencv2/opencv.hpp>

#include "work_queue.h"
#include "threading.h"
#include "feature_extractor.h"

/*
    This class is similar to the System class in ORB-SLAM, it is the main thread to control the 
    entire struction-from-motion procedure.
*/

class Controller:public Thread
{
public:
    Controller(const std::string img_folder);
    Controller(int n_readers, int n_extractors, int ms_pathq, int ms_imgq, int ms_featureq, 
            const std::string img_folder, const ReaderOptions ropts, const FeatureExtractionOptions feopts);
    ~Controller() = default;

protected:
    void Run() override;

private:
    int num_readers, num_extractors;
    std::vector<std::shared_ptr<ImageReader>> image_readers;
    std::vector<std::shared_ptr<FeatureExtractor>> feature_extractors;
    
    std::string image_folder;

    int max_size_path_queue, max_size_image_queue, max_size_feature_queue;
    std::shared_ptr<WorkQueue<std::string>> image_path_queue;
    std::shared_ptr<WorkQueue<cv::Mat>> image_queue;

    std::shared_ptr<WorkQueue<std::vector<cv::KeyPoint>>> kpts_queue;
    std::shared_ptr<WorkQueue<cv::Mat>> desc_queue;

    // Options
    const ReaderOptions reader_options;
    const FeatureExtractionOptions feature_extarction_options;
};
#endif