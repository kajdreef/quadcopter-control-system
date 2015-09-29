function [ y ] = my_sqrt3( x )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
mu = [349350 312450];
p = [21   -76  65  -14  238 586];
p = [16   -64  64  -16  256 512+64];
x1 = (x - mu(1))./mu(2);
y = ((((p(1)*x1 + p(2)).*x1 + p(3)).*x1 + p(4)).*x1 + p(5)).*x1+p(6);

y = y - (y.*y-x)./(2*y);
end

