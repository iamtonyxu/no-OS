function [pn, iData, qData] = pngen(numSamples, varargin)
%PNGEN Generate complex PN sequence using MATLAB built-in PN generator.
%
%   pn = PNGEN(numSamples) returns a complex PN sequence of length
%   numSamples with default settings.
%
%   pn = PNGEN(numSamples, Name, Value, ...) supports options:
%       'Amplitude'   : Overall amplitude applied to enabled channels.
%       'EnableI'     : Enable I channel (true/false).
%       'EnableQ'     : Enable Q channel (true/false).
%       'IAmplitude'  : I-channel amplitude (overrides 'Amplitude' for I).
%       'QAmplitude'  : Q-channel amplitude (overrides 'Amplitude' for Q).
%       'Bipolar'     : true -> map PN bits to {-1, +1}, false -> {0, 1}.
%       'PolynomialI' : PN polynomial for I channel.
%       'PolynomialQ' : PN polynomial for Q channel.
%       'InitStateI'  : Initial conditions for I channel PN generator.
%       'InitStateQ'  : Initial conditions for Q channel PN generator.
%
%   [pn, iData, qData] = PNGEN(...) also returns I and Q column vectors.

	p = inputParser;
	p.FunctionName = mfilename;

	addRequired(p, 'numSamples', @(x) isnumeric(x) && isscalar(x) && x > 0 && isfinite(x));
	addParameter(p, 'Amplitude', 1, @(x) isnumeric(x) && isscalar(x) && isfinite(x));
	addParameter(p, 'EnableI', true, @(x) islogical(x) || isnumeric(x));
	addParameter(p, 'EnableQ', true, @(x) islogical(x) || isnumeric(x));
	addParameter(p, 'IAmplitude', [], @(x) isempty(x) || (isnumeric(x) && isscalar(x) && isfinite(x)));
	addParameter(p, 'QAmplitude', [], @(x) isempty(x) || (isnumeric(x) && isscalar(x) && isfinite(x)));
	addParameter(p, 'Bipolar', true, @(x) islogical(x) || isnumeric(x));
	addParameter(p, 'PolynomialI', [7 6 0]);
	addParameter(p, 'PolynomialQ', [7 3 0]);
	addParameter(p, 'InitStateI', [0 0 0 0 0 0 1]);
	addParameter(p, 'InitStateQ', [1 0 0 0 0 0 1]);

	parse(p, numSamples, varargin{:});
	args = p.Results;

	numSamples = floor(double(args.numSamples));
	enableI = logical(args.EnableI);
	enableQ = logical(args.EnableQ);
	bipolar = logical(args.Bipolar);

	if isempty(args.IAmplitude)
		iAmp = double(args.Amplitude);
	else
		iAmp = double(args.IAmplitude);
	end

	if isempty(args.QAmplitude)
		qAmp = double(args.Amplitude);
	else
		qAmp = double(args.QAmplitude);
	end

	iData = zeros(numSamples, 1);
	qData = zeros(numSamples, 1);

	if enableI
		iData = iAmp * localGenPn(numSamples, args.PolynomialI, args.InitStateI, bipolar);
	end

	if enableQ
		qData = qAmp * localGenPn(numSamples, args.PolynomialQ, args.InitStateQ, bipolar);
	end

	pn = complex(iData, qData);
end

function seq = localGenPn(numSamples, polynomial, initState, bipolar)
	% comm.PNSequence is part of Communications Toolbox.
	gen = comm.PNSequence( ...
		'Polynomial', polynomial, ...
		'InitialConditions', initState, ...
		'SamplesPerFrame', numSamples, ...
		'Mask', 0);

	seq = double(gen());
	if bipolar
		seq = 2 * seq - 1;
	end
end
