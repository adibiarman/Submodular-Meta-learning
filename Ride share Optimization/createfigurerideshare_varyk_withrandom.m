function createfigurerideshare_varyk_withrandom(X1, YMatrix1)
%CREATEFIGURE(X1, YMatrix1)
%  X1:  vector of x data
%  YMATRIX1:  matrix of y data

%  Auto-generated by MATLAB on 01-Jun-2020 15:12:02

% Create figure
figure1 = figure;

% Create axes
axes1 = axes('Parent',figure1);
hold(axes1,'on');

% Create multiple lines using matrix input to plot
plot1 = plot(X1,YMatrix1,'MarkerSize',15,'LineWidth',3,'Parent',axes1);
set(plot1(1),'DisplayName','train Greedy','Marker','x');
set(plot1(2),'DisplayName','random set','Marker','+',...
    'Color',[1 0.600000023841858 0.7843137383461]);
set(plot1(3),'DisplayName','meta Greedy alg2','Marker','o','Color',[1 0 0]);
set(plot1(4),'DisplayName','Random meta-Greedy','Marker','*');
set(plot1(5),'DisplayName','Test Greedy','Marker','square',...
    'Color',[0.600000023841858 0 0.600000023841858]);
set(plot1(6),'DisplayName','meta Greedy alg1','Marker','.');

% Create ylabel
ylabel('Objective value');

% Create xlabel
xlabel('k');

box(axes1,'on');
grid(axes1,'on');
% Set the remaining axes properties
set(axes1,'FontName','Times New Roman','FontSize',20);
% Create legend
legend1 = legend(axes1,'show');
set(legend1,...
    'Position',[0.560013262599469 0.172970479704797 0.265915119363395 0.238929889298893]);

