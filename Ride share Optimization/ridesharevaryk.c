#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>
#include "random.h"
#include "rngs.h"
#include "rvgs.h"



// define which random number generator to use
#define RNGS   1
//#undef RNGS
//#define RAND   1
#undef RAND
#define STAT 0
#define DELTA 0:0.01
#define LEVEL1 1
#define LEVEL2 1
#undef CHECKNAN
#define MAXCHAR 1000
#define pi 3.1415

int li,ki, n,m,res, points_per_task;

double* locations_lon;
double* locations_lat;

double* ground_lon;
double* ground_lat;

int* S_greedy_train;
int* S_alg1;
int* S_alg2;
int* S_randomized;
int** S_personal;
int** S_final;
double* marginals;
double** marginals_personal;



struct task{
    int center;
    double* lons;
    double* lats;
};




int sign(double v)
{
return v > 0 ? 1 : (v < 0 ? -1 : 0);
}



double max(double A, double B)
{
return (A>B)?A:B;
}


double RVuniform(void)
{
    return ((random() + 0.5) / (1.0 + RAND_MAX));
}

int randi(int n){
    return floor(n*RVuniform());
}


void *allocate(int n)
{
    void *p;

    if ((p = malloc(n)) == NULL)
    {
        printf("/***********************************/");
        printf("/*    W   A   R   N   I   N   G    */");
        printf("/***********************************/");
        printf("/*      Not sufficient memory!     */");
        printf("/***********************************/");
        exit(1);
    }

    return p;
}








//****** Reading the file
int read_file()
{

    char command[500];
    char str[MAXCHAR];
    FILE *fp;


    char *ptr;
    double ret;
    char newString[2][10];

    int i,j,l,r,t,T,ctr;
    T = 0;
        fp=fopen("col1.txt", "r");
        while (fgets(str, MAXCHAR, fp) != NULL)
        {

               j=0; ctr=0;
               for(i=0;i<=(strlen(str));i++)
               {
                   // if space or NULL found, assign NULL into newString[ctr]
                   if(str[i]==' '||str[i]=='\0')
                   {
                       newString[ctr][j]='\0';
                       ctr++;  //for next word
                       j=0;    //for next word, init index to 0
                   }
                   else
                   {
                       newString[ctr][j]=str[i];
                       j++;
                   }
               }
            sscanf(newString[0], "%lf", &ret);
            locations_lat[T] = ret;

            T++;

        }
        fclose(fp);
        T = 0;
        fp=fopen("col2.txt", "r");
        while (fgets(str, MAXCHAR, fp) != NULL)
        {

               j=0; ctr=0;
               for(i=0;i<=(strlen(str));i++)
               {
                   // if space or NULL found, assign NULL into newString[ctr]
                   if(str[i]==' '||str[i]=='\0')
                   {
                       newString[ctr][j]='\0';
                       ctr++;  //for next word
                       j=0;    //for next word, init index to 0
                   }
                   else
                   {
                       newString[ctr][j]=str[i];
                       j++;
                   }
               }
            sscanf(newString[0], "%lf", &ret);
            locations_lon[T] = ret;

            T++;

        }

        fclose(fp);

    return 0;
    //**********************Finished reading the file
}

double distance_convert(double lon1, double lat1, double lon2, double lat2)
{
    double distance;
    //radius=6371;
    lat1=lat1*pi/180;
    lat2=lat2*pi/180;
    lon1=lon1*pi/180;
    lon2=lon2*pi/180;

    //printf("lat1= %lf, lat2= %lf, lon1= %lf, lon2 = %lf \n", lat1, lat2, lon1, lon2);
    distance = 6371.0*( fabs( (lon2-lon1)*cos((lat1+lat2)/2) ) + fabs( lat2-lat1 ) ); //Pythagoran distance

    //printf("distance = %lf\n", distance);
    return 2.0 - 2.0/(1.0 + exp(-0.2*distance));

}


