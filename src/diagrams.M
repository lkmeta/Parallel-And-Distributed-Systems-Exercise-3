cpu = [ 6.57; 12.61; 20.97; 111.28; 240.43; ];
gpu = [ 0.14; 0.18; 0.23; 0.37; 1.14; ];
gpu_scaled = gpu * 10
titles = ["ImageSize: 64x64 Patchsize:3"; "ImageSize: 64x64 Patchsize:5"; "ImageSize: 64x64 Patchsize:7"; "ImageSize: 128x128 Patchsize:3"; "ImageSize: 128x128 Patchsize:5"; ]
combined = [];
name = {"cpu time"; "gpu time"; "cpu time (right axis)"};


for i=1:5
    counter = 1;
    combined(i, counter) = cpu(i);
    counter = 2;
    combined(i, counter) = gpu_scaled(i);
end

for i=1:5
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
speedup_x_axis = {'64-3', '64-5', '64-7', '128-3', '128-5'};

speedup = cpu ./ gpu;


bar(speedup, 'r');
title('Speedup');
set(gca,'xticklabel', speedup_x_axis);