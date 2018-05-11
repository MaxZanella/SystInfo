#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "libfractal/fractal.h"
//Rajout
#include <pthread.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdint.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <SDL/SDL.h>

#define itoc(x) ((0x00ffffff/MAX_ITER)*(x))
#define MAX_ITER 4096
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
#define RMASK 0xff000000
#define GMASK 0x00ff0000
#define BMASK 0x0000ff00
#define AMASK 0x000000ff
#else
#define RMASK 0x000000ff
#define GMASK 0x0000ff00
#define BMASK 0x00ff0000
#define AMASK 0xff000000
#endif


/**** Declaration variable globale******/

int iter1=0; //compte le nbre de fractales crees
int iter2=0; // compte le nbre de fractales retiree du buffer1
int iter3=0; //compte le nbre de fractale calculee et ajoutee au buffer2
int iter4=0;// compte le nbre de fractales  comparees
char ** files;
int NLECTEURS=0; //nombre de threads de lecture

int tous=0; //si fichier bmp pour chaque fractale (-d) : 1 oui 0 non

/**Coordination Producteur-Consommateur (1)****/
pthread_mutex_t mutex1;
sem_t empty1;
struct fractal ** buffer1;
sem_t full1;
int count1 = 0; //endroit dans le buffer1 où se trouve la premiere case libre
/**Coordination Producteur-Consommateur (2)*****/
pthread_mutex_t mutex2;
sem_t empty2;
struct fractal ** buffer2;
sem_t full2;
int count2=0; //pour connaitre le dernier element de buffer2

/************************************************/


/**** Methodes provoquant une segmentation fault -> refefinie ici****/

int write_bitmap_sdl1(const struct fractal *f, const char *fname)
{

    SDL_Surface *back;
    SDL_Rect pix;
    int w, h, i, j, col;
    uint32_t pcol, rcol, gcol, bcol;
    w = f->width;
    h = f->height;

    back = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 32, RMASK, GMASK, BMASK, AMASK);
    if (!back)
        return -1;

    for (i = 0; i < w; i++) {
        for (j = 0; j < h; j++) {
            col = itoc(f->tab[(f->width)*j+i]);
            rcol = col & 0xff;
            gcol = (col >> 8) & 0xff;
            bcol = (col >> 16) & 0xff;
            pcol = SDL_MapRGB(back->format, rcol, gcol, bcol);

            pix.w = pix.h = 1;
            pix.x = i;
            pix.y = j;
            SDL_FillRect(back, &pix, pcol);
        }
    }

    if (SDL_SaveBMP(back, fname) < 0)
        return -1;

    SDL_FreeSurface(back);

    return 0;
}
void fractal_set_value1(struct fractal *f, int x, int y, int val)
{

  f->tab[f->width*y +x]=val;

}
static int iter_julia1(double zx, double zy, double a, double b, int it)
{
    /* prevent infinite loop */
    if (it > 4096)
        return 0;

    /* prevent leaving Julia set
     * if distance to origin >= 2
     */
    if (zx*zx + zy*zy >= 4.0)
        return it;

    /* compute next iteration
     * f(z) = z^2 + c
     * z = x + yi
     * c = a + bi
     */
    return iter_julia1(zx*zx - zy*zy + a, 2*zx*zy + b, a, b, it+1);
}

int fractal_compute_value1(struct fractal *f, int x, int y)
{

    double zx, zy;
    double a, b;
    int val;
    int w, h;



    a = fractal_get_a(f);
    b = fractal_get_b(f);

    w = fractal_get_width(f);
    h = fractal_get_height(f);

    zx = ((double)x / (double)w) * 3.0 - 1.5;
    zy = ((double)y / (double)h) * 3.0 - 1.5;

    val = iter_julia1(zx, zy, a, b, 0);

    fractal_set_value1(f, x, y, val);

    return val;
}

/*** en cas d'erreur***/

void error(int err, char*msg)
{
  fprintf(stderr,"%s a retourné %d, message d'erreur : %s\n",msg,err,strerror(errno));
  exit(EXIT_FAILURE);
}

/********Ouverture de fichier********/

int openFile (char *filename)
{

  int file = open((char *)filename,O_RDONLY);
  if(file!=0)
  error(file,"erreur lors d'ouverture d'un fichier");
  return file;
}

/*permet de lire une ligne et envoie la fractale correspondantes*/

struct fractal *readLine(char* ligne) {

