/** 
    Title: client.c
    Authors: Marcelo Linck
    e-mails: marcelo.linck@acad.pucrs.br
             

    Version: 1.2
    Date: 11/11/2013
    Modified: 11/14/2013

    Description: Gallows Game using TCP connection with sockets - Client

**/


#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#define MSG_SIZE    80
#define N_CLIENTS   10
#define STR_SIZE    20
#define CHANCES     5
#define END_GAME    -10

void Title(){				//Cabeçalho do jogo 
	system("clear");
    printf("\tJOGO DA FORCA! \n\t\t-versao alfa-\n\n");
}

int HandleClient(char * IP, int port)	//Função que chama a parte cliente quando o jogo inicia
{
    int sock,
        size, // tamanho da mensagem
        i, // loop 
        j,
        ascii_,
        old_size,
        true_size = 0,
        end_game = 0,
        count_body = 0,
        hits = 0;

    struct sockaddr_in server;
    struct hostent *host, *gethostbyname();

    char    buffer[MSG_SIZE],
            ans[MSG_SIZE],
            hide_word[MSG_SIZE],
            word[MSG_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//Cria socket

    if (sock  <0) {
        fprintf(stderr,"Erro: sock()\n");
        exit(1);
    }

    host = gethostbyname(IP);
    if (host == 0) {
        fprintf(stderr,"%s: host desconhecido",IP);
        exit(1);
    }

    strncpy((char *)&server.sin_addr,(char *)host->h_addr, host->h_length);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    sleep(2);		//Conecta ao outro cliente
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        fprintf(stderr,"Erro: connect()\n");
        exit(1);
    }

    size=recv(sock, buffer, MSG_SIZE-1, 0); // recebe a palavra/frase
    buffer[size] = '\0';
    old_size = size; // para poder verificar no vetor da frase/palavra
    strcpy(word, buffer); // em word fica a palavra original
    do{
        Title();  
        printf("\t\n\n");  // balaca 
        true_size = 0; // vê quantas LETRAS tem
        for (i = 0; i < size; i++){ // percorre toda a palavra
            if (buffer[i] == ' '){ // se houver espaço
                hide_word[i] = ' '; // coloca em hide_word o espaço
                printf(" "); // imprime o espaço
            }
            else {
                hide_word[i] = '_'; // se houver LETRA coloca em hide_word o underline
                printf(" _ "); // imprime o underline
                true_size++;  // incrementa o número de letras    
            }
        }
        printf("\n\n\n"); 
        printf(" A palavra/frase tem %d posicoes.\n", true_size); // diz quantas letras tem 
        printf(" Digite uma letra para tentar adivinhar: "); // tá escrito
        gets(ans); // lê o que foi inserido
        ans[1] = '\0'; // para o caso de ser inserido mais de um caracter
        ascii_ = ans[0]; // para fazer a compraçao abaixo
    }while(ascii_ < 97 || ascii_ > 122); // apenas minusculas pela tabela ascii :-)
    printf("\n");    
    sprintf(buffer, "%s", ans); // coloca a resposta no buffer
    size = strlen(buffer);
        if (send(sock, buffer, size, 0) != size) // manda a letra
                fprintf(stderr,"Erro: send(%s)\n", buffer);  
    while(ascii_ > -3){ //-3 é palavra encontrada, -4 é fim de jogo 
        while(buffer[0] != '~'){ // enquanto nao receber esse sinal
            size = recv(sock, buffer, MSG_SIZE, 0); // recebe o resultado
            buffer[size] = '\0';
            j = 0;
            for (i = 0; i < old_size; i++){
                ascii_ = buffer[j]; // ascii_ recebe a posicao ou msg
                if (ascii_ == 125) // se for 125
                    ascii_ = 0; // ascii_ recebe zero
                else if (ascii_ == -2){ // se for -2 (letra nao encontrada)
                    count_body++; // conta o boneco :-)
                    printf("\tLetra nao encontrada\n"); // mostra na tela
                }
                else if (ascii_ == -3){ // se for -3 (palavra encontrada)
                    printf("\tPALAVRA ENCONTRADA!\n"); //imprime
                    end_game = END_GAME; // utilizada mais embaixo
                }
                else if (ascii_ == -4){ // fim de jogo
                    printf("\tFIM DE JOGO\n");
                    end_game = END_GAME; /// utilizada mais embaixo
                }
                if (ascii_ >= 0 && ascii_< MSG_SIZE && ascii_ != 126){ // se nao for 126 ('~') e dentro dos limites do vetor
                    if (i == ascii_){ // se for a posicao de ascii_
                        hide_word[i] = word[i]; // hide_word recebe a letra
                        if (size < old_size){ // gambiarra (nao me lembro o porquê disso)
                            j++; // proxima posicao 
                            hits++; // aumenta quantos acertos
                        }
                    }
                }
                else {
                    i = old_size; // caso seja msg de fim de jogo/palavra encontrada/letra nao encontrada sai do loop
                }
            }            
        }
        /************************************************
                        DESENHO DO BONECO
        ************************************************/
        Title();
        printf("\n\n");
        printf("\t_\n");
        printf("\t |\n");
        if (count_body == 1){
            printf("\t 0\n");
        }
        else if (count_body == 2){
            printf("\t<0\n");
        }
        else if (count_body == 3){
            printf("\t<0>\n");
        }
        else if (count_body == 4){
            printf("\t<0>\n");
            printf("\t |\n");
        }
        else if (count_body == 5){
            printf("\t<0>\n");
            printf("\t |\n");
            printf("\t/ 7\n");
            printf("\t____\n");
        }
        printf("\n\n");
        /***********************************************
                    IMPRIME O HIDE VECTOR
        ************************************************/
        for (i = 0; i < old_size; i++){
            printf(" %c ", hide_word[i]);
        }
        printf("\n\n");  
        if (end_game != END_GAME){ // se nao terminou ainda, faz o usuario mandar uma letra
            do{    
                printf(" -Caracteres: %d \n", true_size);
                printf(" -Acertos: %d\n", hits);
                printf("\n Digite uma letra para tentar adivinhar: ");
                gets(ans);
                ans[1] = '\0';
                ascii_ = ans[0];
            }while(ascii_ < 97 && ascii_ > 122); // apenas minusculas pela tabela ascii :-)
            printf("\n");    
            sprintf(buffer, "%s", ans);
            size = strlen(buffer);
            if (send(sock, buffer, size, 0) != size) // manda a letra
                    fprintf(stderr,"Erro: send(%s)\n", buffer);  
        }
        else{ // se terminou
            do{ 
            	printf("Precione ENTER para continuar.\n");
                gets(ans);
            	Title();
                printf("O jogo acabou!\nVoce deseja continuar?\n (1) Sim\n (2) Nao\n");
                gets(ans);
                ans[1] = '\0';
                ascii_ = ans[0];
            }while(ascii_ != 49 && ascii_ != 50);
            printf("\n");    
            sprintf(buffer, "%s", ans);            
            size = strlen(buffer);
            if (send(sock, buffer, size, 0) != size) // manda a letra
                    fprintf(stderr,"Erro: send(%s)\n", buffer); 
            ascii_ = -3;
        }
        /***************************************************************************************
                                        FIM DA LÓGICA DA FORCA
        *****************************************************************************************/ 
    }
    size=recv(sock, buffer, MSG_SIZE-1, 0); // recebe a palavra/frase
    buffer[size] = '\0';
    close(sock);
    if (buffer[0] == '1') return 1;		//Se ainda quiser jogar retorna 1
    else {			//Senao 0
        printf("O jogadores não desejam mais jogar.\n");
        return 0; 
    }

}

