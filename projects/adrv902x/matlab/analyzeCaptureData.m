% This script can be used to plot the AM-AM and AM-PM characteristics of the RF power amplifier.
% The inputs of this script are 4096 samples of Tu, Tx and ORx samples captured in the DPD buffer.
% The script compensates for the gain and phase on the ORx. The gain phase compensation function is available in CalDelayPhase.m

function [u, x, yAligned] = analyzeCaptureData(fs, u, x, y)

if 1
    [intDelay, fracDelay, yAligned, m] = CalDelayPhase(u, y);
    yAligned = yAligned.';
else
    yAligned = y;
end

if 1
m = 0;
yAligned = yAligned(1:end-m);
x = x(1:end-m);
u = u(1:end-m);
end

figure;
subplot(2,1,1);
plot(abs(x)); hold on
plot(abs(y));
title("capture signal abs");
legend('abs(tx)', 'abs(orx)');

pmu = unwrap(angle(u)) - mean(unwrap(angle(u)));
pmy = unwrap(angle(yAligned)) - mean(unwrap(angle(yAligned)));
pmx = unwrap(angle(x)) - mean(unwrap(angle(x)));

amtu = abs(u);
amorx = abs(yAligned);
amtx = abs(x);

subplot(2,1,2);
plot(amtu); hold on; grid on;
plot(amtx); hold on;
plot(amorx); hold off;
title('Time Domain');
legend('Tu', 'Tx', 'ORx');

finalFs = fs; % 2*491.52e6

figure;
subplot(2,2,1);
[ptx, f] = pwelch(u, [], [], [], finalFs, 'centered', 'power');
plot(f, pow2db(ptx)); hold on; grid on;

[ptx, f] = pwelch(x, [], [], [], finalFs, 'centered', 'power');
plot(f, pow2db(ptx)); hold on; grid on;

[ptx, f] = pwelch(yAligned, [], [], [], finalFs, 'centered', 'power');
plot(f, pow2db(ptx)); hold on; grid on;

title('Freq Domain');
legend('Tu', 'Tx', 'ORx');

pmtu = angle(yAligned./u); % angle((y-u)./y) - angle(mean((y-u)./y))
pmtu = pmtu - mean(pmtu);
gain = amorx./amtu;

%figure;
subplot(2,2,2);
plot(amtu, amorx, 'r.'); grid on;
xlabel('Input Magnitude');
ylabel('Output Magnitude');
%title('AM-AM, SKY66318, DPD ON, -20dBFS');
ylim([0, 0.6]);
title("AM-AM");

%figure;
subplot(2,2,3);
plot(amtu, (180/pi)*(pmtu - mean(pmtu)), 'm.'); grid on;
xlabel('Input Magnitude');
ylabel('Output Phase(degrees)');
ylim([-20, 20]);
title("AM-PM");

%figure;
subplot(2,2,4);
plot(amtu, 20*log10(abs(gain)), 'b.'); grid on;
xlabel('Input Magnitude');
ylabel('AM-AM Gain(dB)');
ylim([-3, 3]);
title("AM-Gain");