double function_computation(struct task* tasks, int chosen_task, int* set, int set_size)
{
    int i, j;
    double val, max_val, lon1, lat1;

    val = 0;

    for(i=0;i<points_per_task;i++)
    {
        max_val = 0;
        lon1 = tasks[chosen_task].lons[i];
        lat1 = tasks[chosen_task].lats[i];

        for(j=0;j<set_size;j++)
        {
            max_val = max(max_val, distance_convert(lon1, lat1, ground_lon[set[j]], ground_lat[set[j]] ) );
           // printf("%lf \n", distance_convert(lon1, lat1, ground_lon[set[j]], ground_lat[set[j]] ));
        }
        val += max_val;

    }

    return val;

}


void greedy_train(struct task* tasks, int k)
{
    int i, j, round, best_item;
    double max_val,val,f_S;


    for( round = 0; round < k; round++)
    {
       // printf("round = %d \n", round );
        max_val = 0;

        for(i=0;i<n;i++)
        {
            val = 0;

            S_greedy_train[round] = i;

            for(j=0;j<m;j++)
                val += function_computation(tasks, j , S_greedy_train , round+1);

           // printf("val = %lf, i=%d \n", val, i);

            if(val > max_val)
            {
                max_val = val;
                best_item = i;
            }

        }
        //printf("round = %d, max_val = %lf, best_item = %d \n", round, max_val, best_item);

        S_greedy_train[round] = best_item;


        f_S = 0;
        for(j=0;j<m;j++)
            f_S += function_computation(tasks, j , S_greedy_train , round +1);
    }


}

void greedy_train_lazy_version(struct task* tasks, int k)
{
    int i, j, round, best_item, found, second_max_val;
    double max_val,val,f_S;

    //round 0 (picking the first element)
    for(i=0;i<n;i++)
    {
        val = 0;

         S_greedy_train[0] = i;

         for(j=0;j<m;j++)
             val += function_computation(tasks, j , S_greedy_train , 1);

        marginals[i] = val;


         if(val > max_val)
         {
             max_val = val;
             best_item = i;
         }
    }
    f_S = max_val;
    S_greedy_train[0] = best_item;
    marginals[best_item] = -1;

    //rounds 1 to k-1
    for( round = 1; round < k; round++)
    {
        //printf("round = %d \n", round );
        max_val = 0;
        second_max_val = 0;

        found = 0;

        while(found == 0)
        {

            max_val = 0;
            //finding the element that has the best marginal from the previous round
            for(i=0;i<n;i++)
            {
                if(marginals[i] > max_val)
                {
                    max_val = marginals[i];
                    best_item = i;
                }

            }


            //recomputing the marginal for that top element
            val = 0;
            S_greedy_train[round] = best_item;
            
             for(j=0;j<m;j++)
                 val += function_computation(tasks, j , S_greedy_train , round + 1);

            marginals[best_item] = val - f_S;
           found = 1;
            for(i=0;i<n;i++)
                if(marginals[best_item] < marginals[i])
                    found  = 0;


            if(found == 1)
            {
                S_greedy_train[round] = best_item;
                               
                marginals[best_item] = -1;
              

                f_S = 0;
                for(j=0;j<m;j++)
                    f_S += function_computation(tasks, j , S_greedy_train , round +1);


            }
        }

    }

}



