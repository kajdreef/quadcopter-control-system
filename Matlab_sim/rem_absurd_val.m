function [ res ] = rem_absurd_val( x, prev_x, Filt )
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here

if((x<Filt.min)||(x>Filt.max))
    res = prev_x;
    return
end

res = x;
 
end