function [bias, phi, p]=kalman( sphi, sp, bias, phi, p,Filt )
%UNTITLED7 Summary of this function goes here
%   Detailed explanation goes here
    
    p = round(sp - bias,3);
	phi = round(phi + round(p/2^9,3),3); %dt*2
	e = round(phi-sphi,3);
	phi = round(phi-round(e/2^7,3),3);
	bias = round(bias + round(e/2^12,3),3);
end