void alg_2_lazy_version(struct task* tasks,int k,int l)
{
    int i, t, round, best_item, found, num_founds;
    double max_val,val,f_S_ind;
    
    double* max_vals;
    double* f_S;
    int* best_items;
    int* founds;
    
    
    max_vals = (double*) malloc((m)*sizeof (double));
    best_items = (int*) malloc((m)*sizeof (int));
    founds = (int*) malloc((m)*sizeof (int));
    f_S = (double*) malloc((m)*sizeof (double));
    
    
    //phase 1, construction S_personel[t] for all t
    
    //round 0 (picking the first element)
    
    for(t = 0; t< m; t++)
        max_vals[t] = 0;
    //printf("phase 1, round = %d \n", 0 );
    for(i=0;i<n;i++)
    {
        for(t=0;t<m;t++)
        {
              S_personal[t][0] = i;
              val = function_computation(tasks, t ,  S_personal[t] , 1);

              marginals_personal[t][i] = val;


               if(val > max_vals[t])
               {
                   max_vals[t] = val;
                   best_items[t] = i;
               }
          }
    }
    for(t=0;t<m;t++)
    {
        f_S[t] = max_vals[t];
        S_personal[t][0] = best_items[t];
        marginals_personal[t][best_items[t]] = -1;
    }

    //rounds 1 to k-1
    for( round = 1; round < k-l; round++)
    {
    num_founds = 0;

    //printf("phase 1, round = %d \n", round );
        for(t=0;t<m;t++)
            max_vals[t] = 0;
        
        for(t=0;t<m;t++)
            founds[t] = 0;

        found = 0;

        while(found == 0)
        {
            found = 1;
            for(t=0;t<m;t++)
                max_vals[t] = 0;
                  //finding the element that has the best marginal from the previous round
            for(i=0;i<n;i++)
            {
                for(t=0;t<m;t++)
                    if(founds[t] == 0)
                        if(marginals_personal[t][i] > max_vals[t])
                        {
                            max_vals[t] = marginals_personal[t][i];
                            best_items[t] = i;
                        }
            }

                  //recomputing the marginal for that top element

            for(t=0;t<m;t++)
            {
                if(founds[t] == 0)
                {
                    S_personal[t][round] = best_items[t];
                    val = function_computation(tasks, t , S_personal[t] , round + 1);

                    marginals_personal[t][best_items[t]] = val - f_S[t];

                    founds[t] = 1;
                    for(i=0;i<n;i++)
                        if(marginals_personal[t][best_items[t]] < marginals_personal[t][i])
                        {
                            founds[t]  = 0;
                            S_personal[t][round] = i;
                            
                            val = function_computation(tasks, t , S_personal[t] , round + 1);

                            marginals_personal[t][i] = val - f_S[t];
                        }


                    if(founds[t] == 1)
                    {
                        S_personal[t][round] = best_items[t];

                        marginals_personal[t][best_items[t]] = -1;

                        f_S[t] = function_computation(tasks, t , S_personal[t] , round +1);
                        
                        num_founds++;


                    }
                    if(founds[t] == 0)
                        found = 0;
                }
              }

          }
    }
    
    
    for(t=0;t<m;t++)
        for(i=0;i<k-l;i++)
            S_final[t][i] = S_personal[t][i];
    
    //updating the set S_tr in phase 2
    
    //round 0 (picking the first element)
       for(i=0;i<n;i++)
       {
           val = 0;
           for(t=0;t<m;t++)
               S_final[t][k-l] = i;

            for(t=0;t<m;t++)
                val += function_computation(tasks, t , S_final[t] , k-l+1);

           marginals[i] = val;


            if(val > max_val)
            {
                max_val = val;
                best_item = i;
            }
       }
       f_S_ind = max_val;
        for(t=0;t<m;t++)
            S_final[t][k-l] = best_item;
        S_alg2[0] = best_item;
        marginals[best_item] = -1;

    
    
    
       //rounds 1 to k-1
       for( round = k-l+1; round < k; round++)
       {
          // printf("phase 2, round = %d \n", round );
           max_val = 0;

           found = 0;

           while(found == 0)
           {

               max_val = 0;
               //finding the element that has the best marginal from the previous round
               for(i=0;i<n;i++)
               {
                   if(marginals[i] > max_val)
                   {
                       max_val = marginals[i];
                       best_item = i;
                   }

               }


               //recomputing the marginal for that top element
                for(t=0;t<m;t++)
                   S_final[t][round] = best_item;
               
               
               
               val = 0;
                for(t=0;t<m;t++)
                    val += function_computation(tasks, t , S_final[t] , round + 1);

               marginals[best_item] = val - f_S_ind;

               found = 1;
               for(i=0;i<n;i++)
                   if(marginals[best_item] < marginals[i])
                       found  = 0;


               if(found == 1)
               {
                   for(t=0;t<m;t++)
                       S_final[t][round] = best_item;

                   marginals[best_item] = -1;

                   f_S_ind = 0;
                   for(t=0;t<m;t++)
                       f_S_ind += function_computation(tasks, t , S_final[t] , round +1);
                   
                   S_alg2[round - (k-l)] = best_item;


               }
           }

       }

    
    
    
    
}











