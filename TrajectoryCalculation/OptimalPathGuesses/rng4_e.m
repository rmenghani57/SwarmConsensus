function bezpoints = rng4_e()

%control points while minimizing energy, rng(4) 49/4/3
% max=15, min=10
%removed last two control points
%101.152681323070,96.4574148869066;
%    99.8699810977579,99.7695385394696]
%{
bezpoints = [3,3;
    11.5708662012322,6.24739841344291;
    20.1417324024643,9.49479682688583;
    22.9572079328632,12.9871895419627;
    25.7726834632621,16.4795822570395;
    31.6008135905590,22.5345950431752;
    37.4289437178560,28.5896078293109;
    36.4221398538589,34.1102610848650;
    35.4153359898619,39.6309143404190;
    40.6172960396261,46.3028778427637;
    45.8192560893904,52.9748413451084;
    48.1373340717298,57.4277608562016;
    50.4554120540692,61.8806803672948;
    55.8964521740350,67.9869659205415;
    61.3374922940007,74.0932514737881;
    66.7579087693324,75.3119426094297;
    72.1783252446641,76.5306337450713;
    79.1154322182910,80.1123748402586;
    84.0457812548016,82.9782965447438;
    86.9241846729215,81.6864691746267;
    89.9220186262348,80.3956519571331;
    95.5437399975665,88.4461750794275];
%}

%or don't remove last two control points
bezpoints = [3,3;
    11.5708662012322,6.24739841344291;
    20.1417324024643,9.49479682688583;
    22.9572079328632,12.9871895419627;
    25.7726834632621,16.4795822570395;
    31.6008135905590,22.5345950431752;
    37.4289437178560,28.5896078293109;
    36.4221398538589,34.1102610848650;
    35.4153359898619,39.6309143404190;
    40.6172960396261,46.3028778427637;
    45.8192560893904,52.9748413451084;
    48.1373340717298,57.4277608562016;
    50.4554120540692,61.8806803672948;
    55.8964521740350,67.9869659205415;
    61.3374922940007,74.0932514737881;
    66.7579087693324,75.3119426094297;
    72.1783252446641,76.5306337450713;
    79.1154322182910,80.1123748402586;
    84.0457812548016,82.9782965447438;
    86.9241846729215,81.6864691746267;
    89.9220186262348,80.3956519571331;
    95.5437399975665,88.4461750794275;
    101.152681323070,96.4574148869066;
    99.8699810977579,99.7695385394696];
end