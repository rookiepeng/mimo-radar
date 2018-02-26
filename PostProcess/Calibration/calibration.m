%% clear workspace
clear;
close all;

%% import data
list= importdata('list');
list=char(list(2:65));
list=list(:,2:2:50);

importdata(list(1,:));
for ii=1:64
    data(ii,:)=importdata(list(ii,:));
end
data=data(:,32:2510);
data=data-mean(data,2);
[nr,nc]=size(data);

%% radar parameters
fs=240e3;
chirp=100; % (Hz) frequency of chirp signal
BW=250E6;
Nfft=2^20;
freq=0:fs/Nfft:fs;
c=3e8;
rr = c/(2*BW);
max_range = rr*fix(fs/chirp)/2;
weight=zeros(1,64);

range=0:max_range*2/Nfft:max_range*2-max_range*2/Nfft;

%% range profile
SpecData=fft(data.*repmat(taylorwin(nc,5,-35)',nr,1),Nfft,2);
SpecDataLog=20*log10(abs(SpecData));
figure(10);
imagesc(linspace(0,max_range*2,Nfft),1:64,SpecDataLog);
axis([0,10,0,50]);

%figure(11);
%imagesc(linspace(0,max_range*2,Nfft),1:64,angle(SpecData));

figure(12);
for ii=1:64
    plot(range, SpecDataLog(ii,:));
    hold on;
end
hold off;
axis([0,10,0,50]);

%% remove near range data
startRange=1.5;
startPoint=fix(startRange/(max_range*2/Nfft));

%% reference channel
[ampRef, locRef]=max(abs(SpecData(1,startPoint:Nfft/2)));
locRef=locRef+startPoint;
phaseRef=angle(SpecData(1,locRef))/pi*180;
if phaseRef<0
    phaseRef=phaseRef+360;
end
calSpecData(1,:)=SpecData(1,startPoint:Nfft/2)/ampRef;
weight(1)=1/ampRef;

%% calibration
shift=zeros(1,64);
for ii=2:64
    [amp, loc]=max(abs(SpecData(ii,startPoint:Nfft/2)));
    loc=loc+startPoint;
    SpecData(ii,:)=SpecData(ii,:)/amp;
    phase=angle(SpecData(ii,loc))/pi*180;
    if phase<0
        phase=phase+360;
    end
    SpecData(ii,:)=SpecData(ii,:)*exp(1i*(phaseRef-phase)/180*pi);
    diff=loc-locRef;
    shift(ii)=diff;
    calSpecData(ii,:)=SpecData(ii,startPoint+diff:Nfft/2+diff);
    weight(ii)=exp(1i*(phaseRef-phase)/180*pi)/amp;
end

calSpecDataLog=20*log10(abs(calSpecData));
figure(20);
imagesc(linspace(startRange,max_range,Nfft/2-startPoint),1:64,calSpecDataLog,[-40,0]);
axis([startRange,10,1,64]);

%% plot calibrated range profile
rangeCal=startRange:max_range*2/Nfft:max_range+max_range*2/Nfft;
figure(21);
for ii=1:64
    plot(rangeCal, calSpecDataLog(ii,:));
    hold on;
end
hold off;
axis([startRange,10,-30,0]);

figure(22);
for ii=1:64
    plot(rangeCal, (angle(calSpecData(ii,:)))/pi*180);
    hold on;
end
hold off;
axis([startRange,10,-180,180]);

%% results
% weight: phase and amplitude calibration value
% shift: range calibration value
