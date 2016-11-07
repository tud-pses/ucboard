
%%

ucb = ucboard('COM4');


%%

delete(ucb);
clear('ucb');

%%

ucb.sendCmd('?VER');

%%

ucb.sendCmd('?IMU OPT');

%%

ucb.sendCmd('!DAQ STOP');

ucb.sendCmd('!IMU OPT ~GFILT=0');

%ucb.sendCmd('!DAQ GRP 1 _TICS ~TS=1');
ucb.sendCmd('!DAQ GRP 1 _TICS AZ GZ ~TS=1');

ucb.sendCmd('!DAQ START');

%%


clszAFilters = {'0'};
clszGFilters = {'0'};
%clszAFilters = {'-1', '0', '1', '2', '3', '4', '5', '6', '7'};
%clszGFilters = {'-2', '-1', '0', '1', '2', '3', '4', '5', '6', '7'};

if (length(clszAFilters) ~= length(clszGFilters))
    error('');
end

Nf = length(clszAFilters);

N = 5 * 1000;

tics = zeros(N, Nf);
dataAZ = zeros(N, Nf);
dataGZ = zeros(N, Nf);



for f = 1:Nf
    ucb.sendCmd(['!IMU OPT ~GFILT=', clszGFilters{f}, ' ~AFILT=', clszAFilters{f}]);

    pause(1);
    
    ucb.sendCmd('!DAQ START');

    %pb = ProgressBar(N);
    
    ucb.flushinput();

    k = 1;
    while (k <= N)


        msg = ucb.receiveMsg();
        
        [grp, tics(k, f), dataAZ(k, f), dataGZ(k, f)] = ...
                                                    ucb.parseDAQData(msg);

        if ~isempty(grp)
            k = k + 1;
            %pb = updateinc(pb);
        end

    end % for k
    %delete(pb);
    
    ucb.sendCmd('!DAQ STOP');
end % for f

disp('Finished!');

%%

nErr = 0;
nNoResp = 0;

for k = 1:100
    disp(k);
    resp = ucb.sendCmd('?IMU OPT', false);
    
    
    if (isempty(resp()))
        nNoResp = nNoResp + 1;
    elseif (resp(2) ~= '~')
        nErr = nErr + 1;
    end
end

%%

data = dataAZ;
clszFilters = clszAFilters;

Nf = size(data, 2);

Ts = 1e-3;

figure;

for f = 1:Nf
    x = data(:, f);
    x = x - mean(x);
    x = x(1:1000);
    [psd, w] = pwelch(x);
    
    fplot = w / pi * 0.5 * 1/Ts;
    
    plot(fplot, psd);
    
    if (f == 1)
        hold('on');
    end
end % for f

hold('off');

legend(clszFilters);

xlabel('f [Hz]');

