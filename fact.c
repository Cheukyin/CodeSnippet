#include<iostream>

using namespace std;

int fact(int n)

{
    if(n==1)
    {
        return 1;
    }
    else
    {
        return n*fact(n-1);
    }
}
int pow(int b,int c)

{
int j,d=1;
    for(j=0;j<c;j++)
    {
        d=d*b;
    }
    return d;
}

int main(){
int x;
int i,n;
int a=0;
int b=0;
float sum=0;
cout<<"enter the no : ";
cin>>x;
cout<<"enter the no of terms: ";
cin>>n;
for(i=0;i<=n;i++){
    a=pow(x, i);
    b=fact(i);
    sum=sum+a/b;}
cout<<"the sum of series is"<<sum<<endl;
return 0;
}
