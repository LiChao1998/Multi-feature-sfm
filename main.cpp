#include <iostream>
#include <string>
#include "controller.h"

int main(int argc, char* argv[])
{
    if(argc != 2)
    {
        std::cerr << "Usage: ./using_ceres /path/to/images" << std::endl;
        return 0;
    }
    std::string image_folder(argv[1]);
    std::cout << "The image folder is: " << image_folder << std::endl;

    std::shared_ptr<Controller> controller = std::make_shared<Controller>(image_folder);
    controller->Start();
    controller->Wait();
    return 0;
}