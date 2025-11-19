function [filterCoeffs] = CorrectionFilter(state, hqVal)
    TXQEC_NUM_STATE_VARS = 3;
    TXQEC_INITIAL_GAIN_Q_FLOAT = 0x4000;
    len = TXQEC_NUM_STATE_VARS - 1;
    
    filterCoeffs = struct('I', zeros(1, 2*len + 1), 'Q', zeros(1, 2*len + 1));

    % Build hqq filter
    valp = state.alpha(1) * hqVal;
    filterCoeffs.Q(len + 1) = round(valp * TXQEC_INITIAL_GAIN_Q_FLOAT);
    
    % Build hiq filter
    for i = 2:TXQEC_NUM_STATE_VARS
        valp = 0.5 * (state.alpha(i) + state.beta(i));
        valm = 0.5 * (state.alpha(i) - state.beta(i));

        filterCoeffs.Q(len + i) = round(valm * hqVal * TXQEC_INITIAL_GAIN_Q_FLOAT);
        filterCoeffs.Q(len - i + 2) = round(valp * hqVal * TXQEC_INITIAL_GAIN_Q_FLOAT);
    end
end