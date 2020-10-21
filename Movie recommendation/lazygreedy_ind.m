function [T_lazy,rholazy] = lazygreedy_ind(T_lazy,rholazy,matrix_user_rate,genremoviescollect,g,users,user_weight)
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
%value of maximizer element
max_value=0;
num_mov=1995;
%available_ind=ones(num_mov,1);
%available_ind(T_lazy)=0;
[sz1 ~]=size(T_lazy);

% lazy greedy
% optfind=1 if we find the element which maximizes the marginal f(.|S^k)
optfind=0;
% valid indecis
%ind_marginal_lazy=logical(available_ind);
%     ind_marginal_lazy=ismember(1:num_mov,T_lazy);
%     ind_marginal_lazy=~ind_marginal_lazy;
%  rholazy(T_lazy)=0;
ind_marginal_lazy=setdiff(1:num_mov,  T_lazy);
while optfind==0
    %value of maximizer element :maxvalue%%%
    %  maximizer element :b 'th  element of  rholazy(ind_marginal_lazy)
    %  maximizer element :max_ind 'th  element of  dataset
    variable=rholazy(ind_marginal_lazy');
    %variable=rholazy;
    [max_value,index]= max(variable);
    
    if max_value==0
        %S^k is locally optimal
        optfind=1;
    end
    max_ind=ind_marginal_lazy(index);
    
    if sz1==0
        % function value for maxind
        f=f_total_ind(matrix_user_rate,genremoviescollect,g,max_ind,users,user_weight);
        
    else
        % function value for maxind
        f =f_total_ind(matrix_user_rate,genremoviescollect,g,[max_ind;T_lazy],users,user_weight)-f_total_ind(matrix_user_rate,genremoviescollect,g,T_lazy,users,user_weight);
        % updating marginal rho
        rholazy(max_ind)=f;
    end
    % condition that force maxind be a maximizer of f(.|S^k)
    
    
    if (f>=max( variable))==1
        optfind=1;
    end
end
% finding indces and the set
T_lazy=[max_ind;T_lazy];