void alg_rand_lazy_version(struct task* tasks,int k,int l)
{
    int i, t, round, best_item, found, num_founds;
    double max_val,val,f_S_ind;
    
    double* max_vals;
    double* f_S;
    int* best_items;
    int* founds;
    
    
    max_vals = (double*) malloc((m)*sizeof (double));
    best_items = (int*) malloc((m)*sizeof (int));
    founds = (int*) malloc((m)*sizeof (int));
    f_S = (double*) malloc((m)*sizeof (double));
    double randval;
    randval=RVuniform();
    int h=0;
    int hph1=0;
    int hph2=0;
    double ratiolk=0;
       ratiolk = (double)l / (double)k;;
    while((hph1<k-l)&&(hph2<l))
    {    randval=RVuniform();
    if (randval>=ratiolk)
    {
        
    
    //phase 1, construction S[t] for all t
    
    
    for(t = 0; t< m; t++)
        max_vals[t] = 0;
    //printf("phase 1, round = %d \n", 0 );
    for(i=0;i<n;i++)
    {
        for(t=0;t<m;t++)
        {
              S_final[t][h] = i;
              val = function_computation(tasks, t ,  S_final[t] , h+1);

              marginals_personal[t][i] = val;


               if(val > max_vals[t])
               {
                   max_vals[t] = val;
                   best_items[t] = i;
               }
          }
        
        }
     for(t=0;t<m;t++)
     {
         S_final[t][h] = best_items[t];
    }
        h++;
        hph1++;
    }
    
 
    if (randval<ratiolk)
    {
    
    
    //updating the set S_tr in phase 2
    
       for(i=0;i<n;i++)
       {
           val = 0;
           for(t=0;t<m;t++)
               S_final[t][h] = i;

            for(t=0;t<m;t++)
                val += function_computation(tasks, t , S_final[t] , h+1);

           marginals[i] = val;


            if(val > max_val)
            {
                max_val = val;
                best_item = i;
            }
       }
       f_S_ind = max_val;
        for(t=0;t<m;t++)
        { S_final[t][h] = best_item;}
            S_randomized[hph2] = best_item;

        h++;
        hph2++;
    
       //rounds 1 to k-1
       

    
    }
}

    while(hph1<k-l)
{
    
      //phase 1, construction S_personel[t] for all t
      
      //round 0 (picking the first element)
      
      for(t = 0; t< m; t++)
          max_vals[t] = 0;
      //printf("phase 1, round = %d \n", 0 );
      for(i=0;i<n;i++)
      {
          for(t=0;t<m;t++)
          {
                S_final[t][h] = i;
                val = function_computation(tasks, t ,  S_final[t] , h+1);

                marginals_personal[t][i] = val;


                 if(val > max_vals[t])
                 {
                     max_vals[t] = val;
                     best_items[t] = i;
                 }
            }
          
      }
      for(t=0;t<m;t++)
      {
          S_final[t][h] = best_items[t];
      }
          h++;
          hph1++;
}
while(hph2<l)
{
   
    //updating the set S_tr in phase 2
    
    //round 0 (picking the first element)
       for(i=0;i<n;i++)
       {
           val = 0;
           for(t=0;t<m;t++)
               S_final[t][h] = i;

            for(t=0;t<m;t++)
                val += function_computation(tasks, t , S_final[t] , h+1);

           marginals[i] = val;


            if(val > max_val)
            {
                max_val = val;
                best_item = i;
            }
       }
       f_S_ind = max_val;
        for(t=0;t<m;t++)
        { S_final[t][h] = best_item;}
            S_randomized[hph2] = best_item;

        h++;
        hph2++;
    
       //rounds 1 to k-1
       

}


}




















