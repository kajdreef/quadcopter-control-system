function [ y ] = my_sqrt4( x )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
mu = [349350 312450];
p = [-75   334  587];
x1 = (x - mu(1))./mu(2);
y = (p(1)*x1 + p(2)).*x1 + p(3);

y = y - (y.*y-x)./(2*y);

end

