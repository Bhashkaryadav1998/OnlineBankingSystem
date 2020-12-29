#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<inttypes.h>
#include<stdlib.h>
#include<netinet/ip.h>
#include<sys/socket.h>
#include<sys/types.h>
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
void deposit(int fda,int fdt,char account_number[],double amt,int sd){
	struct Account a;
	struct Transaction t;
	int i,j,n1,n2,flag,feedback;
	while((j=read(fda,&a,sizeof(a)))>0){
		n1=strlen(account_number);
		n2=strlen(a.account_number);
		flag=1;
		if(n1==n2){
			for(i=0;i<n1;i++){
				if(account_number[i]!=a.account_number[i]){
					flag=0;
					break;
				}
			}
			if(flag){
				if((amt+a.balance)<0){
					feedback=0;
					write(sd,&feedback,sizeof(feedback));
					return;
				}
				a.balance=amt+a.balance;
				for(i=0;i<n2;i++) t.account_number[i]=a.account_number[i];
				t.account_number[i]='\0';
				time_t raw=time(NULL);
				if (raw == -1) {
        
                puts("The time() function failed\n");
                return;
              }
				struct tm *htm=localtime(&raw);
				if(htm==NULL){
					puts("Local time conversion failed\n");
					return;
				}
sprintf(t.date,"%d-%d-%d  %02d:%02d:%02d",htm->tm_mday,(htm->tm_mon)+1,(htm->tm_year)+1900,htm->tm_hour,htm->tm_min,htm->tm_sec);
               t.amount=amt;
			   t.balance_rem=a.balance;
			   lseek(fda,-sizeof(a),SEEK_CUR);
	           write(fda,&a,sizeof(a));
	           write(fdt,&t,sizeof(t));
			   if(amt<0){
				   feedback=1;
			   write(sd,&feedback,sizeof(feedback));
			   }
			   return;
			}
		}
	}

}
void balance(int fda,char acc_no[],int sd){
	struct Account a;
	int i,j,n1,n2,flag;
	while((j=read(fda,&a,sizeof(a)))>0){
		n1=strlen(acc_no);
		n2=strlen(a.account_number);
		flag=1;
		if(n1==n2){
			for(i=0;i<n1;i++){
				if(acc_no[i]!=a.account_number[i]){
					flag=0;
					break;
				}
			}
			if(flag){
				write(sd,&a,sizeof(a));
				return;
			}

		}
			
	}
}
void print_Transactions(int fdt,char account_number[],int sd){
	struct Transaction t;
	int i,j,n1,n2,flag,check=1;
	while((j=read(fdt,&t,sizeof(t)))>0){
		n1=strlen(account_number);
		n2=strlen(t.account_number);
		flag=1;
		if(n1==n2){
			for(i=0;i<n1;i++){
				if(account_number[i]!=t.account_number[i]){
					flag=0;
					break;
				}
			}
			if(flag){
				write(sd,&check,sizeof(check));
				write(sd,&t,sizeof(t));
			}
		
		}
	}
	check=0;
    write(sd,&check,sizeof(check));
}
int isUnique(int fdc,char uname[]){
	int i,flag=1;
	struct Customer c;
	while((i=read(fdc,&c,sizeof(c)))>0){
		int n1=strlen(c.username);
		int n2=strlen(uname);
		if(n1==n2){
			flag=1;
			for(int j=0;j<n1;j++){
				if(c.username[j]!=uname[j]){
					flag=0;
					break;
				}
			}
			if(flag) return 0;
		}
	}
	return 1;
}
void add(char uname[],char passwd[],short type,int sd){
	long long account_num;
	struct Customer c,c2;
	struct Account a,a2;
	int fdc=open("customer",O_RDWR);
	int fda=open("accounts",O_RDWR);
    while (read(fdc,&c2,sizeof(c2))>0)
	{
		if(c2.username[0]=='\0'){
			lseek(fdc,-sizeof(c),SEEK_CUR);
			break;
		}
	}
	while (read(fda,&a2,sizeof(a2))>0)
	{
		if(a2.account_number[0]=='\0'){
            lseek(fda,-sizeof(a2),SEEK_CUR);
			break;
		}
	}
	int n=strlen(uname),i;
	for(i=0;i<n;i++){
		c.username[i]=uname[i];
	}
	c.username[i]='\0';
	n=strlen(passwd);
	for(i=0;i<n;i++) c.password[i]=passwd[i];
	c.password[i]='\0';
	c.type=type;
	account_num=((long long)rand()<<39)^(((long long)rand()<<26)^(((long long)rand()<<13)^((long long)rand())));
	if(account_num<0) account_num=-account_num;
	for(i=0;i<16;i++){
		c.account_number[i]='0'+(account_num%10);
		account_num/=10;
	    a.account_number[i]=c.account_number[i];
	}
	c.account_number[i]='\0';
	a.account_number[i]='\0';
	a.balance=0;
	int k=-1;
	if(write(fdc,&c,sizeof(c))==-1){
        write(sd,&k,sizeof(k));
		return;
	}
	if(write(fda,&a,sizeof(a))==-1){
		write(sd,&k,sizeof(k));
		return;
	}
	k=1;
	write(sd,&k,sizeof(k));
}
void delete(char username[1024]){
int i,flag=1,cnt=0,fdcr=open("customer",O_RDONLY),fdcw=open("customer",O_WRONLY),fdar=open("accounts",O_RDONLY),fdaw=open("accounts",O_WRONLY),fdtr=open("transaction",O_RDONLY),fdtw=open("transaction",O_WRONLY);
	struct Customer c,tmp;
	struct Account a;
	struct Transaction t;
	while(read(fdcr,&c,sizeof(c))>0){
		int n1=strlen(username);
		int n2=strlen(c.username);
		if(n1==n2){
			flag=1;
			for(i=0;i<n1;i++) 
			{
				if(username[i]!=c.username[i]){
				flag=0;
				break;
			}
			}

			if(flag){
				tmp=c;
				lseek(fdcw,cnt*sizeof(c),SEEK_SET);
				c.username[0]='\0';
				c.password[0]='\0';
				c.type=0;
				c.account_number[0]='\0';
				write(fdcw,&c,sizeof(c));
				break;
			}
		}
		cnt++;
	}
	cnt=0;
	if(tmp.type==1){
		while(read(fdar,&a,sizeof(a))>0){
		int n1=strlen(tmp.account_number);
		int n2=strlen(a.account_number);
		if(n1==n2){
			flag=1;
			for(i=0;i<n1;i++) 
			{
				if(tmp.account_number[i]!=a.account_number[i]){
				flag=0;
				break;
			}
			}

			if(flag){
				lseek(fdaw,cnt*sizeof(a),SEEK_SET);
				a.account_number[0]='\0';
				a.balance=0.0;
				write(fdaw,&a,sizeof(a));
				break;
			}
		}
		cnt++;
	}
	cnt=0;
	while(read(fdtr,&t,sizeof(t))>0){
		int n1=strlen(tmp.account_number);
		int n2=strlen(t.account_number);
		if(n1==n2){
			flag=1;
			for(i=0;i<n1;i++) 
			{
				if(tmp.account_number[i]!=t.account_number[i]){
				flag=0;
				break;
			}
			}

			if(flag){
				lseek(fdtw,cnt*sizeof(t),SEEK_SET);
				t.account_number[0]='\0';
				write(fdtw,&t,sizeof(t));
			}
		}
		cnt++;
	}
	}
}
void modify(char username[1024],int ch,int sd){
	if(ch==1){
		char muser[1024];
		read(sd,muser,sizeof(muser));
		int fd=open("customer",O_RDWR),flag,i;
		struct Customer c;
		while(read(fd,&c,sizeof(c))>0){
			int n1=strlen(c.username);
			int n2=strlen(username);
			if(n1==n2){
			flag=1;
			for(i=0;i<n1;i++) 
			{
				if(c.username[i]!=username[i]){
				flag=0;
				break;
			}
			}

			if(flag){
				int n=strlen(muser);
				for(i=0;i<n;i++){
					c.username[i]=muser[i];
				}
				c.username[i]='\0';
			    lseek(fd,-sizeof(c),SEEK_CUR);
				write(fd,&c,sizeof(c));
				break;
			}
		}

	}}
	else if(ch==2){
		char mpasswd[1024];
		read(sd,mpasswd,sizeof(mpasswd));
		int fd=open("customer",O_RDWR),flag,i;
		struct Customer c;
		while(read(fd,&c,sizeof(c))>0){
			int n1=strlen(c.username);
			int n2=strlen(username);
			if(n1==n2){
			flag=1;
			for(i=0;i<n1;i++) 
			{
				if(c.username[i]!=username[i]){
				flag=0;
				break;
			}
			}

			if(flag){
				int n=strlen(mpasswd);
				for(i=0;i<n;i++){
					c.password[i]=mpasswd[i];
				}
				c.password[i]='\0';
			    lseek(fd,-sizeof(c),SEEK_CUR);
				write(fd,&c,sizeof(c));
				break;
			}
		}
	}
	}
	else if(ch==3){
		short type;
		read(sd,&type,sizeof(type));
		int fd=open("customer",O_RDWR),flag,i;
		struct Customer c,tmp;
		while(read(fd,&c,sizeof(c))>0){
			int n1=strlen(c.username);
			int n2=strlen(username);
			if(n1==n2){
			flag=1;
			for(i=0;i<n1;i++) 
			{
				if(c.username[i]!=username[i]){
				flag=0;
				break;
			}
			}

			if(flag){
				write(sd,&c,sizeof(c));
				tmp=c;
				if(c.type==2 && type==1){
				}
				else if(c.type==1 && type==2){
					char user[1024],pass[1024];
					int isUni;
					read(sd,user,sizeof(user));
					int fdc=open("customer",O_RDONLY);
					if(isUnique(fdc,user)){
						isUni=1;
						write(sd,&isUni,sizeof(isUni));
						read(sd,pass,sizeof(pass));
						int n=strlen(pass);
				        for(i=0;i<n;i++){
					    c.password[i]=pass[i];
				}
				c.password[i]='\0';
				c.type=type;
				n=strlen(user);
				for(i=0;i<n;i++) c.username[i]=user[i];
				c.username[i]='\0';
				tmp.type=2;
				lseek(fd,-sizeof(c),SEEK_CUR);
				write(fd,&tmp,sizeof(tmp));
				lseek(fd,0,SEEK_END);
				write(fd,&c,sizeof(c));
					}
					else{
						isUni=0;
					write(sd,&isUni,sizeof(isUni));
					return;
					}
				}
				break;
			}
		}
		}
	}
	else return;
}
void searchCustomer(int sd){
	int fd=open("customer",O_RDONLY);
	struct Customer c;
	int check=1;
	while(read(fd,&c,sizeof(c))>0){
		 write(sd,&check,sizeof(check));
         write(sd,&c,sizeof(c));
	}
	check=0;
	write(sd,&check,sizeof(check));
}
int main()
{
	char uname[1024],passwd[1024];
	int i,j,flag,n1,n2,fda,fdt,fdc,fdcw,fdcr,repeat;
	double amt;
	struct sockaddr_in serv,cli;

    int sd = socket (AF_UNIX, SOCK_STREAM, 0);

   serv.sin_family = AF_UNIX;
   serv.sin_addr.s_addr =INADDR_ANY;
   serv.sin_port = htons (7000);
   if(bind (sd, (void*)&serv, sizeof (serv))<0){
	   perror("Error on binding:");
       exit(EXIT_FAILURE);
   }
   
   listen (sd, 5);
   int sz_cli=sizeof(cli);
   while(1)
   {
	   int nsd;
	if((nsd = accept (sd,(struct sockaddr*)&cli, &sz_cli))<0) {
perror("Error on accept:");
exit(EXIT_FAILURE);
}
	if(fork()==0){
		int fd=open("customer",O_RDWR);
	if(fd==-1){
		printf("Some unknown error occured\n");
		exit(0);
	}
	struct Customer c;
	read(nsd,uname,sizeof(uname));
	read(nsd,passwd,sizeof(passwd));
	repeat=0;
	while((i=read(fd,&c,sizeof(c)))>0){
		n1=strlen(uname);
		n2=strlen(c.username);
		if(n1==n2){
			flag=1;
			for(j=0;j<n1;j++){
				if(c.username[j]!=uname[j]){
					flag=0;
					break;
				}
			}
			if(flag){
				n1=strlen(passwd);
				n2=strlen(c.password);
				if(n1==n2){
					flag=1;
					for(j=0;j<n1;j++){
						if(c.password[j]!=passwd[j]){
							flag=0;
							break;
						}
					}
					if(flag){
						repeat=1;
						write(nsd,&repeat,sizeof(repeat));
						write(nsd,&c,sizeof(c));
						while(repeat){
							if(c.type==0){
                            int ch;
							read(nsd,&ch,sizeof(ch));
							char user[1024],pass[1024];
							switch (ch)
							{
							case 1:
								read(nsd,user,sizeof(user));
								int isUniq;
								fdc=open("customer",O_RDONLY),fda;
								if(isUnique(fdc,user)){
									isUniq=1;
									write(nsd,&isUniq,sizeof(isUniq));
									read(nsd,pass,sizeof(pass));
									short x;
									read(nsd,&x,sizeof(x));
									add(user,pass,x,nsd);
								}
								else{
									isUniq=0;
									write(nsd,&isUniq,sizeof(isUniq));
								}
								break;
							case 2:
							      read(nsd,user,sizeof(user));
								  delete(user);
								  break;
							case 3:
							      read(nsd,user,sizeof(user));
								  int ch;
								  read(nsd,&ch,sizeof(ch));
								  modify(user,ch,nsd);
								  break;
							case 4:
							      searchCustomer(nsd);
								  break;
							          
							      
							default:
							    repeat=0;
								break;
							}
						}
						else{
							int ch;
						read(nsd,&ch,sizeof(ch));
						switch (ch)
						{
						case 1:
							read(nsd,&amt,sizeof(amt));
							fda=open("accounts",O_RDWR);
							fdt=open("transaction",O_RDWR|O_APPEND);
							deposit(fda,fdt,c.account_number,amt,nsd);
							break;
						case 2:
							read(nsd,&amt,sizeof(amt));
							fda=open("accounts",O_RDWR);
							fdt=open("transaction",O_RDWR|O_APPEND);
							deposit(fda,fdt,c.account_number,-amt,nsd);
							break;
						case 3:
					        fda=open("accounts",O_RDONLY);
							balance(fda,c.account_number,nsd);
							break;
						case 4:
						    read(nsd,&c,sizeof(c));
							lseek(fd,-sizeof(c),SEEK_CUR);
							write(fd,&c,sizeof(c));
							break;
						case 5:
						    fdt=open("transaction",O_RDONLY);
							print_Transactions(fdt,c.account_number,nsd);
							break;
						case 6:
						    repeat=0;
							break;
						    
						default:
						repeat=0;
							break;
						}
						}
						}
						break;
					}
				}
			}
		}
	}
	if(repeat==0) write(nsd,&repeat,sizeof(repeat));
	}
    else{
		close(nsd);
	}
   }
	return 0;
}
