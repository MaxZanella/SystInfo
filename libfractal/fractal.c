#include <stdlib.h>
#include "fractal.h"

struct fractal *fractal_new(const char *name, int width, int height, double a, double b)
{
  printf("1\n");
  struct fractal * fract = (struct fractal *) malloc(sizeof(struct fractal));
  printf("1\n");
   if(fract==NULL)
   {
     printf("FRACT NULL\n");
     return NULL;

   }
   printf("1\n");
     printf("DANS FRACTAL NEW\n");
     fract->name = name; //Checker si bonne taille?
     printf("DANS FRACTAL NEW\n");
     fract->width = width;
     printf("fractal_new width = %d", fract->width);
     fract->height = height;
     fract->r = a;
     fract->c = b;
     printf("DANS FRACTAL NEW\n");
/*     int * r= (int *)malloc(sizeof(int)*width*height);
     if(r==NULL)
     {
       return NULL;
     }
     else{
     fract->tab = r;

   }

     printf("DANS FRACTAL NEW\n");
     if(fract->tab==NULL)
     {
       return NULL;
     }
     */
     printf("DANS FRACTAL NEW11\n");
     fract->moyenne=0;
     printf("DANS FRACTAL NEW22\n");
      return fract;


}

void fractal_free(struct fractal *f)
{

    free(f->tab);
    free(f); //Verifier si bien desalloue??
}

const char *fractal_get_name(const struct fractal *f)
{

    return f->name;
}

int fractal_get_value(const struct fractal *f, int x, int y)
{
    printf("GET VALUE\n");
    printf("GET VALUE\n");
    printf("GET VALUE\n");
    printf("GET VALUE\n");
    printf("GET VALUE\n");

    return *(f->value + y*f->width + x);


}

void fractal_set_value(struct fractal *f, int x, int y, int val)
{
  printf("SET VALUE\n");
  printf("SET VALUE\n");
  printf("SET VALUE\n");
  printf("SET VALUE\n");
  printf("SET VALUE\n");
  printf("SET VALUE\n");
  printf("SET VALUE\n");
  printf("SET VALUE\n");

  f->tab[(f->width)*y +x]=val;
}

int fractal_get_width(const struct fractal *f)
{

    return f->width;
}

int fractal_get_height(const struct fractal *f)
{

    return f->height;
}

double fractal_get_a(const struct fractal *f)
{

    return f->r;
}

double fractal_get_b(const struct fractal *f)
{

    return f->c;
}
