function [ y ] = my_sqrt6( x )
%UNTITLED6 Summary of this function goes here
%   Detailed explanation goes here

sq = cumsum(ones(1,1024)).^2;
i=0;

if sq(512+i)<=x
    i = i+512;
end
if sq(256+i)<=x
    i = i+256;
end
if sq(128+i)<=x
    i = i+128;
end
if sq(64+i)<=x
    i = i+64;
end
if sq(32+i)<=x
    i = i+32;
end
if sq(16+i)<=x
    i = i+16;
end
if sq(8+i)<=x
    i = i+8;
end
if sq(4+i)<=x
    i = i+4;
end
if sq(2+i)<=x
    i = i+2;
end
if sq(1+i)<=x
    i = i+1;
end
y = i;
end

