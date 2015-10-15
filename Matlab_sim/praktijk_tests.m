clear
close all

log6 = dlmread('m_off_pitch1.txt',' ');
proc_log = dlmread('log.txt',' ');

Filt.a0 = 1;

x = log6(1:2048,2);
dx= log6(2049:end,3);
t = log6(1:2048,1)/1e6;
[t,i] =sortrows(t,1);
x = x(i)-504+400*(rand(length(t),1)-0.5);
dx = -1*dx(i)+20*(rand(length(t),1)-0.5);

bias = -310;
phi = 0;
p = 0;

for i=1:length(x)
    %x(i) = rem_absurd_val(x(i),xy(1),Filt);
    x_filt(i) = x(i);
    %[x_filt(i),xy] = BF_2nd(x(i),xy,Filt);
    %[x_filt(i),xy] = LP_1st(x(i),xy);
    [bias(i+1), phi(i+1), p(i+1)]=kalman( x_filt(i), dx(i), bias(i), phi(i), p(i),Filt );
end

proc_dx = proc_log(4099:end,2);
proc_x = proc_log(4099:end,1);
plot(t,x,'.')
hold on
plot(t,dx,'.')
plot(t,phi(2:end));
plot(t,p(2:end));
plot(t(3:end),proc_x)
plot(t(3:end),proc_dx)
legend('Measured x','Measured dx','Computer x','Computer dx','X32 x','X32 dx')