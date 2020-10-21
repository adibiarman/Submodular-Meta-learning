function createfiguretwostage(X1, YMatrix1)
%CREATEFIGURE(X1, YMatrix1)
%  X1:  vector of x data
%  YMATRIX1:  matrix of y data

%  Auto-generated by MATLAB on 10-Jun-2020 21:12:17

% Create figure
figure1 = figure;

% Create axes
axes1 = axes('Parent',figure1);
hold(axes1,'on');

% Create multiple lines using matrix input to plot
plot1 = plot(X1,YMatrix1,'MarkerSize',15,'LineWidth',3,'Parent',axes1);
set(plot1(1),'DisplayName','Greedy-Train','Marker','x');
set(plot1(2),'DisplayName','Random','MarkerSize',20,'Marker','+',...
    'Color',[1 0.600000023841858 0.7843137383461]);
set(plot1(3),'DisplayName','Meta-Greedy','Marker','o','Color',[1 0 0]);
set(plot1(4),'DisplayName','Randomized Meta-Greedy','Marker','*');
set(plot1(5),'DisplayName','Greedy-Test','Marker','square',...
    'Color',[0.749019622802734 0 0.749019622802734]);
set(plot1(6),'DisplayName','Replacment Greedy','Marker','hexagram');

% Create ylabel
ylabel({'Function Value'});

% Create xlabel
xlabel({'k'});

box(axes1,'on');
grid(axes1,'on');
% Set the remaining axes properties
set(axes1,'FontName','Times New Roman','FontSize',20);
% Create legend
legend1 = legend(axes1,'show');
set(legend1,...
    'Position',[0.3974868087407 0.134057971014493 0.456211216641828 0.322463768115942],...
    'Interpreter','latex',...
    'FontSize',30);

