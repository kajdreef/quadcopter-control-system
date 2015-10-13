clear
close all

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

log = log6;

t = log(1:2048,1)/1e6;
[t,i] =sortrows(t,1);
x1 = log(1:2048,2)-504;
x2 = log(1:2048,3)-499;
x3 = log(1:2048,4)-513;
dx1= log(2049:end,2)-378;
dx2= log(2049:end,3)-310;
dx3= log(2049:end,4)-488;
x1 = x1(i);
x2 = x2(i);
x3 = x3(i);
dx1 = dx1(i);
dx2 = dx2(i);
dx3 = dx3(i);

figure
plot(t,x1,'.')
hold on
%plot(t,x2,'.')
%plot(t,x3,'.')
%plot(t,dx1,'.')
plot(t,dx2,'.')
%plot(t,dx3,'.')

%legend('x1','x2','x3','dx1','dx2','dx3')