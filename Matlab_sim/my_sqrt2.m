function [ y ] = my_sqrt2( x )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
mu = [349350 312450];
p = [-27   88  -86  231  599];
p = [-2^5   2^6+2^5  -2^6  2^8  599];
x1 = (x - mu(1))./mu(2);
y = (((p(1)*x1 + p(2)).*x1 + p(3)).*x1 + p(4)).*x1 + p(5);

y = y - (y.*y-x)./(2*y);
end

