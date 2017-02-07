/** 
    Title: server.c
    Authors: Marcelo Linck
    e-mails: marcelo.linck@acad.pucrs.br
             

    Version: 1.2
    Date: 11/11/2013
    Modified: 11/14/2013

    Description: Gallows Game using TCP connection with sockets - Server

**/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#define MSG_SIZE    80
#define N_CLIENTS   10
#define STR_SIZE    20

struct __player{
    char IP[STR_SIZE];
    char username[STR_SIZE];
    char status[STR_SIZE];        
    int connection;
};

int n_clients=0,
    sock_connection;

struct __player playerList[N_CLIENTS];

void * HandleClient(){			//thread do cliente
    int local_connection = sock_connection,
        size=0,
        i=0,
        j=0,
        index,
        my_index=n_clients-1,
        ok=0;

    char    buffer[MSG_SIZE],
    		comparer[STR_SIZE];

    size = recv(local_connection, buffer, MSG_SIZE, 0);	//Recebe opcao do cliente
    buffer[size] = '\0';
    strcpy(playerList[n_clients-1].username, buffer); 

    do {
	    do {
	        for(i=0;i<n_clients;i++){		//Arruma string da lista e envia
	            sprintf(buffer, "\t[%d]\t%s\t%s\n", 
	                    i, playerList[i].username, playerList[i].status);
	            size=strlen(buffer);
	            if (send(local_connection, buffer, size, 0) != size)
	                fprintf(stderr,"Erro: send(%s)\n", buffer);
	        }
	        sleep(1);
	        buffer[0]='.';		// '.' indica fim da lista
	        sleep(1);
	        size=strlen(buffer);
	        if (send(local_connection, buffer, size, 0) != size)
	            fprintf(stderr,"Erro: send(%s)\n", buffer);
	        size = recv(local_connection, buffer, MSG_SIZE, 0);
	        buffer[size] = '\0';		//recebe opcao do cliente
	    }while(buffer[0] == 'A' || buffer[0] == 'a');	//Se 'A' atualiza

	    if(buffer[0] == 'h' || buffer[0] == 'H'){	//Se H, muda status
	    	strcpy(playerList[my_index].status, "Hosting");
	    	while(1);
	    }
	    else{
	    		index = atoi(buffer);	//Se for um convite, verifica indice
		    	sprintf(buffer, "%s", playerList[my_index].username);
		    	size=strlen(buffer);	//Envia convite ao indice requisitado
		    	if (send(playerList[index].connection, buffer, size, 0) != size)
		            fprintf(stderr,"Erro: send(%s)\n", buffer);
		    	size = recv(playerList[index].connection, buffer, MSG_SIZE, 0);
		        buffer[size] = '\0';		//Espera resposta
		        if(buffer[0]=='n' || buffer[0]=='N'){
		        	if (send(local_connection, buffer, size, 0) != size)
	        			fprintf(stderr,"Erro: send(%s)\n", buffer);
		        }
		        else break;		//Inicia jogo 
	    }
	}while(1);

    strcpy(buffer, playerList[index].IP);	//Manda IP do Host para o cliente
	size=strlen(buffer);
    if (send(local_connection, buffer, size, 0) != size)
        fprintf(stderr,"Erro: send(%s)\n", buffer);
    strcpy(buffer, playerList[my_index].IP);
	size=strlen(buffer);
    if (send(playerList[index].connection, buffer, size, 0) != size)
        fprintf(stderr,"Erro: send(%s)\n", buffer);

	strcpy(playerList[my_index].status, "Jogando");		//Muda Status
	strcpy(playerList[index].status, "Jogando");

    close(playerList[index].connection);		//Fecha conexao com o servidor
    close(local_connection); 
}

int main(int argc, char **argv)
{                                                 
    pthread_t th[N_CLIENTS];
    if (argc != 2) {     
        fprintf(stderr, "Uso:\n\t%s <porta>\n", argv[0]);
        exit(1);
    }

    struct sockaddr_in  server; 
    struct sockaddr client;

    socklen_t client_size = sizeof(struct sockaddr);                               
 
    int     sock;

    char    buffer[MSG_SIZE];

    /* Cria socket. */            
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {                                            
        fprintf(stderr,"Erro: socket()\n");                 
        exit(1);                                           
    } 
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;                         
    server.sin_port = htons(atoi(argv[1])); 
    if (bind(sock, (struct sockaddr *)&server, sizeof server) < 0) {
        fprintf(stderr,"Erro: bind()\n");             
        exit(1);
    }

    /*Aguarda conexoes do cliente */ 
       
    listen(sock, N_CLIENTS);
    
    while(1){
    
        sock_connection = accept(sock, &client, &client_size);	//aceita conexao com cliente
        n_clients++; //aumenta numero de cliente conectados

        //Captura IP do cliente
        sprintf(playerList[n_clients-1].IP, "%d.%d.%d.%d",  client.sa_data[2]&0x000000ff,
                                                            client.sa_data[3]&0x000000ff,
                                                            client.sa_data[4]&0x000000ff,
                                                            client.sa_data[5]&0x000000ff);

        //seta variaveis da struct
        printf("Conectado ao %s \n", playerList[n_clients-1].IP);
        sprintf(playerList[n_clients-1].status, "Livre");
        playerList[n_clients-1].connection = sock_connection;

        //Inicia thread para o cliente
        if(pthread_create(&th[n_clients-1], NULL, HandleClient, NULL)!=0)
        {  
            fprintf(stderr,"Erro: pthread_create()\n");
            exit(1);
        }
    }
    close(sock);
    return(0);
} 
