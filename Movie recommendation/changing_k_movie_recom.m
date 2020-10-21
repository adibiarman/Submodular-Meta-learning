clear all;
clc;
load('Summarydata.mat')

matrix_user_rate=sparse(matrix_user_rate);
% genremoviscollect: 1995*18 matrix each row i represent the indicator of
% film i genres.
%genrestr: 18 genres
%indexinvuser: vector that map user1:200 to their userid e.g.
%indexinvuser(1<ind<200)= 1<i<138325
%indinvmovie: vector that map movie1:1995 to their movieid e.g.
%indinvmovie(1<ind<1995)= 1<i<127098
%matrix_user_rate: each movie is coulmn, each row is user, each element(i,j) is
%rate for j th movie by ith user
%movie_collection: movie ids
%sum_in_genre:num of movie in each genre
%user_weight: weights that each user have in each genres w_{i,j}
%userindex:userid of each user 1 to 200


%%%%%%%%%*************%%%%%%%%% training
%%
%%%%%%%%%%%%%%%%%%%%%%%initialization of training
% # trainig size:= m
m=100;
% #pick number of  points in sol:= k

% #pick number of  points in sol in training of greedy:= l

g=18;
%  the random positions  rand_pos of training
%ground set selection groundset
sz_of_gnd_set=1995;
%groundset=randi([1 100000],1,sz_of_gnd_set);
groundset=1:sz_of_gnd_set;
num_user_train=m;
user_train=100:199;
    numtest=100;
