classdef ucboard < handle

    properties
        ser
        bEchoCmds
        bDispCmdResps
        bDispRxMsgs
    end % porperties
    
    methods
        % Constructor
        function obj = ucboard(comport, varargin)
            
            p = inputParser();
            p.KeepUnmatched = false;

            p.addParameter('BaudRate', 921600, @isnumeric);
            p.addParameter('EchoCmds', true, @islogical);
            p.addParameter('DisplayCmdResps', true, @islogical);
            p.addParameter('DispRxMsgs', false, @islogical);

            p.parse(varargin{:});

            baudrate = p.Results.BaudRate;
            bEchoCmds = p.Results.EchoCmds;
            bDispCmdResps = p.Results.DisplayCmdResps;
            bDispRxMsgs = p.Results.DispRxMsgs;
            
            if (nargin < 2), baudrate = 921600; end
            
            ser = serial(comport);
            set(ser, 'BaudRate', baudrate);
            set(ser, 'DataBits', 8);
            set(ser, 'Parity', 'none');
            set(ser, 'StopBits', 1);
            set(ser, 'ReadAsyncMode', 'continuous');
            set(ser, 'Terminator', 3);
            set(ser, 'Timeout', 1);
            set(ser, 'InputBufferSize', 10000);

            fopen(ser);
            
            obj.ser = ser;
            
            obj.bEchoCmds = bEchoCmds;
            obj.bDispCmdResps = bDispCmdResps;
            obj.bDispRxMsgs = bDispRxMsgs;
        end % function ucboard
        
        % Destructor
        function delete(obj)
            fclose(obj.ser);
        end % function delete
        
        function resp = sendCmd(obj, cmd, bDisp)
            
            if (nargin < 3)
                bDispRx = obj.bDispCmdResps;
                bEchoCmd = obj.bEchoCmds;
            else
                bDispRx = bDisp;
                bEchoCmd = bDisp;
            end
            
            if (bEchoCmd)
                disp(cmd);
            end

            cmd = sprintf([cmd, '\n']);
            
            flushinput(obj.ser);

            fwrite(obj.ser, cmd);

            resp = '';

            while (true)
                rx = receiveMsg(obj, false);

                if (length(rx) <= 1)
                    break;
                elseif (rx(1) == ':')
                    resp = rx;
                    break;
                end
            end
            
            if (bDispRx)
                if ~isempty(resp)
                    disp(resp);
                else
                    disp('[no response!]');
                end
            end
            
        end % function sendCmd

        function msg = receiveMsg(obj, bDispRx)
            
            if (nargin < 2), bDispRx = obj.bDispRxMsgs; end
            
            msg = '';

            while (true)
                rx = fgetl(obj.ser);

                if (length(rx) <= 1)
                    break;
                else
                    msg = strtrim(rx);
                    break;
                end
            end
            
            if (bDispRx)
                if ~isempty(msg)
                    disp(msg);
                end
            end
        end % function sendCmd
        
        function flushinput(obj)
            flushinput(obj.ser);
        end % function flushinput

        function varargout = parseDAQData(obj, msg) %#ok<INUSL>
            bInvalidMsg = false;
            
            if isempty(msg)
                bInvalidMsg = true;
            elseif (msg(1) ~= '#')
                bInvalidMsg = true;
            elseif (msg(2) ~= '#')
                bInvalidMsg = true;
            end
            
            if (~bInvalidMsg)
                t = regexp(msg, '^##(\d+):(.*)', 'tokens');
            
                if isempty(t)
                    bInvalidMsg = true;
                end
            end
            
            if (~bInvalidMsg)
                grp = str2double(t{1}{1});

                t = regexp(t{1}{2}, '([^\|]*)', 'tokens');

                vals = cell(length(t) + 1, 1);
                vals{1} = grp;

                for it = 1:length(t)
                    s = strtrim(t{it}{1});

                    vals{it + 1} = str2num(s); %#ok<ST2NM>
                end % for it
            end
            
            if (~bInvalidMsg)
                if (nargout <= 1)
                    varargout{1} = vals;
                else
                    varargout = vals;
                end
            else
                if (nargout <= 1)
                    varargout{1} = [];
                else
                    varargout = cell(nargout, 1);
                    
                    varargout{1} = [];
                
                    for io = 2:nargout
                        varargout{io} = nan;
                    end % for io
                end
            end
        end % function parseDAQData
        
    end % methods
    
end % classdef ucboard
