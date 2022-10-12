#include <gnc_functions.hpp>
#include <bits/stdc++.h>
using namespace std;

class Drone
{
private:
    int i;
    int xsquare;
    int ysquare;
    int sum1;

    int min = INT_MAX;
    int min_index = INT_MAX;
    int time_sensor_value = 20;
    vector<bool> availability = vector<bool>(3, true);
    int key;
    ros::NodeHandle nh;
public:
    Drone(int i) 
    {
        this->i = i;
    }

    bool checksensor() 
    {
        vector<double> altitude, fuel, temp, battery;
        vector<bool> technical_sensor;
        nh.getParam("/altitude", altitude);
        nh.getParam("/fuel", fuel);
        nh.getParam("/temp", temp);
        nh.getParam("/technical_sensor", technical_sensor);
        if(altitude[i] >= 100 && fuel[i] > 50 && temp[i] < 100 && technical_sensor[i] == true)
            return true;
        else
            return false;
    }
    bool check_availability() 
    {
        vector<bool> availability;
        nh.getParam("/availability", availability);
        return availability[i];
    }

    int battery() 
    {
        vector<double> bat;
        nh.getParam("/bat", bat);
        return bat[i];
    }

    void decrypt()
    {
        int global_key;
        nh.getParam("/global_key", global_key);
        key = global_key + 23;
        key = key * 3;
        key = key * 5;
        ROS_INFO("DECRYPTED, local key value: %d\n", key);
    }

    int check_authenticity() 
    {
        if(key == 12345)
            return 1;
        else
            return 0;
    }

    int linear_distance(int a, int b) 
    {
        int x_coord, y_coord;
        nh.getParam("/x_coord", x_coord);
        nh.getParam("/y_coord", y_coord);
        xsquare = (x_coord - a) * (x_coord - a);
        ysquare = (y_coord - a) * (y_coord - b);
        sum1 = xsquare + ysquare;
        sum1 = int(sqrt(sum1));
        return sum1;
    }

    void update_distance(int distance) 
    {
        vector<double> update_dist;
        nh.getParam("/update_dist", update_dist);
        update_dist[i] = distance;
        nh.setParam("/update_dist", update_dist);
    }

    // update_shortest_distance function wherein each drone calculates the drone which is nearest to the requested node and updates in a global list
    void update_shortest_distance()  // x stores the drone which is updating the list
    {
        int N;
        vector<double> update_dist;
        nh.getParam("/N", N);
        nh.getParam("/update_dist", update_dist);
        for(int j = 0; j < N; j++) {
            if(update_dist[j] < min) {
                min = update_dist[j];
                min_index = j;
            }
        }

        // ROS_INFO("Drone %d min_dist %d and min_index %d!\n", i, min, min_index);
        
        vector<double> shortest_dist;
        vector<int> selected_drone;
        vector<int> chosen;
        nh.getParam("/shortest_dist", shortest_dist);
        nh.getParam("/selected_drone", selected_drone);
        nh.getParam("/chosen", chosen);
        shortest_dist[i] = min;
        selected_drone[i] = min_index;
        chosen[i] = min_index;

        nh.setParam("/shortest_dist", shortest_dist);
        nh.setParam("/selected_drone", selected_drone);
        nh.setParam("/chosen", chosen);
        min = INT_MAX;
        min_index = INT_MAX;
    }
};

//     int time_to_reach_reqloc(int i) 
//     {
//         return time_sensor_value;
//     }
// };