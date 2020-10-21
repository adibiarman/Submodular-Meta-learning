function [f_a_T] = f_a_T_ind(a,g,T,matrix_user_rate,genremoviescollect,weight)
%f_a(T)= sum_{j in1 to g}w_{i,j} R(a,T,j)

f_a_T=0;

for j=1:g
% f_a = sum_{j in1 to g}w_{i,j} R(a,T,j)
%R(i,T,j): represent the highest rating given by user a
%to a movie from the set T with genre j
%set T with genre j=T(genremoviescollect(T,j)==1)
setgenrej=T(genremoviescollect(nonzeros(T),j)==1);
if(length(T)==1)

var1=(matrix_user_rate(a,setgenrej));
else
    var1=max(matrix_user_rate(a,setgenrej));
end
%var1=max(matrix_user_rate(a,T(genremoviescollect(T,j)==1)));
if(var1>0)
f_a_T=f_a_T+weight(a,j)*var1;
end
end

end