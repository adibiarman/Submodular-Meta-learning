function [f_total_T] =f_total_ind(matrix_user_rate,genremoviescollect,g,T_1tom,a_1tom,weight)
%||||||||f_total=sum_{i \in m} f_i(T)|||||||||||||| [f_total_T] =f_total_ind(g,T_1tom,a_1tom,matrix_user_rate,genremoviescollect)
%   Detailed explanation goes here
f_total_T=0;
m=length(a_1tom);
[~, sz1]=size(T_1tom);
if sz1==1
    for i=1:m
        % summation of f_i( T)
        f_total_T=f_total_T+ f_a_T_ind(a_1tom(i),g,T_1tom,matrix_user_rate,genremoviescollect,weight);
    end
else
    for i=1:m
        % summation of f_i(T_i)
        f_total_T=f_total_T+ f_a_T_ind(a_1tom(i),g,T_1tom(:,i),matrix_user_rate,genremoviescollect,weight);
    end
end