double test_perfomance(struct task* tasks, int* S_tr, int k,int l)
{
    int i, t, round, best_item, found, num_founds;
    double max_val,val,f_S_ind, test_val;
    
    double* max_vals;
    double* f_S;
    int* best_items;
    int* founds;
    
    
    max_vals = (double*) malloc((m)*sizeof (double));
    best_items = (int*) malloc((m)*sizeof (int));
    founds = (int*) malloc((m)*sizeof (int));
    f_S = (double*) malloc((m)*sizeof (double));
    
    
    test_val = 0;
    
    for(t=0;t<m;t++)
    {
        for(i=0;i<l;i++)
            S_final[t][i] = S_tr[i];
    }
        //round 0 (picking the first element)
          //round 0 (picking the first element)
          
    for(t = 0; t< m; t++)
        max_vals[t] = 0;
   // printf("round = %d \n", l );
    for(i=0;i<n;i++)
    {
        for(t=0;t<m;t++)
        {
            S_final[t][l] = i;
            val = function_computation(tasks, t ,  S_final[t] , l+1);
            marginals_personal[t][i] = val;

            if(val > max_vals[t])
            {
                max_vals[t] = val;
                best_items[t] = i;
            }
        }
    }
    for(t=0;t<m;t++)
    {
        f_S[t] = max_vals[t];
        S_final[t][l] = best_items[t];
        marginals_personal[t][best_items[t]] = -1;
    }
    
    
    //rounds 1 to k-1
    
    
    
    for( round = l+1; round < k; round++)
    {
        //printf("round = %d \n", round );
        for(t=0;t<m;t++)
            max_vals[t] = 0;
        
        for(t=0;t<m;t++)
            founds[t] = 0;

        found = 0;

        while(found == 0)
        {
            found = 1;
            for(t=0;t<m;t++)
                max_vals[t] = 0;
                  //finding the element that has the best marginal from the previous round
            for(i=0;i<n;i++)
            {
                for(t=0;t<m;t++)
                    if(founds[t] == 0)
                        if(marginals_personal[t][i] > max_vals[t])
                        {
                            max_vals[t] = marginals_personal[t][i];
                            best_items[t] = i;
                        }
            }

                  //recomputing the marginal for that top element

            for(t=0;t<m;t++)
            {
                if(founds[t] == 0)
                {
                    S_final[t][round] = best_items[t];
                    val = function_computation(tasks, t , S_final[t] , round + 1);

                    marginals_personal[t][best_items[t]] = val - f_S[t];

                    founds[t] = 1;
                    for(i=0;i<n;i++)
                        if(marginals_personal[t][best_items[t]] < marginals_personal[t][i])
                        {
                            founds[t]  = 0;
                            S_final[t][round] = i;
                            
                            val = function_computation(tasks, t , S_final[t] , round + 1);

                            marginals_personal[t][i] = val - f_S[t];
                        }


                    if(founds[t] == 1)
                    {
                        S_final[t][round] = best_items[t];

                        marginals_personal[t][best_items[t]] = -1;

                        f_S[t] = function_computation(tasks, t , S_final[t] , round +1);
                        


                    }
                    if(founds[t] == 0)
                        found = 0;
                }
            }
        }
    }
        
        
    test_val = 0;
    
    for(t=0;t<m;t++)
        test_val += function_computation(tasks, t , S_final[t] , k);
    
    return test_val/m;
    
    
}





