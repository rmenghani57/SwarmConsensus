#include <gnc_functions.hpp>
#include <bits/stdc++.h>

class Input
{
private:
    ros::NodeHandle nh;
public:
    int random_number;
    Input()
    {
        nh = ros::NodeHandle("~");
    }
    void generate_request()
    {
        random_number = rand() % 100 + 1;
        ROS_INFO("Generated Random Number: %d\n", random_number);
        if(random_number <= 33)
            send_coordinates_server(10, 20);
        else if(random_number <= 66)
            send_coordinates_server(20, 30);
        else
            send_coordinates_server(30, 40);
        nh.setParam("/request_from_building", 1);
    }
    void send_coordinates_server(int x1, int y1)
    {
        nh.setParam("/x_server", x1);
        nh.setParam("/y_server", y1);
    }
};