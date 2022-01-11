
#include "stdes.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

char* itoa(int val, int base){
    static char buf[32] = {0};
    int is_neg = 0;
    if(val < 0){
        is_neg = 1;
        val *= -1;
    }
    int i = 30;
    for(; val && i ; --i, val /= base)
        buf[i] = "0123456789abcdef"[val % base];
    if(is_neg){
        buf[i] = '-';
        return &buf[i];
    }
    return &buf[i+1];
}

int isSpecialCharacter(char p){
    return p == '\n' || p == '\r' || p == '\t' || p == ' ' || p == '\v' || p == '\f' || p == '\0';
}

int parseSpecialCharacter(char *p, char c){
    switch(c){
        case 'n':
            *p = '\n';
            break;
        case 'r':
            *p = '\r';
            break;
        case 't':
            *p = '\t';
            break;
        case 'v':
            *p = '\v';
            break;
        case 'f':
            *p = '\f';
            break;
        case '0':
            *p = '\0';
            break;
        default:
            return 0;
    }
    return 1;
}

int isAlphanumericCharacter(char c){
    // printf("Is AlphanumericCharacter : |%c|, return -> %d\n", c, c >= 33 && c <= 126);
    return c >= 33 && c <= 126;
}

int isDigit(char c){
    // printf("Is digit : |%c|, return -> %d\n ", c, c >= 48 && c <= 57);
    return c >= 48 && c <= 57;
}

int evalCharacterType(int flag, char c){
    // Non digit alphanumerical characters
    if(!flag)
        return !isDigit(c);
    // All alphanumerical characters
    return isAlphanumericCharacter(c);
}

int evalSeparator(char p, int separator){
    // printf("Eval Separator : p -> |%c|, eval -> |%d|\n", p, p == separator);
    if(isSpecialCharacter(p) && isSpecialCharacter(separator)){
        // printf("EvalSeparator : arg_string_char -> |%c| separator -> |%c|\n", p, separator);
        return 1;
    }
    if(p == separator)
        return 1;
    return 0;
}

FICHIER *ouvrir(const char *nom, char mode){
    FICHIER *f = (FICHIER *)malloc(sizeof(FICHIER));
    if(mode == LECTURE){
        f->fd = open(nom, O_RDONLY); // On ouvre le descripteur de fichier en lecture seule
    }else if(mode == ECRITURE){
        f->fd = open(nom, O_CREAT | O_WRONLY, 0666); // On ouvre le descripteur de fichier avec les flags d'écriture et de creation au besoin
    }
    else{
        return NULL; // retourne NULL si le mode n'est pas bon
    }
    if(f->fd < 0){
        return NULL;
    }
    
    f->mode = mode;
    f->tampon_read = (char *)malloc(TAILLE_BUFFER*sizeof(char)); // Allocation mémoire du tampon de lecture
    f->tampon_write = (char *)malloc(TAILLE_BUFFER*sizeof(char));// Allocation mémoire du tampon d'écriture
    f->cursor_read = 0;
    f->cursor_write = 0;
    return f; // retourne une structure FICHIER* correctement initialisée
}


int fermer(FICHIER*f){
    free(f->tampon_read); // On libére la mémoire allouée au tampon de lecture
    free(f->tampon_write); // On libére la mémoire allouée au tampon d'écriture
    close(f->fd); // On ferme le descriptieur de fichier
    f = NULL;
    return 0;
}

int lire(void *p, unsigned int taille, unsigned int nbelem, FICHIER *f){
    int nb_Octets_Lu = 0;
    char * tmp = (char*) p;
    if(f->mode == LECTURE){
        int taille_octets = taille  * nbelem;
        if(taille == TAILLE_BUFFER)
            return read(f->fd, tmp, TAILLE_BUFFER);
        else if(taille < TAILLE_BUFFER && f->cursor_read == 0){
            f->nb_bytes_read = read(f->fd, f->tampon_read, TAILLE_BUFFER); // On lit taille_octets elements du descripteur de fichier fd dans le buffer tampon_read
            if(taille > f->nb_bytes_read)
                return -1;
            memcpy(tmp, &f->tampon_read[f->cursor_read], taille);
            f->cursor_read += taille;
            nb_Octets_Lu += taille;
        }

        while(f->cursor_read+taille <= f->nb_bytes_read && nb_Octets_Lu < taille_octets){
            memcpy(tmp, &f->tampon_read[f->cursor_read], taille);
            f->cursor_read += taille;
            nb_Octets_Lu += taille;
            if(f->cursor_read == TAILLE_BUFFER){
                f->cursor_read = 0;
                f->nb_bytes_read = read(f->fd, f->tampon_read, TAILLE_BUFFER);
            } else if(f->nb_bytes_read != TAILLE_BUFFER && f->nb_bytes_read - f->cursor_read < taille){
                break;
            } else if(TAILLE_BUFFER - f->cursor_read < taille){
                int lastBytes = TAILLE_BUFFER - f->cursor_read;
                memcpy(f->tampon_read, &f->tampon_read[f->cursor_read], lastBytes);
                f->cursor_read = lastBytes;
                f->nb_bytes_read = lastBytes;
                f->nb_bytes_read += read(f->fd, &f->tampon_read[f->cursor_read], TAILLE_BUFFER-lastBytes);
            }
        }
    }

    return nb_Octets_Lu;
}

