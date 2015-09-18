function [ res,xy ] = BF_2nd( x, xy, Filt )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here


xy(3) = xy(2);
xy(2) = xy(1);
xy(1) = x;

% Shift the y values and calculate the new value for y(n)
xy(6) = xy(5);
xy(5) = xy(4);
xy(4) = Filt.a0*xy(1) + Filt.a1*xy(2) + Filt.a2*xy(3) - Filt.b1*xy(5) - Filt.b2*xy(6);

res = xy(4);

end

