function [ y ] = my_sqrt5( x )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
n = 3;  %rounding
p = [-588   1407  175];
y = round(round(p(1)*x,n) + p(2),n);
y = round(round(y.*x,n) + p(3),n);
y=round(y/1024,n);
y = round(y - round(round(round(y.*y,n)-round(x,n),n)./round(2*y,n),n),n);
y=round(y*1024,n);

% y = p(1)*x + p(2);
% y = y.*x + p(3);
% y=y/1024;
% y = y - (y.*y-x)./(2*y);
% y=y*1024;

end

