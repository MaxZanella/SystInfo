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

/**** Declaration variable globale******/

int iter1=0; //compte le nbre de fractales crees
int iter2=0; // compte le nbre de fractales retiree du buffer1
int iter3=0; //compte le nbre de fractale calculee et ajoutee au buffer2
int iter4=0;// compte le nbre de fractales  comparees
int NLECTEURS=0;
int flag=0;
/**Coordination Producteur-Consommateur (1)****/
pthread_mutex_t mutex1;
sem_t empty1;
struct fractal * buffer1;
sem_t full1;
int count1 = 0; //endroit dans le buffer1 où se trouve la premiere case libre
/**Coordination Producteur-Consommateur (2)*****/
pthread_mutex_t mutex2;
sem_t empty2;
struct fractal * buffer2;
sem_t full2;
int count2=0; //pour connaitre le dernier element de buffer2

/************************************************/
void fractal_set_value1(struct fractal *f, int x, int y, int val)
{
  printf("SET VALUE\n");
printf("%d %d\n",x,y);
  f->tab[f->width*y +x]=val;
  printf("ALLOUE\n");
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
  printf("COMPUTE_VALUE\n");
    double zx, zy;
    double a, b;
    int val;
    int w, h;
printf("COMPUTE_VALUE\n");


    a = fractal_get_a(f);
    b = fractal_get_b(f);
printf("COMPUTE_VALUE\n");
    w = fractal_get_width(f);
    h = fractal_get_height(f);
printf("COMPUTE_VALUE\n");
    zx = ((double)x / (double)w) * 3.0 - 1.5;
    zy = ((double)y / (double)h) * 3.0 - 1.5;
printf("COMPUTE_VALUE\n");
    val = iter_julia1(zx, zy, a, b, 0);
    printf("before set\n");
    fractal_set_value1(f, x, y, val);
    printf("after set");
printf("COMPUTE_VALUE");
    return val;
}


void error(int err, char*msg)
{
  fprintf(stderr,"%s a retourné %d, message d'erreur : %s\n",msg,err,strerror(errno));
  exit(EXIT_FAILURE);
}

/********Ouverture de fichier********/

int openFile (char *filename)
{
  printf("%s\n",filename);
  int file = open((char *)filename,O_RDONLY);
  if(file!=0)
  error(file,"erreur lors d'ouverture d'un fichier");
  return file;
}


struct fractal *readLine(char* ligne) {
  printf("debut readLinelol, %s\n", ligne);
    char name[65];
    int width, height;
    double r, c;
    int a=0;

    struct fractal * fract=NULL;
    int i=0;
    while(a<5)
    {
      printf("souci?\n");
      char * concat = (char *)malloc(sizeof(char)*65);
      printf("non\n");
    while(i<strlen(ligne))
    {
      printf("%c",ligne[i]);
      printf("1\n");
      char z=ligne[i];
      if(ligne[i]==' ')
      {

        if(a==0)
        {
          strcpy(name,concat);
          printf("%s\n",name);
          a++;
        }
        if(a==1)
        {
          width = atoi(concat);
          printf("YOUPI%d\n",width);
          a++;
        }
        if(a==2)
        {
          height = atoi(concat);
          printf("HOURRA%d\n",height);
          a++;
        }
        if(a==3)
        {
          r =atof(concat);
          printf("%f\n",r);
          a++;
        }
        if(a==4)
        {
          c=atof(concat);
          printf("%f\n",c);
          a++;
        }
      }
      else
      {
        printf("2\n");
      strcat(concat,&z);
      printf("%s\n",concat);
      i++;
      printf("4\n");
    }
    printf("5\n");
    i++;


    }
  }
      printf("fin de readline\n");
      fract = fractal_new("fract",200,200,0.5,0.5);
      int * p= (int *)malloc(sizeof(int)*(fract->width)*(fract->height));
      if(p==NULL)
      {
        return NULL;
      }
      else{
      fract->tab = p;
    }


