
clear all;
close all;
clc;

%% generate bch polynome 
% parameter for PDF1
N1 = 127;   % 82
K1 = 106;   % 61
gen_poly1 = bchgenpoly(N1,K1);

% parameter for PDF2
N2 = 63;    % 
K2 = 51;    % 
gen_poly2 = bchgenpoly(N2,K2);

%% iq parameter to modulate
A = 511;
offset = 512;

IQ1 = dec2hex(round(cosd(45)*A + offset));
IQ0 = dec2hex(round(cosd(135)*A + offset));

%% PDF1 with BCH
%bits for sync from 1 - 25
sync = [1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 0 0 1 0 1 1 1 1];

pdf1 = [1 1 0 0 1 1 0 0 1 0 1 1 1 1 1 1 1 0 1 1 1 0 1 0 0 0 0 1 1 1 0 1 1 1 0 0 1 0 1 ...
   0 0 0 0 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 ...
   0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0];

pdf1_to_encode = gf(pdf1);
pdf1withbch = bchenc(pdf1_to_encode,N1,K1);

pdf1_encode = pdf1withbch([1:61, 107:end])

% place here bits from c code
pdf1_from_c = [pdf1_encode];

if pdf1_encode == pdf1_from_c
   'true'
else 
   'false'
end

%% PDF2 with BCH
degree_lat = dec2bin(48,7)
%degree_lat = decimalToBinaryVector(insert_here_your_decimal_value,7)    %only for windows ;(
minutes_lat = dec2bin(22/4,4)
%minutes_lat = decimalToBinaryVector(insert_here_your_decimal_value/4,4)
degree_lon = dec2bin(14,8)
%degree_lat = decimalToBinaryVector(insert_here_your_decimal_value,7)    %only for windows ;(
minutes_lon = dec2bin(30/4,4)
%minutes_lat = decimalToBinaryVector(insert_here_your_decimal_value/4,4)

latitude_flag = [0]; % for N->0 for S->1
latitude_degrees = [0 1 1 0 0 0 0];
latitude_minutes = [0 1 0 1];
longtitude_flag = [0];  % for E->0 for W->1
longtitude_degrees = [0 0 0 0 1 1 1 0];
longtitude_minutes = [0 1 1 1];

zeros = [0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0];

pdf2 = [1 latitude_flag latitude_degrees latitude_minutes longtitude_flag longtitude_degrees longtitude_minutes zeros];

pdf2_to_encode = gf(pdf2);
pdf2withbch = bchenc(pdf2_to_encode,N2,K2);

pdf2_encode = pdf2withbch([1:26, 52:end])

% place here bits from c code
pdf2_from_c = [pdf2_encode];

if pdf2_encode == pdf2_from_c
   'true'
else 
   'false'
end

%% Message ready to modualte
msg = [sync pdf1_encode pdf2_encode]

% place here bits from c code
msg_from_c = [msg];

if msg == msg_from_c
   'true'
else 
   'false'
end


%% Modulation

msg_to_send=[];

[msg_size_o, msg_size_i] = size(msg);
for (odx=1:msg_size_o)
    for (idx = 1:msg_size_i)
        if msg(odx,idx) == 1
            iq_val=IQ1;
        elseif msg(odx,idx) == 0
            iq_val=IQ0;
        end
        msg_to_send=[msg_to_send iq_val];
    end
end

%disp(msg_to_send)
  