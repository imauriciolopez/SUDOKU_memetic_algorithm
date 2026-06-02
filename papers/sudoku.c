/* 
Program to solve a Sudoku puzzle. Each block is examined and the open cells are
filled with the remaining integers, so the block contains all nine integers. 
The energy is equal to the 243 minus the sum of the number of unique elements 
in each row, column and block. An MC move consists of randomly 
selecting a pair of non-fixed elements in a randomly selected block. These
elements are then exchanged to generate the trial configuration. 
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int check(int i, int k, int n[][9]){

  // determines number of unique elements in each row (k=1) or column (k!=1)

  int nums[9]={1,2,3,4,5,6,7,8,9};
  int j, i1, nu;

  nu = 0;

  for(j=0;j<9;j++){

    if(k==1){
      i1 = n[i][j]-1;
    }

    else{
      i1 = n[j][i]-1;
    }

    if(i1==-1){
      return -1;
    }
    
    if(nums[i1]!=0){
      nu+=1;
      nums[i1]=0;
    }
  }
  return nu;
}

int checksq(int is, int js, int n[][9]){

  // determines number of unique elements in square is, js

  int nums[9]={1,2,3,4,5,6,7,8,9};
  int i, j, i1, nu, i0, j0;

  nu = 0;

  i0 = is*3;
  j0 = js*3;

  nu = 0;

  for(i=0;i<3;i++){

    for(j=0;j<3;j++){

      i1 = n[i0+i][j0+j] - 1;

    if(i1==-1){
      return -1;
    }
	if(nums[i1]!=0){

	  nu+=1;

	  nums[i1]=0;
	}
    }
  }
  return nu;
}


int energy(int n[][9]){

  // computes the "energy" of the configuration

  int i,j;
  int sum = 0;

  for(i=0;i<9;i++){
    sum += check(i,1,n) + check(i,2,n);
  }

  for(i=0;i<3;i++){
    for(j=0;j<3;j++){
      sum += checksq(i,j,n);
    }
  }
  return 243 - sum;
}

double ran0(int *idum)

{

  //generates a uniform random number between 0 and 1

  int ia, im, iq, ir, mask, k; 
  double am;

  ia=16807, im=2147483647;
  am = 1./im;
  iq = 127773, ir=2836, mask=123459876;

  k = *idum/iq;
  *idum = ia*(*idum-k*iq)-ir*k;
  if(*idum<0)*idum+=im;
  return am*(*idum);
}

void show(int n[][9]){

  //displays a Sudoku configuration on the screen

  int i, j;
  printf("\n-------------------------\n");
   for(i=0;i<9;i++){
     printf("| ");
     for(j=0;j<9;j=j+3){

       printf("%d %d %d | ",n[i][j],n[i][j+1],n[i][j+2]);
     }
     if((i+1)%3==0){
       printf("\n-------------------------\n");
     }else printf("\n");
   }
   return;
}


main()
{
  int n[9][9], mask[9][9];
  int i, j, in, i1, j1, i2, j2, ix, iy, k, kk, ll;
  int nums[9],num1[9];
  int ib, jb, kr, e, emin, ep, ntemp, ntrial, zero;
  float temp;

 FILE *fin ;

 fin  = fopen("sudoku.dat","r");

 kr = 894331;
 temp = 0.25;
 ntrial = 1000000;
 emin = 18;
 zero = 0;

 // read in the puzzle from sudoku.dat

   for(i=0;i<9;i++){
     for(j=0;j<9;j++){
     fscanf(fin,"%d",&in);
     n[i][j] = in;
   }
}

   show(n);

   fclose(fin);


   printf("\n Temperature, number of steps? ");
   scanf("%f %d",&temp,&ntrial);

   // assign the remaining integers to open cells so that each block
   // contains all integers once only.

   for(i=0;i<9;i++){
     for(j=0;j<9;j++){
       mask[i][j] = 1;
       if(n[i][j]==0)mask[i][j] = 0;
     }
   }

   for(ib=0;ib<3;ib++){
     for(jb=0;jb<3;jb++){
       for(k=0;k<9;k++){
       nums[k]=k+1;
       }
       for(i=0;i<3;i++){
	 for(j=0;j<3;j++){
	   i1 = ib*3 + i;
	   j1 = jb*3 + j;

	   if(n[i1][j1]!=0){
	     ix = n[i1][j1];
	     nums[ix-1]=0;
	   }
	 }
       }

       iy = -1;

       for(k=0;k<9;k++){

	 if(nums[k]!=0){
	   iy+=1;
	   num1[iy] = nums[k];
	 }
       }

       kk = 0;

       for(i=0;i<3;i++){
	 for(j=0;j<3;j++){

	   i1 = ib*3 + i;
	   j1 = jb*3 + j;

	   if(n[i1][j1]==0){
	     n[i1][j1] = num1[kk];
	     kk+=1;
	   }
	 }
       }

     }
   }

   show(n);

   e = energy(n);

   printf("\ninitial energy: %d \n",e);

   FILE *output;

   output = fopen("energy.dat","w");

  //  fprintf(output," %d %f \n ",zero,e);



   //start Monte Carlo loop

   for(ll=0;ll<ntrial;ll++){

     // at each step pick at random a block and two moveable elements in the block

     ib = 3*(int)(3.0*ran0(&kr));
     jb = 3*(int)(3.0*ran0(&kr));

     do{
       i1 = (int)(3.0*ran0(&kr));
       j1 = (int)(3.0*ran0(&kr));
     } while(mask[ib+i1][jb+j1]==1);

     do{
       i2 = (int)(3.0*ran0(&kr));
       j2 = (int)(3.0*ran0(&kr));
     } while(mask[ib+i2][jb+j2]==1);


     //swap the movable elements and compute the energy of the trial configuration

     ntemp = n[ib+i1][jb+j1];
     n[ib+i1][jb+j1] = n[ib+i2][jb+j2];
     n[ib+i2][jb+j2] = ntemp;

     ep = energy(n);

     if(ll%200 == 0)fprintf(output,"%d %d\n",ll,ep);

     if(ep<emin){
       printf("step %d energy %d\n",ll,ep);
       emin = ep;
     }

     // stop computing if the solution is found

     if(ep==0)break;

     // accept or reject the trial configuration using the Monte Carlo criterion

	 if(exp((float)(e-ep)/temp)>(float)ran0(&kr)){
	   e = ep;
	 }
	 else{
	   ntemp = n[ib+i1][jb+j1];
	   n[ib+i1][jb+j1] = n[ib+i2][jb+j2];
	   n[ib+i2][jb+j2] = ntemp;
	 }	   
   }

   if(ep==0){

   printf("\nSolution found after %d steps!\n",ll);
   fprintf(output,"%d %d\n",ll,ep);

   show(n);
   }
   else{
   printf("\nNo solution found after %d steps!\n",ntrial);
   }
   fclose(output);
}


