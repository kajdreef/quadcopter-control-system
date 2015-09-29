function [ y ] = my_sqrt2_n( x )
%UNTITLED Summary of this function goes here
%   Detailed explanation goes here
mu = [349350 312450];
p = [-27   88  -86  231  599];
p = [-2^5   2^6+2^5  -2^6  2^8  599];

x_p = (x - 349350)./312450;
y = bit(x_p,

end