double test_perfomance_ideal(struct task* tasks, int k,int l)
{
    int i, t, round, best_item, found, num_founds;
    double max_val,val,f_S_ind, test_val;
    
    double* max_vals;
    double* f_S;
    int* best_items;
    int* founds;
    
    
    max_vals = (double*) malloc((m)*sizeof (double));
    best_items = (int*) malloc((m)*sizeof (int));
    founds = (int*) malloc((m)*sizeof (int));
    f_S = (double*) malloc((m)*sizeof (double));
    
    
    test_val = 0;
    
        //round 0 (picking the first element)
          //round 0 (picking the first element)
          
    for(t = 0; t< m; t++)
        max_vals[t] = 0;
    //printf("round = %d \n", 0 );
    for(i=0;i<n;i++)
    {
        for(t=0;t<m;t++)
        {
            S_final[t][0] = i;
            val = function_computation(tasks, t ,  S_final[t] , 1);
            marginals_personal[t][i] = val;

            if(val > max_vals[t])
            {
                max_vals[t] = val;
                best_items[t] = i;
            }
        }
    }
    for(t=0;t<m;t++)
    {
        f_S[t] = max_vals[t];
        S_final[t][0] = best_items[t];
        marginals_personal[t][best_items[t]] = -1;
    }
    
    
    //rounds 1 to k-1
    
    
    
    for( round = 1; round < k; round++)
    {
       // printf("round = %d \n", round );
        for(t=0;t<m;t++)
            max_vals[t] = 0;
        
        for(t=0;t<m;t++)
            founds[t] = 0;

        found = 0;

        while(found == 0)
        {
            found = 1;
            for(t=0;t<m;t++)
                max_vals[t] = 0;
                  //finding the element that has the best marginal from the previous round
            for(i=0;i<n;i++)
            {
                for(t=0;t<m;t++)
                    if(founds[t] == 0)
                        if(marginals_personal[t][i] > max_vals[t])
                        {
                            max_vals[t] = marginals_personal[t][i];
                            best_items[t] = i;
                        }
            }

                  //recomputing the marginal for that top element

            for(t=0;t<m;t++)
            {
                if(founds[t] == 0)
                {
                    S_final[t][round] = best_items[t];
                    val = function_computation(tasks, t , S_final[t] , round + 1);

                    marginals_personal[t][best_items[t]] = val - f_S[t];

                    founds[t] = 1;
                    for(i=0;i<n;i++)
                        if(marginals_personal[t][best_items[t]] < marginals_personal[t][i])
                        {
                            founds[t]  = 0;
                            S_final[t][round] = i;
                            
                            val = function_computation(tasks, t , S_final[t] , round + 1);

                            marginals_personal[t][i] = val - f_S[t];
                        }


                    if(founds[t] == 1)
                    {
                        S_final[t][round] = best_items[t];

                        marginals_personal[t][best_items[t]] = -1;

                        f_S[t] = function_computation(tasks, t , S_final[t] , round +1);
                        


                    }
                    if(founds[t] == 0)
                        found = 0;
                }
            }
            
        }
    }
        
        
    test_val = 0;
    
    for(t=0;t<m;t++)
        test_val += function_computation(tasks, t , S_final[t] , k);
    
    return test_val/m;
    
}





