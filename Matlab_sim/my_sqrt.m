function [ res ] = my_sqrt( a )

a = uint64(a);
rem = uint64(0);
root = uint64(0);
devisor = uint64(0);

for i=0:15
    root = root*2;
    rem = rem*4+floor(a/(2^30));
    a = a*4;
    devisor = root*2 + 1;
    if(devisor <= rem)
        rem = rem-devisor;
        root = root + 1;
    end
    
end

res = root/2;
end

