#include <stdio.h>
#include <mpi.h>
#include <math.h>
#include <stdlib.h>
#include <libhpc.h>

int mymergesort(int *array, long n);
int dimension(unsigned int dim, unsigned int ranking, int *value);

int nproc, p;
long nelem;

int main(int argc, char *argv [])
{
    /* Variabili */
    char *filename;
    int ranking, taskId;
    FILE *file;
    char *nline = NULL;
    char *end;
    char * taskName = "./test/Test_Task";
    ssize_t read;
    size_t lenght;
    long index1, index2;
    double time0, time1;
    
    /* Argomento dato in input con i valori da ordinare */
    if(argc > 1)
        filename = argv[1];
    else
    {
    	printf("ERRORE: File input non inserito\n");   
    	return 0;
    }
    
    /* Inizializzare MPI: Deve essere chiamata da tutti i processi prima di ogni altra chiamata MPI */
    MPI_Init(&argc , &argv);
    /*  Dato un comunicatore comm (per noi è sempre MPI_COMM_WORLD):
        Chi sono io? ranking è l’identificatore del processo corrente (da 0 a size-1) */
    MPI_Comm_rank(MPI_COMM_WORLD, &ranking);
    /* Quanti siamo? nproc è la dimensione del comunicatore */
    MPI_Comm_size(MPI_COMM_WORLD, &nproc);
    
    /* Neccessaria una potenza di due o solo un processore */
    if((pow((double) nproc, 1/log2((double) nproc)) != 2) && (nproc != 1))
    {
        printf("ERRORE: Numero processori: %d non è potenza di 2\n", nproc);   
        /* Abortire in caso d’errore MPI: termina tutti i processi associati a un certo comunicatore */
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 0;
    }
    
    file = fopen(filename, "r");
    if(file == NULL)
    {
        printf("ERRORE: Impossibile aprire file di input\n");
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 0;
    }
    
    nelem = 0;
    lenght = 0;
    /* Verifica numero elementi nel file di input */
    while ((read = getline(&nline, &lenght, file)) != -1)
        nelem++;
    fclose(file);
    /* Numero elementi nel file di input deve essere divisibile per il numero di processori */
    if(nelem % nproc !=0)
    {
        printf("ERRORE: File di input: numero elementi %d non è multiplo intero del numero di processori %d \n", nelem, nproc);
        MPI_Abort(MPI_COMM_WORLD, 1);
        return 0;
    }
    
    /* Lettura file input e loro scrittura: in ogni processore ho nelem/nproc elementi */
    int * input = malloc((nelem/nproc) * sizeof(int));
    file = fopen(filename, "r");
    index1 = 0;
    index2 = 0;
    while ((read = getline(&nline, &lenght, file)) != -1)
    {
        if(index1 >= ranking * (nelem/nproc) && index1 < (ranking + 1) * (nelem/nproc))
            input[index2++] = (int) strtol(nline, &end, 10);
        index1++;
    }
    fclose(file);

    taskId = 0;
    hpmInit(taskId, taskName);
    hpmStart(1,"Esecuzione"); 
    
    /* Sincronizzare i processi: Blocca il chiamante finché tutti i processi effettuano la chiamata */
    MPI_Barrier(MPI_COMM_WORLD);
    /* Misurare il tempo di esecuzione: I timer standard (es. POSIX) non sono adeguati per programmi MPI */
    time0 = MPI_Wtime();
    
    /* Ordino gli elementi in ogni processore */
    mymergesort(input,(nelem/nproc));
    MPI_Barrier(MPI_COMM_WORLD);
    
    /* Attivo una dimensione alla volta (log2 P dimensioni) */
    for(int i = 0; i < log2((double) nproc); i++)
    {
        p = i;
        for(int j = i; j>=0; j--)
        {
            MPI_Barrier(MPI_COMM_WORLD);
            dimension(j, ranking, input);
            MPI_Barrier(MPI_COMM_WORLD);
        }
    }
    
    time1 = MPI_Wtime();
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    /* Stampo input ordinato */
    MPI_Status status;
    int buf = 0;
    if(ranking == 0)
    {
    	
        for(long i=0; i<nelem/nproc; i++)
            printf("%d: %d\n", ranking, input[i]); 
        if(nproc != 1)
            MPI_Send(&buf, 1, MPI_INT, ranking+1, 0, MPI_COMM_WORLD);
    }
    else
    {
        if(nproc != 1)
        {
            MPI_Recv(&buf, 1, MPI_INT, ranking-1, 0, MPI_COMM_WORLD, &status);
            
             for(long i=0; i<nelem/nproc; i++)
                 printf("%d: %d\n", ranking, input[i]); 
            if(ranking != nproc-1)
                MPI_Send(&buf, 1, MPI_INT, ranking+1, 0, MPI_COMM_WORLD);
        }
    }
    
    MPI_Barrier(MPI_COMM_WORLD);
    
    
    hpmStop(1);
    hpmTerminate(taskId);
    
    /* Output per dati */ 
    /* */
    /* */
    

    /* printf("%d;%d;%f\n", ranking, nelem, time1 - time0); */
    

    /* Output per dati */ 
    /* */
    /* */

    free(input);
    /* Terminare MPI */
    MPI_Finalize();
    return 0;
}

