y = 1:1023;
x = y.*y;

[P1,~,mu1] = polyfit(x,y,1);
[P2,~,mu2] = polyfit(x,y,2);
[P3,~,mu3] = polyfit(x,y,3);
[P4,~,mu4] = polyfit(x,y,4);
[P5,~,mu5] = polyfit(x,y,5);
my_y = my_sqrt2(x);
my_y2 = my_sqrt3(x);
my_y3 = my_sqrt4(x);

figure
hold on
plot(x,y)
%plot(x,polyval(P1,x,[],mu1))
%plot(x,polyval(P2,x,[],mu2))
%plot(x,polyval(P3,x,[],mu3))
%plot(x,polyval(P4,x,[],mu4))
plot(x,my_y3)
