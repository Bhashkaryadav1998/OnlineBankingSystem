#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<inttypes.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/ip.h>
#include<fcntl.h>
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
void modify(int sd,char username[1024],int ch){
	if(ch==1){
		char muser[1024];
		printf("Enter the new username for the account: ");
		scanf("%s",muser);
		write(sd,muser,sizeof(muser));
		}
	else if(ch==2){
		char mpasswd[1024];
		printf("Enter the new Password for the account: ");
		scanf("%s",mpasswd);
		write(sd,mpasswd,sizeof(mpasswd));
		
	}
	else if(ch==3){
		short type;
		printf("Enter the new type for the account: ");
		scanf("%hi",&type);
		write(sd,&type,sizeof(type));
		struct Customer c,tmp;
		read(sd,&c,sizeof(c));
		
				if(c.type==2 && type==1){
					printf("Error cannot change joint account to single user!!\n");
				}
				else if(c.type==1 && type==2){
					char user[1024],pass[1024];
					printf("Enter the new account username which you want to join(UNIQUE): ");
					scanf("%s",user);
					write(sd,user,sizeof(user));
					int isUniq;
					read(sd,&isUniq,sizeof(isUniq));
					if(isUniq){
						printf("Enter the password for the account: ");
						scanf("%s",pass);
						write(sd,pass,sizeof(pass));
					}
					else printf("Username not unique try again\n");
				}
	}
	else return;
}
void searchCustomer(int sd){
	printf("Account Number		Username	Password		Type\n");
	struct Customer c;
	int check;
	read(sd,&check,sizeof(check));
	while(check!=0){
		 read(sd,&c,sizeof(c));
		 read(sd,&check,sizeof(check));
         printf("%s	   %s	    %s	       %hi\n",c.account_number,c.username,c.password,c.type);
	}
}
void print_Transactions(int sd){
	printf("Account No.					Date			Amount		 	New Balance\n");
	struct Transaction t;
	int check;
	read(sd,&check,sizeof(check));
	while(check!=0){
		 read(sd,&t,sizeof(t));
	printf("%s			 %s		  %.2lf		   %.2lf\n",t.account_number,t.date,t.amount,t.balance_rem);
	read(sd,&check,sizeof(check));
	}

}
int main()
{
    char uname[1024],passwd[1024];
	int repeat;
	double amt;
	struct Customer c;
	struct Account a;
	struct Transaction t;
    struct sockaddr_in server;
int sd = socket(AF_UNIX, SOCK_STREAM, 0);
server.sin_family = AF_UNIX;
server.sin_addr.s_addr = INADDR_ANY;
server.sin_port = htons(7000);
connect(sd, (void *)(&server), sizeof(server));
    printf("-------------Welcome to Banking Management System-------------\n\n");
	printf("Kindly Enter Your Credentials:--------\n\n");
	printf("Username: ");
	scanf("%s",uname);
	printf("Password: ");
	scanf("%s",passwd);
    write(sd,uname,sizeof(uname));
	write(sd,passwd,sizeof(passwd));
	read(sd,&repeat,sizeof(repeat));
	if(repeat){
		read(sd,&c,sizeof(c));
       while(repeat){
							if(c.type==0){
							printf("-------------Welcome Admin-------------\n\n");
							printf("Press 1 to add an account\n");
							printf("Press 2 to delete an account\n");
							printf("Press 3 to modify an account\n");
							printf("Press 4 to search\n");
							int ch;
							scanf("%d",&ch);
							write(sd,&ch,sizeof(ch));
							char user[1024],pass[1024];
							switch (ch)
							{
							case 1:
								printf("Enter a USERNAME(unique): ");
								scanf("%s",user);
								write(sd,user,sizeof(user));
								int isUnique;
								read(sd,&isUnique,sizeof(isUnique));
								if(isUnique){
									printf("Enter Password for account: ");
									scanf("%s",pass);
									write(sd,pass,sizeof(pass));
									printf("Enter Type of account(0,1,2): ");
									short x;
									int k;
									scanf("%hi",&x);
									write(sd,&x,sizeof(x));
                                    if(read(sd,&k,sizeof(k))==-1) printf("Some unknown error occured\n");
									else printf("User Added Successfully\n");
								}
								else{
									printf("Username Already Exists Try Again\n");
								}
								break;
							case 2:
							      printf("Enter username whose account you want to delete: ");
								  scanf("%s",user);
								  write(sd,user,sizeof(user));
								  break;
							case 3:
							      printf("Enter username whose account you want to modify: ");
								  scanf("%s",user);
								  write(sd,user,sizeof(user));
								  int ch;
								  printf("Press 1 to modify username\n");
								  printf("Press 2 to modify password\n");
								  printf("Press 3 to modify type of account\n");
								  printf("Press 4 to exit\n");
								  scanf("%d",&ch);
								  write(sd,&ch,sizeof(ch));
								  modify(sd,user,ch);
								  break;
							case 4:
							      searchCustomer(sd);
								  break;
							          
							      
							default:
							    repeat=0;
								break;
							}
						}
						else{
							int ch;
						printf("-------------Welcome %s-------------\n\n",uname);
						printf("Press 1 for Deposit\n");
						printf("Press 2 for Withdraw\n");
						printf("Press 3 for Balance Enquiry\n");
						printf("Press 4 for Password Change\n");
						printf("Press 5 for View details\n");
						printf("Press 6 for Exit\n");
						printf("Enter your choice: ");
						scanf("%d",&ch);
						write(sd,&ch,sizeof(ch));
						switch (ch)
						{
						case 1:
							printf("Enter amount to be deposited in account no. %s: ",c.account_number);
							scanf("%lf",&amt);
							write(sd,&amt,sizeof(amt));
							break;
						case 2:
							printf("Enter amount to be withdrawn from account no. %s: ",c.account_number);
							scanf("%lf",&amt);
							write(sd,&amt,sizeof(amt));
							int feedback;
							read(sd,&feedback,sizeof(feedback));
							if(feedback) printf("Success\n");
							else printf("Error: Amount cannot be withdrawn Insufficient balance\n");
							break;
						case 3:
					        read(sd,&a,sizeof(a));
							printf("Your account balance is Rs. %.2lf\n",a.balance);
							break;
						case 4:
						    printf("Enter your new Password: ");
							scanf("%s",c.password);
							if(write(sd,&c,sizeof(c))!=-1) printf("Password changed Successfuly\n");
							else printf("Some unknown error occured\n");
							break;
						case 5:
							print_Transactions(sd);
							break;
						case 6:
						    repeat=0;
						    printf("Thanks for logging in\n\n");
							break;
						    
						default:
						repeat=0;
						printf("Invalid choice\n");
							break;
						}
						}
						}
	}
	else{
        printf("Invalid Username or Password\n");
		printf("Connection closed.....\n");
		close(sd);
	}
   
    return 0;
}