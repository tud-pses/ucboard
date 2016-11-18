
%%

ucb = ucboard('COM9');


%%

delete(ucb);
clear('ucb');

%%

ucb.sendCmd('?VER');


%%

ucb.sendCmd('!DAQ STOP');

ucb.sendCmd('!DAQ GRP 1 _TICS MX MY MZ MH');


N = 2000;

tics = zeros(N, 1);
data = zeros(N, 4);


ucb.sendCmd('!DAQ START');

pb = ProgressBar(N);

ucb.flushinput();

kk = 1;
for k = 1:N
    msg = ucb.receiveMsg();
    
    if isempty(msg)
        continue;
    end

    [grp, tics(kk), data(kk, 1), data(kk, 2), data(kk, 3), data(kk, 4)] = ...
                                                ucb.parseDAQData(msg);

    if ~isempty(grp)
        kk = kk + 1;
        pb = updateinc(pb);
    end
    
%     if (k == 1000)
%         ucb.sendCmd('!DRV F 0');
%     elseif (k == 2000)
%         ucb.sendCmd('!DRV F 300');
%     elseif (k == 3000)
%         ucb.sendCmd('!DRV F 1000');
%     elseif (k == 4000)
%         ucb.sendCmd('!DRV OFF');
%     end

end % for k
delete(pb);

ucb.sendCmd('!DAQ STOP');

tics = tics(1:kk-1);
data = data(1:kk-1,:);

disp('Finished!');

%%

heading = 180 * atan2(data(:,2), data(:,1)) / pi;
vb = (heading < 0);
heading(vb) = 360 + heading(vb);

%%
vh = [];

vh(1) = subplot(2, 1, 1);
%plot(tics(1:end-1)/1000, diff(tics));
plot(tics/1000, data(:,1:3));
legend({'MX', 'MY', 'MZ'});

vh(2) = subplot(2, 1, 2);
plot(tics/1000, [data(:,4), heading]);

linkaxes(vh, 'x');


%%

mx = data(:, 1);
my = data(:, 2);
mz = data(:, 3);

maxmx = max(mx);
minmx = min(mx);
maxmy = max(my);
minmy = min(my);
maxmz = max(mz);
minmz = min(mz);

%%


mx = data(:, 1);
my = data(:, 2);
mz = data(:, 3);


mx = (mx - minmx) * 2 * 1000 / (maxmx - minmx) - 1000;
my = (my - minmy) * 2 * 1000 / (maxmy - minmy) - 1000;
mz = (mz - minmz) * 2 * 1000 / (maxmz - minmz) - 1000;


heading = 180 * atan2(my, mx) / pi;
vb = (heading < 0);
heading(vb) = 360 + heading(vb);

%%
vh = [];

vh(1) = subplot(2, 1, 1);
%plot(tics(1:end-1)/1000, diff(tics));
plot(tics/1000, [mx, my, mz]);
legend({'MX', 'MY', 'MZ'});

vh(2) = subplot(2, 1, 2);
plot(tics/1000, [data(:,4), heading]);

linkaxes(vh, 'x');
