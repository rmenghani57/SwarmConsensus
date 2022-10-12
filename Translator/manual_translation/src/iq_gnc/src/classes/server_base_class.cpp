#include <gnc_functions.hpp>
#include <bits/stdc++.h>

class Server 
{
private:
    int time_value;
    int local_key;
    ros::NodeHandle nh;
public:
    Server() {
        nh = ros::NodeHandle("~");
        this->time_value = 20;
    }

    // sendcoordinates function stores the x and y coordinates of the requesting node globally
    void sendcoordinates(int x, int y)
    {
        nh.setParam("/x_coord", x);
        nh.setParam("/y_coord", y);
    }

    // time_to_serve_request function returns the total time taken by the drone to serve the request
    // (It does not include the time taken to make the decision by the drones)
    int time_to_serve_request()
    {
        // Gets and returns the time sensor value from the drone and also from the server end
        return time_value;
    }

    void encrypted_key(int i)
    {
        local_key = i / 5;
        local_key = local_key / 3;
        nh.setParam("/global_key", local_key - 23);
    }
};