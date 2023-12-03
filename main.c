#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define MAX_LENGTH 31


struct Operation{
    int id;
    float temps;
    int exclus[MAX_LENGTH];
    int nbexc;
    int precedence[MAX_LENGTH];
    int nbprec;
    int couleur;

}Operation;

struct Operation newOp(int id, float temps){
    struct Operation New;
    New.temps = temps;
    New.id = id;
    New.nbexc = 0;
    for(int i  = 0; i< MAX_LENGTH; i++){
        New.exclus[i] = -1;
    }
    New.nbprec = 0;
    for(int i  = 0; i< MAX_LENGTH; i++){
        New.precedence[i] = -1;
    }
    New.couleur = -1;
    return New;
}

void afficheOp(struct Operation o){
    printf("Operation %d\n",o.id);
    printf("temps:%f\n",o.temps);
    printf("couleur:%d\n",o.couleur);

    printf("exclusion:[");
    if(o.exclus[0]!=-1) {
        printf("%d", o.exclus[0]);
    }
    for(int i  = 1; i< MAX_LENGTH; i++){
        if(o.exclus[i]!=-1) {
            printf(",%d", o.exclus[i]);
        }
    }
    printf("]\n");

    printf("precedence:[");
    if(o.precedence[0]!=-1) {
        printf("%d", o.precedence[0]);
    }
    for(int i  = 1; i< MAX_LENGTH; i++){
        if(o.precedence[i]!=-1) {
            printf(",%d", o.precedence[i]);
        }
    }
    printf("]\n");
}
//--------------------------------------------------------------------------------------------------------------------------------------
struct Bloc{
    struct Operation op;
    struct Bloc *suivant;
}Bloc;

typedef struct Bloc *ListeOp ;

void initVide( ListeOp *L){
    *L = NULL ;
}

ListeOp ajoute(struct Operation op, ListeOp l)
{
    ListeOp tmp = (ListeOp) malloc(sizeof(Bloc) );
    tmp->op = op ;
    tmp->suivant = l ;
    return tmp ;
}

void empile(struct Operation op, ListeOp *L)
{
    *L = ajoute(op,*L) ;
}

void affiche_Liste(ListeOp l)
{
    if(l!=NULL){
        affiche_Liste(l->suivant);
        afficheOp(l->op);
        printf("\n");
    }
}

struct Operation *PtrOp(ListeOp *l,int id){
    if(l==NULL){
        printf("n'est pas dans la liste");
    }else{
        if((*l)->op.id == id){
            return &(*l)->op;
        }else{
            return PtrOp(&(*l)->suivant,id);
        }
    }
}

//--------------------------------------------------------------------------------------------------------------

float temps_cycle(char const *fichier){
    FILE* file = fopen(fichier,"r");
    if (file==NULL) {
        perror("Erreur lors de l'ouverture du fichier");
    }

    float res;

    fscanf(file, "%f",&res);

    fclose(file);
    return res;
}

void operations(ListeOp *l, char const *fichier){
    FILE* file = fopen(fichier,"r");
    if (file==NULL) {
        perror("Erreur lors de l'ouverture du fichier");
    }

    int id;
    float temps;
    while(fscanf(file, "%d %f", &id, &temps) != EOF){
        struct Operation tmp = newOp(id, temps);
        empile(tmp, l);
    }

    fclose(file);
}

void exclusion(ListeOp *l, char const *fichier){
    FILE* file = fopen(fichier,"r");
    if (file==NULL) {
        perror("Erreur lors de l'ouverture du fichier");
    }

    int i, j;

    while(fscanf(file, "%d %d", &i, &j) != EOF){
        struct Operation *ptr_i = PtrOp(l,i);
        struct Operation *ptr_j = PtrOp(l,j);

        (ptr_i)->exclus[(ptr_i)->nbexc++] = (ptr_j)->id;
        (ptr_j)->exclus[(ptr_j)->nbexc++] = (ptr_i)->id;

    }
    fclose(file);
}

void precedence(ListeOp *l, char const *fichier){
    FILE* file = fopen(fichier,"r");
    if (file==NULL) {
        perror("Erreur lors de l'ouverture du fichier");
    }

    int i, j;

    while(fscanf(file, "%d %d", &i, &j) != EOF){
        struct Operation *ptr_i = PtrOp(l,i);
        struct Operation *ptr_j = PtrOp(l,j);

        (ptr_i)->precedence[(ptr_i)->nbprec++] = (ptr_j)->id;
        (ptr_j)->precedence[(ptr_j)->nbprec++] = (ptr_i)->id;

    }
    fclose(file);
}


//-------------------------------------------------------------------------------------------------------------------------

bool memeCouleur(struct Operation op1, struct Operation op2){
    return op1.couleur == op2.couleur;
}

bool aUneCouleur(struct Operation o){
    return o.couleur > -1;
}

int compterOperations(ListeOp *l) {
    int count = 0;
    ListeOp* pl;
    pl = l;
    while ( *pl != NULL) {
        count++;
        pl  = &(*pl)->suivant;
    }
    return count;
}

