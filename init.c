#include<stdio.h>
#include<unistd.h>
#include<time.h>
#include<string.h>
#include <fcntl.h>
struct Customer{
char username[1024];
char password[1024];
char account_number[1024];
short type;
};
/*
type=0 for administrator
     1 for normal user
	 2 for joint account holder
*/
struct Account{
char account_number[1024];
double balance;
};
struct Transaction
{
	char account_number[1024];
	char date[100];
	double amount;
    double balance_rem;
};
int main(){
    open("accounts",O_CREAT|O_RDWR,0744);
    open("transaction",O_CREAT|O_RDWR,0744);
    int fd=open("customer",O_CREAT|O_RDWR,0744);
    struct Customer c;
    c.account_number[0]='\0';
    strcpy(c.username,"admin");
    strcpy(c.password,"admin");
    c.type=0;
    write(fd,&c,sizeof(c));
    c.username[0]='\0';
    c.password[0]='\0';
    write(fd,&c,sizeof(c));
    return 0;
}