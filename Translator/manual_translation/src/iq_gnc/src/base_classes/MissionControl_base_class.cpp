#include <bits/stdc++.h>
#include <gnc_functions.hpp>
using namespace std;

class MissionControl
{
private:

	ros::NodeHandle nh;

public:
	MissionControl()
	{
		
	}
	void reset_arrays()
	{
		vector<int> drone_time;
		nh.getParam("/drone_time", drone_time);
		int N;
		nh.getParam("/N", N);
		vector<int> drone_candidates;
		nh.getParam("/drone_candidates", drone_candidates);
		  for (int i = 0; i <= (N - 1); i++)
		  {
		    drone_candidates[i] = 0;
			nh.setParam("/drone_candidates", drone_candidates);
		    drone_time[i] = 0;
			nh.setParam("/drone_time", drone_time);
		  }

	}
	bool check_leader_pos()
	{
		vector<int> leader_position;
		nh.getParam("/leader_position", leader_position);
		vector<int> goal;
		nh.getParam("/goal", goal);
		  if ((leader_position[0] == goal[0]) && (leader_position[1] == goal[1]))
		    return true;
		  else
		    return false;
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
		  for (int id = 0; id <= (N - 1); id++)
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
		  for (int id = 0; id <= (N - 1); id++)
		  {
		    if (in_swarm(id))
		    {
		      reached = (reached && (drone_location_x[id] == goal_location[0])) && (drone_location_y[id] == goal_location[1]);
		    }
		  }
		
		  return reached;
	}
	int linear_distance(vector<int> goal, int a, int b)
	{
		//vector<int> goal;
		//nh.getParam("/goal", goal); does not like same name for argument and variable
		int x_coord = goal[0];
		int y_coord = goal[1];
		int xsquare = (x_coord - a) * (x_coord - a);
		int ysquare = (y_coord - b) * (y_coord - b);
		int sum1 = xsquare + ysquare;
		sum1 = int(sqrt(sum1));          // casting to integer
		return sum1;
	}
	bool within_reach(int x,int y)
	{
		vector<int> leader_position;
		nh.getParam("/leader_position", leader_position);
		int comm_reach;
		nh.getParam("/comm_reach", comm_reach);
		  if (linear_distance(leader_position, x, y) <= comm_reach)
		  {
		    return true;
		  }
		  else
		  {
		    return false;
		  }
	}
	bool is_capabilities_needed(int capability)
	{
		vector<int> mission_capabilities;
		nh.getParam("/mission_capabilities", mission_capabilities);
		  for (int i = 0; i <= 2; i++)
		  {
		    if (mission_capabilities[i] == capability)
		      return true;
		  }
		
		  return false;
	}
	int possible_member()
	{
		vector<int> drone_capability;
		nh.getParam("/drone_capability", drone_capability);
		int N;
		nh.getParam("/N", N);
		vector<int> drone_status;
		nh.getParam("/drone_status", drone_status);
		vector<int> drone_location_y;
		nh.getParam("/drone_location_y", drone_location_y);
		vector<int> drone_battery;
		nh.getParam("/drone_battery", drone_battery);
		vector<int> drone_location_x;
		nh.getParam("/drone_location_x", drone_location_x);
		int candidate_counter;
		nh.getParam("/candidate_counter", candidate_counter);
		vector<int> drone_candidates;
		nh.getParam("/drone_candidates", drone_candidates);
		  for (int i = 0; i <= (N - 1); i++)
		  {
		    if ((((drone_status[i] == 0) && (drone_battery[i] > 49)) && within_reach(drone_location_x[i], drone_location_y[i])) && is_capabilities_needed(drone_capability[i]))
		    {
		      drone_candidates[i] = 1;    // these need to get set on the parameter server
		      candidate_counter++;
			  nh.setParam("/drone_candidates", drone_candidates);
			  nh.setParam("/candidate_counter", candidate_counter);
		    }
		  }
		
		return candidate_counter;
	}
	bool voting_results(vector<int> check_votes)           // modified argument of function
	{
		int Needed;
		nh.getParam("/Needed", Needed);
		//vector<int> check_votes;
		//nh.getParam("/check_votes", check_votes);
		  for (int i = 0; i <= (Needed - 2); i++)
		  {
		    if (check_votes[i] != check_votes[i + 1])
		      return true;
		  }
		
		  return false;
	}
	void elect_leader()
	{
		vector<int> votes;
		nh.getParam("/votes", votes);
		vector<int> drone_status;
		nh.getParam("/drone_status", drone_status);
		int voting_in_prog;
		nh.getParam("/voting_in_prog", voting_in_prog);
		vector<int> mission_capabilities;
		nh.getParam("/mission_capabilities", mission_capabilities);
		int Needed;
		nh.getParam("/Needed", Needed);
		int updating_mission;
		nh.getParam("/updating_mission", updating_mission);
		vector<int> check_votes;
		nh.getParam("/check_votes", check_votes);
		int id_voter;
		nh.getParam("/id_voter", id_voter);
		  for (int i = 0; i <= (Needed - 1); i++)
		  {
		    int cap = mission_capabilities[i];
		    id_voter = find_drone_in_swarm(cap);
		    check_votes[i] = votes[id_voter];
			nh.setParam("/check_votes", check_votes);
		  }
				
		  if (voting_results(check_votes) == false)
		  {
		    drone_status[votes[id_voter]] = 1;
			nh.setParam("/drone_status", drone_status);
		  }
		  voting_in_prog = false;
		  nh.setParam("/voting_in_prog", voting_in_prog);
		  updating_mission = true;
		  nh.setParam("/updating_mission", updating_mission);
	}
	void elect_members()
	{
		int voting_in_prog;
		nh.getParam("/voting_in_prog", voting_in_prog);
		int Needed;
		nh.getParam("/Needed", Needed);
		int candidate_counter;
		nh.getParam("/candidate_counter", candidate_counter);
		int id_voter;
		nh.getParam("/id_voter", id_voter);
		vector<int> member_votes;
		nh.getParam("/member_votes", member_votes);
		vector<int> drone_status;
		nh.getParam("/drone_status", drone_status);
		int vote_counter;
		nh.getParam("/vote_counter", vote_counter);
		vector<int> check_member_votes;
		nh.getParam("/check_member_votes", check_member_votes);
		vector<int> drone_capability;
		nh.getParam("/drone_capability", drone_capability);
		vector<int> mission_capabilities;
		nh.getParam("/mission_capabilities", mission_capabilities);
		int updating_mission;
		nh.getParam("/updating_mission", updating_mission);
		  if (vote_counter == Needed)
		  {
		    for (int i = 0; i <= (Needed - 1); i++)
		    {
		      int cap = mission_capabilities[i];
		      id_voter = find_drone_in_swarm(cap);
		      check_member_votes[i] = member_votes[id_voter];
			  nh.setParam("/check_member_votes", check_member_votes);
		    }		

		    if (voting_results(check_member_votes) == false)
		    {
		      int curr_id = find_drone_in_swarm(drone_capability[member_votes[id_voter]]);
		      drone_status[curr_id] = 0;
		      drone_status[member_votes[id_voter]] = -1;
			  nh.setParam("/drone_status", drone_status);
			}			
		    voting_in_prog = false;
			nh.setParam("/voting_in_prog", voting_in_prog);
		  }
		  updating_mission = true;
		  nh.setParam("/updating_mission", updating_mission);
		  candidate_counter = 0;
		  nh.setParam("/candidate_counter", candidate_counter);
		  vote_counter = 0;
		  nh.setParam("/vote_counter", vote_counter);
		  reset_arrays();
	}
};