  if(ligne[0]=='#')
  {
    return NULL;
  }
    char * name = (char *)malloc(sizeof(char)*64);
    char * buf = (char *) malloc(sizeof(char)*64);
    if(buf==NULL)
    {
      return NULL;
    }
    int width, height;
    double r, c;

    struct fractal * fract=NULL;

    int size=0;
    int err=0;

    char * cursor;

    char * line = (char *)malloc(sizeof(char)*100);
    if(line==NULL)
    {
      free(buf);
      return NULL;
    }

    strcpy(line,ligne);
    int i;
    for(i=0;i<4;i++)
    {

      cursor = strstr(line," ");

       size=strlen(line)-strlen(cursor);
       int j;
        for(j=0; j<size;j++)
        {
          buf[j]=line[j];
        }
        if(i==0)
        {

          strcpy(name,buf);
          strcpy(line,cursor+1);

        }
        else if(i==1)
        {
          width = atoi(buf);
          strcpy(line,cursor+1);

        }
        else if(i==2)
        {
          height = atoi(buf);
          strcpy(line,cursor+1);

        }
        else if(i==3)
        {

          r=atof(buf);
          strcpy(line,cursor+1);

        }
        else if(i==4)
        {
          c=atof(buf);


        }

      }
      c=atof(line);


      free(buf);
      free(line);
      if(err==1)
      {
        return NULL;
      }

    fract = (struct fractal *) malloc(sizeof(struct fractal));

       if(fract==NULL)
       {

         return NULL;

       }

    //  fract = fractal_new(name,width,height,r,c);
    fract->name = name;
    fract->width=width;
    fract->height=height;
    fract->r=r;
    fract->c=c;

      int * p= (int *)malloc(sizeof(int)*(fract->width)*(fract->height));

      if(p==NULL)
      {
        return NULL;
      }
      else{

      fract->tab = p;
    }

      return fract;
    }
    char * setzero (char * concat, int size)
    {
      int k;
      for(k=0;k<size;k++)
      {
        concat[k]=0;
      }
      return concat;
    }
/*methode de lecture de fichier
cree une ligne et la donne à readLine
renvoie une fractale */

  struct fractal * readFile(int file)
  {

    char * buf = (char *)malloc(sizeof(char));
    if(buf==NULL)
    {
      return NULL;
    }
    int lect = read(file,buf,sizeof(char));

    struct fractal * fract=NULL;

    while(lect>0)
    {

      while(fract==NULL && lect>0)
      {
      int newLine=0;
      int size =100;
      char * concat = (char *)malloc(sizeof(char)*size); //string qui va concaténer chaque caractère lu
      concat = setzero(concat,size); //! remise à zero
      int length=0;

      if(concat==NULL)
      {

        free(buf);
        return NULL;
      }

      while(newLine==0&&lect>0)
      {

        if(*buf=='\n')
        {

          newLine=1; //on a fini la ligne
        }
        else
        {
        concat[length]=*buf;
        length++;

        lect = read(file,buf,sizeof(char));
      }

      }


      fract = readLine(concat);

      free(concat);

    }
    if(fract != NULL)
    {
      free(buf);
      return fract;
    }
    }

    free(buf);
  return NULL;
    }


/******Producteur1 *******/
void *producteur1(void * file)
{
  int fd;
  char * filename = (char *) file;

  fd = open(filename,O_RDONLY);
  if(fd<0)
  {
    error(fd,"erreur lors de l'ouverture d'un fichier");
  }

  struct fractal * fract;

  fract = readFile(fd); // -->correspond a la lecture et creation d'une structure fractale

  while(fract != NULL)
  {

    sem_wait(&empty1); // attente slot libre
    pthread_mutex_lock(&mutex1); // acces au tableau et a la variable count1

    buffer1[count1]= fract; //ajout d'une fractale
    count1++;
    iter1++;//une fractale ajoutée de plus

    pthread_mutex_unlock(&mutex1); //fin d'acces
    sem_post(&full1);  //un slot rempli en plus

    fract = readFile(fd); // correspond a la lecture et creation d'une structure fractale

    if(fract==NULL)
    {


      pthread_mutex_lock(&mutex1);

      NLECTEURS--;

      pthread_mutex_unlock(&mutex1);
      if(NLECTEURS==0)
      {
        int boucle =0;
        while(boucle==0)
        {
          if(iter1==iter4 && iter3==iter1) // on attend que les autres threads avancent
          {

            sem_post(&empty2); //il y a un slot libre de plus
            boucle=1;
          }
        }

    }
    }


}


int err = close(fd);
if(err!=0)
{
  error(err,"fermeture fichier");
}
pthread_exit(NULL);
}


