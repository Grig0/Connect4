/*Draga eu din viitor, sper ca vei stii mai multe despre client server decat mine*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <signal.h>

/* portul folosit */
#define PORT 2057

/* codul de eroare returnat de anumite apeluri */
extern int errno;

bool castigator(char tabla[100][100]){
	char culoare; int i, j;
	for ( i = 5; i >= 0; i--){
		for ( j = 0; j < 7; j++){
			//cout << tabla[i][j];
			if (tabla[i][j] != 'O'){
				if (j <= 7 - 4)
					if (tabla[i][j] == tabla[i][j + 1] && tabla[i][j + 1] == tabla[i][j + 2] && tabla[i][j + 2] == tabla[i][j + 3] ||	//verif linie
						tabla[i][j] == tabla[i - 1][j + 1] && tabla[i - 1][j + 1] == tabla[i - 2][j + 2] && tabla[i - 2][j + 2] == tabla[i - 3][j + 3]) return true; //diagonala dreapta
				if (i >= 6 - 4){
					if (tabla[i][j] == tabla[i - 1][j] && tabla[i - 1][j] == tabla[i - 2][j] && tabla[i - 2][j] == tabla[i - 3][j]) return true;//verif coloana
					if (j>2)
						if (tabla[i][j] == tabla[i - 1][j - 1] && tabla[i - 1][j - 1] == tabla[i - 2][j - 2] && tabla[i - 2][j - 2] == tabla[i - 3][j - 3]) return true;//diagonala stanga
				}
			}
		}
	}
	return false;
}

int main ()
{
  struct sockaddr_in server;    // structura folosita de server
  struct sockaddr_in from;
  char msg[100];                //mesajul primit de la client
  char msgrasp[100]=" ";        //mesaj de raspuns pentru client
  int sd;                       //descriptorul de socket

  /* crearea unui socket */
  if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
      perror ("[server]Eroare la socket().\n");
      return errno;
    }

  /* pregatirea structurilor de date */
  bzero (&server, sizeof (server));
  bzero (&from, sizeof (from));

  /* umplem structura folosita de server */
  /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
  /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
  /* utilizam un port utilizator */
    server.sin_port = htons (PORT);

  /* atasam socketul */
  if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
      perror ("[server]Eroare la bind().\n");
      return errno;
    }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen (sd, 2) == -1)
    {
      perror ("[server]Eroare la listen().\n");
      return errno;
    }