      printf("READLINE : width ==== %d\n",fract->width);
      printf("NOM FRACATAl %s\n",fractal_get_name(fract));
      return fract;
    }

  struct fractal * readFile(int file)
  {
    printf("debut lecture\n");
    char * buf = (char *)malloc(sizeof(char));
    int lect = read(file,buf,sizeof(char));
    struct fractal * fract=NULL;
    printf("avant boucle while\n");
    while(lect>0)
    {
      printf("1\n");
      while(fract==NULL && lect>0)
      {printf("1\n");
      int newLine=0;
      char * concat = (char *)malloc(sizeof(char)*100);
      printf("1\n");
      if(concat==NULL)
      {
        return NULL;
      }
      while(newLine==0&&lect>0)
      {
        printf("2");
        if(*buf=='\n')
        {
          newLine=1;
        }
        else
        {
        strcat(concat, buf);
        printf("%s\n",concat);
      }
      lect = read(file,buf,sizeof(char));
      }
      printf("avant readLine\n");
      fract = readLine(concat);
      printf("readfile width = %d\n", fract->width);
    }
    if(fract != NULL)
    {
      free(buf);
      return fract;
    }
    }
    printf("lect est nul\n");
    free(buf);
  return NULL;
    }

/*******Lecteur fichier*******/
/*struct fractal * readFile (int file)
{
  int a; //chaine de caractere en cours de lecture (nom puis largeur etc...)
  int err; // 0 = OK, 1 = recommence une ligne, 2 fin de ficher

char * name;
int height;
int width;
double reel;
double complx;


//lecture caractère par caractère

char * buf = (char *)malloc(sizeof(char));
int lect = read(file,buf,sizeof(char));
int length =0;
while(lect>0)
{
  a=0;
  err=0;
while(err==0)
{
  char * concat; //malloc ou pas et specifier la taille?
while(*buf!='\0' && *buf!='\n' && *buf!=' ' && lect>0) //lecture caractere par caractere jusque espace
{
  strcat(concat, buf);
  length++;
  lect = read(file,buf,sizeof(char));
}
if(buf[length]==' ')
{
  a++; // on a un 'mot' en plus (d'abord le nom, puis la largeur etc.)
  if(a==1)
  {  //correspond au nom
  name = concat;
}
  else if (a==2)  // correspond a la largeur
  {
  width = atoi(concat);

}
  else if(a==3)  // correspond a la longueur
  {
  int height = atoi(concat);
}
  else if (a==4)  // correspond au reel
  {
  double reel = atof(concat);
}
  else if (a==5)  // correspond au complexe
  {
  double complx = atof(concat);
}
  else if (a>5) //mauvais format d'entree, trop de mots
  {
    err=1;
  }
}
else if(buf[length]=='\n')
{
  if(a==5)
  {
    free(buf);
    struct fractal * fract = fractal_new(name, width, height, reel, complx);  // on cree la fractale qu'on va renvoyer

    int tab[width][height]; //necessaire?
    fract->tab = tab;
    return fract;
  }
  else
  {
    err=1;
  }
}
else if(buf[length]=='\0')
{
  if(a==5)
  {
    free(buf);
    int cl = close(file);
    if(cl!=0)
    {
       error(cl,"fermeture du fichier");
    }
    struct fractal * fract = fractal_new(name, width, height, reel, complx);
    int tab[width][height]; //necessaire?
    fract->tab = tab;
    return fract;

  }
  else
  {
    free(buf);
    return NULL;
  }
}
}
}
  if(a==5)
  {
    free(buf);
    int cl = close(file);
    if(cl!=0)
    {
       error(cl,"fermeture du fichier");
    }
    struct fractal * fract = fractal_new(name, width, height, reel, complx);
    int tab[width][height]; //necessaire?
    fract->tab = tab;
    return fract;

  }
  else
  {
    free(buf);
    return NULL;
  }

return NULL;
}
*/

/******Producteur1 *******/
void *producteur1(void * file)
{

  int * tete = (int *) file;
  int fd = *tete;
  printf("Entree producteur1\n");
  printf("%d\n",fd);
 //ouvre le fichier et renvoie la tete de lecture
  struct fractal * fract;
  printf("lecture fractale\n");
  fract = readFile(fd); // A definir -->correspond a la lecture et creation d'une structure fractale
printf("fin de lecture fractale\n");
printf("%d\n",fract->width);
  while(fract != NULL)
  {

    sem_wait(&empty1); // attente slot libre
    pthread_mutex_lock(&mutex1); // acces au tableau et a la variable count1

    buffer1[count1]= *fract; //ajout d'une fractale
    count1++;
    iter1++;
printf("fratale ajoutee\n");
    pthread_mutex_unlock(&mutex1); //fin d'acces
    sem_post(&full1);  //un slot rempli en plus

    printf("%d, %d\n", count1, buffer1[0].width);
    fract = readFile(fd); // correspond a la lecture et creation d'une structure fractale
    printf("NLECTEURS=%d\n",NLECTEURS);
    if(fract==NULL)
    {
      printf("NLECTEURS=%d\n",NLECTEURS);
      pthread_mutex_lock(&mutex1);

      NLECTEURS--;
      if(NLECTEURS==0)
      {
      sem_post(&empty2); //il y a un slot libre de plus
    }
      pthread_mutex_unlock(&mutex1);
    }
    printf("NLECTEURS=%d\n",NLECTEURS);

}
printf("on sort de la lecture de fichier\n");

int err = close(fd);
if(err!=0)
{
  error(err,"fermeture fichier");
}
pthread_exit(NULL);
}

