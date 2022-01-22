%******************************************************************************\
%* Technische Universitaet Darmstadt, Institut fuer Nachrichtentechnik
%* Copyright (c) 2005
%*
%* Author:
%*	Volker Fischer
%*
%* Description:
%* 	This file automatically reads all relevant simulation results
%*	Note: for correct outputs/plots this script needs Matlab >= R14
%*
%******************************************************************************
%*
%* This program is free software; you can redistribute it and/or modify it under
%* the terms of the GNU General Public License as published by the Free Software
%* Foundation; either version 2 of the License, or (at your option) any later 
%* version.
%*
%* This program is distributed in the hope that it will be useful, but WITHOUT 
%* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
%* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
%* details.
%*
%* You should have received a copy of the GNU General Public License along with
%* this program; if not, write to the Free Software Foundation, Inc., 
%* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
%*
%******************************************************************************/

function [] = plotsimres(doremcompnamein)
global plotcolors;
global LW;
global figstretch;
global doremcompname;
	
	% Check parameters
	if nargin < 1
        doremcompname = 0; % do not remove by default
    else
        doremcompname = doremcompnamein;
	end

	close all;
	
	% definitions
	plotcolors = {'b-' 'g-' 'r-' 'k-' 'm-' 'y-' 'c-' ...
        'b--' 'g--' 'r--' 'k--' 'm--' 'y--' 'c--' ...
        'bd-' 'gd-' 'rd-' 'kd-' 'md-' 'yd-' 'cd-' ...
		'b+-' 'g+-' 'r+-' 'k+-' 'm+-' 'y+-' 'c+-' ...
		'b*-' 'g*-' 'r*-' 'k*-' 'm*-' 'y*-' 'c*-'};
	LW = 2; % line width
	figstretch = 1.6;


	% Get file names of simulation results files
	files = dir('*.dat');
	if (length(files) == 0)
		disp('no simulation output files found');
		return;
	end

	% sort files by date
	filedates = [];
	for i = 1:length(files)
        filesdates(i) = datenum(files(i).date);
	end
	[dummy, sortdateidxs] = sort(filesdates);
	files = files(sortdateidxs);


	% sort files
	simresBERIDEAL = struct([]);
	simresMSE = struct([]);
	simresBER = struct([]);
	simresSYNC = struct([]);
	simrescntBERIDEAL = 1;
	simrescntMSE = 1;
	simrescntBER = 1;
	simrescntSYNC = 1;
    newestfile = -1;
	for i = 1:length(files)
        % sort out SIMTIME files
        if isempty(findstr(files(i).name, '_SIMTIME'))
            % sort out BERIDEAL files
            if ~isempty(findstr(files(i).name, 'BERIDEAL_'))
                simresBERIDEAL(simrescntBERIDEAL).name = files(i).name;
                simrescntBERIDEAL = simrescntBERIDEAL + 1;
                newestfile = 0;
            end

            % sort out BER files
            if ~isempty(findstr(files(i).name, 'BER_'))
                simresBER(simrescntBER).name = files(i).name;
                simrescntBER = simrescntBER + 1;
                newestfile = 1;             
            end

            % sort out MSE files
            if ~isempty(findstr(files(i).name, 'MSE_'))
                simresMSE(simrescntMSE).name = files(i).name;
                simrescntMSE = simrescntMSE + 1;
                newestfile = 2;               
            end

            % sort out SYNC files
            if ~isempty(findstr(files(i).name, 'SYNC_'))
                simresSYNC(simrescntSYNC).name = files(i).name;
                simrescntSYNC = simrescntSYNC + 1;
                newestfile = 3;            
            end        
        end
	end

    % plot results in that order that the newest file is plotted at last so
    % that this plot is upmost
    switch newestfile
        case {0}
            plotBERresults(simresBER);
			plotMSEresults(simresMSE);
            plotSYNCresults(simresSYNC);
            plotBERIDEALresults(simresBERIDEAL);
        case {1}
			plotMSEresults(simresMSE);
            plotSYNCresults(simresSYNC);
            plotBERIDEALresults(simresBERIDEAL);
            plotBERresults(simresBER);
        case {2}
            plotSYNCresults(simresSYNC);
            plotBERIDEALresults(simresBERIDEAL);
            plotBERresults(simresBER);
			plotMSEresults(simresMSE);
        otherwise
            plotBERIDEALresults(simresBERIDEAL);
            plotBERresults(simresBER);
			plotMSEresults(simresMSE);
            plotSYNCresults(simresSYNC);
    end
return;



