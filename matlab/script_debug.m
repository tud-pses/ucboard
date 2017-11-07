
%%

ucb = ucboard('COM5');


%%

delete(ucb);
clear('ucb');

%%

ucb.sendCmd('?VER');

%%

N = 100;

for k = 1:N
	ucb.sendRaw('?VER');
end % for k
