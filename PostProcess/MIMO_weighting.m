%%
%  Calculate weightings for the MIMO radar
%
%  Version 1
%  Zhengyu Peng
%%

clear;

%% Configuration
wavelength=1;
k=2*pi/wavelength;   % wave number
angleStep=0.05;
theta=0:angleStep:360;
elementNumber=8;
spacingMIN=0.5; % minimum space between two elements
spacingMAX=3.5;   % maximum space between two elements
mainbeam=18;     % width of the main lobe

count=1;
for ii=-45:2.5:45
    %thetaM=-45;      % location of the main lobe
    thetaM=90+ii;
    
    d=[0, 14.5838, 20.8875, 29.1431, 42.1765, 49.8930, 71.9496, 85.1416];
    location=d;
    d=d/12.4;
    
    figure(1);
    plot(d,zeros(1,length(d)),'x');
    axis([0,d(length(d)),-1,1]);
    
    %% Array factor
    A=zeros(length(theta),elementNumber);
    for nn=1:length(d)
        A(:,nn)=exp(1i*k*d(nn)*cosd(theta));
    end
    
    %% Main lobe
    A_M=zeros(1,elementNumber);
    for nn=1:length(d)
        A_M(:,nn)=exp(1i*k*d(nn)*cosd(thetaM));
    end
    
    %% Side lobe
    theta_SL=[0:angleStep:thetaM-mainbeam/2,thetaM+mainbeam/2:angleStep:180];
    %theta_ML=90+mainbeam/2:angleStep:180;
    
    A_SL=zeros(length(theta_SL),elementNumber);
    for nn=1:length(d)
        A_SL(:,nn)=exp(1i*k*d(nn)*cosd(theta_SL));
    end
    
    %% Optimization
    cvx_begin %quiet
    variable w(elementNumber) complex
    minimize( max(abs(A_SL*w)) )
    subject to
    A_M*w==1;
    cvx_end
    
    %% Plot result
    %w=ones(1,elementNumber)';
    figure(2);
    plot(theta,20*log10(abs(A*w))-max(20*log10(abs(A*w))));
    axis([0,180,-30,0]);
    
    weightMatrix(count,:)=w.';
    count=count+1;
end

% for ii=1:16
%     tsv(ii,:)=[ii,location(ii)/1000,0,0,abs(w(ii)), -angle(w(ii))/pi*180,0,0,0];
% end