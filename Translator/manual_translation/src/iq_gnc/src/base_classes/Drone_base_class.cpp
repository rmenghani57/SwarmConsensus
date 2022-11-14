#include <bits/stdc++.h>
#include <gnc_functions.hpp>

class Drone
{
private:
	int x;
	int y;
	//added
	ros::NodeHandle nh;
	int i;

public:
	Drone(int i)
	{
		this->i = i;
	}
	bool is_leader(int id)
	{
		vector<int> drone_status;
		nh.getParam("/drone_status", drone_status);
		  return drone_status[id] == 1;
	}
	bool in_swarm(int id)
	{
		vector<int> drone_status;
		nh.getParam("/drone_status", drone_status);
		  return drone_status[id] != 0;
	}
	int find_drone_in_swarm(int capability)
	{
		vector<int> drone_capability;
		nh.getParam("/drone_capability", drone_capability);
		int N;
		nh.getParam("/N", N);
		vector<int> drone_status;
		nh.getParam("/drone_status", drone_status);
		  for (int i = 0; i < (N - 1); i++)
		  {
		    if ((drone_status[id] != 0) && (drone_capability[id] == capability))
		    {
		      return id;
		    }
		  }
		
		  return -1;
	}
	bool swarm_reached_goal()
	{
		vector<int> goal_location;
		nh.getParam("/goal_location", goal_location);
		int N;
		nh.getParam("/N", N);
		vector<int> drone_location_y;
		nh.getParam("/drone_location_y", drone_location_y);
		vector<int> drone_location_x;
		nh.getParam("/drone_location_x", drone_location_x);
		  bool reached = true;
		  for (int i = 0; i < (N - 1); i++)
		  {
		    if (in_swarm(id))
		    {
		      reached = (reached && (drone_location_x[id] == goal_location[0])) && (drone_location_y[id] == goal_location[1]);
		    }
		  }
		
		  return reached;
	}
	void update_leader_position(int id)
	{
		vector<int> leader_position;
		nh.getParam("/leader_position", leader_position);
		vector<int> drone_location_x;
		nh.getParam("/drone_location_x", drone_location_x);
		vector<int> drone_location_y;
		nh.getParam("/drone_location_y", drone_location_y);
		leader_position[0] = drone_location_x[id];
		leader_position[1] = drone_location_y[id];
		nh.setParam("/leader_position", leader_position);
	}
	bool reached_goal(int id)
	{
		vector<int> goal_location;
		nh.getParam("/goal_location", goal_location);
		vector<int> drone_location_x;
		nh.getParam("/drone_location_x", drone_location_x);
		vector<int> drone_location_y;
		nh.getParam("/drone_location_y", drone_location_y);
		  return (drone_location_x[id] == goal_location[0]) && (drone_location_y[id] == goal_location[1]);
	}
	int linear_distance()
	{
		vector<int> goal;
		nh.getParam("/goal", goal);
		  int x_coord = goal[0];
		  int y_coord = goal[1];
		  int xsquare = (x_coord - a) * (x_coord - a);
		  int ysquare = (y_coord - b) * (y_coord - b);
		  int sum1 = xsquare + ysquare;
		  sum1 = fint(sqrt(sum1));
		  return sum1;
	}
	void calculate_drone_times(int cap)
	{
		vector<int> drone_capability;
		nh.getParam("/drone_capability", drone_capability);
		vector<int> drone_time;
		nh.getParam("/drone_time", drone_time);
		int N;
		nh.getParam("/N", N);
		int dist;
		nh.getParam("/dist", dist);
		vector<int> leader_position;
		nh.getParam("/leader_position", leader_position);
		vector<int> drone_location_y;
		nh.getParam("/drone_location_y", drone_location_y);
		vector<int> drone_location_x;
		nh.getParam("/drone_location_x", drone_location_x);
		vector<int> time_to_swarm;
		nh.getParam("/time_to_swarm", time_to_swarm);
		vector<int> drone_candidates;
		nh.getParam("/drone_candidates", drone_candidates);
		  for (int i = 0; i < (N - 1); i++)
		  {
		    if ((drone_candidates[i] == 1) && (drone_capability[i] == cap))
		    {
		      dist = linear_distance(leader_position, drone_location_x[i], drone_location_y[i]);
		      if ((dist > 0) && (dist < 9))
		      {
		        drone_time[i] = time_to_swarm[dist - 1];
		      }
		    }
		  }
		
	}
	void move(int id)
	{
		vector<int> goal_location;
		nh.getParam("/goal_location", goal_location);
		vector<int> drone_location_y;
		nh.getParam("/drone_location_y", drone_location_y);
		vector<int> drone_battery;
		nh.getParam("/drone_battery", drone_battery);
		vector<int> drone_location_x;
		nh.getParam("/drone_location_x", drone_location_x);
		  if (drone_location_x[id] != goal_location[0])
		  {
		    drone_location_x[id] = drone_location_x[id] + 1;
		    drone_battery[id] -= 1;
		  }
		  else
		  {
		    if (drone_location_y[id] != goal_location[1])
		      drone_location_y[id] = drone_location_y[id] + 1;
		    drone_battery[id] -= 1;
		  }
		  if (is_leader(id))
		    update_leader_position(id);
		  x = drone_location_x[id];
		  y = drone_location_y[id];
	}
	void vote(int id)
	{
		vector<int> drone_capability;
		nh.getParam("/drone_capability", drone_capability);
		int vote_counter;
		nh.getParam("/vote_counter", vote_counter);
		int N;
		nh.getParam("/N", N);
		vector<int> votes;
		nh.getParam("/votes", votes);
		vector<int> drone_status;
		nh.getParam("/drone_status", drone_status);
		  for (int i = 0; i < (N - 1); i++)
		  {
		    if ((drone_status[i] == (-1)) && (drone_capability[i] == 1))
		    {
		      votes[id] = i;
			  nh.setParam("/votes", votes);
		    }
		  }
		
		  vote_counter++;
	}
	int find_min()
	{
		vector<int> drone_capability;
		nh.getParam("/drone_capability", drone_capability);
		int N;
		nh.getParam("/N", N);
		vector<int> drone_candidates;
		nh.getParam("/drone_candidates", drone_candidates);
		  int min = 100;
		  for (int i = 0; i < (N - 1); i++)
		  {
		    if ((drone_capability[i] == capability) && (drone_candidates[i] == 1))
		    {
		      if ((arr[i] != 0) && (arr[i] < min))
		      {
		        min = arr[i];
		      }
		    }
		  }
		
		  return min;
	}
	void vote_member(int id)
	{
		vector<int> drone_capability;
		nh.getParam("/drone_capability", drone_capability);
		int min_time;
		nh.getParam("/min_time", min_time);
		int N;
		nh.getParam("/N", N);
		vector<int> drone_time;
		nh.getParam("/drone_time", drone_time);
		vector<int> member_votes;
		nh.getParam("/member_votes", member_votes);
		vector<int> drone_battery;
		nh.getParam("/drone_battery", drone_battery);
		vector<int> drone_candidates;
		nh.getParam("/drone_candidates", drone_candidates);
		  for (int i = 0; i < (N - 1); i++)
		  {
		    if (drone_candidates[i] == 1)
		    {
		      int current_drone = find_drone_in_swarm(drone_capability[i]);
		      calculate_drone_times(drone_capability[i]);
		      min_time = find_min(drone_time, drone_capability[current_drone]);
		      if (drone_time[i] == min_time)
		      {
		        if ((drone_battery[current_drone] < 50) && (drone_battery[i] > drone_battery[current_drone]))
		        {
		          member_votes[id] = i;
				  nh.setParam("/member_votes", member_votes);
		        }
		      }
		    }
		  }
		
	}
};
