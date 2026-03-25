close all;
clear all;
clc;

% generate txlol waveform with custom parameters
%       'NumSamples'      : Number of samples to generate.
%       'PnPowerDb'       : PN amplitude relative to full scale, in dBFS.
%       'PerturbScaler'   : I-channel perturbation in DAC counts.
%       'DacBits'         : DAC resolution.
%       'EnableI'         : Enable I PN sequence.
%       'EnableQ'         : Enable Q PN sequence.
%       'PolynomialI'     : PN polynomial for I.
%       'PolynomialQ'     : PN polynomial for Q.
%       'InitStateI'      : Initial state for I PN.
%       'InitStateQ'      : Initial state for Q PN.
%       'PlotEnable'      : Plot time/frequency domain results.


[waveform, iData, qData, info] = txlol_waveform_gen('NumSamples', 4096, ...
                                                    'PnPowerDb', -80, ...
                                                    'PerturbScaler', 32, ...
                                                    'DacBits', 14, ...
                                                    'EnableI', true, ...
                                                    'EnableQ', false, ...
                                                    'PlotEnable', true);

