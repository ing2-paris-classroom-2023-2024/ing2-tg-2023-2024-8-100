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
    bool visite;

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
    New.visite = false;
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
    printf("](nb=%d)\n",o.nbexc);

    printf("precedence:[");
    if(o.precedence[0]!=-1) {
        printf("%d,", o.precedence[0]);
    }
    for(int i  = 1; i< MAX_LENGTH; i++){
        if(o.precedence[i]!=-1) {
            printf("%d,", o.precedence[i]);
        }
    }
    printf("] (nb=%d)\n",o.nbprec);
    printf("visite= %d\n",o.visite);

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

ListeOp copieListeOperations(ListeOp original) {
    ListeOp copie = NULL;
    ListeOp courant = original;

    while (courant != NULL) {
        ListeOp nouveau = (ListeOp)malloc(sizeof(struct Bloc));
        if (nouveau == NULL) {
            // Gestion de l'erreur d'allocation mÃ©moire
            return NULL;
        }

        nouveau->op = courant->op;
        nouveau->suivant = NULL;

        if (copie == NULL) {
            copie = nouveau;
        } else {
            ListeOp temp = copie;
            while (temp->suivant != NULL) {
                temp = temp->suivant;
            }
            temp->suivant = nouveau;
        }

        courant = courant->suivant;
    }

    return copie;
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
        perror("Erreur lors de l'ouverture du fichier temps_cycle");
    }

    float res;

    fscanf(file, "%f",&res);

    fclose(file);
    return res;
}

