function [ y ] = my_sqrt7( x )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
n = 2;  %rounding
p = [820   273];

y = p(1)*x + p(2);
%y = y.*x + p(3);
y=y/1024;
y = y - (y.*y-x)./(2*y);
y=y*1024;

% y = round(p(1)*x,n) + p(2);
% y = round(y.*x,n) + p(3);
% y=round(y/1024,n);
% y = round(y - round(round(round(y.*y,n)-round(x,n),n)./round(2*y,n),n),n);
% y=y*1024;

end

