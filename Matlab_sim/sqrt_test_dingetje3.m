x = -0:1/1024:1;
y = 1024*sqrt(x);



P2 = polyfit(x,y,1);

my_y = my_sqrt7(x);

close all
plot(x,y)
hold on
plot(x,my_y)