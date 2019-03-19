
int B=5;
int D=5;
int func(int *p) { 
   // int a;
      p = &B;  
}
/*
int add (int *b_add,int *c_add,int *cnt_add,int *sum)
{
*sum = *b_add+*c_add;
*b_add+=1;
*c_add+=9;
*cnt_add+=1;
return 0;
}
*/

int funD(int **p1) { 
   // int b;
    *p1 = &D;
}
void reset_handler(void)
{
//int b=15 ;
//int c=16;
//int cnt;
//int sum;
int A = 1;
int C = 3;
int a=0;
int c=0; //A = r3 

int *ptrA = &A;
int *ptrB = &C;

   
   func(&ptrA);  
   funD(&ptrB);
   a = *ptrA; 
   c = *ptrB;
   /*
	while(cnt<=3)
    {
   add(&b,&c,&cnt,&sum);
	}
   */	
}
