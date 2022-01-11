#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "stdes.h"

int main (int argc, char **argv)
{
  FICHIER *f1, *f2;
  // int n;
  if (argc != 3) exit (-1);

  f1 = ouvrir (argv[1], 'E');
  if (f1 == NULL) exit (-1);

  // n = fecriref (f1, "Test char: '%c' and string: '%s' with normal number 12\n", 'a', "bonjour");
  // fecriref (f1, "Test number: '%d' char written on the above line\n", n);
  
  // n = fecriref (f1, "Test char: %s and string: %s with normal number 12\n", "hey hey", "bonjour");
  // fecriref (f1, "Test number: '%c' '%c' '%c' '%c' '%c' char written on the above line\n", 'H', 'E', 'L', 'L', 'O');

  // for (n=-11;n<11;n++)
  //   fecriref (f1, "%d ", n);
  // fecriref (f1, "\n", n);

#if 0
  /* Error, as expected: */
  fecriref (f1, "Num: '%e'\n");
#endif
  
 //fermer (f1);

  {
    char c;
    //  int  d;
    char my_word[128];
    // // char w1[10], w2[10], w3[10];
    f2 = ouvrir (argv[2], 'L');
    
    fliref (f2, "tes%c h end %s", &c, my_word);
    printf("Read '%c' and '%s'\n", c, my_word);
    // // fliref (f2, "%s %s %s", w1, w2, w3);
    // // printf("Read '%s' and '%s' and '%s' from %s\n", w1, w2, w3, argv[2]);
    // // fliref (f2, "%s", w1);
    // // printf("Read '%s' from %s\n", w1, argv[2]);
    //  fliref (f2, "end %d %s", &d, my_word);
    //  fecriref (f1, "Read '%d' and '%s' from %s\n", d, my_word, argv[2]);
    // // fliref (f2, "end %d %s", &d, my_word);
    // // printf ("Now read '%d' and '%s' from %s\n", d, my_word, argv[2]);

     fermer (f2);
     fermer(f1);
  }
  return 0;
}
