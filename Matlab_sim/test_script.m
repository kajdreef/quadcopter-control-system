[a b] = butter(2,0.5);

% Let op! a en b zijn gereversed in de definitie in de help van matlab!

Filt.a0 = a(1)/b(1);
Filt.a1 = a(2)/b(1);
Filt.a2 = a(3)/b(1);
Filt.b1 = b(2)/b(1);
Filt.b2 = b(3)/b(1);
Filt.min = -2;
Filt.max = 2;
Filt.C1 = 5;
Filt.C2 = Filt.C1*50;
Filt.dt = 0.001;

xy = zeros(6,1);

t = 0:1/1270:1;

x = 100*sin(t*2*pi) + 50*sin(100*t*2*pi);

dx = 100*2*pi*cos(t*2*pi);

bias = 0;
phi = 0;
p = 0;

figure
plot(t,x);
hold on
plot(t,dx);

for i=1:length(t)
    x(i) = rem_absurd_val(x(i),xy(1),Filt);
    % [x_filt(i),xy] = BF_2nd(x(i),xy,Filt);
    [x_filt(i),xy] = LP_1st(x(i),xy);
    [bias(i+1), phi(i+1), p(i+1)]=kalman( x_filt(i), dx(i), bias(i), phi(i), p(i),Filt );
end



plot(t,x_filt);
plot(t,phi(2:end));
plot(t,p(2:end));