int ecrire(const void *p, unsigned int taille, unsigned int nbelem, FICHIER *f){
    int nb_written_bytes = 0;
    char * tmp = (char *) p;
    f->cursor_write = 0;
    f->nb_bytes_written = 0;
    if(f->mode == ECRITURE){
        int taille_octets = taille  * nbelem; 
        if(taille > TAILLE_BUFFER)
            return f->nb_bytes_written;

        while(nb_written_bytes < taille_octets){
            if(*tmp == '\0')
                break;
            memcpy(&f->tampon_write[f->cursor_write], tmp, taille);
            f->cursor_write += taille;
            tmp += taille;
            nb_written_bytes += taille;
            if(f->cursor_write == TAILLE_BUFFER){
                f->nb_bytes_written += write(f->fd, f->tampon_write, TAILLE_BUFFER);
                f->cursor_read = 0;
            }
            if(f->cursor_write + taille > TAILLE_BUFFER){
                f->nb_bytes_written += write(f->fd, f->tampon_write, f->cursor_write);
                f->cursor_write = 0;
            }
        }
        if((*tmp == '\0' && f->cursor_write != 0) || taille_octets == 1 ){
            f->nb_bytes_written += write(f->fd, f->tampon_write, f->cursor_write);
        }
    }
    return f->nb_bytes_written;
}


int fecriref (FICHIER *f, const char *format, ...){
    va_list parametres;
    va_start(parametres, format);
    
    char * arg_string;
    int nbEcrit = -1, isPercentage = 0, arg_int;
    
    while(*format != '\0'){
        if(isPercentage){
            switch (*format){
                case 's':
                    arg_string = va_arg(parametres, char*);
                    nbEcrit += ecrire(arg_string, strlen(arg_string), 1, f);
                    isPercentage = 0;
                    break;
                
                case 'd':
                    arg_int = va_arg(parametres, int);
                    arg_string = itoa(arg_int, 10);
                    nbEcrit += ecrire(arg_string, strlen(arg_string), 1, f);
                    isPercentage = 0;
                    break;

                case 'c':
                    arg_int = va_arg(parametres, int);
                    // printf("Writting : %c\n", arg_int);
                    nbEcrit += ecrire(&arg_int, sizeof(char), 1, f);
                    isPercentage = 0;
                    break;
                
                default:
                    isPercentage = 0;
                    break;
            }
        } else if(*format == '%')
            isPercentage = 1;        
        else {
            nbEcrit += ecrire(format, 1, 1, f);
        }
        format++;
    }
    va_end(parametres);
    return nbEcrit;
}

int fliref (FICHIER *f, const char *format, ...){
    va_list parametres;
    va_start(parametres, format);

    char *arg_string, separator, tmp, parse_int[TAILLE_BUFFER];
    int *arg_int, nbLu = -1, nbReadCurrent = 0, continueRead = 1, isPercentage = 0, onlyAlphabets = 0, validRead = 1, idx_parse_int;
    char *arg_char;
    while(*format != '\0' && validRead){
        if(isPercentage){
            switch (*format){
                case 's':
                    arg_string = va_arg(parametres, char*);
                    if(*(format+1) == '%' && *(format+1) == 'd'){
                        onlyAlphabets = 1;
                    }
                    else{
                        onlyAlphabets = 0;
                        separator = *(format+1);
                    }

                    continueRead = 1;
                    do{
                        nbReadCurrent = lire(&tmp, 1, 1, f);
                        
                        if(nbReadCurrent == 0 || (evalSeparator(tmp, separator) && evalCharacterType(onlyAlphabets, tmp)))
                            continueRead = 0;
                        else{
                            nbLu += nbReadCurrent;
                            *arg_string = tmp;
                            arg_string++;
                        }
                    } while(continueRead);

                    *arg_string = '\0';

                    if(onlyAlphabets == 1)
                        isPercentage = 1;
                    else 
                        isPercentage = 0;
                    break;
                
                case 'd':
                    arg_int = va_arg(parametres, int *);
                    idx_parse_int = 0;
                    separator = *(format+1);
                    continueRead = 1;
                    do{
                        nbReadCurrent = lire(&tmp, 1, 1, f);
                        
                        if(nbReadCurrent == 0 || (evalSeparator(tmp, separator) && !isDigit(tmp)))
                            continueRead = 0;
                        else{
                            nbLu += nbReadCurrent;

                            parse_int[idx_parse_int++] = tmp;
                        }
                        
                    } while(continueRead);

                    parse_int[idx_parse_int] = '\0';

                    *arg_int = atoi(parse_int);
                    
                    if(separator == '%')
                        isPercentage = 1;
                    else 
                        isPercentage = 0;
                    break;

                case 'c':
                    arg_char = va_arg(parametres, char *);
                    
                    separator = *(format+1);

                    nbReadCurrent = lire(&tmp, 1, 1, f);
                    
                    nbLu += nbReadCurrent;
                    *arg_char = tmp;

                    if(separator == '%')
                        isPercentage = 1;
                    else 
                        isPercentage = 0;
                    break;
                
                default:
                    isPercentage = 0;
                    break;
            }
            format++;

        } else {
            if(*format == '%'){     
                isPercentage = 1; 
                format++;
            } else{
                if(isSpecialCharacter(*format) && isSpecialCharacter(tmp))
                    format++;
                else{
                    nbReadCurrent = lire(&tmp,1,1,f);
                    if(tmp != *format){
                        validRead = 0;
                    }
                    else
                        format++;
                }
                if(nbReadCurrent == 0){
                    validRead = 0;
                }
            }
        }     
    }
    va_end(parametres);
    return nbLu;
}