/* ######################## FUNZIONI ######################## */
int merge(int *array, long s, long ind, long e)
{
    long i, j, k;
    int *array2 = malloc((nelem/nproc) * sizeof(int));
    
    i = s;
    j = ind+1;
    k = 0;
    
    while (i <= ind && j <= e)
    {
        if (array[i] < array[j])
        {
            array2[k] = array[i];
            i++;
        }
        else
        {
            array2[k] = array[j];
            j++;
        }
        k++;
    }
    while (i <= ind)
    {
        array2[k] = array[i];
        i++;
        k++;
    }
    while (j <= e)
    {
        array2[k] = array[j];
        j++;
        k++;
    }
    for (k=s; k<=e; k++)
        array[k] = array2[k-s];
    
    free(array2);
    return 0;
}

int mymergesort(int *array, long n)
{
    for (long i=1; i<=n-1; i = 2*i)
    {
        for (long j=0; j<n-1; j = j + (2*i))
        {
            long ind = j + i - 1;
            long e = n - 1;
            if((j + 2*i - 1) < (n - 1))
                e = j + 2*i - 1;
            merge(array, j, ind, e);
        }
    }
    return 0;
}

int dimension(unsigned int dim, unsigned int ranking, int *value)
{
    unsigned int ur, dest;
    int index, bit, rbit;
    long r_elem, index1, index2;
    int *rvalue = malloc((nelem/nproc) * sizeof(int));
    int *merge = malloc(2 * (nelem/nproc) * sizeof(int));
    int *value1;
    int *value2;
    
    MPI_Status status;
    ur = ranking;
    
    /* Verifica se dimensione valida */
    if(dim < 0 || dim >= log2((double) nproc))
        return 1;
    
    dest = (ur)^(1<<dim);
    r_elem = nelem/nproc;
    index=0;
    
    /* Trasferisco gli elementi da ordinare a blocchi di dimensione all'altro processore */
    while(1)
    {
        int s_elem = (int) r_elem;
        if((int) pow(2.0, 13.0) < r_elem)
            s_elem = (int) pow(2.0, 13.0);
        /* Spedire un messaggio:
                - Ritorna quando il messaggio è stato copiato dal sistema
                - Quando ritorna value puo’ essere sovrascritto
                - Non è detto che quando ritorna il messaggio sia stato ricevuto!!!
                - Potrebbe essere nella memoria di sistema del mittente */
        MPI_Send(&(value[index]), s_elem, MPI_INT, dest, 0, MPI_COMM_WORLD);
        /* Ricevere un messaggio:
                - Ritorna quando il messaggio è stato ricevuto nel buffer nello spazio utente
                - s_elem deve essere ≥ della lunghezza del msg ricevuto
                - dest può essere MPI_ANY_SOURCE
                - Il tag identifica quale messaggio da dest si vuole ricevere, tag può essere MPI_ANY_TAG
                - status contiene informazioni aggiuntive */
        MPI_Recv(&(rvalue[index]), s_elem, MPI_INT, dest, 0, MPI_COMM_WORLD, &status);
        index += s_elem;
        r_elem -= s_elem;
        /* Se non sono più presenti elementi da ordinare termina il ciclo */
        if(r_elem == 0)
            break;
    }
    
    index1 = 0;
    index2 = 0;
    value1 = value;
    value2 = rvalue;
     /* Unisco le due liste ordinate di due processori */
    for(long i=0 ; i<2*nelem/nproc; i++)
    {
        if(index1 == nelem/nproc)
        {
            /* Se ho già letto tutta la prima lista continuo solamente con la seconda senza confronti */
            merge[i] = value2[index2];
            index2++;
            continue;
        }
        if(index2 == nelem/nproc)
        {
            /* Se ho già letto tutta la seconda lista continuo solamente con la prima senza confronti */
            merge[i] = value1[index1];
            index1++;
            continue;
        }
        if(value1[index1] <= value2[index2])
        {
            /* Confronto i valori delle due liste per formare una lista ordinata */
            merge[i] = value1[index1];
            index1++;
            continue;
        }
        /* Unico caso rimanente: confrontando le due liste il valore più piccolo è nella seconda */
        merge[i] = value2[index2];
        index2++;
    }
    
    bit = (ranking>>dim)&0x00000001;
    rbit = (ranking>>(p+1))&0x00000001;
    /* Bitonic sort */
    if(bit != rbit)
    {
        for(long i=0; i<nelem/nproc; i++)
            value[i] = merge[i+nelem/nproc];
    }
    else
    {
        for(long i=0; i<nelem/nproc; i++)
            value[i] = merge[i];
    }
    
    free(rvalue);
    free(merge);
    return 0;
}
