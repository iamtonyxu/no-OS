function [orx] = txlol_model(tx, setPhs, setGain)
% Apply channel model: IQ rotation + gain
transferMatrix = [cos(setPhs), sin(setPhs); 
                  -sin(setPhs), cos(setPhs)];

orx = [real(tx), imag(tx)] * transferMatrix * setGain;
orx = orx(:,1) + 1j*orx(:,2);