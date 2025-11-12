function [x, Cov] = UpdateState(z, H, R, x, Cov)
    % UpdateState - Perform the Kalman filter update step
    %
    % Inputs:
    %   z   - observation (scalar)
    %   H   - transformation matrix (row vector)
    %   R   - observation noise variance (scalar)
    %   x   - state variables (column vector)
    %   Cov - covariance matrix
    %
    % Outputs:
    %   x   - updated state variables
    %   Cov - updated covariance matrix

    TXQEC_NUM_STATE_VARS = length(x);
    
    % Kalman Gain:
    % K = P * H' * (H * P * H' + R)^-1
    %
    % Update state variables:
    % x1 = x + K * (y - H * x0)
    %
    % Update covariance matrix
    % P1 = (I - K * H) * P0

    % First compute common P*H' vector
    PHT = Cov * H';
    
    % compute (H*P*H' + R) denominator
    den = H * PHT + R;
    
    % compute innovation
    innov = z - H * x;
    
    % Update state: x += P * H' * innov / den
    scale = innov / den;
    x = x + PHT * scale;
    
    % Update covariance matrix
    % P = (I - K * H) * P
    % P = P - (PHT) * (PHT)' / den
    scale = 1.0 / den;
    Cov = Cov - PHT * PHT' * scale;
end
