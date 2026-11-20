#include <stdio.h>
#include <stdlib.h>
#include "rclcpp/rclcpp.hpp"
#include "../include/codbot_base/codbot_base.h"

int main(int argc,char **argv)
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<CCodbotBase>());
    rclcpp::shutdown();
    return 0;
}
