clear
close all
t = 0:1/500:10;
rx = 10*sin(t*10);
x = rx + 100*sin(100*t+0.1) + 100*sin(180*t+0.2);
dx = 100*cos(t*10) + 0.1*t;

figure
plot(t,rx)
hold on
plot(t,x,'.')
plot(t,dx)

Filt.dt = 1/2^9;
Filt.C1 = 2^10;
Filt.C2 = 2^15;
bias =0;
kx = zeros(1,length(t));
kdx = zeros(1,length(t));

for i=1:(length(t)-1)
    [bias,kx(i+1),kdx(i+1)]=kalman(x(i),dx(i),bias,kx(i),kdx(i),Filt);
end

plot(t,kx)

legend('Real x','Measured x','Measured dx','Kalman x')