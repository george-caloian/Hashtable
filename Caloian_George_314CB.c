/* CALOIAN George - 314CB */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>


typedef struct lista 
{
    char *key;
    char *val;
    struct lista *urm;
} Tlista;

typedef struct tabdisp
{
    int size;
    Tlista **buc;
} Ttabdisp;

//Functia Construieste primeste ca parametru numarul de bucketuri si construieste Tabela de dispersie
Ttabdisp *Construieste ( int size )
{
	int i;    
	Ttabdisp *new_table;
    
    if ( size < 1 )
    	return NULL;
    //alocari de memorie
    if ( ( new_table = malloc ( sizeof ( Ttabdisp ) ) ) == NULL )
        return NULL;
    
    if ( ( new_table->buc = malloc ( sizeof ( Tlista * ) * size ) ) == NULL )
        return NULL;
    
    for ( i=0; i < size; i++ ) 
    	new_table->buc[i] = NULL;
    
    //setam campul size
    new_table->size = size;
    
    return new_table;
}

//Functia hash returneaza restul impartirii sumei
//caracterelor din cheie la numarul de bucketuri
unsigned int hash(Ttabdisp *hashtable, char *key)
{
	unsigned int hval = 0;
	int i=0, a[100], k=0,q;
	
	//trecem in codurile ascii
	while ( key[i] )	
	{
		a[i] = (int)key[i];
		i++;
		k++;
	}
	
	//calculam suma
	for( q = 0 ; q < k ; q++ )
		hval = hval + a[q];
		
	//returnam restul impartirii la numarul de bucketuri	
	return hval % hashtable->size;
}
//functia Cauta verifica daca key exista in tabela
Tlista *Cauta(Ttabdisp *hashtable, char *key)
{
    Tlista *list;
    unsigned int hval = hash(hashtable, key);
    
    //parcurgem doar secventa corespunzatoare valorii
    //returnate de funtia hash ( doar bucketul respectiv)
    for(list = hashtable->buc[hval]; list != NULL; list = list->urm)
        if ( strcmp ( key, list->key ) == 0)
        	return list;
    return NULL;
}

//functia Introduce pune elementul in tabela de dispersie
int Introduce(Ttabdisp *hashtable, char *key, char *val)
{
    Tlista *nou,*aux,*aux1;
    Tlista *current_list;
    unsigned int hval = hash(hashtable, key);
    
    if ((nou = malloc(sizeof(Tlista))) == NULL)
    	return 1;
    
    current_list = Cauta(hashtable, key);
    //verificam daca elemntul nu exista deja
    if ( current_list != NULL )
    	return 2;
    
    nou->key = strdup(key);
    nou->val = strdup(val);
    nou->urm=NULL;
    
    //cazul in care bucketul este vid
    if(hashtable->buc[hval] == NULL)
		hashtable->buc[hval] = nou;
    
    else
		//cazul in care bucketul contine un singur element
		//si noul element trebuie introdus dupa acesta
		if(hashtable->buc[hval]->urm == NULL && strcmp(hashtable->buc[hval]->key,key ) < 0 ) 
	    	hashtable->buc[hval]->urm = nou;
		else
			//respectiv inainte
	   		if(hashtable->buc[hval]->urm == NULL && strcmp(hashtable->buc[hval]->key,key ) > 0 )
	    	{
				nou->urm=hashtable->buc[hval];
				hashtable->buc[hval]=nou;
	   		}
	   		else	//bucketul contine 2 sau mai multe
	   				//elemente
	    	{
				aux=hashtable->buc[hval];
				aux1=aux->urm;
				//mutam cele 2 auxiliare pana cand ajungem cu
				//ele in zona in care trebuie plasat noul
				//element
				while(aux1->urm != NULL && strcmp(aux1->key,key) < 0 )
				{
		    		aux = aux1;
		    		aux1 = aux1->urm; 		
				}
				//cazul in care noul element trebuie introdus
				//la finalul bucketului
				if(strcmp(aux1->key,key)<0)
		    		aux1->urm = nou;
				else
					//cazul in care noul element trebuie
					//introdus INTRE cele 2 auxiliare
		    		if(strcmp(aux1->key,key)>0 && strcmp(aux->key,key)<0)				
		    		{
						nou->urm = aux1;
						aux->urm = nou;
		    		}
		    		else
		    		{	
		    			//cazul in care trebuie introdus
		    			//inaintea primului auxiliar
		    			//(bucketul are doar 2 elemente)
						nou->urm = aux;
						hashtable->buc[hval] = nou;
		    		}
	    	}
    
    return 0;
}

//Functia Elimina sterge elementul din tabela de dispersie
void Elimina(Ttabdisp *hashtable,char *key)
{
    int i;
    Tlista *del=NULL,*pre=NULL, *rez;
    unsigned int hval = hash(hashtable, key);
    
    rez=Cauta(hashtable,key);
	
	//verificam daca elementul se afla in tabela
	if(rez==NULL)
		return;
	
	pre = hashtable->buc[hval];
    
    //cazul in care elementul se gaseste pe prima pozitie
    //in bucket
    if (strcmp(key, hashtable->buc[hval]->key) == 0)
	{
		del = hashtable->buc[hval];
		hashtable->buc[hval]=del->urm;
		free(del->key);
		free(del);
		return;
	}
	
	pre = hashtable->buc[hval];
	del = hashtable->buc[hval]->urm;
	
	while ( del != NULL )
	{
		if( strcmp ( key, del->key ) == 0 )
		{
			//restabilim legaturile
			if( del->urm != NULL )
				pre->urm = del->urm;
			else
				pre->urm = NULL;
			free ( del->key );
			free ( del );
		}
		//avansam
		pre = del;
		if(del->urm != NULL)		
			del = pre->urm;
		else
			del = NULL;
	}
}

