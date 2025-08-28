function [txqec] = txqecInit_FindChannelEstimate(tu_aligned, rx_aligned, txqec_pre)
    % Init value
    % Device            |   ADRV9009    |   ADRV9026    |   
    % Init_Phase        |   512         |   1024        |
    % Init_Gain         |   0x4000      |   0x4000      |
    % GAIN_SCALE        |   15400       |   15400       |
    % PHASE_SCALE       |   20372       |   84883       |
    % PARAM_ADJ_SCALE   |   0.5         |   0.5         |
    % PHS_SCALE_FACTOR  |   [0.3, 3.0]  |   [0.8, 1.2]  |
    % GAIN_SCALE_FACTOR |   [1.0, 1.0]  |   [0.8, 1.2]  |

    INITIAL_PHASE_VALUE = 512;
    INITIAL_GAIN_VALUE = 0x4000;
    TXQEC_GAIN_SCALE = 15400;
    TXQEC_PHASE_SCALE = 84883;
    PARAM_ADJ_SCALE = 0.5;
    gain_scale_factor = 1;
    phase_scale_factor = 1;

    lo_ghz = 3.55; % GHz
    delta_gain = 0.00;%0.002;
    
    if nargin < 3
        txqec_pre.gain = INITIAL_GAIN_VALUE;
        txqec_pre.phase = INITIAL_PHASE_VALUE;
    end

    % read and accumulate data from correlator
    u = real(tu_aligned); v = imag(tu_aligned);
    z = real(rx_aligned); y = imag(rx_aligned);
    
    u = u - mean(u); v = v - mean(v);
    z = z - mean(z); y = y - mean(y);
    
    out.uu = u * u';
    out.vv = v * v';
    out.uv = u * v';
    out.zu = z * u';
    out.yu = y * u';
    out.zv = z * v';
    out.yv = y * v';
    
    % estimate gain and phase parameters 
    det = out.uu * out.vv - out.uv * out.uv;
    
    % check if not nearly singular 
    % (signal too small or u nearly identical to v)
    if det >= 1e6 %1e4
        a = (out.zu * out.vv - out.zv * out.uv) / det;
        b = (out.yu * out.vv - out.yv * out.uv) / det;
        c = (-out.zu * out.uv + out.zv * out.uu) / det;
        d = (-out.yu * out.uv + out.yv * out.uu) / det;
    else
        a = 0; b = 0; c = 0; d = 0;
    end
    
    precision = (out.uu + out.vv) * (a * a + b * b);
    fprintf("det = %.3f, precision = %.3f\n", det, precision);
    
    % Check if the precision in our calculation is 'good' enough
    % Compute the quadrature error correction adjustment
    if precision > 30
        gain_correction = sqrt((a*a + b*b) / (c*c + d*d)) - (1+delta_gain);
        phase_correction =  -(a*c + b*d) / (a*d - b*c);
        %phase_correction =  -(a*c + b*d) / (a*a + b*b); % optional
    
        fprintf("gain_est = %.3f, phase_est = %.3f\n", ...
            1/(gain_correction+1), -phase_correction*180/pi);
    
        % Scale to code words        
        txqec_adj_gain_scale_factor = TXQEC_GAIN_SCALE * gain_scale_factor;
        txqec_adj_phase_scale_factor = TXQEC_PHASE_SCALE * phase_scale_factor;

        adj.gain  = gain_correction * txqec_adj_gain_scale_factor;
        adj.phase = (phase_correction * txqec_adj_phase_scale_factor) / lo_ghz;
    
        % Only adjust by a fraction of the correction to avoid overshooting
        gain_adjust  = adj.gain * PARAM_ADJ_SCALE;
        phase_adjust = adj.phase * PARAM_ADJ_SCALE;
    
        txqec.gain = round(txqec_pre.gain + gain_adjust);
        txqec.phase = round(txqec_pre.phase - phase_adjust);
    else
        disp("model precision is pretty low, exit now.");
    end
        
end
