clear all
a = 1
b = 1+i
global d
global e
d = 1
e = 1+i
f = true
global g
g = false
save flags.mat

clear all
a = {1, [1, 1], ones(2)}
save struct.mat

clear all
a = double([1, 2, 3]);
b = single([1, 2, 3]);
d = uint8([1, 2, 3]);
e = uint16([1, 2, 3]);
save numeric.mat

clear all
a = true
b = ones(2,3) == 0
save logical.mat