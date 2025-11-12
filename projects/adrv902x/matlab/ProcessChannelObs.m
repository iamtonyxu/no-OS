function [obs] = ProcessChannelObs(chanEst)
    %% Calcuate Channel Observation Data based on Channel Estimate
    % ASSUMPTIONS:
    %   - Positive frequency observation comes first by default
    %   - Input data contains paired observations at +ve and -ve frequencies
    %
    % INPUTS:
    %   chanEst:   Array of channel estimates for each tone frequency, and tones are paired, for example
    %              chanEst(1) for +f1, chanEst(2) for -f1, chanEst(3) for +f2, chanEst(4) for -f2, ...
    %
    % OUTPUTS:
    %   obs:       Array of channel observation data corresponding to chanEst
    %
    n = numel(chanEst);
    obs = repmat(struct('realR',0,'imagR',0,'obsNoise',0), 1, n);

    for i = 1:numel(chanEst)
        if mod(i, 2) == 1
            % Process positive frequency observation
            DDi = chanEst{i}.Di;
            DDq = chanEst{i}.Dq;
            UUi = chanEst{i+1}.Ui;
            UUq = chanEst{i+1}.Uq;
        else
            % Process negative frequency observation
            DDi = chanEst{i}.Di;
            DDq = chanEst{i}.Dq;
            UUi = chanEst{i-1}.Ui;
            UUq = chanEst{i-1}.Uq;
        end

        % Compute (D + U)
        num.i = DDi + UUi;
        num.q = DDq + UUq;

        % Compute (D - U)
        den.i = DDi - UUi;
        den.q = DDq - UUq;

        % Compute Re{(D + U)/(D - U)} and Im{(D + U)/(D - U)}
        denMag = (den.i * den.i) + (den.q * den.q);

        if denMag > 0.0
            obs(i).realR =  ((num.i * den.i) + (num.q * den.q)) / denMag;
            obs(i).imagR = -((num.i * den.q) - (num.q * den.i)) / denMag;
        else
            obs(i).realR = 0;
            obs(i).imagR = 0;
        end
        obs(i).obsNoise = 0; % Placeholder for observed noise
        % printf obs as debug info
        fprintf("Obs %d: realR = %.6f, imagR = %.6f\n", i, obs(i).realR, obs(i).imagR);
    end

end