% *******************************************************************
% BERIDEAL results **************************************************
% *******************************************************************
function [] = plotBERIDEALresults(simresBERIDEAL)
global doremcompname;

    numdataBERIDEAL = length(simresBERIDEAL);
	if (numdataBERIDEAL > 0)
        figure;

        % load data
        for i = 1:numdataBERIDEAL
            resBERIDEAL(i).data = load(simresBERIDEAL(i).name);
        end

        % plot results
        [handles simresBERIDEAL] = plotresults(resBERIDEAL, simresBERIDEAL);    
        numdataBERIDEAL= length(simresBERIDEAL); % update length (might have changed if data was empty)

        % generate legend
        for i = 1:numdataBERIDEAL
            % remove underscores
            simresBERIDEAL(i).name = replaceunderscores(simresBERIDEAL(i).name);

            % remove ".dat"
            simresBERIDEAL(i).name = removedat(simresBERIDEAL(i).name);        

            % remove BERIDEAL
            simresBERIDEAL(i).name = removesimtype(simresBERIDEAL(i).name);             

            % remove length of simulation
            simresBERIDEAL(i).name = removelenofsim(simresBERIDEAL(i).name, 5);

            % remove computer name if requested
            if doremcompname ~= 0
                simresBERIDEAL(i).name = removecompname(simresBERIDEAL(i).name);
            end
        end

        setlegend(simresBERIDEAL, handles);

        title('Bit-Error-Rate Results (BERIDEAL)');
        xlabel('SNR [dB]');
        ylabel('BER');
	end
return;



% *******************************************************************
% BER results *******************************************************
% *******************************************************************
function [] = plotBERresults(simresBER)
global doremcompname;

	numdataBER= length(simresBER);
	if (numdataBER > 0)
        figure;

        % load data
        for i = 1:numdataBER
            resBER(i).data = load(simresBER(i).name);
        end

        % plot results
        [handles simresBER] = plotresults(resBER, simresBER);    
        numdataBER= length(simresBER); % update length (might have changed if data was empty)

        % generate legend
        for i = 1:numdataBER
            % remove underscores
            simresBER(i).name = replaceunderscores(simresBER(i).name);

            % remove ".dat"
            simresBER(i).name = removedat(simresBER(i).name);

            % remove BER
            simresBER(i).name = removesimtype(simresBER(i).name);        

            % remove length of simulation
            simresBER(i).name = removelenofsim(simresBER(i).name, 6); 

            % remove computer name if requested
            if doremcompname ~= 0
                simresBER(i).name = removecompname(simresBER(i).name);
            end        
        end

        setlegend(simresBER, handles);

        title('Bit-Error-Rate Results (BER)');
        xlabel('SNR [dB]');
        ylabel('BER');
	end
return;



% *******************************************************************
% SYNC results ******************************************************
% *******************************************************************
function [] = plotSYNCresults(simresSYNC)
global doremcompname;

	numdataSYNC= length(simresSYNC);
	if (numdataSYNC > 0)
        figure;

        % load data
        for i = 1:numdataSYNC
            resSYNC(i).data = load(simresSYNC(i).name);
        end

        % plot results
        [handles simresSYNC] = plotresults(resSYNC, simresSYNC);    
        numdataSYNC= length(simresSYNC); % update length (might have changed if data was empty)

        % generate legend
        for i = 1:numdataSYNC
            % remove underscores
            simresSYNC(i).name = replaceunderscores(simresSYNC(i).name);

            % remove ".dat"
            simresSYNC(i).name = removedat(simresSYNC(i).name);

            % remove BER
            simresSYNC(i).name = removesimtype(simresSYNC(i).name);        

            % remove length of simulation
            simresSYNC(i).name = removelenofsim(simresSYNC(i).name, 5);   

            % remove computer name if requested
            if doremcompname ~= 0
                simresSYNC(i).name = removecompname(simresSYNC(i).name);
            end        
        end

        setlegend(simresSYNC, handles);

        title('Synchronization Results');
        xlabel('SNR [dB]');
        ylabel('user defined');
	end
return;



