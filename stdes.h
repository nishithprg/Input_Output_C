#ifndef _STDES_H
#define _STDES_H


#include<sys/types.h>
#include<sys/stat.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>

#define TAILLE_BUFFER 1000
#define ECRITURE 'E'
#define LECTURE 'L'

struct _ES_FICHIER{
    int fd;
    int nb_bytes_read;
    int cursor_read;
    char *tampon_read;
    int nb_bytes_written;
    int cursor_write;
    char *tampon_write;
    char mode;
};
typedef struct _ES_FICHIER FICHIER;

// FICHIER *stdout;
// FICHIER *stderr;

/* mode: 'L' = lecture, 'E' = écriture */
FICHIER *ouvrir(const char *nom, char mode);
int fermer(FICHIER*f);
int lire(void *p, unsigned int taille, unsigned int nbelem, FICHIER *f);
int ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f);
int vider(FICHIER *f);

int fecriref (FICHIER *f, const char *format, ...);
/* directly in stdout */
int ecriref (const char *format, ...);
int fliref (FICHIER *f, const char *format, ...);


#endif

