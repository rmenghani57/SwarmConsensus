clc, clear, close all

positions = [100:100:1000];
times = [];

for dist = positions
    
  xf = [dist,dist];
  main;
  times = [times, tt]; 
  
end


fprintf('Copy and paste this into Uppaal: \n double time_to_swarm[100]={')
%printed_times = sprintf('%4.2f,', times);
printed_times = sprintf('%d,', int64(times));
printed_times(end) = [];
fprintf(printed_times)
fprintf('};')