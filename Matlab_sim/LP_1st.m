function [ res,xy ] = LP_1st( x,xy )
%UNTITLED9 Summary of this function goes here
%   Detailed explanation goes here

RC = 1/(2*pi*300);
alpha = 0.001/(0.001+RC);

xy(3) = xy(2);
xy(2) = xy(1);
xy(1) = x;

% Shift the y values and calculate the new value for y(n)
xy(6) = xy(5);
xy(5) = xy(4);
xy(4) = xy(5) + alpha*(xy(1) - xy(5));

res = xy(4);

end