/* servim in mod iterativ clientii... */
  while (1)
    {
      int client[10];
      int length = sizeof (from);
	  int i = 0, ok, j, n = 6, m = 7, contor, coloana, scor[2] = { 0, 0 }; char  tabla[100][100];
      printf ("[server]Asteptam la portul %d...\n",PORT);
      fflush (stdout);
        while(i<2)
        {
      /* acceptam numarul necesar de clienti (stare blocanta pina la realizarea conexiunii) */
			client[i] = accept (sd, (struct sockaddr *) &from, &length);

      /* eroare la acceptarea conexiunii de la un client */
			if (client[i] < 0)
			{
				perror ("[server]Eroare la accept().\n");
				continue;
			}
			else
                {/*s-a realizat conexiunea, se pregateste mesajul pt asteptarea oponentului*/
        bzero(msgrasp, 100);
        strcat(msgrasp,"Asteapta logarea unui adversar\n");
        /*se trimite un  mesaj care anunta clientul ca trebuie sa mai astepte*/
        if(write(client[i], msgrasp,100)<=0)
                {
                perror ("[server]Eroare la write() catre client1.\n");
                continue;
                }
        else{
           printf("[server]Mesajul(wait) a fost trimis cu succes.\n");
	   i++;}
                }
        }
        /*asta inseamna ca s-au conectat ambii jucatori */
        bzero(msgrasp,100);
        strcat(msgrasp, "Start");
        /*se trimit iar mesaje la ambii jucatori pt a anunta inceperea jocului*/
        for(j=0;j<2;j++)
        {
        if(write(client[j],msgrasp,100)<=0)
                {
                perror ("[server]Eroare la write() catre client2.\n");
                continue;
                }
        else printf ("[server]Mesajul(start) a fost trimis cu succes.\n");
	
        }
	
        //anuntam primul client ca va primi culoarea rosie si va incepe primul
	bzero(msgrasp,100);
        strcat(msgrasp, "Ai primit culoarea rosie \nPoti face prima mutare.");
	 if(write(client[0],msgrasp,100)<=0)
                {
                perror ("[server]Eroare la write() catre primul client.\n");
				close(client[0]);
				close(client[1]);
                }
        else printf ("[server]Mesajul(first move) a fost trimis cu succes.\n");

	//anuntam al doile client ca va primi culoarea galbena si va trebui sa astepte sa mute primul jucator 
	bzero(msgrasp,100);
        strcat(msgrasp, "Ai primit culoarea galbena\nAsteapta sa faca adversarul prima mutare");
         if(write(client[1],msgrasp,100)<=0)
                {
                perror ("[server]Eroare la write() catre al doilea client client.\n");
                continue;
                }
        else printf ("[server]Mesajul(second move) a fost trimis cu succes.\n");
		
	// incepe jocul
		int continua = 1;
		_play_again:
		for (i = 0; i < n; i++)
			for (j = 0; j < m; j++)
				tabla[i][j] = 'O';
		char mesaj_tabla_joc[100] = "OOOOOOO\nOOOOOOO\nOOOOOOO\nOOOOOOO\nOOOOOOO\nOOOOOOO\n\n1234567";
		contor = 0; ok = 0;
		int jucator ;

		while (continua)
		{
			//afisam tabla de joc
			for (i = 0; i < 2; i++){
				bzero(msgrasp, 100); 
				strcat(msgrasp, mesaj_tabla_joc);
				//verificare castigator
				if (castigator(tabla)) if (contor % 2 == i) strcat(msgrasp, "\n\n\tAi pierdut\n\n"); else { strcat(msgrasp, "\n\n\tAi castigat\n\n"); ok = 1; scor[i]++; }
				else
					if (contor == n*m) {
					strcat(msgrasp, "\n\n\tEgalitate\n\n"); 
					ok = 1;
					}
				if (write(client[i], msgrasp, 100) <= 0)
				{
					perror("[server]Eroare la write() catre client.\n");
					close(client[0]);
					close(client[1]);
				}
				else printf("[server]Mesajul(tabla de joc) a fost trimis cu succes.\n");
			}
			
			//continuare jocului la decizia clientului

			/*chestia de mai jos e inspaimantatoare, nu stiu daca o voi continua..... vreodata. 
			UPDATE: Dupa multe moficari de cod in client si server, tone de printf-uri pt a descoperi 
			unde da segmentation fault si ore intregi pierdute pe cod care mergea rulat ca si program separat
			fara sa dea segmentation fault, am reusit in sfarsit sa rezolv aceasta problema fara a intra in 
			depresie (prea mult) sau a deveni (si mai) alcolic*/
			
			
			if (ok == 1){
				char decizie[100],scor_s[45];
				int copil = 1, parinte = 1;
				pid_t pid;
				if ((pid = fork()) < 0) {								//fork
					perror("fork() error");
				}
				else if (pid == 0) jucator = 1; else jucator = 0;		/*fork creat cu scopul de a servi clientii simultan (ma rog nu e chiar simultan) 
																		  da stiu, dar cel putin functioneaza (yup, si asta e tot ce conteaza)*/
				bzero(msgrasp, 100);
				sprintf(msgrasp, "%d", scor[jucator]);							//|		Concatenam scorul astfel incat fiecare jucator
				strcat(msgrasp, " - ");											//|				sa il poata vedea corespunzator	
				sprintf(scor_s, "%d", scor[jucator ^ 1]);						//|
				strcat(msgrasp, scor_s);										//|				 primul numar = scorul propriu
				if (write(client[jucator], msgrasp, 100) <= 0)					//|			   a doilea numar = scorul adversarului
				{
					perror("[server]Eroare la write() catre client.\n");
					close(client[0]);
					close(client[1]);
				}
				else printf("[server]Mesajul(scor) a fost trimis cu succes.\n");

				printf("[server]Asteptam decizia jucatorilor...\n");
				bzero(decizie, 100);
				fflush(stdout);
				if (read(client[jucator], decizie, 100) <= 0)
				{
					perror("[server]Eroare la read() de la client.\n");
					close(client[jucator]);    // inchidem conexiunea cu clientul 

				}
				else {
					printf("[server]:Mesajul a fost receptionat...%s\n", decizie);
				}
				if (decizie[0] == 'Y' || decizie[0] == 'y'){
					if (pid == 0) copil = 0;
					else parinte = 0;
					bzero(msgrasp, 100);
					strcat(msgrasp, "\tAsteptati decizia adversarului.");
					if (write(client[jucator], msgrasp, 100) <= 0)
					{
						perror("[server]Eroare la write() catre client.\n");
						close(client[0]);
						close(client[1]);
					}
					else printf("[server]Mesajul(asteapta continuarea) a fost trimis cu succes.\n");
					}
				if (decizie[0] == 'N' || decizie[0] == 'n'){
					close(client[jucator]);
					bzero(msgrasp, 100);
					strcat(msgrasp, "\tAdversarul a renuntat. Vei fi deconectat.");
					if (write(client[jucator ^ 1], msgrasp, 100) <= 0)
					{
						perror("[server]Eroare la write() catre client.\n");
						close(client[0]);
						close(client[1]);
					}
					else printf("[server]Mesajul(game over) a fost trimis cu succes.\n");
					sleep(3);
					close(client[jucator ^ 1]);
					return 0;
				}
				if (pid == 0)			exit(0);//terminare copil
				else
				{
					int returnStatus;
					waitpid(pid, &returnStatus, 0);  // Parent process waits here for child to terminate.
					kill(pid, SIGKILL);				// ma asigur ca nu ramane nici un proces zombie
				}
				i = 0;
				while (i < 2){
					bzero(msgrasp, 100);
					strcat(msgrasp, "\tPuteti continua jocul.\n");
					if (write(client[i], msgrasp, 100) <= 0)
					{
						perror("[server]Eroare la write() catre client.\n");
						close(client[0]);
						close(client[1]);
					}
					else printf("[server]Mesajul(continuati jocul) a fost trimis cu succes.\n");
					i++;
				}
			}
			/*o desigur repetarea rundei in functie de decizie ce ar putea fi asa de greu, ce naiv am fost*/
			if (ok == 1) {
				goto _play_again;
			}
			//se alege clientul
			if (contor % 2 == 0) jucator = 0; else jucator = 1;
			/* se astepta mesajul de la client */
			_reincearca:
			bzero(msg, 100);
			printf("[server]Asteptam numarul...\n");
			fflush(stdout);
			if (read(client[jucator], msg, 100) <= 0)
			{
				perror("[server]Eroare la read() de la client.\n");
				close(client[jucator]);    /* inchidem conexiunea cu clientul */
				
			}
			else {
				printf("[server]:Mesajul a fost receptionat...%s\n", msg);
			}
			//asezarea piesei
			coloana = atoi(msg);
			if (coloana < 1 || coloana>7){
				printf("dar nu este corect\n");
				bzero(msgrasp, 100);
				strcat(msgrasp,"Numarul coloanei este incorect");
				if (write(client[jucator], msgrasp, 100) <= 0)
				{
					perror("[server]Eroare la write() catre client.\n");
					close(client[0]);
					close(client[1]);
				}
				else printf("[server]Mesajul(coloana gresita) a fost trimis cu succes.\n");
				goto _reincearca;
			}
			else{
				bzero(msgrasp, 100);
				strcat(msgrasp, "Numarul coloanei este bun");
				if (write(client[jucator], msgrasp, 100) <= 0)
				{
					perror("[server]Eroare la write() catre client.\n");
					close(client[0]);
					close(client[1]);
				}
				else printf("[server]Mesajul(confirmarea) a fost trimis cu succes.\n");
			}
			j = 5;coloana--;// numarul real al coloanei
			while (tabla[j][coloana] != 'O' && j > -1) j--;
			if (j < 0) {
				printf("dar nu este corect\n");
				bzero(msgrasp, 100);
				strcat(msgrasp, "Coloana este deja plina");
				if (write(client[jucator], msgrasp, 100) <= 0)
				{
					perror("[server]Eroare la write() catre client.\n");
					close(client[0]);
					close(client[1]);
				}
				else printf("[server]Mesajul(coloana gresita) a fost trimis cu succes.\n");
				goto _reincearca;
			}
			else{
				if (jucator == 0){
					tabla[j][coloana] = 'R';
					mesaj_tabla_joc[(j+1)*(m + 1) - (m - coloana+1)] = 'R';
				}
				else{
					tabla[j][coloana] = 'G';
					mesaj_tabla_joc[(j+1)*(m + 1) - (m - coloana +1)] = 'G';
				}
				bzero(msgrasp, 100);
				strcat(msgrasp, "\nRandul tau");
				if (write(client[jucator^1], msgrasp, 100) <= 0)	// <-- e frumos sa te joci cu bitii din cand in cand :D
				{
					perror("[server]Eroare la write() catre client.\n");
					close(client[0]);
					close(client[1]);
				}
				else printf("[server]Mesajul(urmatorul) a fost trimis cu succes.\n");
			}

			
			contor++;
		}						//jocul
    }			// while server
}		//main



