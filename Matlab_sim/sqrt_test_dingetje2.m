x = -0:1/1024:2.3;
y = 1024*sqrt(x);



P2 = polyfit(x,y,2);

my_y = my_sqrt5(x);

close all
plot(x,y)
hold on
plot(x,my_y)