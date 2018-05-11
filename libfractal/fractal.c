#include <stdlib.h>
#include "fractal.h"
#include <string.h>
struct fractal *fractal_new(const char *name, int width, int height, double a, double b)
{

  struct fractal * fract = (struct fractal *) malloc(sizeof(struct fractal));

   if(fract==NULL)
   {

     return NULL;

   }

     strcpy(fract->name,name);

     fract->width = width;

     fract->height = height;
     fract->r = a;
     fract->c = b;




     fract->moyenne=0;

    return fract;


}

void fractal_free(struct fractal *f)
{
    free(f->name);
    free(f->tab);
    free(f); 
}

const char *fractal_get_name(const struct fractal *f)
{

    return f->name;
}

int fractal_get_value(const struct fractal *f, int x, int y)
{

int width = f->width;
    return f->tab[width*y+x];


}

void fractal_set_value(struct fractal *f, int x, int y, int val)
{

int width = f->width;
  f->tab[width*y +x]=val;
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