void FractalCompute(struct fractal * f)
{
  printf("DEBUT FRACTALE COMPUTE");

  int width = f->width;
  printf("compute width = %d\n",f->width);
  int height = f->height;
  printf("compute\n");
  int val=0;
  printf("compute\n");
  for(int i=0;i<width;i++)
  {
    printf("compute\n");
    for(int j=0; j<height;j++)
    {
      printf("compute\n");
      printf("CALCUL\n");
    val = val +  fractal_compute_value1(f, i, j); //met a jour directement
    printf("val = %d\n",val);
    }
  }
  printf("CALCUL VAL FINI\n");
  f->moyenne = (double) val/(width*height);
  printf("ON ALLOUE MOYENNE, %f\n", f->moyenne);
}

/*******Consommateur1********/
void *consommateur1(void * a)
{

printf("DEBUT CONSOMMATEUR1");
  while(1==1)
  {
    struct fractal * f;
    sem_wait(&full1); //attente d'un slot rempli
    pthread_mutex_lock(&mutex1); //acces au tableau et a la variable count1 et a iter2
    printf("ACCES AU TABLEAU CONSO 1\n");
    f = &buffer1[count1-1]; //retrait d'une fractale
    count1--;
    printf("LA WIDTH FRACTALE VAUT %d\n",f->width);
    iter2++;//nbre  de fractale cree par le producteur 1;
    pthread_mutex_unlock(&mutex1); // fin d'acces
    sem_post(&empty1); //il y a un slot libre de plus
    printf("la  valeur de width est %d\n", f->width);
    printf("count1, %d\n",count1);
    FractalCompute(f);
    
printf("apres calcul fractal\n");
/******consommateur1 devient producteur2 ******/

      sem_wait(&empty2); // attente slot libre
      pthread_mutex_lock(&mutex2); // acces au buffer2 et a la variable count2

      buffer2[count2]= *f; //ajout d'une fractale (calculee)
      count2++;
      iter3++;
      printf("Ajout fractal calculee");
      pthread_mutex_unlock(&mutex2); //fin d'acces
      sem_post(&full2);  //un slot rempli en plus
  }
}

struct fractal * compareFractale(struct fractal * fractmax, struct fractal * f1)
{
  if(fractmax->moyenne >= f1->moyenne)
  {
    free(f1);
    return fractmax;
  }
  else
  {
    free(fractmax);
    return f1;
  }
}

/***** Consommateur2 ----> Comparateur de fractales*****/

void * consommateur2(void * a)
{
  printf("DEBUT CONSOMMATEUR 2\n");
  struct fractal * f = (struct fractal *)malloc(sizeof(struct fractal));
  if(f==NULL)
  {
    return NULL;
  }
  struct fractal * fractmax = (struct fractal *)malloc(sizeof(struct fractal));
  if(fractmax==NULL)
  {
    free(f);
    return NULL;
  }

  char * fileOut = (char *) a;
  int fini=0;
  iter3=0; //nbre de fractales comparees -->utile pour condition d'arret??
  while(1)
  {
    printf("conmparateur attend...\n");
    sem_wait(&full2); //attente d'un slot rempli
    pthread_mutex_lock(&mutex2); //acces au buffer2 et a la variable count2
    pthread_mutex_lock(&mutex1);
    if(NLECTEURS==0 && iter1==iter4)
    {
      fini=1;
      pthread_mutex_unlock(&mutex1);
    }
    else
    {

    pthread_mutex_unlock(&mutex1);
    f = &buffer2[count2-1]; //retrait d'une fractale
    count2--;
    iter4++;
    printf("ET UNE DE PLUS A COMPARER...\n");
    pthread_mutex_lock(&mutex1);
    printf("iter 3 = %d, iter 1 = %d", iter3, iter1);
    if(iter4==iter1)
    {
      if(NLECTEURS==0||(iter3==iter4 && count1==0 && iter2==iter3 &&count2==0))
      {
      printf("FINI YOUPI\n");
      fini=1;
    }
  }
}
    pthread_mutex_unlock(&mutex1);
    pthread_mutex_unlock(&mutex2); // fin d'acces
    sem_post(&empty2); //il y a un slot libre de plus
    printf("COmparateur sort du sémaphore\n");
    if(iter3==1)
    {
      *fractmax = *f;
    }
    else
    {
      fractmax = compareFractale(fractmax,f);
    }
    if(fini==1)
    {
      printf("DEBUT ECRITURE FICHER BITMAP\n");
      write_bitmap_sdl(fractmax, a);
      printf("FIN ECRITURE FICHIER BITMAP\n");
      return (void *) a;
    }
  }

}

