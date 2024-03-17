#include "threading.h"

Thread::Thread(): started_(false), stopped_(false), paused_(false), pausing_(false),
                finished_(false)
{
}

void Thread::Start()
{
    std::unique_lock<std::mutex> lock(mutex_);
    Wait(); // Wait the thread to join
    thread_ = std::thread(&Thread::RunFunc, this);
    started_ = true;
    stopped_ = false;
    pausing_ = false;
    paused_ = false;
    finished_ = false;
}

void Thread::Stop()
{
    {
        std::unique_lock<std::mutex> lock(mutex_);
        stopped_ = true;
    }
    Resume();
}

void Thread::Pause()
{
    std::unique_lock<std::mutex> lock(mutex_);
    paused_ = true;
}

void Thread::Resume()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if(paused_)
    {
        paused_ = false;
        pause_condition_.notify_all();
    }
}

void Thread::Wait()
{
    if(thread_.joinable())
    {
        thread_.join();
    }
}

bool Thread::IsStarted()
{
    std::unique_lock<std::mutex> lock(mutex_);
    return started_;
}

bool Thread::IsStopped()
{
    std::unique_lock<std::mutex> lock(mutex_);
    return stopped_;
}

bool Thread::IsFinished()
{
    std::unique_lock<std::mutex> lock(mutex_);
    return finished_;
}

bool Thread::IsPaused()
{
    std::unique_lock<std::mutex> lock(mutex_);
    return paused_;
}

bool Thread::IsRunning()
{
    std::unique_lock<std::mutex> lock(mutex_);
    return started_ && !pausing_ && !finished_;
}

void Thread::RunFunc()
{
    Run();
    {
        std::unique_lock<std::mutex> lock(mutex_);
        finished_ = true;
    }
}

std::thread::id Thread::GetThreadId() const
{
    return std::this_thread::get_id();
}

// Block the run function so that it cannot get the lock
void Thread::BlockIfPaused()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if(paused_)
    {
        pausing_ = true;
        pause_condition_.wait(lock);
        pausing_ = false;
    }
}

ImageReader::ImageReader():opts(){}

ImageReader::ImageReader(const ReaderOptions& options, 
                        std::shared_ptr<WorkQueue<std::string>> inp_path_queue,
                        std::shared_ptr<WorkQueue<cv::Mat>> outp_mat_queue): opts(options)
{
    this->InpPathQueue = inp_path_queue;
    this->OutpMatQueue = outp_mat_queue;
}

void ImageReader::Run()
{
    while(true)
    {
        std::string path = InpPathQueue->Pop();
        if(path == "") break;
        boost::filesystem::path fpath(path);
        if(!exists(fpath))
            continue;
        cv::Mat img = cv::imread(path);
        int h = img.rows, w = img.cols;
        if(opts.resize_x == -1)
        {
            bool outp = OutpMatQueue->Push(std::move(img));
        }
        else if(opts.resize_y == -1)
        {
            cv::Mat resize_img;
            int resize_x = opts.resize_x;
            double ratio = (double)h / (double)w;
            int resize_y = (int)(ratio * (double)resize_x);
            cv::resize(img, resize_img, cv::Size(resize_x, resize_y), 0, 0, cv::INTER_LINEAR);
            bool outp = OutpMatQueue->Push(std::move(resize_img));
        }
        else
        {
            cv::Mat resize_img;
            int resize_x = opts.resize_x;
            int resize_y = opts.resize_y;
            cv::resize(img, resize_img, cv::Size(resize_x, resize_y), 0, 0, cv::INTER_LINEAR);
            bool outp = OutpMatQueue->Push(std::move(resize_img));
        }
    }
}