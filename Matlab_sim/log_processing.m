log1 = dlmread('m_log_vert2_2.txt',' ');
log2 = dlmread('m_off_hor1.txt',' ');
log3 = dlmread('m_off_hor2.txt',' ');
log4 = dlmread('m_off_horizontal.txt',' ');
log5 = dlmread('m_off_horizontal 2.txt',' ');
log6 = dlmread('m_off_pitch1.txt',' ');
log7 = dlmread('m_off_pitch2.txt',' ');
log8 = dlmread('m_off_roll1.txt',' ');
log9 = dlmread('m_off_roll2.txt',' ');
log10 = dlmread('m_off_vert1_1.txt',' ');
log11 = dlmread('m_off_vert1_2.txt',' ');
log12 = dlmread('m_off_vert2_1.txt',' ');
log13 = dlmread('m_off_yaw1.txt',' ');
log14 = dlmread('m_off_yaw2.txt',' ');
log15 = dlmread('m_on1.txt',' ');
log16 = dlmread('m_on2.txt',' ');
log17 = dlmread('m_on3.txt',' ');


%% Filter section
[a b] = butter(2,25/500);

% Let op! a en b zijn gereversed in de definitie in de help van matlab!

Filt.a0 = a(1)/b(1);
Filt.a1 = a(2)/b(1);
Filt.a2 = a(3)/b(1);
Filt.b1 = b(2)/b(1);
Filt.b2 = b(3)/b(1);
Filt.min = -2;
Filt.max = 2;
Filt.C1 = 5;
Filt.C2 = Filt.C1*1000;
Filt.dt = 1/500;

xy = zeros(6,1);

x = log17(1:2048,2);
dx= log17(2049:end,2);
t = log17(1:2048,1)/1e6;
[t,i] =sortrows(t,1);
x = x(i);
dx = dx(i);

bias = 0;
phi = 0;
p = 0;

close all
figure

%plot(x);
plot(t,x,'.');
hold on
plot(t,dx,'.');

for i=1:length(x)
    %x(i) = rem_absurd_val(x(i),xy(1),Filt);
    %x_filt(i) = x(i);
    [x_filt(i),xy] = BF_2nd(x(i),xy,Filt);
    %[x_filt(i),xy] = LP_1st(x(i),xy);
    [bias(i+1), phi(i+1), p(i+1)]=kalman( x_filt(i), dx(i), bias(i), phi(i), p(i),Filt );
end



plot(t,x_filt);
plot(t,phi(2:end));
plot(t,p(2:end));
xlim([min(t) max(t)])