/*Lecture de la fractale en entrée standard*/
void *entreestandard(void * ligne)
{
    char * line = (char *) ligne;
    struct fractal * fract;
    if(line[0]!='+')
    {
    fract=readLine(line);
    sem_wait(&empty1); // attente slot libre
    pthread_mutex_lock(&mutex1); // acces au tableau et a la variable count1

    buffer1[count1]= fract; //ajout d'une fractale
    count1++;
    iter1++;
    pthread_mutex_unlock(&mutex1); //fin d'acces
    sem_post(&full1);  //un slot rempli en plus
}
      pthread_mutex_lock(&mutex1);

      NLECTEURS--;

      pthread_mutex_unlock(&mutex1);
      if(NLECTEURS==0)
      {
        int boucle =0;
        while(boucle==0)
        {
          if(iter1==iter4 && iter3==iter1)
          {
            sem_post(&empty2);
          }
        }

    }



pthread_exit(NULL);
}

/*Calucl de la fractal et de sa valeur moyenne*/
void FractalCompute(struct fractal * f)
{


  int width = f->width;

  int height = f->height;

  int val=0;
int i;
  for(i=0;i<width;i++)
  {
int j;
    for(j=0; j<height;j++)
    {

    val = val +  fractal_compute_value1(f, i, j); //met a jour directement

    }
  }

  f->moyenne = (double) val/(width*height);

}

/*******Consommateur1********/
void *consommateur1(void * a)
{


  while(1==1)
  {

    struct fractal * f;
    sem_wait(&full1); //attente d'un slot rempli
    pthread_mutex_lock(&mutex1); //acces au tableau et a la variable count1 et a iter2

    f = buffer1[count1-1]; //retrait d'une fractale
    count1--;

    iter2++;//nbre  de fractale cree par le producteur 1;

    pthread_mutex_unlock(&mutex1); // fin d'acces
    sem_post(&empty1);
 //il y a un slot libre de plus

    FractalCompute(f);


/******consommateur1 devient producteur2 ******/

      sem_wait(&empty2); // attente slot libre
      pthread_mutex_lock(&mutex2); // acces au buffer2 et a la variable count2

      buffer2[count2]= f; //ajout d'une fractale (calculee)
      count2++;
      iter3++; //une fractael calculee de plus

      pthread_mutex_unlock(&mutex2); //fin d'acces
      sem_post(&full2);  //un slot rempli en plus

      if(NLECTEURS==0&&iter3==iter1)
      {

pthread_exit(NULL);
    }
  }
}
/*compare les 2 fractales et renvoient celle dont la  valeur moyenne est la plus elevee*/
struct fractal * compareFractale(struct fractal * fractmax, struct fractal * f1)
{
  if(fractmax->moyenne >= f1->moyenne)
  {
    fractal_free(f1);
    return fractmax;
  }
  else
  {
    fractal_free(fractmax);
    return f1;
  }
}



/***** Consommateur2 ----> Comparateur de fractales*****/

void * consommateur2(void * a)
{


  struct fractal * f = (struct fractal *)malloc(sizeof(struct fractal));
  if(f==NULL)
  {
    return NULL;
  }
  struct fractal * fractmax = (struct fractal *)malloc(sizeof(struct fractal));
  if(fractmax==NULL)
  {
    fractal_free(f);
    return NULL;
  }


  int fini=0;

  while(1)
  {

    sem_wait(&full2);
    pthread_mutex_lock(&mutex2);
    pthread_mutex_lock(&mutex1);
    if(NLECTEURS==0 && iter1==iter4)
    {
      fini=1; //plus rien a comparer
      pthread_mutex_unlock(&mutex1);
    }
    else
    {

    pthread_mutex_unlock(&mutex1);
    f = buffer2[count2-1]; //retrait d'une fractale
    count2--;
    iter4++;

    pthread_mutex_lock(&mutex1);


    if(iter4==iter1)
    {
      if(NLECTEURS==0||(iter3==iter4 && count1==0 && iter2==iter3 &&count2==0))
      {

      fini=1;
    }
  }
}
    pthread_mutex_unlock(&mutex1);// fin d'acces
    pthread_mutex_unlock(&mutex2); // fin d'acces
    sem_post(&empty2); //il y a un slot libre de plus


    if(iter4==1)
    {
      *fractmax = *f;

    }
    else
    {
      if(tous==0) //un seul fichier bitmap
      {
        if(f->name==fractmax->name)
        {
          error(-1,"error au moins 2 fois le même nom de fractal : argument invalide");
        }

      fractmax = compareFractale(fractmax,f);
    }
    else if(tous==1) //un fichier bitmap par fractale
    {
      if(f->name==fractmax->name)
      {
        error(-1,"error au moins 2 fois le même nom de fractal : argument invalide");
      }

    write_bitmap_sdl1(f, strcat(f->name,".bmp"));
    fractmax = compareFractale(fractmax,f);
  }
    }
    if(fini==1)
    {

      write_bitmap_sdl1(fractmax, strcat(a,".bmp"));

      free(buffer1);
      free(buffer2);
      fractal_free(fractmax);
      return (void *) a;
    }
  }

}