/******* Lecture Arguments*****/
/*void readArg(int * nbrethreads,int * tous,int * nbre, int * entree, char** fichier, int argc, char**argv)
{
  for(int i=1;i<argc;i++)
  {
    if(argv[i][0]=='-')
    {
      if(argv[i][1]=='-')
      {
        *nbrethreads = atoi(argv[i][13]);
      }
      else if(argv[i][1]=='d')
      {
        *tous=1;
      }
      else if(argv[i][1]=='\0')
      {
        *entree=1;
      }
    }
    else
    {
      strcpy(fichier[*nbre],argv[i]);
      *nbre = *nbre +1;
    }
}
}*/
/********Ouverture de fichier********/


int main (int argc, char *argv[])
{
    printf("1\n");
    /* TODO */
    int nbrefile =0; //nbre de file a lire +1 (output)
    char * files[argc]; //! dernier est le file de output
    int stdin =0;  //1 si oui 0 si non
    int tous=0; //si fichier bmp pour chaque fractale (-d) : 1 oui 0 non
    int NTHREADS = 4;  //A Definir NOMDRE DE THREADS de calcul --> ! limite
    int maxthread=0;
    int err;
printf("2\n");
    for(int i=1;i<argc;i++) //lis les arguments
    {
      printf("rentre\n");
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
          stdin=1;
        }
      }
      else
      {
        printf("rentre2\n");
        int length = strlen(argv[i]);
        char * r = (char *)malloc(sizeof(char)*(length+1));
        strcpy(r,argv[i]);
        files[nbrefile]=r;

        printf("%s\n",files[nbrefile]);
        nbrefile++;

      }
  }

  printf("3\n");

    /***********************************************************
                    producteurs-consommateurs 1
    ****************************************************************/

     NLECTEURS = (nbrefile) -1; //Nbre de threads de lecture de fichier --> = nbre de fichier a lire
flag =NLECTEURS;
    int NbreSlot=2*( NTHREADS ); // A Definir NOMBRE DE SLOTS BUFFER --> 2*NThREADS

    //tableau commun aux producteurs-consommateurs

    buffer1 = (struct fractal *) malloc(sizeof(struct fractal)*NbreSlot); //tableau commun aux producteurs-consommateurs

    pthread_mutex_init(&mutex1,NULL); //Initialisation du mutex1
    sem_init(&empty1,0,NbreSlot); //buffer vide
    sem_init(&full1,0, 0); //buffer vide


    /*******Initialisation des threads de lecture (producteurs)******/
printf("hello\n");
    pthread_t lecteurs[NLECTEURS];
    for(int i=0;i<NLECTEURS;i++)
    {
      printf("max\n");
printf("max\n");
int fd = open(files[i],O_RDONLY);
printf("max\n");

if(fd<=0)
{
error(fd,"erreur lors d'ouverture d'un fichier");
}
printf("max\n");
printf("%d\n",fd);
    err = pthread_create(&lecteurs[i], NULL,&producteur1,(void *) &fd);
    if(err!=0) //ERREUR
    { error(err,"erreur lorsque creation de thread de lecture");}
  }

 /*****Initialisation des threads de calcul (consommateurs)******/

    pthread_t threads[NTHREADS];

    for(int i=0;i<NTHREADS;i++)
    {
    err = pthread_create(&threads[i], NULL,&consommateur1, NULL);
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
buffer2 =(struct fractal *)malloc(sizeof(struct fractal)*NbreSlot);


/******** Initialisation du thread de comparaison *******/

pthread_t threadcmp;
char * fileOut;
fileOut = files[nbrefile -1];
err = pthread_create(&threadcmp,NULL,&consommateur2, (void *)fileOut);
if(err!=0)
  {error(err,"erreur lorsque creation du thread de comparaion");} // ERREUR
int *r;
pthread_join(threadcmp, (void **)&r);
return 0;
}