int HandleHost(int port)		//Função do cliente/servidor/host
{                                                 
    struct sockaddr_in  server; 
    struct sockaddr client;

    socklen_t client_size = sizeof(struct sockaddr);                               
 
    int     sock,  
            size,
            true_size = 0,
            i,
            j = 0,
            chances = 0,
            end_game = 0,
            hits = 0,
            word_size = 0,
            sock_connection;

    char    buffer[MSG_SIZE], 
            ans[MSG_SIZE],
            word[MSG_SIZE];

    /* Cria socket. */            
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock < 0) {                                            
        fprintf(stderr,"Erro: socket()\n");                 
        exit(1);                                           
    } 
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;                         
    server.sin_port = htons(port); 
    if (bind(sock, (struct sockaddr *)&server, sizeof server) < 0) {
        fprintf(stderr,"Erro: bind()\n");             
        exit(1);
    }

    /*Aguarda conexoes do cliente */ 
       
    listen(sock, N_CLIENTS);

    sock_connection = accept(sock, &client, &client_size);
    do{
        Title();
        printf("Jogo Iniciado!\n   Digite uma palavra/frase: "); 
        gets(word); // coloca a palavra/frase na variável word
        Title();
        printf("\n\n A palavra/frase é: "); // mostra que palavra/frase é 
        printf("%s\n", word); // mostra que palavra/frase é
        printf("  (1) Confirmar\n  (0) Modificar \n _"); // apenas para confirmar
        gets(ans); // pega a resposta           
    }while(ans[0] != '1');
    Title();
    printf("Adversario esta em sua primeira tentativa.\n"); 
    sprintf(buffer, "%s", word);  // coloca no buffer a palavra      
    size = strlen(buffer); // tamanho da frase/palavra
    word_size = size; // atribui o tamanho da palavra em word_size
    if (send(sock_connection, buffer, size, 0) != size) // manda a palavra/frase
            fprintf(stderr,"Erro: send(%s)\n", buffer);
    /**** Conta as letras => Também há no cliente ***/
    for (i = 0; i < size; i++){
        if (buffer[i] != ' ')
            true_size++;      
    }
    /**************************************************************************/
    while(END_GAME != end_game){ // loop para o "servidor" receber a letra do "cliente" e enviar os índices
        j = 0; // indice 
        size=recv(sock_connection, buffer, MSG_SIZE, 0); // recebe a letra 
        buffer[size] = '\0';
        chances++; // temos 5 chances VER O DEFINE CHANCES
        Title();
        printf(" Palavra: %s\n", word);
        printf(" Letra Recebida: %c\n", buffer[0]); // indica que letra foi recebida
        for (i = 0; i < word_size; i++){ // percorre toda a variavel word
            if (buffer[0] == word[i]){ // se a letra recebida bater com alguma do word 
                ans[j] = i; // o indice vai para o vetor ans
                if (ans[j] == 0) // se for 0 
                    ans[j] = 125; // assume 125 => '{', pois 0 em ascii é NULL
                sprintf(ans, "%c", ans[j]); // converte para char
                size = strlen(ans); // tamanho da mensagem
                if (send(sock_connection, ans, size, 0) != size) // manda a posicao UMA A UMA
                    fprintf(stderr,"Erro: send(%s)\n", ans); 
                j++; // incrementa j
                hits++; // incrementa os acertos
            }
            usleep(500); // tempo para enviar 
            if (i == word_size-1){ // se for a última volta no loop do size
                if (j == 0){ // nao houve acertos
                    printf(" Letra inexistente!\n"); // avisa
                    sprintf(ans, "%c", -2);  // -1 é o padrao para letra nao encontrada 
                    size = strlen(ans); // tamanho da mensagem
                    if (send(sock_connection, ans, size, 0) != size) // manda a msg
                        fprintf(stderr,"Erro: send(%s)\n", ans);
                    usleep(500); // tempo para enviar 
                }
                else if (j != 0){ // se houve incremento
                    chances--; //diminui as chances que foram incrementadas anteriormente
                    printf(" Letra Existe!\n"); // avisa
                }
                if (hits == true_size){ // palavra encontrada
                    printf("\tPALAVRA ENCONTRADA!\n"); //imprime
                    end_game = END_GAME; // para sair do loop
                    sprintf(ans, "%c", -3); // -3 é o padrao para palavra encontrada
                    size = strlen(ans);
                    if (send(sock_connection, ans, size, 0) != size) // manda a msg
                        fprintf(stderr,"Erro: send(%s)\n", ans);
                    sleep(1); // tempo para enviar -
                }
                else if (chances == CHANCES){ // sem mais chances
                    end_game = END_GAME; // para sair do loop
                    printf("\tFIM DE JOGO!\n"); // avisa
                    sprintf(ans, "%c", -4); //-4 é o padrao para fim de jogo
                    size = strlen(ans); // tamanho da msg
                    if (send(sock_connection, ans, size, 0) != size) // manda a msg
                        fprintf(stderr,"Erro: send(%s)\n", ans);
                    sleep(1); // tempo para enviar
                }
                sprintf(ans, "%s", "~"); // esse é o padrao para que o "cliente" pare de esperar
                size = strlen(ans);
                if (send(sock_connection, ans, size, 0) != size) // manda a msg
                    fprintf(stderr,"Erro: send(%s)\n", ans); 
            }
        }
        if (chances != CHANCES && end_game != END_GAME)
            printf(" Chances restantes: %d \n", CHANCES-chances); // diz quantas chances o adbserário ainda tem

        /***************************************************************************************
                                        FIM DA LÓGICA DA FORCA
        *****************************************************************************************/       
    }
    size=recv(sock_connection, buffer, MSG_SIZE, 0); 	//Recebe 1 se o adversario deseja jogar denovo
    buffer[size] = '\0';
    if (buffer[0]=='1') {		//Se quiser jogar denovo, manda resposta para o adversario
    	Title();
        printf("\n O adversario deseja jogar novamente.\nVoce deseja continuar?\n (1) Sim\n (2) Nao\n");
        gets(buffer);
        size = strlen(buffer);
        if (send(sock_connection, buffer, size, 0) != size) 
            fprintf(stderr,"Erro: send(%s)\n", buffer); 
        close(sock_connection);		//Fecha conexoes
        close(sock);
        Title();
        if(buffer[0]=='1') {		
        	printf("Adversario iniciando novo jogo.\n");
        	return 1;
        }
        else{
            if (send(sock_connection, buffer, size, 0) != size)
                fprintf(stderr,"Erro: send(%s)\n", buffer); 
            close(sock_connection);
            close(sock);
            return 0;
        } 
    }
    else {		//Se nao quiser mais jogar, fim de jogo
        printf("O jogador adversario não deseja mais jogar\n");
        if (send(sock_connection, buffer, size, 0) != size)
            fprintf(stderr,"Erro: send(%s)\n", buffer); 
        close(sock_connection);
        close(sock);
        return 0;
    }
} 

