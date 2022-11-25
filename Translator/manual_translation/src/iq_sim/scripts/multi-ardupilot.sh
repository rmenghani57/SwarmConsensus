#!/bin/bash

gnome-terminal --tab -- bash -c "sim_vehicle.py -v ArduCopter -f gazebo-drone0 -I0 --out=tcpin:0.0.0.0:8000" 

gnome-terminal --tab -- bash -c "sim_vehicle.py -v ArduCopter -f gazebo-drone1 -I1 --out=tcpin:0.0.0.0:8100" 
gnome-terminal --tab -- bash -c "sim_vehicle.py -v ArduCopter -f gazebo-drone2 -I2 --out=tcpin:0.0.0.0:8200" 
gnome-terminal --tab -- bash -c "sim_vehicle.py -v ArduCopter -f gazebo-drone3 -I3 --out=tcpin:0.0.0.0:8300" 
gnome-terminal --tab -- bash -c "sim_vehicle.py -v ArduCopter -f gazebo-drone4 -I4 --out=tcpin:0.0.0.0:8400" 

gnome-terminal --tab -- bash -c "sim_vehicle.py -v ArduCopter -f gazebo-drone5 -I5 --out=tcpin:0.0.0.0:8500" 
gnome-terminal --tab -- bash -c "sim_vehicle.py -v ArduCopter -f gazebo-drone6 -I6 --out=tcpin:0.0.0.0:8600" 
gnome-terminal --tab -- bash -c "sim_vehicle.py -v ArduCopter -f gazebo-drone7 -I7 --out=tcpin:0.0.0.0:8700" 

#removed --console and tcpin 8100 ports



