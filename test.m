clear all;

function y = f (x)
  y = 1/x;
endfunction

[q, ier, nfun, err] = quad ("f", 1, 5)