int main(int argc, char **argv)
{

    char command[500];
    char str[MAXCHAR];
    FILE *fp;
    FILE *fp2;




    char *ptr;
    double ret,p, val_greedy_train;
    char newString[2][10];
    double result[30][10]= {0};
    int l=1;

    int i,j,r,t,T,ctr,round;


    int chosen_index;

    struct task* tasks;
    struct task* new_tasks;
    struct task* new_taskscent;

    srand ( time(NULL) );

    if (argc != 5)
    {
        printf("Usage: ./rideshare n m k l\n");
        printf("where: n         ...  number of users\n");
        printf("       m         ...  number of tasks \n");
        printf("       k         ...  final cardinality constraint\n");
        printf("       l        ...  starting point for G \n");
        exit(1);
    }
    {
        n = (int)atoi(argv[1]);
        m = (int)atoi(argv[2]);
        ki = (int)atoi(argv[3]);
        li = (int)atoi(argv[4]);
    }

    points_per_task = 100;


    
    printf("n=%d, m = %d, ki = %d, li = %d \n",n,m,ki,li);




    // ******* Reading the data file
    //the data arrays
    locations_lon = (double*) malloc((1100000)*sizeof (double));
    locations_lat = (double*) malloc((1100000)*sizeof (double));

    read_file();

    // subsampling the data tthe number of users n
      p = n/(1028135.0);

      ground_lon = (double*) malloc((1100000)*sizeof (double));
      ground_lat = (double*) malloc((1100000)*sizeof (double));

      for(i=0;i<1100000;i++)
      {
          ground_lon[i] = 0;
          ground_lat[i] = 0;
      }

    


  n = 0;

    for(i=0;i<1028135;i++)
     {
         if(RVuniform() < p)
         {
             ground_lon[n] = locations_lon[i];
             ground_lat[n] = locations_lat[i];
             n++;
         }
     
     }

    int max_ind_k=10;
// ******* max_ind_k

    for(int ind_k=0;ind_k<max_ind_k;ind_k++)

    { int k=0;
        
// ******* vary k
        k=ind_k*3+5;
       if (ind_k==9) {
           k=30;
        }
        l=(int) floor((k*li)/10);
       
//*******

// ******* vary l
      //  k=20;
      //  l=(int) 5+ind_k*5;
      //  if(ind_k==3)
      //      l=16;
      //  if(ind_k==4)
      //      l=18;
//*******
        printf("n=%d, m = %d, k = %d, l = %d \n",n,m,k,l);
        

    // ******* Defining the tasks and allocating memory

    tasks = (struct task*) malloc((m)*sizeof (struct task));
    for(i=0;i<m;i++)
    {
        tasks[i].lons = (double*) malloc((points_per_task)*sizeof (double));
        tasks[i].lats = (double*) malloc((points_per_task)*sizeof (double));
        chosen_index = 0;
        while (chosen_index < points_per_task)
            chosen_index = floor(10000*RVuniform());

        tasks[i].center = chosen_index;
        int negh=0;
               int ii=0;
               int ch_index2=0;
               for (j = 0; j < points_per_task/10 ; j++)
               {negh=0;
                   ii=0;
                   while (negh < 10)
                   { ii++;
                      ch_index2 = ii;
                       if(distance_convert(locations_lon[chosen_index-points_per_task+j], locations_lat[chosen_index-points_per_task+j], locations_lon[ch_index2], locations_lat[ch_index2])>0.9)
                       {
                           tasks[i].lons[j*10+negh] = locations_lon[ch_index2];
                           tasks[i].lats[j*10+negh] = locations_lat[ch_index2];
                           negh++;
                       }
                       
                       if(ii>300000)
                       {
                           while (negh < 10)
                           {
                           tasks[i].lons[j*10+negh] = locations_lon[chosen_index-points_per_task+j*10+negh];
                           tasks[i].lats[j*10+negh] = locations_lat[chosen_index-points_per_task+j*10+negh];
                           negh++;

                           }
                       }
                   }
               }
           }
   // exit(1);

  
 
    //this array will be used for the lazy greedy implementations
    marginals = (double*) malloc((n)*sizeof (double));
    marginals_personal = (double**) malloc((m)*sizeof (double*));
    
    for(i=0;i<m;i++)
        marginals_personal[i] = (double*) malloc((n)*sizeof (double));

    
    
    S_greedy_train = (int*) malloc((k)*sizeof (int)); //entrely running greedy on the training set
    S_alg1 = (int*) malloc((l)*sizeof (int)); //S_tr from Algorthm 1
    S_alg2 = (int*) malloc((l)*sizeof (int)); // S_tr from Algorithm 2
    S_randomized = (int*) malloc((l)*sizeof (int)); //S_tr from the randomized algorithm
    S_personal = (int**) malloc((m)*sizeof (int*)); //S_personal[i] is the set S_i in the paper
    for(i=0;i<m;i++)
        S_personal[i] = (int*) malloc((k-l)*sizeof (int));
    S_final = (int**) malloc((m)*sizeof (int*)); //S_personal[i] is the set S_i in the paper
    for(i=0;i<m;i++)
        S_final[i] = (int*) malloc((k)*sizeof (int));

        
    
    
    for(i=0;i<k;i++)
        S_greedy_train[i] = 0;



    // *********running the greedy algorithm on the training set to choose all the k elements (no personalization)

     greedy_train_lazy_version(tasks,k);
    
    
    
    printf("Algorithm 1 ... \n");
    
    // see phase 1 of algorithm 1
    for(i=0;i<l;i++)
        S_alg1[i] = S_greedy_train[i];

    
    printf("Algorithm 2 ... \n");
    alg_2_lazy_version(tasks,k,l);

    //greedy_train(tasks);

   printf("Algorithm rand ... \n");
   alg_rand_lazy_version(tasks,k,l);

    
    printf("Evaluating test performances ... \n");
    
 



    // *********performance at test
    new_tasks = (struct task*) malloc((m)*sizeof (struct task));
    for(i=0;i<m;i++)
    {

        new_tasks[i].lons = (double*) malloc((points_per_task)*sizeof (double));
        new_tasks[i].lats = (double*) malloc((points_per_task)*sizeof (double));
        chosen_index = 0;
        while (chosen_index < points_per_task)
            chosen_index = floor(20000*RVuniform());

        new_tasks[i].center = chosen_index;
        int negh=0;
        int ii=0;
        int ch_index2=0;
        for (j = 0; j < points_per_task/10 ; j++)
        {negh=0;
            ii=0;
           
            while (negh < 10)
            {   ii++;
                ch_index2 = ii;
                if(distance_convert(locations_lon[chosen_index-points_per_task+j], locations_lat[chosen_index-points_per_task+j], locations_lon[ch_index2], locations_lat[ch_index2])>0.9)
                {
                    new_tasks[i].lons[j*10+negh] = locations_lon[ch_index2];
                    new_tasks[i].lats[j*10+negh] = locations_lat[ch_index2];
                    negh++;

                }
                if(ii>300000)
                {
                    while (negh < 10)
                    {
                    new_tasks[i].lons[j*10+negh] = locations_lon[chosen_index-points_per_task+j*10+negh];
                    new_tasks[i].lats[j*10+negh] = locations_lat[chosen_index-points_per_task+j*10+negh];
                    negh++;
                        
                    }
               }
                
            }
        }
    }

    //evaluate the performance of S_greed_train
    val_greedy_train = 0;

    for(j=0;j<m;j++)
    {  val_greedy_train += function_computation(new_tasks, j , S_greedy_train , k);
        
    }
    val_greedy_train = val_greedy_train/m;
    result[ind_k][0]=val_greedy_train/points_per_task;

    printf("test performance of S_train = %lf,k=%d \n", result[ind_k][0],k);
    //evaluate of random S
    val_greedy_train = 0;
    for( round = 1; round < k; round++)
    {
               S_greedy_train[round] = round;
               
    }

    for(j=0;j<m;j++)
    {  val_greedy_train += function_computation(new_tasks, j , S_greedy_train , k);
        
    }
    val_greedy_train = val_greedy_train/m;
    result[ind_k][1]=val_greedy_train/points_per_task;

    printf("test performance of S_random = %lf,k=%d \n", result[ind_k][1],k);
    //
    
    //evaluate the performance of S_alg_1
        result[ind_k][2]=test_perfomance(new_tasks, S_alg1,k,l)/points_per_task;

    printf("test performance of Algorithm 1 = %lf,k=%d \n",result[ind_k][2] ,k);
    
    //evaluate the performance of S_alg_2
        result[ind_k][3]=test_perfomance(new_tasks, S_alg2,k,l)/points_per_task;

    printf("test performance of Algorithm 2 = %lf ,k=%d\n", result[ind_k][3],k);
    //evaluate the performance of S_alg_rand
        result[ind_k][4]=test_perfomance(new_tasks, S_randomized,k,l)/points_per_task;

    printf("test performance of Algorithm rand = %lf ,k=%d\n", result[ind_k][4],k);

    //ideal performance (full greedy for each task)
        result[ind_k][5]=test_perfomance_ideal(new_tasks,k,l)/points_per_task;

    printf("upper bound on test performance (full greedy for each task)= %lf, k=%d\n", result[ind_k][5],k);



    } printf("[");
    for(int ind_k=0;ind_k<max_ind_k;ind_k++)
    printf("%f %f %f %f %f %f;\n", result[ind_k][0],result[ind_k][1],result[ind_k][2],result[ind_k][3],result[ind_k][4],result[ind_k][5]);
    
    printf("]");
}