void operation(ListeOp *l, char const *fichier){
    FILE* file = fopen(fichier,"r");
    if (file==NULL) {
        perror("Erreur lors de l'ouverture du fichier operation");
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
        perror("Erreur lors de l'ouverture du fichier exclusion");
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

void prescedence(ListeOp *l, char const *fichier){
    FILE* file = fopen(fichier,"r");
    if (file==NULL) {
        perror("Erreur lors de l'ouverture du fichier prescedence");
    }

    int i, j;

    while(fscanf(file, "%d %d", &i, &j) != EOF){
        //struct Operation *ptr_i = PtrOp(l,i);
        struct Operation *ptr_j = PtrOp(l,j);

        //(ptr_i)->precedence[(ptr_i)->nbprec++] = j;
        (ptr_j)->precedence[(ptr_j)->nbprec++] = i;

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
    }else {
        if (l->op.couleur == -1) {
            return false;
        } else {
            return toutesOperationsColorees(l->suivant);
        }
    }
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
    float temps_total;
    int couleur;  //ou id
}Station;


struct Station newStatCoul(float temps_cycle, int couleur,ListeOp l){
    struct Station New;
    New.temps_cycle = temps_cycle;
    New.couleur = couleur;
    New.temps_total = 0;

    int i=0;
    ListeOp current = l;
    while (current != NULL) {
        if (current->op.couleur == couleur) {
            New.operations[i] = current->op.id;
            New.temps_total += current->op.temps;
            i++;
        }
        current = current->suivant;
    }
    New.nb_operations = i;
    for(int j = i; j<MAX_LENGTH; j++){
        New.operations[j] = -1;
    }
    return New;
}


void afficheS(struct Station s){
    printf("Station %d\n",s.couleur);
    printf("Temps cycle:%f\n",s.temps_cycle);
    printf("Temps total:%f\n",s.temps_total);
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

bool tritopofini(int tritopo[], int nbop){
    for(int i =0; i<nbop; i++){
        if(tritopo[i]==-1){
            return false;
        }
    }
    return true;
}

void retirerDesSucceseurs(ListeOp *l, int id, int nbop){
    ListeOp *pl;
    pl = l;
    while(*pl!=NULL){
        for(int i=0; i< nbop ;i++ ){
            if((*pl)->op.precedence[i]==id){
                (*pl)->op.precedence[i]=-1;
                (*pl)->op.nbprec--;

            }
        }
        pl = &(*pl)->suivant;
    }
}

int* triTopo(ListeOp* l){
    int nbop = compterOperations(l);
    int* tritopo = (int*)malloc(nbop * sizeof(int));
    int t = 0;
    for(int i  = 0; i< nbop; i++){
        tritopo[i] = -1;
    }
    ListeOp* pl;
    ListeOp copie = copieListeOperations(*l);
    while (!tritopofini(tritopo,nbop)) {
        pl = &copie;
        while ((*pl)->op.nbprec!=0 || (*pl)->op.visite){
            pl  = &(*pl)->suivant;
        }
        if(*pl==NULL){
            perror("le graphique a un cycle (n'est pas un DAG)");
        }else{


            tritopo[t] = (*pl)->op.id;
            t++;
            retirerDesSucceseurs(&copie,(*pl)->op.id,nbop);
            (*pl)->op.visite = true;

        }
    }
    return tritopo;
}




//*****************************************************************************
struct BlocS{
    struct Station s;
    struct BlocS *suivant;
}BlocS;

typedef struct BlocS *ListeS;

void initVideS( ListeS *L){
    *L = NULL ;
}

ListeS ajouteS(struct Station s, ListeS l){
    ListeS tmp = (ListeS) malloc(sizeof(BlocS) );
    tmp->s = s ;
    tmp->suivant = l ;
    return tmp ;
}

void empileS(struct Station s, ListeS *L){
    *L = ajouteS(s,*L) ;
}

void affiche_ListeS(ListeS l){
    if(l!=NULL){
        affiche_ListeS(l->suivant);
        afficheS(l->s);
        printf("\n");
    }
}



//****************************************************************


struct Station newStat(float temps_cycle, int id){
    struct Station New;
    New.temps_cycle = temps_cycle;
    New.temps_total = 0;

    New.nb_operations = 0;
    for(int j = 0; j<MAX_LENGTH; j++){
        New.operations[j] = -1;
    }
    New.couleur =id;
    return New;
}

bool ajouterOp(struct Operation op, struct Station *s){
    if(s->temps_total +op.temps <= s->temps_cycle){
        s->operations[s->nb_operations] = op.id;
        s->temps_total += op.temps;
        s->nb_operations++;
        return true;
    }else{
        return false;
    }
}

bool creatfini(int tritopo[], int nbop){
    for(int i =0; i<nbop; i++){
        if(tritopo[i]!=-1){
            return false;
        }
    }
    return true;
}


void creationStationsPresTemp(ListeOp *l,float temps_cycle, ListeS *res){
    int nbop = compterOperations(l);
    int* tritopo = (int*)malloc(nbop * sizeof(int));
    tritopo = triTopo(l);
/*
    printf("Tri Topologique:");
    for(int i  = 1; i< nbop; i++){
        printf("%d ,",tritopo[i]);
    }
    printf("\n\n");
*/
    int i = 0;
    int j = 0;
    while(!creatfini(tritopo,nbop) && i<nbop){
        struct Station sta= newStat(temps_cycle,j);

        while( i<nbop &&  ajouterOp( *PtrOp(l,tritopo[i]) ,&sta )){
            tritopo[i]=-1;
            i++;
        }
        empileS(sta,res);
        j++;
    }
}


//****************************************************************


int main(){
    system("cls");
    ListeOp l;
    initVide(&l);

    const char *tmpccl = "temps_cycle.txt";
    float tempscycle;
    tempscycle = temps_cycle(tmpccl);

    const char *op = "operation.txt";
    operation(&l,op);

    const char *exc = "exclusion.txt";
    exclusion(&l,exc);

    const char *prec = "prescedence.txt";
    prescedence(&l,prec);

    //affiche_Liste(l);


    int nb_couleur = coloration(&l);
    //printf("nb couleur:%d\n", nb_couleur );


    ListeS S_exc;
    initVideS(&S_exc);
    for(int i = 0; i<nb_couleur; i++){
        struct Station station = newStatCoul(tempscycle,i,l);
        empileS(station,&S_exc);
    }
    printf("Contrainte d'exclusion:\n");
    affiche_ListeS(S_exc);

    printf("===============================================================\n");

    printf("Contrainte de precedence et de temps de cycle:\n");
    ListeS S_pres_tem;
    initVideS(&S_pres_tem);
    creationStationsPresTemp(&l,tempscycle,&S_pres_tem);
    affiche_ListeS(S_pres_tem);

    printf("\nFIN MAIN\n");

    return 0;
}

