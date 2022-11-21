#!/bin/bash

gnome-terminal --tab -- bash -c "sim_vehicle.py -v ArduCopter -f gazebo-drone0 --console -I0 --out=tcpin:0.0.0.0:8000 " 

gnome-terminal --tab -- bash -c "sim_vehicle.py -v ArduCopter -f gazebo-drone1 --console -I1 --out=tcpin:0.0.0.0:8100 " 
gnome-terminal --tab -- bash -c "sim_vehicle.py -v ArduCopter -f gazebo-drone2 --console -I2 --out=tcpin:0.0.0.0:8200 " 
gnome-terminal --tab -- bash -c "sim_vehicle.py -v ArduCopter -f gazebo-drone3 --console -I3 --out=tcpin:0.0.0.0:8300 " 
gnome-terminal --tab -- bash -c "sim_vehicle.py -v ArduCopter -f gazebo-drone4 --console -I4 --out=tcpin:0.0.0.0:8400 " 

gnome-terminal --tab -- bash -c "sim_vehicle.py -v ArduCopter -f gazebo-drone5 --console -I5 --out=tcpin:0.0.0.0:8500 " 
gnome-terminal --tab -- bash -c "sim_vehicle.py -v ArduCopter -f gazebo-drone6 --console -I6 --out=tcpin:0.0.0.0:8600 " 
gnome-terminal --tab -- bash -c "sim_vehicle.py -v ArduCopter -f gazebo-drone7 --console -I7 --out=tcpin:0.0.0.0:8700 " 