%%%%%%%%%%%%%%%%%%%%%%%%
%%
%%%%%%%%%%%%%%%%%%%%%%%%%meta learning training
%meta learn. alg. sol. in train. T_meta
diff_k_l=0.8;
setofk=[5 10 15 20 25 30]
len_difk=length(setofk)
for ind_k=1:length(setofk)
  
        %%
    % changing k
    k=setofk(ind_k)
    l=round(diff_k_l*k);
      %%
    % changing l
    % k=15;
    %  l=setofk(ind_k)
    clear rhoalg2 indices_alg2 ind_marginal_alg2 max_value_alg2 b_alg2 T_alg2 J_test
    %  clear T_meta_train rho rhometa T_meta_train rhoopt T_alg2 rhoalg22 rhotest T_obvi_test
    T_meta_train=[];
    %%
    parfor  b= 1:sz_of_gnd_set
        
        %marginals rho
        rho(b)=f_total_ind(matrix_user_rate,genremoviescollect,g,[b],user_train,user_weight);
    end
    rho=sparse(rho);
    %meta learning training
    % margina of meta learning rhometa
    rhometa=rho;
    for  t=1:l
        t
        [T_meta_train,rhometa] = lazygreedy_ind(T_meta_train,rhometa,matrix_user_rate,genremoviescollect,g,user_train,user_weight);
    end
    %%%%%%%%%%%%%%%%%%%%%%%%%
      %%%%%%%%%%%%%%%%%%%%%%
    %%
    
    
    S_final=[];
    max_val=0
    randval= rand;
    h=1;
    hph1=1;
    hph2=1;
    while((hph1<k-l+1)&&(hph2<l+1))
        h
            randval= rand
        if (randval>=l/k)
            parfor t = 1:m
                max_vals(t) = 0;
            end
            for i=1:sz_of_gnd_set
                
               for t=1:m
                    if  length(S_final)==0
                        vali(t,i) =f_a_T_ind(user_train(t),g,[i],matrix_user_rate,genremoviescollect,user_weight);;
                    else
                    vali(t,i) =f_a_T_ind(user_train(t),g,[S_final(t,:)';i],matrix_user_rate,genremoviescollect,user_weight);
                    end
                    marginals_personal(t,i) = vali(t,i);
                end
            end
            clear i t
            
            parfor indt=1:m
                [max_vals(indt),best_items(indt) ]= max(vali(indt,:));
                S_final(indt,h) = best_items(indt);
            end
            h=h+1;
            h
            hph1=hph1+1;
        end
        %%
        if (randval<(l)/k)
            %//updating the set S_tr in phase 2
      
            
            for i=1:sz_of_gnd_set
                valii (i)= 0;
                
                      A_temp=S_final;
                    A_temp(:,h)= i*ones(m,1);
           
                    valii (i) = f_total_ind(matrix_user_rate,genremoviescollect,g,A_temp',user_train,user_weight);
     
            end
            marginals = valii;
            
            [max_val,best_item ]= max(valii);
    
                S_final(:,h) = best_item*ones(m,1);

            S_randomized(hph2) = best_item;
            
            h=h+1;
            h
            hph2=hph2+1;
        end
    end
   
    while(hph2<l+1)
        
        
        %//updating the set S_tr in phase 2
       parfor i=1:sz_of_gnd_set
            valii (i)= 0;
            A_temp=S_final;
                              A_temp(:,h)= i*ones(m,1);
                valii (i) = f_total_ind(matrix_user_rate,genremoviescollect,g,A_temp',user_train,user_weight);
        
        end
        marginals = valii;
        
        [max_val,best_item ]= max(valii);

            S_final(:,h) = best_item*ones(m,1);
                          
        S_randomized(hph2) = best_item;
        
        h=h+1;
        h
       hph2=hph2+1;
    end
%      while(hph1<k-l+1)
%         h
% 
%             max_vals= zeros(m,1);
% 
%         for(i=1:sz_of_gnd_set)
%             for ti=1:m
%                 
%                 vali(ti,i) =f_a_T_ind(user_train(ti),g,[S_final(ti,:)';i],matrix_user_rate,genremoviescollect,user_weight);
%                 
%                 marginals_personal(ti,i) = vali(ti,i);
%             end
%         end
%         
%         for tii=1:m
%             [max_vals(tii) ,best_items(tii)]= max(vali(tii,:));
%             S_final(tii,h) = best_items(tii);
%         end
%         h=h+1;
%         h
%         hph1=hph1+1;
%     end
    %%
    %%%%%%%%%%%%%%%%%%%%%%%%% training greedy on sum
    T=[];
    rhoopt=rho;
    
    for  t=1:k
        t
        %lazy greedy for greedy on sum
        [T,rhoopt] = lazygreedy_ind(T,rhoopt,matrix_user_rate,genremoviescollect,g,user_train,user_weight);
        % obviii_final(t)=f_total_ind(matrix_user_rate,genremoviescollect,g,T,user_train,user_weight);
    end
    %%%%%%%%%%%%%%%%%%%%%%
    %%
    %%%%%%%%%%%%%%%%%%%%%%%%alg2 alg training
    
    %for this part first we rum greedy for k-l on each f_i and then compute
    %the remaining set S
    parfor j=1:m
        rhoalg2{j}=0;
        % feasible for each f_i
        ind_marginal_alg2{j}=[];
        % variable to make everything parallel
        indices_alg2{j}=[];
        aa{j}=0;
    end
    % init of S
    T_alg2=zeros(k,m);
    for  t=1:k-l
        t
        
        for j=1:m
            max_indd{j}=0;
            A=zeros(size(groundset));
            parfor  b= groundset
                % computing marginals
                A(b)=f_a_T_ind(user_train(j),g,[b],matrix_user_rate,genremoviescollect,user_weight);
            end
            rhoalg2{j}=A;
        end
        for j=1:m
            % feasible inds
            optfind=0;
            ind_marginal_alg2{j}=setdiff(groundset, T_alg2);
            while optfind==0
                %value of maximizer element :maxvalue%%%
                %  maximizer element :b 'th  element of  rhometa(ind_marginal_meta)
                %  maximizer element :max_ind 'th  element of  ubersep14array
                [max_value_alg2{j},b_alg2{j}]= max(rhoalg2{j}(ind_marginal_alg2{j}));
                max_indd{j}=ind_marginal_alg2{j}(b_alg2{j});
                if max_value_alg2{j}==0
                    optfind=1;
                end
                if t==1
                    % function value for maxind
                    f=   f_a_T_ind(user_train(j),g,[max_indd{j}],matrix_user_rate,genremoviescollect,user_weight);
                else
                    % function value for maxind
                    f=f_a_T_ind (user_train(j),g,([max_indd{j};T_alg2(:,j)]),matrix_user_rate,genremoviescollect,user_weight)-  f_a_T_ind (user_train(j),g,(T_alg2(:,j)),matrix_user_rate,genremoviescollect,user_weight);
                    % updating marginal rho
                    rhoalg2{j}(max_indd{j})=f;
                end
                % condition that force maxind be a maximizer of f(.|S^k)
                if sum(1-(f>= rhoalg2{j}(ind_marginal_alg2{j})))==0
                    optfind=1;
                end
            end
            % finding indces and the set
            indices_alg2{j}=[max_indd{j} ,  indices_alg2{j}];
            aa{j}=max_indd{j};
        end
        for j=1:m
            T_alg2(t,j)=aa{j};
        end
    end
    %%%%%%%%%%%%%%%%%%%%%%%%
    %
    indalg22=[];
    %
 parfor  b= groundset
        % marginals
        J_test=T_alg2;
        
        J_test(k-l+1,:)=( b.*ones(m,1))';
        rhoalg22(b)=f_total_ind(matrix_user_rate,genremoviescollect,g, (J_test),user_train,user_weight)-f_total_ind(matrix_user_rate,genremoviescollect,g, (T_alg2),user_train,user_weight);
        
    end
    rhoalg22=sparse(rhoalg22);
    for t=k-l+1:k
        t
        optfind=0;
        ind_marginal_alg22=setdiff(groundset,  indalg22);
        while optfind==0
            
            [max_value_alg2,b]= max(rhoalg22( ind_marginal_alg22));
            b
            J_test=T_alg2;
            
            max_inddi= ind_marginal_alg22(b);
            J_test(t,:)=(ind_marginal_alg22(b).*ones(m,1))';
            
            max_inddi= ind_marginal_alg22(b);
            
            if max_value_alg2==0
                optfind=1;
            end
            
            if t==1
                f=   f_total_ind(matrix_user_rate,genremoviescollect,g, (J_test),user_train,user_weight);
            else
                f=f_total_ind(matrix_user_rate,genremoviescollect,g, (J_test),user_train,user_weight)- f_total_ind(matrix_user_rate,genremoviescollect,g, (T_alg2),user_train,user_weight);
                rhoalg22(max_inddi)=f;
            end
            if sum(1-(f>= rhoalg22(ind_marginal_alg22)))==0
                optfind=1;
            end
            
        end
        T_alg2(t,:)=( max_inddi.*ones(m,1))';
        indalg22=[indalg22;max_inddi];
    end
    T_alg2_train=indalg22;
    
    %%%%%%%********%%%%%%%%%%%
    %
    %%%%%***********%%%%%%%%%%%%intializition of test
    for ind_test=1:numtest
        user_test=ind_test:ind_test+4;
        
        
        parfor  b= 1:sz_of_gnd_set
            % computing marginals
            rhotest(b)=f_total_ind(matrix_user_rate,genremoviescollect,g,[b],user_test,user_weight);
        end
        %%%%%%%%%%%%%%%%%%%%%%%
        
        %%%%%%%%%%%%%%%%%%%%%%%%%metagreedy in test
        tic;
        % updating T_meta_test with T_meta training
        T_meta_test=T_meta_train;
        parfor  b=  1:sz_of_gnd_set
            %   computing marginal
            %  we need to recompute marginal because of the intialization of T_meta_test is T_meta.
            rhometatest(b)= f_total_ind(matrix_user_rate,genremoviescollect,g,[b; T_meta_test],user_test,user_weight)-f_total_ind(matrix_user_rate,genremoviescollect,g,T_meta_test,user_test,user_weight);
        end
        rhometatest=sparse(rhometatest);
        for  t=1:k-l
            t
            [T_meta_test,rhometatest] = lazygreedy_ind(T_meta_test,rhometatest,matrix_user_rate,genremoviescollect,g,user_test,user_weight);
            f_meta(t,ind_test)=f_total_ind(matrix_user_rate,genremoviescollect,g,T_meta_test,user_test,user_weight);
        end
        time_meta(ind_test)=toc;
        %%%%%%%%%%%%%%%%%%%%%%
         %%%%%%%%%%%%%%%%%%%%%%%%%rand metagreedy in test
         %%
        tic;
        % updating T_meta_test with T_meta training
        T_rand_test=S_randomized';
        parfor  b=  1:sz_of_gnd_set
            %   computing marginal
            %  we need to recompute marginal because of the intialization of T_meta_test is T_meta.
            rhorandtest(b)= f_total_ind(matrix_user_rate,genremoviescollect,g,[b; T_rand_test],user_test,user_weight)-f_total_ind(matrix_user_rate,genremoviescollect,g,T_rand_test,user_test,user_weight);
        end
        rhorandtest=sparse(rhorandtest);
        for  t=1:k-l
            t
            [T_rand_test,rhorandtest] = lazygreedy_ind(T_rand_test,rhorandtest,matrix_user_rate,genremoviescollect,g,user_test,user_weight);
            f_random(t,ind_test)=f_total_ind(matrix_user_rate,genremoviescollect,g,T_rand_test,user_test,user_weight);
        end
        time_rand(ind_test)=toc;
        %%%%%%%%%%%%%%%%%%%%%%
        %%%%%%%%%%%%%%%%%%%%%%%%%alg2 in test
        tic;
        T_alg2_test=(T_alg2_train);
        %  T_alg2_test=[];
        indices_alg2= indalg22;
        J_alg2_test=T_alg2_test;
        parfor  b= groundset
            rhoalg2test(b)=f_total_ind(matrix_user_rate,genremoviescollect,g,[b; T_alg2_test],user_test,user_weight)-f_total_ind(matrix_user_rate,genremoviescollect,g,T_alg2_test,user_test,user_weight);
        end
        rhoalg2test=sparse(rhoalg2test);
        for  t=1:k-l
            t
            [T_alg2_test,rhoalg2test] = lazygreedy_ind(T_alg2_test,rhoalg2test,matrix_user_rate,genremoviescollect,g,user_test,user_weight);
            
            f_alg2(t,ind_test)=f_total_ind(matrix_user_rate,genremoviescollect,g,T_alg2_test,user_test,user_weight);
        end
        time_alg2(ind_test)=toc;
        %  T_alg2_test=[T_alg2_train;T_alg2_test];
        %%%%%%%%%%%%%%%%%%%%%%
        
        %%%%%%%%%%%%%%%%%%%%%%% greedy in test
        tic
        T_opt_test=[];
        rhoopttest=rhotest;
        for  t=1:k
            t
            % lazy greedy in test time
            [T_opt_test,rhoopttest] = lazygreedy_ind(T_opt_test,rhoopttest,matrix_user_rate,genremoviescollect,g,user_test,user_weight);
            %   f_opt(t,ind_test)=f_total_ind(matrix_user_rate,genremoviescollect,g,T_opt_test,user_test,user_weight);
            
        end
        time_opt(ind_test)=toc;
        %%%%%%%%%%%%%%%%%%%%%%%
        T_obvi_test=[T];
        
        f_final_obvi(ind_k,ind_test)= f_total_ind(matrix_user_rate,genremoviescollect,g,T_obvi_test,user_test,user_weight);
        f_meta1(ind_k,ind_test)= f_total_ind(matrix_user_rate,genremoviescollect,g,T_meta_test,user_test,user_weight);
        f_alg21(ind_k,ind_test)= f_total_ind(matrix_user_rate,genremoviescollect,g,T_alg2_test,user_test,user_weight);
        f_opt11(ind_k,ind_test)= f_total_ind(matrix_user_rate,genremoviescollect,g,T_opt_test,user_test,user_weight);
                f_rand(ind_k,ind_test)= f_total_ind(matrix_user_rate,genremoviescollect,g,T_rand_test,user_test,user_weight);
                f_randomset(ind_k,ind_test)= f_total_ind(matrix_user_rate,genremoviescollect,g,1:k,user_test,user_weight);

%                  for i=1:k
%                 f_final_obvi(i,ind_test)= f_total_ind(matrix_user_rate,genremoviescollect,g,T_obvi_test(k-i+1:k),user_test,user_weight);
%                 f_meta1(i,ind_test)= f_total_ind(matrix_user_rate,genremoviescollect,g,T_meta_test(k-i+1:k),user_test,user_weight);
%                 f_alg21(i,ind_test)= f_total_ind(matrix_user_rate,genremoviescollect,g,T_alg2_test(k-i+1:k),user_test,user_weight);
%                         f_opt11(i,ind_test)= f_total_ind(matrix_user_rate,genremoviescollect,g,T_opt_test(k-i+1:k),user_test,user_weight);
%                         f_rand(i,ind_test)= f_total_ind(matrix_user_rate,genremoviescollect,g,T_rand_test,user_test,user_weight);
% 
%             end
        
    end
end
%%%%%%%%%*********%%%%%%%%%%
%%%%%%%%%*********%%%%%%%%%%plots
%%
time_greedy_ave=sum(time_opt)/numtest;
time_meta_ave=sum(time_meta)/numtest;
time_alg2_ave=sum(time_alg2)/numtest;

meta_final=zeros(len_difk,1);
alg2_final=zeros(len_difk,1);
algrand_final=zeros(len_difk,1);

traingreedy_final=zeros(len_difk,1);
greedy_final=zeros(len_difk,1);
rndset_final=zeros(len_difk,1);

%averaging
for i=1:numtest
    meta_final=f_meta1(:,i)/numtest+meta_final;
    alg2_final=f_alg21(:,i)/numtest+alg2_final;
        algrand_final=f_rand(:,i)/numtest+algrand_final;
    traingreedy_final=f_final_obvi(:,i)/numtest+traingreedy_final;
    greedy_final=f_opt11(:,i)/numtest+greedy_final;
        rndset_final=f_randomset(:,i)/numtest+rndset_final;

end
%figure
createfigureforvaryingk(setofk, [meta_final,alg2_final,traingreedy_final,greedy_final,algrand_final]/25)

