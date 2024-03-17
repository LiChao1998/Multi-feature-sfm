#include "controller.h"

#include <iostream>
#include <boost/filesystem.hpp>
#include <string>

// s1 must contains s2
bool end_same(const std::string& s1, const std::string& s2)
{
    int len_s1 = s1.length();
    int len_s2 = s2.length();
    if(len_s1 < len_s2) return false;
    std::string s1_end = s1.substr(len_s1 - len_s2, len_s2);
    if(s1_end == s2) return true;
    else return false;
}

Controller::Controller(const std::string img_folder): num_readers(10), num_extractors(10), 
                        max_size_path_queue(100), max_size_image_queue(10), 
                        max_size_feature_queue(-1), image_folder(img_folder), feature_extarction_options(), 
                        reader_options(-1, -1)
{
    
    image_path_queue = std::make_shared<WorkQueue<std::string>>(max_size_path_queue);
    image_queue = std::make_shared<WorkQueue<cv::Mat>>(max_size_image_queue);

    kpts_queue = std::make_shared<WorkQueue<std::vector<cv::KeyPoint>>>(max_size_feature_queue);
    desc_queue = std::make_shared<WorkQueue<cv::Mat>>(max_size_feature_queue);

    for(int i = 0; i < num_readers; i++)
    {
        image_readers.push_back(std::make_shared<ImageReader>(reader_options, 
                                image_path_queue, image_queue));
    }

    for(int j = 0; j < num_extractors; j++)
    {
        feature_extractors.push_back(std::make_shared<FeatureExtractor>(feature_extarction_options, image_queue, 
                                                                        kpts_queue, desc_queue));
    }
}


Controller::Controller(int n_readers, int n_extractors, int ms_pathq, int ms_imgq, int ms_featureq, 
            const std::string img_folder, const ReaderOptions ropts, const FeatureExtractionOptions feopts):
            num_readers(n_readers), num_extractors(n_extractors), max_size_path_queue(ms_pathq),
            max_size_image_queue(ms_imgq), max_size_feature_queue(ms_featureq), image_folder(img_folder),
            reader_options(ropts), feature_extarction_options(feopts)
{
    image_path_queue = std::make_shared<WorkQueue<std::string>>(max_size_path_queue);
    image_queue = std::make_shared<WorkQueue<cv::Mat>>(max_size_image_queue);

    kpts_queue = std::make_shared<WorkQueue<std::vector<cv::KeyPoint>>>(max_size_feature_queue);
    desc_queue = std::make_shared<WorkQueue<cv::Mat>>(max_size_feature_queue);

    for(int i = 0; i < num_readers; i++)
    {
        image_readers.push_back(std::make_shared<ImageReader>(reader_options, 
                                image_path_queue, image_queue));
    }

    for(int j = 0; j < num_extractors; j++)
    {
        feature_extractors.push_back(std::make_shared<FeatureExtractor>(feature_extarction_options, image_queue, 
                                                       kpts_queue, desc_queue));
    }
}

void Controller::Run()
{
    // start the thread
    for(int i = 0; i < num_readers; i++)
    {
        image_readers[i]->Start();
    }
    for(int j = 0; j < num_extractors; j++)
    {
        feature_extractors[j]->Start();
    }
    
    boost::filesystem::path img_folder_path(image_folder);
    if(boost::filesystem::is_directory(img_folder_path))
    {
        boost::filesystem::directory_iterator dir_it(img_folder_path);
        for(const auto& iter:dir_it)
        {
            if(boost::filesystem::is_directory(iter.path()))
                continue;
            std::string p = iter.path().string();
            if(!end_same(p, ".jpg") && !end_same(p, ".png"))
            {
                continue;
            }
                
            bool push_suc = image_path_queue->Push(std::move(p));
        }

        image_path_queue->WaitTillEmpty();
        image_path_queue->Stop();

        for(auto img_reader: image_readers)
        {
            img_reader->Wait();
        }

        image_queue->WaitTillEmpty();
        image_queue->Stop();

        for(auto feature_extractor: feature_extractors)
        {
            feature_extractor->Wait();
        }
    }

}