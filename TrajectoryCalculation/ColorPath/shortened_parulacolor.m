function [blue, green, red] = shortened_parulacolor(color)

%shortened parula
b_color = [0.529200000000000,0.577676190476191,0.626971428571429,0.677085714285714,0.727900000000000,0.779247619047619,0.830271428571429,0.868333333333333,0.881957142857143,0.882842857142857,0.878633333333333,0.871957142857143,0.864057142857143,0.855438095238095,0.846700000000000,0.838371428571429,0.831180952380952,0.826271428571429,0.823957142857143,0.822828571428572,0.819852380952381,0.813500000000000,0.803761904761905,0.791266666666667,0.776757142857143,0.760719047619048,0.743552380952381,0.725385714285714,0.706166666666667,0.685857142857143,0.664628571428572,0.642433333333333,0.619261904761905,0.595428571428571,0.571185714285714,0.547266666666667,0.524442857142857,0.503314285714286,0.483976190476191,0.466114285714286,0.449390476190476,0.433685714285714,0.418800000000000,0.404433333333333,0.390476190476190,0.376814285714286,0.363271428571429,0.349790476190476,0.336028571428571,0.321700000000000,0.306276190476191,0.288642857142857,0.266647619047619,0.240347619047619];
g_color = [0.166300000000000,0.189780952380952,0.213771428571429,0.238600000000000,0.264457142857143,0.291938095238095,0.324242857142857,0.359833333333333,0.387509523809524,0.408614285714286,0.426600000000000,0.443042857142857,0.458571428571429,0.473690476190476,0.488666666666667,0.503985714285714,0.520023809523810,0.537542857142857,0.556985714285714,0.577223809523810,0.596580952380952,0.613700000000000,0.628661904761905,0.641785714285714,0.653485714285714,0.664195238095238,0.674271428571429,0.683757142857143,0.692833333333333,0.701500000000000,0.709757142857143,0.717657142857143,0.725042857142857,0.731714285714286,0.737604761904762,0.742433333333333,0.745900000000000,0.748080952380952,0.749061904761905,0.749114285714286,0.748519047619048,0.747314285714286,0.745600000000000,0.743476190476191,0.741133333333333,0.738400000000000,0.735566666666667,0.732733333333333,0.729900000000000,0.727433333333333,0.725785714285714,0.726114285714286,0.731395238095238,0.745457142857143];
r_color = [0.208100000000000,0.211623809523810,0.212252380952381,0.208100000000000,0.195904761904762,0.170728571428571,0.125271428571429,0.0591333333333334,0.0116952380952381,0.00595714285714286,0.0165142857142857,0.0328523809523810,0.0498142857142857,0.0629333333333333,0.0722666666666667,0.0779428571428572,0.0793476190476190,0.0749428571428571,0.0640571428571428,0.0487714285714286,0.0343428571428572,0.0265000000000000,0.0238904761904762,0.0230904761904762,0.0227714285714286,0.0266619047619048,0.0383714285714286,0.0589714285714286,0.0843000000000000,0.113295238095238,0.145271428571429,0.180133333333333,0.217828571428571,0.258642857142857,0.302171428571429,0.348166666666667,0.395257142857143,0.442009523809524,0.487123809523809,0.530028571428571,0.570857142857143,0.609852380952381,0.647300000000000,0.683419047619048,0.718409523809524,0.752485714285714,0.785842857142857,0.818504761904762,0.850657142857143,0.882433333333333,0.913933333333333,0.944957142857143,0.973895238095238,0.993771428571429];


for j = 1 : length(b_color)
    
    if color >= (j-1)/54
        
        blue = b_color(j);
        green = g_color(j);
        red = r_color(j);
        
        
    end    
    
end


end