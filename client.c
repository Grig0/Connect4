#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

/* codul de eroare returnat de anumite apeluri */
extern int errno;

/* portul de conectare la server*/
int port;
volatile int renunta;

int main(int argc, char *argv[])
{
	int sd, contor, jucator, scor=0;                  // descriptorul de socket
	struct sockaddr_in server;    // structura folosita pentru conectare
	char msg[100];                // mesajul trimis
	char msgrasp[100];

	/* exista toate argumentele in linia de comanda? */
	if (argc != 3)
	{
		printf("Sintaxa: %s <adresa_server> <port>\n", argv[0]);
		return -1;
	}

	/* stabilim portul */
	port = atoi(argv[2]);

	/* cream socketul */
	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror("Eroare la socket().\n");
		return errno;
	}

	/* umplem structura folosita pentru realizarea conexiunii cu serverul */
	/* familia socket-ului */
	server.sin_family = AF_INET;
	/* adresa IP a serverului */
	server.sin_addr.s_addr = inet_addr(argv[1]);
	/* portul de conectare */
	server.sin_port = htons(port);

	/* ne conectam la server */
	if (connect(sd, (struct sockaddr *) &server, sizeof(struct sockaddr)) == -1)
	{
		perror("[client]Eroare la connect().\n");
		return errno;
	}

	/* citirea mesajului waiting for players */
	bzero(msg, 100);
	printf("[client]Asteptam mesaj(wait) de la server\n ");
	fflush(stdout);
	if (read(sd, msg, 100)<0)
	{
		perror("[client]Eroare la read(1) de la server\n");
		return errno;
	}
	printf("%s\n", msg);

	bzero(msg, 100);
	printf("[client]Asteptam mesaj(start) de la server\n");
	fflush(stdout);
	if (read(sd, msg, 100)<0)
	{
		perror("[client]Eroare la read(2) de la server");
		return errno;
	}
	printf("%s\n", msg);

	//Asteptam culorile si ordinea de la server

	bzero(msg, 100);
	printf("[client]Asteptam mesaj(ordinea si culoarea) de la server\n");
	fflush(stdout);
	if (read(sd, msg, 100)<0)
	{
		perror("[client]Eroare la read() de la server");
		return errno;
	}
	printf("%s\n", msg);
	if (strcmp("Ai primit culoarea rosie \nPoti face prima mutare.", msg) == 0) jucator = 0; else jucator = 1;
	// incepen jocul
_play_again:
	contor = 0;
	renunta = 0;
	while (1){

		// primim tabla de joc
		bzero(msg, 100);
		printf("[client]Connect4\n");
		fflush(stdout);
		if (read(sd, msg, 100)<0)
		{
			perror("[client]Eroare la read() de la server");
			return errno;
		}
		printf("%s\n", msg);
		// decizie pt continuarea jocului
		if (strstr(msg, "Ai")){
			int  ok = 1;
			/*char *decizie;*/	/*din cauza faptului ca l-am declarat asa era sa imi pun capat zilelor*/
			char decizie[100];
			//primim scorul
			bzero(msg, 100);
			printf("[client]Scor: ");
			fflush(stdout);
			if (read(sd, msg, 100)<0)
			{
				perror("[client]Eroare la read() de la server");
				return errno;
			}
			printf("%s\n", msg);
			//tratam cazul in care un jucator renunta
			pid_t pid;
			if ((pid = fork()) < 0){
				perror("fork() error");
			}
			else if (pid == 0){				//in copil se va astepta raspunsul server-ului, in cazul in care adversarul renunta
				bzero(msg, 100);
				fflush(stdout);
				if (read(sd, msg, 100) < 0)
				{
					perror("[client]Eroare la read() de la server");
					return errno;
				}
				printf("%s\n\t\tGame over\nApasati enter pt a iesi.", msg);
				renunta = 1;
				exit(0);
			}
			while (ok == 1){
				printf("\n\n\tJucati din nou?   Y/N\n");
				bzero(decizie, 100);
				fflush(stdout);
				scanf("%s", decizie); //read(0, decizie, 100);
				if (renunta == 1) return 0;
				if (decizie[1] == '\0' && (decizie[0] == 'Y' || decizie[0] == 'y' || decizie[0] == 'N' || decizie[0] == 'n'))
					ok = 0;
			}
			kill(pid, SIGKILL);		//Omoara fortat procesul copil astfel incat sa nu ramana nici un proces zombie
			//kill(j**, SIEGHEIL);	just a joke :D
			if (write(sd, decizie, 100) <= 0)
			{
				perror("[client]Eroare la write(decizie) spre server.\n");
				return errno;
			}
			else printf("[client]Mesajul decizie afost trimis catre servar\n");
			if (decizie[0] == 'N' || decizie[0] == 'n'){
				printf("\tGame over\n");
				return 0;
			}
			//else{ printf("Asteptati raspunsul adversarului\n ") }
			//asteapta raspunsul serverului
			bzero(msg, 100);
			fflush(stdout);
			if (read(sd, msg, 100) < 0)
			{
				perror("[client]Eroare la read() de la server");
				return errno;
			}
			printf("%s\n", msg);
			if (strstr(msg,"Vei fi deconectat")) return 0;
			bzero(msg, 100);
			fflush(stdout);
			if (read(sd, msg, 100) < 0)
			{
				perror("[client]Eroare la read() de la server");
				return errno;
			}
			printf("%s\n", msg);
			goto _play_again;
		}
		if (contor % 2 == jucator){
		_repeta:
			printf("[client]Introduceti numarul coloanei pe care vreti sa asezati piesa.");

			//trimitem mesajul la server
			bzero(msgrasp, 100);
			fflush(stdout);
			read(0, msgrasp, 100);		//read in loc de "scanf". cuz why not? :D

			if (write(sd, msgrasp, 100) <= 0)
			{
				perror("[client]Eroare la write(cuvant) spre server.\n");
				return errno;
			}
			//validarea raspunsului din partea serverului
			bzero(msg, 100);
			fflush(stdout);
			if (read(sd, msg, 100) < 0)
			{
				perror("[client]Eroare la read() de la server");
				return errno;
			}
			printf("%s\n", msg);
			if (strcmp("Numarul coloanei este incorect", msg) == 0 || strcmp("Coloana este deja plina", msg) == 0) goto _repeta;
		}
		else{
			bzero(msg, 100);
			printf("[client]Asteapta sa mute adversarul");
			fflush(stdout);
			if (read(sd, msg, 100) < 0)
			{
				perror("[client]Eroare la read() de la server");
				return errno;
			}
			printf("%s\n", msg);
		}

		contor++;

	}
	close(sd);
}

/*		X || X || 0
	   =============
	    0 || 0 || X
	   =============
	    X || 0 || X
		
*/