int main (int argc, char *argv[])
{

    /* TODO */
    int nbrefile =0; //nbre de file a lire +1 (output)
    files=(char **)malloc(sizeof(char *)*argc); //! dernier est le file de output
    int standard =0;  //1 si oui 0 si non

    int NTHREADS = 4;  //A Definir NOMDRE DE THREADS de calcul
    int maxthread=0; //nbre de threads de calcul
    int err;

/******* Lecture Arguments*****/
int i;
    for(i=1;i<argc;i++) //lis les arguments
    {

      if(argv[i][0]=='-')
      {
        if(argv[i][1]=='-')
        {
          maxthread = atoi(argv[i+1]);
          NTHREADS=maxthread;
          i++;
        }
        else if(argv[i][1]=='d')
        {
          tous=1;
        }
        else if(argv[i][1]=='\0')
        {
          standard=1;
        }
      }
      else
      {

        int length = strlen(argv[i]);
        char * r = (char *)malloc(sizeof(char)*(length+1));
        strcpy(r,argv[i]);
        files[nbrefile]=r;


        nbrefile++;

      }
  }

    /***********************************************************
                    producteurs-consommateurs 1
    ****************************************************************/

     NLECTEURS = (nbrefile) -1 + standard; //Nbre de threads de lecture de fichier --> = nbre de fichier a lire

    int NbreSlot=2*( NTHREADS ); // nbre de slots de chaque buffer


    buffer1 = (struct fractal **) malloc(sizeof(struct fractal*)*NbreSlot); //tableau commun aux producteurs-consommateurs 1

    pthread_mutex_init(&mutex1,NULL); //Initialisation du mutex1
    sem_init(&empty1,0,NbreSlot); //buffer vide
    sem_init(&full1,0, 0); //buffer vide

    char ligne[100];
if(standard==1)
{
    printf("Entrez une fractale valide format : nom largeur hauteur réel complexe, si vous ne souhaitez pas rentrer de fractal, tapez +\n");
    fgets(ligne, sizeof(ligne), stdin); //lecture de l'entree standard
}

    /*******Initialisation des threads de lecture (producteurs)******/

    pthread_t lecteurs[NLECTEURS];
    int j;
    for(j=0;j<NLECTEURS-standard;j++)
    {
    err = pthread_create(&lecteurs[j], NULL,&producteur1,(void *) files[j]);
    if(err!=0) //ERREUR
    { error(err,"erreur lorsque creation de thread de lecture");}
  }
  if(standard==1)
  {
    err = pthread_create(&lecteurs[NLECTEURS], NULL,&entreestandard,(void *)ligne);
    if(err!=0) //ERREUR
    { error(err,"erreur lors de creation du thread de lecture de l'entree standard");}
  }


 /*****Initialisation des threads de calcul (consommateurs)******/

    pthread_t threads[NTHREADS];
int k;
    for(k=0;k<NTHREADS;k++)
    {
    err = pthread_create(&threads[k], NULL,&consommateur1, NULL);
    if(err!=0) //ERREUR
    { error(err,"erreur lorsque creation de thread de calcul");}
  }

/***********************************************************************
                  Producteurs Consommateurs 2
*************************************************************************/

pthread_mutex_init(&mutex2,NULL); //Initialisation du mutex1
sem_init(&empty2,0,NbreSlot); //buffer vide
sem_init(&full2,0, 0); //buffer vide
// buffer contenant les fractales calculees
buffer2 =(struct fractal **)malloc(sizeof(struct fractal *)*NbreSlot);


/******** Initialisation du thread de comparaison *******/

pthread_t threadcmp;
char * fileOut;
fileOut = files[nbrefile -1];

err = pthread_create(&threadcmp,NULL,&consommateur2, (void *)fileOut);
if(err!=0)
  {error(err,"erreur lorsque creation du thread de comparaion");} // ERREUR


int *r;
pthread_join(threadcmp, (void **)&r);
free(files);
return 0;
}