bool toutesOperationsColorees(ListeOp l) {
    if (l == NULL) {
        return true;
    }

    if (l->op.couleur == -1) {
        return false;
    }

    return toutesOperationsColorees(l->suivant);
}

int* trierDecroissantExcl(ListeOp *l) {
    int nbop = compterOperations(l);
    int* ids_tries = (int*)malloc(nbop * sizeof(int));
    int i = 0;
    ListeOp pl = *l;
    while (pl != NULL) {
        ids_tries[i] = (pl->op).id;
        pl = pl->suivant;
        i++;
    }
    for (i = 1; i < nbop; i++) {
        int cle = ids_tries[i];
        int j = i - 1;
        while (j >= 0 && PtrOp(l, ids_tries[j])->nbexc < PtrOp(l, cle)->nbexc) {
            ids_tries[j + 1] = ids_tries[j];
            j = j - 1;
        }
        ids_tries[j + 1] = cle;
    }
    return ids_tries;
}

bool sontExclues(struct Operation op1, struct Operation op2) {
    for (int i = 0; i < MAX_LENGTH; ++i) {
        if (op1.exclus[i] == op2.id || op2.exclus[i] == op1.id) {
            return true;
        }
    }
    return false;
}

bool estExclueDeCouleurK(struct Operation op1, ListeOp l, int couleurk) {
    ListeOp current = l;

    while (current != NULL) {
        if (current->op.couleur == couleurk) {
            for (int i = 0; i < MAX_LENGTH; ++i) {
                if (current->op.exclus[i]  == op1.id ||op1.exclus[i]  == current->op.id ) {
                    return true;
                }
            }
        }
        current = current->suivant;
    }
    return false;
}


int coloration(ListeOp *l){
    int nbop = compterOperations(l);

    int* ids_tries = trierDecroissantExcl(l);

    int couleur[nbop];
    int nondecouleurk[nbop];
    for (int i = 0; i <= nbop; i++){
        couleur[i] = -1;
    }

    int couleurK = 0;

    while(!toutesOperationsColorees(*l)){


        int i = 0;
        struct Operation  *po = PtrOp(l, ids_tries[0]);

        while(po->couleur != -1 ){
            i++;
            po = PtrOp(l, ids_tries[i]);
        }
        po->couleur = couleurK;


        couleur[i] = couleurK;
//        printf("operation qu'on colorie de couleur: %d\n", couleurK);
//        afficheOp(*po);


        for(int j = i+1; j<nbop; j++){
            struct Operation  *pj = PtrOp(l, ids_tries[j]);
            if(pj->couleur==-1 && !estExclueDeCouleurK(*pj,*l,couleurK)){
                pj->couleur = couleurK;

                couleur[j] = couleurK;
//              printf("operation qu'on colorie de couleur: %d\n", couleurK);
//              afficheOp(*pj);
            }
        }

        couleurK++;

    }
    return couleurK;
}
//***************************************************************************
struct Station{
    float temps_cycle;
    int operations[MAX_LENGTH];
    int nb_operations;
    int couleur;  //ou id
}Station;


struct Station newStat(float temps_cycle, int couleur){
    struct Station New;
    New.temps_cycle = temps_cycle;
    New.couleur = couleur;
    for(int i  = 0; i< MAX_LENGTH; i++){
        New.operations[i] = -1;
    }
    New.nb_operations = 0;
    return New;
}

void afficheS(struct Station s){
    printf("Station %d\n",s.couleur);
    printf("Temps:%f\n",s.temps_cycle);
    printf("Nombre d'operations:%d\n",s.nb_operations);

    printf("Operations:[");
    if(s.operations[0]!=-1) {
        printf("%d", s.operations[0]);
    }
    for(int i  = 1; i< MAX_LENGTH; i++){
        if(s.operations[i]!=-1) {
            printf(",%d", s.operations[i]);
        }
    }
    printf("]\n");
}
//***************************************************************************
struct BlocS{
    struct Station s;
    struct BlocS *suivant;
}BlocS;

typedef struct BlocS *ListeS;


ListeS ajouteS(struct Station s, ListeS l)
{
    ListeS tmp = (ListeS) malloc(sizeof(BlocS) );
    tmp->s = s ;
    tmp->suivant = l ;
    return tmp ;
}

void empileS(struct Station s, ListeS *L)
{
    *L = ajouteS(s,*L) ;
}



//****************************************************************


int main(){
    system("cls");
    ListeOp l1;
    initVide(&l1);

    const char *tmpccl = "temps_cycle.txt";
    float tempscycle;
    tempscycle = temps_cycle(tmpccl);


    const char *op = "operation.txt";
    operations(&l1,op);

    const char *exc = "exclusion.txt";
    exclusion(&l1,exc);

    const char *prec = "prescedence.txt";
    precedence(&l1,prec);

    int nb_couleur = coloration(&l1);
    printf("nb couleur:%d\n", nb_couleur );

    affiche_Liste(l1);


   return 0;
}

