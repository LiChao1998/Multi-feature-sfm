#ifndef THREADING_H
#define THREADING_H

#include <atomic>
#include <climits>
#include <functional>
#include <future>
#include <list>
#include <queue>
#include <unordered_map>
#include <thread>
#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

#include <boost/filesystem.hpp>

#include "work_queue.h"
#include "options.h"

class Thread
{
public:
    Thread();
    virtual ~Thread() = default;


    virtual void Start();
    virtual void Stop();
    virtual void Pause();
    virtual void Resume();
    virtual void Wait();

    bool IsStarted();
    bool IsStopped();
    bool IsPaused();
    bool IsRunning();
    bool IsFinished();

    void BlockIfPaused();

protected:
    virtual void Run() = 0;

    std::thread::id GetThreadId() const;

private:
    void RunFunc();

    std::thread thread_;
    std::mutex mutex_;
    std::condition_variable pause_condition_;

    bool started_;
    bool stopped_;
    bool paused_;
    bool pausing_;
    bool finished_;
};

class ImageReader:public Thread
{
public:
    ImageReader();
    ImageReader(const ReaderOptions& options, 
                std::shared_ptr<WorkQueue<std::string>> inp_path_queue,
                std::shared_ptr<WorkQueue<cv::Mat>> outp_mat_queue);
    ~ImageReader() = default;

protected:
    void Run() override;

private:
    std::shared_ptr<WorkQueue<std::string>> InpPathQueue;
    std::shared_ptr<WorkQueue<cv::Mat>> OutpMatQueue;

    const ReaderOptions opts;
};
#endif