% *******************************************************************
% MSE results *******************************************************
% *******************************************************************
function [] = plotMSEresults(simresMSE)
global doremcompname;
global plotcolors;
global LW;

    numdataMSE = length(simresMSE);
	if (numdataMSE > 0)
        figure;

        % load data
        for i = 1:numdataMSE
            resMSE(i).data = load(simresMSE(i).name).';
        end

        % plot results
        [handles simresMSE] = plotresults(resMSE, simresMSE, 1);    
        numdataMSE= length(simresMSE); % update length (might have changed if data was empty)

        % generate legend
        for i = 1:numdataMSE
            % remove underscores
            simresMSE(i).name = replaceunderscores(simresMSE(i).name);

            % remove ".dat"
            simresMSE(i).name = removedat(simresMSE(i).name);

            % remove MSE
            simresMSE(i).name = removesimtype(simresMSE(i).name);

            % remove length of simulation
            simresMSE(i).name = removelenofsim(simresMSE(i).name, 5);

            % remove computer name if requested
            if doremcompname ~= 0
                simresMSE(i).name = removecompname(simresMSE(i).name);
            end
        end

        setlegend(simresMSE, handles);

        title('MSE of Channel Estimation Results');
        xlabel('subcarrier index');
        ylabel('MSE [dB]');
	end
return;



% *******************************************************************
% Utility functions *************************************************
% function for removing underscores and replacing them by spaces
function [x] = replaceunderscores(x)
    underscoridxs = findstr(x, '_');

    % replace underscores with spaces
    x(underscoridxs) = ' ';
return;

% function for removing the ".dat" at the end of the file names
function [x] = removedat(x)
    datidx = findstr(x, '.dat');

    % remove ".dat"
    x = x(1:datidx - 1);
return;

% function for removing simulation type (at the beginning of file name)
function [x] = removesimtype(x)
    [token x] = strtok(x);

    x = x(2:end); % remove space
return;

% function for removing the length of simulation token
function [x] = removelenofsim(x, numtoken)
    [token remtoken] = strtok(x);
    if (~isempty(findstr(x, 'CH8')))
        numtoken = numtoken + 1; % for CH8 the Doppler is shown
    end

    for j = 1:numtoken
        [token remtoken] = strtok(remtoken);
    end
    
    % actual cut
    tokenidx = findstr(x, token) - 1;
    x = [x(1:tokenidx) x(tokenidx + length(token) + 2:end)];
return;

% remove computer name
function [x] = removecompname(x)
    [token remtoken] = strtok(x);
    while ~isempty(remtoken)
        [token remtoken] = strtok(remtoken);
    end
    tokenidx = findstr(x, token) - 1;
    x = [x(1:tokenidx) x(tokenidx + length(token) + 2:end)];    
return;

% plot results
function [handles, simnames] = plotresults(simres, simnames, isMSEplot)
global plotcolors;
global LW;

    % no MSE is default
    if nargin ~= 3
        isMSEplot = 0;
    end

    % remove empty data
    simresnew = struct([]);
    simnamesnew = struct([]);
    for i = 1:length(simres)
        if length(simres(i).data) > 0
            simresnew = [simresnew simres(i)];
            simnamesnew = [simnamesnew simnames(i)];
        end
    end
    simnames = simnamesnew; % make sure we get the correct labels!
    
    numdata = length(simresnew);
    handles = [];
    if (numdata > 0)
        % first curve
        if isMSEplot ~= 0
            handtmp = plot(10 * log10(simresnew(1).data), char(plotcolors(1)), 'LineWidth', LW);
            handles = handtmp(1);            
		else
			% if only one data point is available, use marker
			optstr = char(plotcolors(1));
			if length(simresnew(1).data(:, 1)) < 2
				optstr = [optstr '*'];
			end

            handles = semilogy(simresnew(1).data(:, 1), simresnew(1).data(:, 2), ...
                optstr, 'LineWidth', LW);
        end

        grid on;

        if (numdata > 1)
            % plot rest of the curves
            hold on;
            for i = 2:numdata
                if isMSEplot ~= 0
                    handtmp = plot(10 * log10(simresnew(i).data), char(plotcolors(i)), 'LineWidth', LW);
                    handles = [handles handtmp(1)];
				else
					% if only one data point is available, use marker
					optstr = char(plotcolors(i));
					if length(simresnew(i).data(:, 1)) < 2
						optstr = [optstr '*'];
					end

                    handles = [handles ...
                        semilogy(simresnew(i).data(:, 1), simresnew(i).data(:, 2), ...
                        optstr, 'LineWidth', LW)];
                end
            end
        end
    end
return;

% set legend
function [] = setlegend(simres, handles)
global figstretch;
	
	if (length(handles) > 0)
	    legend(handles, simres.name, 'Location', 'NorthEastOutside');

	    % stretch figure since we want to have the legend outside on the right
	    figpos = get(gcf, 'Position');
	    set(gcf, 'Position', [figpos(1) figpos(2) figpos(3) * figstretch figpos(4)]);
	end
return;
















