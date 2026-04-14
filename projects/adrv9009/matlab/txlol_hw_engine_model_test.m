close all;
clear;
clc;

% TXLOL HW engine model self-test for batch-based perturbation stats.

cfg = struct();
cfg.numBatches = 32;          % even and >= 4
cfg.samplesPerBatch = 256;
cfg.guardSamples = 2;
cfg.perturbAmp = 32;
cfg.startSign = 1;
cfg.sqMode = 'batch';         % 'batch' or 'sample'

L = cfg.numBatches;
N = cfg.samplesPerBatch;
G = cfg.guardSamples;
A = cfg.perturbAmp;

totalSamples = L * N;

% Build batch signs.
batchSign = cfg.startSign .* ((-1) .^ (0:(L - 1))).';

% Generate synthetic Tx path (not used by diff directly, but used for DC totals).
rng(7);
txLeakI = 150;
txLeakQ = -60;
txNoise = 6 * randn(totalSamples, 2);
txI = txLeakI + txNoise(:, 1);
txQ = txLeakQ + txNoise(:, 2);

% Generate ORx path.
% obs = base + h * (A * sign(batch)) + noise
hI = 3.0;
hQ = -1.5;
obsBaseI = 420;
obsBaseQ = -240;
obsNoiseStd = 10;

obsI = zeros(totalSamples, 1);
obsQ = zeros(totalSamples, 1);

for b = 1:L
    idx0 = (b - 1) * N + 1;
    idx1 = b * N;
    s = batchSign(b);

    obsI(idx0:idx1) = obsBaseI + (hI * A * s) + obsNoiseStd * randn(N, 1);
    obsQ(idx0:idx1) = obsBaseQ + (hQ * A * s) + obsNoiseStd * randn(N, 1);
end

[hwOut, dbg] = txlol_hw_engine_model(txI, txQ, obsI, obsQ, cfg);

disp(hwOut);

% Reference checks.
validBatches = 2:(L - 1);
L_eff = numel(validBatches);
N_eff = N - 2 * G;

% Expected signal-only component in RxDcDiff (without noise/base leakage residuals).
% RxDcDiff ~= h * A * N_eff * L_eff
expRxDiffI = hI * A * N_eff * L_eff;
expRxDiffQ = hQ * A * N_eff * L_eff;

rxDiffI = double(hwOut.RxDcDiffI);
rxDiffQ = double(hwOut.RxDcDiffQ);

% Estimate channel from model outputs.
hI_est = rxDiffI / double(hwOut.TxPerturb);
hQ_est = rxDiffQ / double(hwOut.TxPerturb);

fprintf('=== TXLOL HW ENGINE MODEL TEST ===\n');
fprintf('numBatches=%d, samplesPerBatch=%d, guard=%d\n', L, N, G);
fprintf('TxPerturb     = %g\n', double(hwOut.TxPerturb));
fprintf('TxPerturbSq   = %g\n', double(hwOut.TxPerturbSq));
fprintf('RxDcDiffI/Q   = %g, %g\n', rxDiffI, rxDiffQ);
fprintf('Expected I/Q  = %.1f, %.1f (signal-only)\n', expRxDiffI, expRxDiffQ);
fprintf('h_est I/Q     = %.5f, %.5f\n', hI_est, hQ_est);
fprintf('h_true I/Q    = %.5f, %.5f\n', hI, hQ);

% Basic sanity assertions.
assert(double(hwOut.TxDcTotalI) == round(sum(txI)), 'TxDcTotalI mismatch');
assert(double(hwOut.TxDcTotalQ) == round(sum(txQ)), 'TxDcTotalQ mismatch');
assert(double(hwOut.ObsDcTotalI) == round(sum(obsI)), 'ObsDcTotalI mismatch');
assert(double(hwOut.ObsDcTotalQ) == round(sum(obsQ)), 'ObsDcTotalQ mismatch');

assert(double(hwOut.TxPerturb) == A * N_eff * L_eff, 'TxPerturb mismatch');
assert(double(hwOut.TxPerturbSq) == (A * A) * N_eff * L_eff, 'TxPerturbSq mismatch');

% Loose tolerance due to random noise.
assert(abs(hI_est - hI) < 0.6, 'hI estimate outside tolerance');
assert(abs(hQ_est - hQ) < 0.6, 'hQ estimate outside tolerance');

% Quick plots for visual inspection.
figure;
subplot(2,1,1);
plot(dbg.batchObsI, '-o');
hold on;
plot(dbg.batchSign .* max(abs(dbg.batchObsI)) * 0.2, '--');
title('Per-batch ObsI integral and sign');
legend('batchObsI', 'scaled sign');
grid on;

subplot(2,1,2);
plot(dbg.batchObsQ, '-o');
hold on;
plot(dbg.batchSign .* max(abs(dbg.batchObsQ)) * 0.2, '--');
title('Per-batch ObsQ integral and sign');
legend('batchObsQ', 'scaled sign');
grid on;

fprintf('PASS: model checks completed.\n');
