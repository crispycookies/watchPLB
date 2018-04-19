f_TX = 406e6; %Hz  transmit frequency
c0=3e8; %m/s   speed of light

lambda_TX = c0 / f_TX; %m    transmit wave length

P_TX_log = 34; %dBm  transmit power
                % https://cospas-sarsat.int/en/documents-pro/system-documents 


P_TX_lin = 10^(P_TX_log/10)*0.001 % W    transmit power

G_TX_log = 3; % dBi antenna gain
G_TX_lin = 10^(G_TX_log/10); 


sigma_SAT = 1; % effective radar cross section of satelite antenna

% https://en.wikipedia.org/wiki/Parabolic_antenna

G_RX_log = 80; %dBi (assumed from Wiki Doku)

Range_GeoStat = 36000*1e3; % m; geo stationary satellite distance 36000km
incident_angle = 5; % degrees incident angle from earth to satellite

R = 1/cos(5/180*pi) * Range_GeoStat  % m range the transmitter must be able to overcome


P_RX_lin = (P_TX_lin * G_TX_lin * sigma_SAT * lambda_TX^2) / (4*pi*R^2 * 4 * pi)    % W power received at satellite

P_RX_log = 10*log10(P_RX_lin / 0.001)   %dBm received @ satellite