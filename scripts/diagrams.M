cpu = [ 4.34; 9.51; 16.02; 74.17; 172.9; 314.46; 1222.4; 2960.49; 4616.09; ];
gpu = [ 0.14; 0.18; 0.23; 0.37; 1.14; 2.75; 5.52; 39.23; 122.25; ];
gpu_scaled = gpu * 10
titles = ["ImageSize: 64x64 Patchsize:3"; "ImageSize: 64x64 Patchsize:5"; "ImageSize: 64x64 Patchsize:7"; "ImageSize: 128x128 Patchsize:3"; "ImageSize: 128x128 Patchsize:5"; "ImageSize: 128x128 Patchsize:7"; "ImageSize: 256x256 Patchsize:3"; "ImageSize: 256x256 Patchsize:5"; "ImageSize: 256x256 Patchsize:7"; ]
combined = [];
name = {"cpu time"; "gpu time"; "cpu time (right axis)"};


for i=1:9
    counter = 1;
    combined(i, counter) = cpu(i);
    counter = 2;
    combined(i, counter) = gpu_scaled(i);
end

for i=1:9
    figure
    yyaxis left
    ylabel('Seconds')
    bar(combined(i,:), 'm')
    set(gca,'xticklabel', {'cpu time','gpu time(scaled x10)'}) 
    title(titles(i)); 
    hold;   
    yyaxis right
    ylabel('Seconds')
    bar(3, gpu(i), 'g');
end

% Speedup
speedup_x_axis = {'64-3', '64-5', '64-7', '128-3', '128-5', '128-7', '256-3', '256-5', '256-7'};

speedup = cpu ./ gpu;


bar(speedup, 'r');
title('Speedup');
set(gca,'xticklabel', speedup_x_axis);