int main(int argc, char *argv[])
{
    int sock,
    	size,
        HostorClient=0;

    struct sockaddr_in server;
    struct hostent *host, *gethostbyname();

    char 	buffer[MSG_SIZE],
    	 	ans[MSG_SIZE];

    if (argc < 4) {
        printf(" Uso:\n\t%s <servidor_ip> <porta_servidor> <porta_jogo>\n", argv[0]) ;
        exit(1) ;
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);	//Cria socket com o servidor

    if (sock  <0) {
        fprintf(stderr,"Erro: sock()\n");
        exit(1);
    }

    host = gethostbyname(argv[1]);
    if (host == 0) {
        fprintf(stderr,"%s: host desconhecido",argv[1]);
        exit(1);
    }

    do{
        system("clear");
        printf("BEM VINDO AO 'JOGO DA FORCA'! \n\t\t-versao alfa-\n\n");
        printf("(1) Connectar-se ao servidor\n(0) Sair\n _");
        fflush(stdin);
        gets(ans);
        if(ans[0] != '1' && ans[0] != '0'){
            printf("\nResposta Invalida!\n");
            sleep(1);
        }
        else if (ans[0] == '0') exit(1);
    }while(ans[0]!='1');

    strncpy((char *)&server.sin_addr,(char *)host->h_addr, host->h_length);
    server.sin_family = AF_INET;
    server.sin_port = htons(atoi(argv[2]));
    //tenta conexao com o servidor
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        fprintf(stderr,"Erro: connect()\n");
        exit(1);
    }

	Title();
    printf("\n\n Usuario: ");
    gets(buffer);
    send(sock, buffer, strlen(buffer), 0);	//Captura nome do usuario e manda ao servidor
    printf("\n Usuario %s conectado com sucesso!\n", buffer);
    do{
	    sleep(1);
	    do{
	        Title();		//Recebe lista de jogadores conectados 
	        printf("Lista de jogadores:\n\tIndice\tUsuario\tStatus\n");
	        do{
	            size=recv(sock, buffer, MSG_SIZE-1, 0);
	            buffer[size] = '\0';	     
	            if(buffer[0]=='.') break;
	            printf("%s", buffer);
	        }while(1);

	        printf("\n -Informar indice do Host\n -Atualizar (A)\n -Hospedar Jogo (H)\n _");
	        gets(buffer);	//Verifica, se quiser atualizar, virar Host ou entrar em jogo 
	        send(sock, buffer, strlen(buffer), 0);
	    }while(buffer[0] == 'A' || buffer[0] == 'a');

	    if(buffer[0] == 'H' || buffer[0] == 'h'){
	    	do{ 			//Se host, espera convite
		    	Title();
		    	printf(" Hospedando servidor, aguardando jogadores...\n");
		    	size=recv(sock, buffer, MSG_SIZE-1, 0);
		        buffer[size] = '\0';
		        printf("\n ***Convite de acesso recebido: %s. Aceita?  Sim(s) | Nao(n)\n _", buffer);
		        gets(buffer);
		        send(sock, buffer, strlen(buffer), 0);
                HostorClient = 1;
		    }while(buffer[0]!='s' && buffer[0]!='S');
		    break;
	    }

	    Title();	//Senao, envia indice para servidor e espera resposta do host
	    printf("\n\nAguardando resposta do Host.\n");
	    size=recv(sock, buffer, MSG_SIZE-1, 0);
		buffer[size] = '\0';
		if(buffer[0] == 'n') printf("\n Host não aceitou seu convite! Voltando ao menu...\n");
        HostorClient = 2;
	}while(buffer[0] == 'n');
	
	if(HostorClient==1) {
		size=recv(sock, buffer, MSG_SIZE-1, 0);
		buffer[size] = '\0';
	}

    int go, port = atoi(argv[3]);		//Captura porta de conexao entre clientes

    do{					//Se Host, chama HandleHost(server) senao HandleClient (client)
        if(HostorClient==1) go=HandleHost(port);
        else go=HandleClient(buffer, port);
        if(HostorClient==1) HostorClient=2;
        else HostorClient=1;
        sleep(2);
        port++;
    }while(go==1); //Se ambos concordam com outro jogo, reinicia.

    printf("\tFIM DE JOGO\n");
    gets(ans);
    close(sock);		//Fecha sock
    exit(0);
}