//Functia Afiseaza
void Afiseaza(Ttabdisp *hashtable,FILE *f)
{
	int i=0;
	Tlista *list;	
	
	for( i = 0 ; i < hashtable->size ; i++ )
	{
		if( hashtable->buc[i] != NULL )
		{
			//afisam indicele bucketului
			fprintf(f,"%d: ",i);
			list = hashtable->buc[i];
			//si fiecare element din el
			while ( list != NULL )
			{
				fprintf(f,"%s ",list->val);
				list=list->urm;
			}
			fprintf(f,"\n");
		}
	}
}

//Functia CeIP returneaza IP-ul corespunzator adresei web
char * CeIP(Ttabdisp *hashtable,char*key)
{
	Tlista *list;
    unsigned int hval = hash(hashtable, key);
    
    for( list = hashtable->buc[hval]; list != NULL; list = list->urm)
        if (strcmp(key, list->key) == 0)
        	return list->val;
    
    return NULL;
}

//Functia AfiseazaBucket va afisa doar Bucketul cerut
void AfiseazaBucket(Ttabdisp *hashtable,int x,FILE * f)
{
	int i=0;
	Tlista *list;	
	
	if( x>=hashtable->size)
		return;
	if( hashtable->buc[x] == NULL )
		fprintf(f,"VIDA\n");
	
	else
	{
		list = hashtable->buc[x];
		while( list != NULL )
		{
			fprintf(f,"%s ",list->val);
			list = list->urm;
		}
		fprintf(f,"\n");
	}
}

void Elib(Ttabdisp *hashtable)
{
    int i;
    Tlista *list, *temp;

    if ( hashtable == NULL )
    	return;

    for( i = 0 ; i < hashtable->size ; i++)
    {
        list = hashtable->buc[i];
        while(list != NULL) 
        {
            temp = list;
            list = list->urm;
            free(temp->key);
            free(temp->val);
            free(temp);
        }
    }

    free(hashtable->buc);
    free(hashtable);
}


int main(int argc,char* argv[])
{ 
	//declaratii variabile
	Ttabdisp *tabdisp;
	int M=atoi(argv[1]),a,k,i=0,j;
	tabdisp = Construieste(M);
	FILE * file, *fileo;
	char buff[100],*token;
	const char s[2]=" ";
	char* cuv[200];
	file=fopen(argv[2],"rt");
	fileo=fopen(argv[3],"w");
	Tlista *rez;
	
	if( !file )
		return 1;
		
	//citim textul linie cu linie
	while(fgets(buff,100,file)!=NULL)
	{
		//impartim cuvintele
		token = strtok(buff,s);
		while(token)
		{
			//salvam cuvintele intr-un vector
			cuv[i] = strdup(token);
			token = strtok(NULL,s);
			i++;
		}
	}
	//eliminam "\n"
	for( j = 0 ; j < i ; j++ )
		cuv[j] = strtok(cuv[j],"\n");
	
	//Pentru liniile care se termina cu spatiu
	//secventa de eliminare a "\n" nu este suficienta
	//asadar eliminam elementele "reziduale" care sunt egale
	//cu NULL
	for( j = 0 ; j < i ; j++ )
	{
		if( cuv[j] == NULL )
		{
			for( k = j ; k < i ; k++)
				cuv[k] = cuv[k+1];
			i--;
		}
	}
	
	/*incepem sa prelucram comenzile
	parcurgem vectorul de cuvinte pana la gasirea uneia
	dintre cele 6 commenzi
	stiind formatul fiecare comenzi pentru fiecare in parte
	folosim functiile construite selectand ca parametrii
	cuvantul imediat urmator sau urmatoarele 2 cuvinte
	dupa caz*/
	for( j = 0 ; j < i ; j++ )
	{
		
		if( strcmp(cuv[j],"put") == 0)
			Introduce(tabdisp,cuv[j+1],cuv[j+2]);	
		else
			if( strcmp(cuv[j],"get") == 0 )	
			{
				if( CeIP(tabdisp,cuv[j+1]) == NULL)
					fprintf(fileo,"NULL\n");
				else
					fprintf(fileo,"%s\n",CeIP(tabdisp,cuv[j+1]));
			}
			else
				if(strcmp(cuv[j],"remove") == 0)
						Elimina(tabdisp,cuv[j+1]);
				else
					if(strcmp(cuv[j],"find") == 0)
					{						
						rez = Cauta(tabdisp,cuv[j+1]);
						if(rez == NULL)
							fprintf(fileo,"False\n");
						else
							fprintf(fileo,"True\n");
					}
					else
						if(strcmp(cuv[j],"print") == 0)
							Afiseaza(tabdisp,fileo);
						else
							if(strcmp(cuv[j],"print_bucket") == 0)
							{	
								a=atoi(cuv[j+1]);
								AfiseazaBucket(tabdisp,a,fileo);
							}
	}
	
	fclose(file);
	fclose(fileo);
	Elib(tabdisp);
	return 0;
}

