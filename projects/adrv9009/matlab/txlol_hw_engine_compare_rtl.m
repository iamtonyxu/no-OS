close all;
clear;
clc;

% Single-entry compare script:
% 1) Generates deterministic stimulus and runs the MATLAB model.
% 2) Invokes ModelSim to compile/run the RTL testbench.
% 3) Parses MAT_RESULT and RTL_RESULT and reports PASS/FAIL.

scriptDir = fileparts(mfilename('fullpath'));
origDir = pwd;
cleanupObj = onCleanup(@() cd(origDir));
cd(scriptDir);

fields = {'TxDcTotalI','TxDcTotalQ','ObsDcTotalI','ObsDcTotalQ', ...
          'TxPerturb','TxPerturbSq','RxDcDiffI','RxDcDiffQ', ...
          'RxDcDiffSqI','RxDcDiffSqQ','ObsDcLastI','ObsDcLastQ'};

testCfgs = [
    struct('numBatches', 10, 'samplesPerBatch', 32,  'guardSamples', 4, 'perturbAmp', 32, 'startSign', 1, 'sqMode', 'batch')
    struct('numBatches', 8,  'samplesPerBatch', 16,  'guardSamples', 0, 'perturbAmp', 8, 'startSign', 1, 'sqMode', 'batch')
    struct('numBatches', 12, 'samplesPerBatch', 24,  'guardSamples', 0, 'perturbAmp', 8, 'startSign', 1, 'sqMode', 'batch')
    struct('numBatches', 16, 'samplesPerBatch', 40,  'guardSamples', 0, 'perturbAmp', 8, 'startSign', 1, 'sqMode', 'batch')
    struct('numBatches', 20, 'samplesPerBatch', 64,  'guardSamples', 0, 'perturbAmp', 8, 'startSign', 1, 'sqMode', 'batch')
];

if isempty(which('system'))
    error('MATLAB system() is unavailable in this environment.');
end

if ~isfolder('work')
    [statusVlib, outVlib] = system('vlib work');
    if statusVlib ~= 0
        fprintf(2, '%s\n', outVlib);
        error('vlib failed with exit code %d.', statusVlib);
    end
end

[statusVmap, outVmap] = system('vmap work work');
if statusVmap ~= 0
    fprintf(2, '%s\n', outVmap);
    error('vmap failed with exit code %d.', statusVmap);
end

[statusVlog, outVlog] = system('vlog txlol_hw_engine.v txlol_hw_engine_tb.v');
if statusVlog ~= 0
    fprintf(2, '%s\n', outVlog);
    error('vlog failed with exit code %d.', statusVlog);
end

failures = {};

for t = 1:numel(testCfgs)
    cfg = testCfgs(t);

    L = cfg.numBatches;
    N = cfg.samplesPerBatch;
    totalSamples = L * N;

    txI = ones(totalSamples, 1) * 100;
    txQ = ones(totalSamples, 1) * (-20);
    obsI = zeros(totalSamples, 1);
    obsQ = zeros(totalSamples, 1);

    for b = 1:L
        s = cfg.startSign * (-1)^(b - 1);
        idx0 = (b - 1) * N + 1;
        idx1 = b * N;
        obsI(idx0:idx1) = 300 + s * cfg.perturbAmp;
        obsQ(idx0:idx1) = -120 + s * (cfg.perturbAmp / 2);
    end

    [hwOut, ~] = txlol_hw_engine_model(txI, txQ, obsI, obsQ, cfg);
    matMap = struct();
    matParts = cell(1, numel(fields));
    for k = 1:numel(fields)
        value = double(hwOut.(fields{k}));
        matMap.(fields{k}) = value;
        matParts{k} = sprintf('%s=%.0f', fields{k}, value);
    end
    fprintf('CASE %d CFG numBatches=%d samplesPerBatch=%d guardSamples=%d\n', ...
        t, cfg.numBatches, cfg.samplesPerBatch, cfg.guardSamples);
    fprintf('MAT_RESULT %s\n', strjoin(matParts, ' '));

    sqModeSample = strcmpi(cfg.sqMode, 'sample');
    vsimCmd = sprintf(['vsim -c txlol_hw_engine_tb ' ...
        '+NUM_BATCHES=%d +SAMPLES_PER_BATCH=%d +GUARD_SAMPLES=%d ' ...
        '+PERTURB_AMP=%d +START_SIGN=%d +SQ_MODE_SAMPLE=%d ' ...
        '-do "run -all; quit -f"'], ...
        cfg.numBatches, cfg.samplesPerBatch, cfg.guardSamples, ...
        cfg.perturbAmp, cfg.startSign, sqModeSample);

    [statusVsim, outVsim] = system(vsimCmd);
    if statusVsim ~= 0
        fprintf(2, '%s\n', outVsim);
        error('vsim failed with exit code %d for case %d.', statusVsim, t);
    end

    rtlLine = '';
    vsimLines = regexp(outVsim, '\r?\n', 'split');
    for k = 1:numel(vsimLines)
        line = strtrim(regexprep(vsimLines{k}, '^#\s*', ''));
        if startsWith(line, 'RTL_RESULT ')
            rtlLine = line;
            break;
        end
    end

    if isempty(rtlLine)
        fprintf('%s\n', outVsim);
        error('Failed to find RTL_RESULT in ModelSim output for case %d.', t);
    end

    fprintf('%s\n', rtlLine);
    rtlMap = parseResultLine(rtlLine, 'RTL_RESULT ', fields);

    mismatch = {};
    for k = 1:numel(fields)
        field = fields{k};
        matVal = matMap.(field);
        rtlVal = rtlMap.(field);
        if matVal ~= rtlVal
            mismatch{end + 1} = sprintf('%s: MAT=%.0f RTL=%.0f', field, matVal, rtlVal); %#ok<AGROW>
        end
    end

    if isempty(mismatch)
        fprintf('CASE %d PASS\n', t);
    else
        fprintf(2, 'CASE %d FAIL\n', t);
        for k = 1:numel(mismatch)
            fprintf(2, '%s\n', mismatch{k});
        end
        failures{end + 1} = sprintf('Case %d failed', t); %#ok<AGROW>
    end
end

if isempty(failures)
    fprintf('COMPARE_PASS all MATLAB and RTL cases match exactly.\n');
else
    error('MATLAB/RTL comparison failed for %d case(s).', numel(failures));
end

function result = parseResultLine(line, prefix, fields)
    payload = extractAfter(line, prefix);
    tokens = regexp(strtrim(payload), '\s+', 'split');
    result = struct();
    for i = 1:numel(fields)
        result.(fields{i}) = NaN;
    end
    for i = 1:numel(tokens)
        kv = regexp(tokens{i}, '^([^=]+)=(.+)$', 'tokens', 'once');
        if isempty(kv)
            continue;
        end
        key = kv{1};
        value = str2double(kv{2});
        if ismember(key, fields)
            result.(key) = value;
        end
    end
    for i = 1:numel(fields)
        if isnan(result.(fields{i}))
            error('Missing field %s while parsing result line.', fields{i});
        end
    end
end