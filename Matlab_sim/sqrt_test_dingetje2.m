x = linspace(-1,1,1024);
y = 1024*sqrt((x+1)/2);



P2 = polyfit(x,y,2);

my_y = my_sqrt5(x);

close all
plot(x,y)
hold on
plot(x,my_y)