function [corrFilterCoeffs] = CalcWidebandFlt(gdFilterData, corrGain)

%% structure definitions
Complex_ChanEst_t = struct("Di", [], "Dq", [], "Ui", [], "Uq", []);
TxQecInit_Obs_t = struct("realR", [], "imagR", [], "obsNoise", []);

numTones = numel(gdFilterData);
freq= zeros(numTones,1);

chanEst = repmat({Complex_ChanEst_t}, numTones, 1); % chanEst for each tone
obs = repmat({TxQecInit_Obs_t}, numTones, 1); % obs for each tone

%% Solve channel estimation for each tone
for i = 1:numTones
    freq(i) = gdFilterData(i).freq;
    corrData = gdFilterData(i).corrData;
    chanEst{i} = SolverComplex2X2(corrData);
end

%% Process channel observations
obs = ProcessChannelObs(chanEst);

%% Estimate Wideband filter
state = EstimateFilter(freq, obs);

% print state.alpha and state.beta as debug info
fprintf("Estimated kalman system state:\n");
for i = 1:numel(state.alpha)
    fprintf("Alpha[%d] = %.6f\n", i-1, state.alpha(i));
end

for i = 1:numel(state.beta)
    fprintf("Beta[%d] = %.6f\n", i-1, state.beta(i));
end

%% Calculate Correction Filter Coefficients
corrFilterCoeffs = CorrectionFilter(state, corrGain);
% printf corrFilterCoeffs as debug info
fprintf("Calculated Correction Filter Coefficients (Q):\n");
len = (numel(corrFilterCoeffs.Q) - 1) / 2;
for i = 1:numel(corrFilterCoeffs.Q)
    fprintf("Coeff[%d] = %d\n", i - (len + 1), corrFilterCoeffs.Q(i));
end

end