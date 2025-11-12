function [chanEst] = SolverComplex2X2(corrData)
    % Extract and re-arrange data
    % float  uu;    Corr (Tx_I * Tx_I) = Auto (Tx_I)
    % float  vv;    Corr (Tx_Q * Tx_Q) = Auto (Tx_Q)
    % float  uv;    Corr (Tx_I * Tx_Q)
    % float  zu;    Corr (Rx_I * Tx_I)
    % float  yu;    Corr (Rx_Q * Tx_I)
    % float  zv;    Corr (Rx_I * Tx_Q
    % float  yv;    Corr (Rx_Q * Tx_Q)

    yxc.i = corrData.zu + corrData.yv;
    yx.i  = corrData.zu - corrData.yv;
    yxc.q = -(corrData.zv - corrData.yu);
    yx.q  =  (corrData.zv + corrData.yu);
    uu    = corrData.uu;
    vv    = corrData.vv;
    uv    = corrData.uv;
    xc2.i = uu - vv;
    xc2.q = -2.0 * uv;
    x2.i  = uu - vv;
    x2.q  = 2.0 * uv;

    %% Compute determinant
    ax2    = uu + vv;
    det.i  = (ax2 * ax2) - ((x2.i * xc2.i) - (x2.q * xc2.q));
    det.q  = (x2.i * xc2.q) + (x2.q * xc2.i);
    detSum = det.i + det.q;

    %% Solve for D and U
    % D = Eyx_c * |x|^2 - (xc)^2 * Eyx
    chanEst.Di = ((ax2 * yxc.i) - ((xc2.i * yx.i) - (xc2.q * yx.q))) / detSum;
    chanEst.Dq = ((ax2 * yxc.q) - ((xc2.i * yx.q) + (xc2.q * yx.i))) / detSum;

    %% U = Eyx * |x|^2 - (x)^2 * Eyx_c
    chanEst.Ui = ((ax2 * yx.i) - ((x2.i * yxc.i) - (x2.q * yxc.q))) / detSum;
    chanEst.Uq = ((ax2 * yx.q) - ((x2.i * yxc.q) + (x2.q * yxc.i))) / detSum;

end