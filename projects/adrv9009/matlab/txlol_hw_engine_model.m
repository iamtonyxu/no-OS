function [hwOut, dbg] = txlol_hw_engine_model(txI, txQ, obsI, obsQ, cfg)
%TXLOL_HW_ENGINE_MODEL Batch-based TXLOL perturbation HW engine model.
%   [hwOut, dbg] = txlol_hw_engine_model(txI, txQ, obsI, obsQ, cfg)
%
%   This model produces an output struct with fields aligned to TxlolHw_Outputs_t:
%   TxDcTotalI, TxDcTotalQ, ObsDcTotalI, ObsDcTotalQ,
%   TxPerturb, TxPerturbSq, RxDcDiffI, RxDcDiffQ,
%   RxDcDiffSqI, RxDcDiffSqQ, ObsDcLastI, ObsDcLastQ.
%
%   Expected cfg fields:
%     numBatches         - total number of batches in capture window (even, >= 4)
%     samplesPerBatch    - samples per batch
%     guardSamples       - samples dropped at both batch edges (>= 0)
%     perturbAmp         - perturbation magnitude per valid sample (>= 0)
%     startSign          - sign of batch #1, either +1 or -1
%     sqMode             - 'batch' or 'sample'
%
%   Notes:
%   - Valid batches are #2..#(L-1), i.e. first/last batches are discarded.
%   - Valid samples in each batch are [guard+1 .. N-guard].
%   - TxPerturb uses ABS accumulation over valid samples:
%       TxPerturb = sum(|p[n]|)
%     so it does not cancel with alternating signs.

    narginchk(5, 5);

    required = {'numBatches','samplesPerBatch','guardSamples','perturbAmp','startSign','sqMode'};
    for k = 1:numel(required)
        if ~isfield(cfg, required{k})
            error('cfg.%s is required.', required{k});
        end
    end

    L = double(cfg.numBatches);
    N = double(cfg.samplesPerBatch);
    G = double(cfg.guardSamples);
    A = double(cfg.perturbAmp);
    startSign = double(cfg.startSign);
    sqMode = char(cfg.sqMode);

    if mod(L, 2) ~= 0 || L < 4
        error('numBatches must be even and >= 4.');
    end
    if G < 0 || G >= (N/2)
        error('guardSamples must satisfy 0 <= G < samplesPerBatch/2.');
    end
    if ~(startSign == 1 || startSign == -1)
        error('startSign must be +1 or -1.');
    end
    if ~(strcmpi(sqMode, 'batch') || strcmpi(sqMode, 'sample'))
        error('sqMode must be ''batch'' or ''sample''.');
    end

    txI = double(txI(:));
    txQ = double(txQ(:));
    obsI = double(obsI(:));
    obsQ = double(obsQ(:));

    totalSamples = L * N;
    if numel(txI) ~= totalSamples || numel(txQ) ~= totalSamples || ...
       numel(obsI) ~= totalSamples || numel(obsQ) ~= totalSamples
        error('All inputs must have length numBatches*samplesPerBatch.');
    end

    % Full-window totals.
    TxDcTotalI = sum(txI);
    TxDcTotalQ = sum(txQ);
    ObsDcTotalI = sum(obsI);
    ObsDcTotalQ = sum(obsQ);

    ObsDcLastI = obsI(end);
    ObsDcLastQ = obsQ(end);

    RxDcDiffI = 0.0;
    RxDcDiffQ = 0.0;
    RxDcDiffSqI = 0.0;
    RxDcDiffSqQ = 0.0;
    TxPerturb = 0.0;
    TxPerturbSq = 0.0;

    % Optional debug traces.
    dbg = struct();
    dbg.batchSign = zeros(L, 1);
    dbg.batchValid = false(L, 1);
    dbg.batchObsI = zeros(L, 1);
    dbg.batchObsQ = zeros(L, 1);

    for b = 1:L
        s = startSign * (-1)^(b - 1);
        dbg.batchSign(b) = s;

        idx0 = (b - 1) * N + 1;
        idx1 = b * N;

        valid0 = idx0 + G;
        valid1 = idx1 - G;

        bi = sum(obsI(valid0:valid1));
        bq = sum(obsQ(valid0:valid1));

        dbg.batchObsI(b) = bi;
        dbg.batchObsQ(b) = bq;

        isValidBatch = (b >= 2) && (b <= (L - 1));
        dbg.batchValid(b) = isValidBatch;

        if isValidBatch
            RxDcDiffI = RxDcDiffI + s * bi;
            RxDcDiffQ = RxDcDiffQ + s * bq;

            nEff = valid1 - valid0 + 1;
            TxPerturb = TxPerturb + abs(A) * nEff;
            TxPerturbSq = TxPerturbSq + (A * A) * nEff;

            if strcmpi(sqMode, 'batch')
                RxDcDiffSqI = RxDcDiffSqI + bi * bi;
                RxDcDiffSqQ = RxDcDiffSqQ + bq * bq;
            else
                oi = obsI(valid0:valid1);
                oq = obsQ(valid0:valid1);
                RxDcDiffSqI = RxDcDiffSqI + sum(oi .* oi);
                RxDcDiffSqQ = RxDcDiffSqQ + sum(oq .* oq);
            end
        end
    end

    hwOut = struct();
    hwOut.TxDcTotalI  = int64(round(TxDcTotalI));
    hwOut.TxDcTotalQ  = int64(round(TxDcTotalQ));
    hwOut.ObsDcTotalI = int64(round(ObsDcTotalI));
    hwOut.ObsDcTotalQ = int64(round(ObsDcTotalQ));

    hwOut.TxPerturb   = int64(round(TxPerturb));
    hwOut.TxPerturbSq = int64(round(TxPerturbSq));

    hwOut.RxDcDiffI   = int64(round(RxDcDiffI));
    hwOut.RxDcDiffQ   = int64(round(RxDcDiffQ));
    hwOut.RxDcDiffSqI = int64(round(RxDcDiffSqI));
    hwOut.RxDcDiffSqQ = int64(round(RxDcDiffSqQ));

    hwOut.ObsDcLastI  = int64(round(ObsDcLastI));
    hwOut.ObsDcLastQ  = int64(round(ObsDcLastQ));
end
