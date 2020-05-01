#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <MQTTClient.h>
#include "MQTTClientPersistence.h"

#define ADDRESS     "tcp://localhost:1883"
char PAYLOAD[100];
char TOPIC[50];
char TOPIC1[50];
char groupname[50];
char CLIENTID[50];
char* extension = ".txt";
#define QOS         2
#define TIMEOUT     100000L
volatile MQTTClient_deliveryToken deliveredtoken;
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char* payloadptr;
    printf("Message arrived\n");
    //printf("     topic: %s\n", topicName);
    //printf("   message: ");
    payloadptr = (char*) message->payload;
    for(i=0; i<message->payloadlen; i++)
    {
        putchar(*payloadptr++);
    }
    putchar('\n');
    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}
void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}
int main(int argc, char* argv[])
{
	int n;
	FILE *fp,*fpt,*fptrr;
	char g_name[50];
	char test[50],test1[50];
  int rc;
  char ch,ch1,ch2;
	  MQTTClient client;
    MQTTClient_deliveryToken token;
start:
	printf("\n");
    printf("Are you Registered Y/N \n");
    scanf("%c",&ch1);
    if(ch1 =='Y' || ch1 == 'y')
    {
    	printf("Enter Unique UserName \n");
		scanf("%s",TOPIC);
		getchar();
		fp = fopen("Users.txt", "a+");
		while(fgets(test ,sizeof(test),fp))
		{
			test[strcspn(test, "\n")]=0;
			if(strcmp(TOPIC, test) == 0)
			{
        strcpy(CLIENTID,TOPIC);
				goto Third;
			}
		}
		fclose(fp);
		printf("You are not Registered !!!!!!!!!!! \n");
			printf("Proceed for Registration Y/N \n");
			scanf("%c",&ch2);
			if(ch2 =='Y' || ch2 == 'y')
			{
			second:
				printf("Enter Unique UserName \n");
				scanf("%s",TOPIC);
				fp = fopen("Users.txt", "a+");
				fprintf(fp, "%s\n", TOPIC);
				printf("Registration Successsful !!!!!!!!!!! \n");
				fclose(fp);
        strcpy(CLIENTID,TOPIC);
				goto Third;
			}
			else
			{
				return 0;
			}
		}
		else
		{
			goto second;
		}
				
Third:
    MQTTClient_create(&client, ADDRESS, CLIENTID,MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 0;
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }
first:
	MQTTClient_subscribe(client, TOPIC, QOS);
  printf("\n");
    printf("1. Want to send personal message \n");
    printf("2. Want to create Group \n");
	printf("3. Want to join Group \n");
	printf("4. Want to send Group message \n");
	printf("5. Want to leave Group \n");
	printf("6. Quit Application \n");
	printf("Enter The Coice \n");
	scanf("%d",&n);
    
	getchar();
	if(n == 1)
	{
    

   	do{
   	printf("\n");
   	printf("Avaliabe Users \n");
   	fp = fopen("Users.txt", "a+");
   	while(fgets(test ,sizeof(test),fp))
   	{
   		puts(test);
   	}
   	fclose(fp);
   	printf("You want to send to ????????? \n");
   	gets(TOPIC1);
    char strr[20]=" ---From ";
   	printf("Enter the Message \n");
    gets(PAYLOAD);
    strcat(strr,TOPIC);
    strcat(PAYLOAD,strr);
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = (void*) PAYLOAD;
    pubmsg.payloadlen = strlen(PAYLOAD);
    pubmsg.qos = QOS;
    pubmsg.retained = 0;
    deliveredtoken = 0;
    MQTTClient_publishMessage(client, TOPIC1, &pubmsg, &token);
    printf("Waiting for publication "
            "on topic %s\n",TOPIC1);
    if(deliveredtoken == token)
    {
    	printf("Send Failed \n");
    	break;
    }
    printf("Send Again Y/x \n ");
    scanf("%c",&ch);
    getchar();
    }while(ch =='Y' || ch == 'y');

    goto first;
	}
	
	if(n == 2)
	{
		printf("Enter Group Name \n");
		gets(groupname);
    	char fileSpec[strlen(groupname)+strlen(extension)+1];
    	snprintf( fileSpec, sizeof( fileSpec ), "%s%s", groupname, extension );
    	fptrr = fopen("groups.txt","a+");
    	while(fgets(test1 ,sizeof(test1),fptrr))
		{
			test1[strcspn(test1, "\n")]=0;
			if(strcmp(fileSpec, test1) == 0)
			{
				printf("Same Group Name exist!!!!!!!!!!!\n ");
				fclose(fptrr);
				goto first;
			}
		}
    	fprintf(fptrr, "%s\n", fileSpec);
    	fclose(fptrr);
    	fpt = fopen( fileSpec, "a+" );
		fprintf(fpt, "%s\n", TOPIC);
		MQTTClient_subscribe(client, groupname, QOS);
		printf("You created and joined the Group::: %s\n\n",groupname);
		fclose(fpt);
		goto first;
	}
    
    if(n == 3)
    {
    	int  w=0;
    	printf("Avaliable groups and users in them \n");
    	fptrr = fopen("groups.txt","a+");
    	while(fgets(test1 ,sizeof(test1),fptrr))
   		{
   			char ffname[50];
   			strcpy(ffname,test1);
   			ffname[strcspn(test1, ".txt")]=0;
   			printf("%d  ",++w);
   			puts(ffname);
   			test1[strcspn(test1, "\n")]=0;
   			 	fpt = fopen(test1,"a+");
    			while(fgets(test ,sizeof(test),fpt))
   				{
   					printf("\t");
   					puts(test);
   				}
   				fclose(fpt);
   		}
   		fclose(fptrr);

   		printf("Which group you want to join \n");
   		gets(g_name);
   		char fileSpec[strlen(g_name)+strlen(extension)+1];
    	snprintf( fileSpec, sizeof( fileSpec ), "%s%s", g_name, extension );
    	char test2[50];
       	fpt = fopen( fileSpec, "a+" );
    	while(fgets(test2 ,sizeof(test2),fpt))
		{
			test2[strcspn(test2, "\n")]=0;
			if(strcmp(TOPIC, test2) == 0)
			{
				printf("You Are already a Member!!!!!!!!!!!\n ");
				fclose(fpt);
				goto first;
			}
		}
		fprintf(fpt, "%s\n", TOPIC);
		MQTTClient_subscribe(client, g_name, QOS);
		printf("You Successsfully Joined!!!!!!!!!!!\n ");
		fclose(fpt);
   		goto first;
    }
    
    if(n == 4)
    {
    	printf("In Which group you want to send Message \t");
    	gets(g_name);
    	//printf("%d\n",strlen(g_name));
    	int k=0,p=0;
    	fptrr = fopen("groups.txt","r");
    	while(fgets(test1 ,sizeof(test1),fptrr))
   		{
   			char ffname[50];
   			strcpy(ffname,test1);
   			ffname[strcspn(ffname, ".txt\n")]=0;
   			test1[strcspn(test1, "\n")]=0;
   				if(strcmp(ffname, g_name) == 0)
   				{
   					k++;
   					fpt = fopen(test1,"r");
   					while(fgets(test ,sizeof(test),fpt))
   					{
						test[strcspn(test, "\n")]=0;
						{
							if(strcmp(test, TOPIC) == 0)
							{
								p++;
								do{
									printf("Enter the Message \n");
    								gets(PAYLOAD);
   								    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    								pubmsg.payload = (void*) PAYLOAD;
    								pubmsg.payloadlen = strlen(PAYLOAD);
    								pubmsg.qos = QOS;
    								pubmsg.retained = 0;
    								deliveredtoken = 0;
    								MQTTClient_publishMessage(client, g_name, &pubmsg, &token);
    								printf("Waiting for publication "
            												"on topic %s\n",g_name);
    								if(deliveredtoken == token)
    									{
    										printf("Send Failed \n");
    										break;
    									}
    								printf("Send Again Y/x \n ");
    								scanf("%c",&ch);
    								getchar();
    								}while(ch =='Y' || ch == 'y');

							}
						}

   					}
   					if(p == 0)
   						{
   						printf("You Doesn't belongs to the mentioned Group!!!!!!\n");
   						fclose(fptrr);
   						goto first;
   						}
   				}

   		}
   		if(k == 0)
   		{
   			printf("Group doesn't Exist!!!!!!\n");
   			fclose(fptrr);
   			goto first;
   		}
   		fclose(fpt);
   		fclose(fptrr);
   		goto first;
    }

    if(n == 5)
    {

    	printf("Which group you want to leave \t");
    	gets(g_name);
    	int k=0,p=0,dl;
    	fptrr = fopen("groups.txt","r");
    	while(fgets(test1 ,sizeof(test1),fptrr))
   		{
   			char ffname[50];
   			strcpy(ffname,test1);
   			ffname[strcspn(ffname, ".txt\n")]=0;
   			test1[strcspn(test1, "\n")]=0;
   				if(strcmp(ffname, g_name) == 0)
   				{
   					dl=0;
   					k++;
   					fpt = fopen(test1,"r");
   					while(fgets(test ,sizeof(test),fpt))
   					{
						test[strcspn(test, "\n")]=0;
						dl++;
							if(strcmp(test, TOPIC) == 0)
							{
								p++;
								break;
   							}
   					}
   					if(p == 0)
   						{
   						printf("You Doesn't belongs to the mentioned Group!!!!!!\n");
   						fclose(fpt);
   						fclose(fptrr);
   						goto first;
   						}

   				}
   			}
   		if(k == 0)
   		{
   			printf("Group doesn't Exist!!!!!!\n");
   			fclose(fpt);
   			fclose(fptrr);
   			goto first;
   		}
   		fclose(fpt);
   		fclose(fptrr);
   		MQTTClient_unsubscribe(client,g_name);
   		printf("You are Removed from group :: %s\n",g_name);
   		strcat(g_name,".txt");
   		fpt = fopen(g_name,"r");
   		fp = fopen("temp.txt","w");
   		int count=1;
   		//printf(" dl = %d\n",dl);
   		while(fgets(test ,sizeof(test),fpt)!= NULL)
   		{
   			if(count != dl)
   				fputs(test,fp);
   			count++;
   		}
   		fclose(fp);
   		fclose(fpt);

   		remove(g_name);
   		rename("temp.txt",g_name);
   		goto first;
    }
   	
   

    if(n == 6)
    {
    MQTTClient_disconnect(client, 100000);
    MQTTClient_destroy(&client);
    return rc;
	}

	else
	{
		printf("Enter Valid Option!!!!!!!!!!!\n\n");
		goto first;
	}
}