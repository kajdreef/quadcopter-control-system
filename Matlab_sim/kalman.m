function [bias, phi, p]=kalman( sphi, sp, bias, phi, p,Filt )
%UNTITLED7 Summary of this function goes here
%   Detailed explanation goes here
    
    p = sp - bias;
	phi = phi + p*Filt.dt;
	e = phi-sphi;
	phi = phi-e/Filt.C1;
	bias = bias + e/Filt.C2;
end

