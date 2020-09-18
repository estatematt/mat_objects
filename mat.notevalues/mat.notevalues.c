/**
	@file
	mat_notevalues - a max object shell
    matthew aidekman - matthewaudio@gmail.com
    Tadha.art
*/

#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object

////////////////////////// object struct
typedef struct _mat_notevalues
{
	t_object ob;			// the object itself (must be first)

    long inIdx;
    void* proxy;
    
    
    int suffix; // 0 = n, 1=nd , 2=nt
    int value;
    
    
    void* numBoxOut;  // mess
    void* commentOut; // mess
    void* dotOut; //mess
    void* tripletOut;  //mess
    void* valOut; //mess
    
    char* mainStrings[8];
    char* suffixStrings[3];
    
    
    
} t_mat_notevalues;

///////////////////////// function prototypes

bool valFromTimeString(char* input, int *val, int* suffix);
bool mat_notevalues_commentStr(t_mat_notevalues *x, char **commentString);
void mat_notevalues_updateDisplay(t_mat_notevalues *x);
bool mat_notevalues_outStr(t_mat_notevalues *x,char **outStr);

//// standard set
void *mat_notevalues_new(t_symbol *s, long argc, t_atom *argv);
void mat_notevalues_free(t_mat_notevalues *x);
void mat_notevalues_assist(t_mat_notevalues *x, void *b, long m, long a, char *s);
void mat_notevalues_int(t_mat_notevalues *x, long n);
void mat_notevalues_anything(t_mat_notevalues *x, t_symbol *s, long ac, t_atom *av);
void mat_notevalues_bang(t_mat_notevalues *x);


//////////////////////// global class pointer variable
void *mat_notevalues_class;


void ext_main(void *r)
{
	t_class *c;

	c = class_new("mat.notevalues", (method)mat_notevalues_new, (method)mat_notevalues_free, (long)sizeof(t_mat_notevalues),
				  0L /* leave NULL!! */, A_GIMME, 0);

	/* you CAN'T call this from the patcher */
	class_addmethod(c, (method)mat_notevalues_assist,			"assist",		A_CANT, 0);
    class_addmethod(c, (method)mat_notevalues_int,"int",A_LONG, 0);
    class_addmethod(c, (method)mat_notevalues_anything,"anything",A_GIMME, 0);
    class_addmethod(c, (method)mat_notevalues_bang,"bang", 0);

    
	class_register(CLASS_BOX, c); /* CLASS_NOBOX */
	mat_notevalues_class = c;

}

void mat_notevalues_assist(t_mat_notevalues *x, void *b, long m, long a, char *s)
{
	
}

void mat_notevalues_free(t_mat_notevalues *x)
{
	; //nothing to free
}


void *mat_notevalues_new(t_symbol *s, long argc, t_atom *argv)
{
	t_mat_notevalues *x = NULL;

	if ((x = (t_mat_notevalues *)object_alloc(mat_notevalues_class))) {

        x->proxy = proxy_new((t_object*)x, 3, &x->inIdx);
        x->proxy = proxy_new((t_object*)x, 2, &x->inIdx);
        x->proxy = proxy_new((t_object*)x, 1, &x->inIdx);
        
        x->valOut=outlet_new(x, NULL);
        x->tripletOut=outlet_new(x, NULL);
        x->dotOut=outlet_new(x, NULL);
        x->commentOut=outlet_new(x, NULL);
        x->numBoxOut=outlet_new(x, NULL);

        x->suffix=0;
        x->value=0;
        x->mainStrings[0]="1";
        x->mainStrings[1]="1/2";
        x->mainStrings[2]="1/4";
        x->mainStrings[3]="1/8";
        x->mainStrings[4]="1/16";
        x->mainStrings[5]="1/32";
        x->mainStrings[6]="1/64";
        x->mainStrings[7]="1/128";
        
        x->suffixStrings[0] = "";
        x->suffixStrings[1] = " •";
        x->suffixStrings[2] = " 3";
        
    
    }
	return (x);
}

/*
 
 
 
 */
void mat_notevalues_int(t_mat_notevalues *x, long n)
{
    long inletIdx = proxy_getinlet((t_object*)x);
    
    if(inletIdx<0 || inletIdx>2){
        error("error %ld %s",__LINE__,__func__);
        return;
    }
    
    if(inletIdx==0){// note value
        
        if(n<0||n>7){
            error("error %ld %s",__LINE__,__func__);
            return;
        }
        
        x->value=n;
        
        if(x->value==7){
            x->suffix=0;
        }else if(x->value==6 && x->suffix==2){
            x->suffix=0;
        }
        
        
    }else if(inletIdx == 1){  //dotted yes or no?
        if(n<0||n>1){
            error("error %ld %s",__LINE__,__func__);
            return;
        }
        
        x->suffix = n==1?1:0;
        
        
    }else if(inletIdx == 2){  // triplet yes or no?
        if(n<0||n>1){
            error("error %ld %s",__LINE__,__func__);
            return;
        }
        
        x->suffix = n==1?2:0;
        
        
    }
    
    //check for suffixes ommitted from notevalue system;
    if(x->value==7){
        x->suffix=0;
    }else if(x->value==6 && x->suffix==2){
        x->suffix=0;
    }
    
    mat_notevalues_bang(x);
    mat_notevalues_updateDisplay(x);
    
    
}



void mat_notevalues_anything(t_mat_notevalues *x, t_symbol *s, long ac, t_atom *av){
    long inletIdx = proxy_getinlet((t_object*)x);
    
    if(inletIdx!=3){
        error("wrong type for this inlet");
        return;
    }
    
    int isASetMessage = -1;
    
    
    //is this a set message?
    if(ac==1){
        if(s==gensym("set") && atom_gettype(av)==A_SYM){
            isASetMessage=1;
        }
    }else if(ac==0){
        isASetMessage=0;
    }
    
    if(isASetMessage==-1){
        error("i dont understand");
        return;
    }
    
    
    int valOut = -1;
    int suffixOut = -1;
    
    char *inStr = NULL;
    if(isASetMessage==1){
        inStr = atom_getsym(av)->s_name;
    }else{
        inStr = s->s_name;
    }
    
    bool err = valFromTimeString(inStr, &valOut, &suffixOut);
    
    if(err){
        error("ERROR! %ld %s",__LINE__,__func__);
        return;
    }
    
    x->value = valOut;
    x->suffix=suffixOut;
    
    
    if(isASetMessage==0){
        mat_notevalues_bang(x);
    }
    mat_notevalues_updateDisplay(x);
}

void mat_notevalues_bang(t_mat_notevalues *x){

    char* outValStr="               ";
    bool err =  mat_notevalues_outStr(x, &outValStr);
    if(err){
        error("ERROR! %ld %ld",__LINE__,__func__);
        return;
    }
    
    t_atom outValAtom[1];
    
    atom_setsym(&outValAtom[0], gensym(outValStr));
    
    outlet_atoms(x->valOut, 1, outValAtom);
    

    free(outValStr);
    
}


//in this case the display is all the objects used to set the note value.
//there's no UI to this object
void mat_notevalues_updateDisplay(t_mat_notevalues *x){
    
    t_atom tripletAtoms[2];
    t_atom dotAtoms[2];
    t_atom commentAtoms[2];
    t_atom numboxAtoms[2];


    atom_setlong(&tripletAtoms[0], 4);
    atom_setlong(&dotAtoms[0], 3);
    atom_setlong(&commentAtoms[0], 2);
    atom_setlong(&numboxAtoms[0], 1);
    
    
    t_symbol *setSymbol=gensym("set");
    atom_setsym(&tripletAtoms[0], setSymbol);
    atom_setsym(&dotAtoms[0], setSymbol);
    atom_setsym(&commentAtoms[0], setSymbol);
    atom_setsym(&numboxAtoms[0], setSymbol);
    
    char* commentStr=NULL;
    bool err = mat_notevalues_commentStr(x,&commentStr);
    if(err){
        error("ERROR!");
        return;
    }
    atom_setsym(&commentAtoms[1], gensym(commentStr));
    
    
    switch(x->suffix){
        case 0:
            atom_setlong(&dotAtoms[1], 0);
            atom_setlong(&tripletAtoms[1], 0);
            break;
        case 1:
            atom_setlong(&dotAtoms[1], 1);
            atom_setlong(&tripletAtoms[1], 0);
            break;
        case 2:
            atom_setlong(&dotAtoms[1], 0);
            atom_setlong(&tripletAtoms[1], 1);
            break;
    }
    
    
    
    atom_setlong(&numboxAtoms[1], x->value);
    
    outlet_atoms(x->tripletOut, 2, tripletAtoms);
    outlet_atoms(x->dotOut, 2, dotAtoms);
    outlet_atoms(x->commentOut, 2, commentAtoms);
    outlet_atoms(x->numBoxOut, 2, numboxAtoms);
    
    
    
    free(commentStr);
    
}




//returns true if there's an error
bool valFromTimeString(char* input, int *val, int* suffix){
    
    unsigned long inLen = strlen(input);
    
    if(inLen<2 || inLen>5){
        return true;
    }
    
    int positionOfN = -1;
    for( int i = 0 ; i < inLen; i++ ){
        if(input[i]=='n'){
            positionOfN=i;
            break;
        }
    }
    
    if(positionOfN==-1){
        return true;
    }
    if (positionOfN>3){
        return true;
    }
    
    
    if(positionOfN==1){
        switch(input[0]){
            case '1':
                *val=0;
                break;
            case '2':
                *val = 1;
                break;
            case '4':
                *val = 2;
                break;
            case '8':
                *val = 3;
                break;
            default:
                return true;
        }
        
    }else if(positionOfN==2){
        if(input[0]=='1' &&
           input[1]=='6' ){
            *val = 4;
            
        }else if(input[0]=='3' &&
                 input[1]=='2' ){
            *val =5;
            
        }else if(input[0]=='6' &&
                 input[1]=='4' ){
            *val = 6;
            
        }else{
            return true;
        }
    }else if(positionOfN==3){
        if(input[0]=='1' &&
           input[1]=='2' &&
           input[2]=='8' ){
            *val = 7;
        }else{
            return true;
        }
    }
    
    if(inLen==positionOfN+1){
        *suffix =0;
    }else if(input[positionOfN+1]=='d'){
        *suffix =1;
    }else if(input[positionOfN+1]=='t'){
        *suffix =2;
    }else{
        return true;
    }
    
    
    if(*val == 7 && *suffix!=0){
        return true;
    }else if(*val==6 && *suffix==2){
        return true;
    }
    
    
    return false;
}



bool mat_notevalues_commentStr(t_mat_notevalues *x, char **commentString){

    if(x->value<0 || x->value>7){
        return true;
    }
    
    if(x->suffix<0 || x->suffix>2){
        return true;
    }
    
    int mainStrLen = strlen(x->mainStrings[x->value]);
    int suffixStrLen = strlen(x->suffixStrings[x->suffix]);
    *commentString = malloc(mainStrLen + suffixStrLen+1);
    
    strcpy(*commentString, "");
    strcat(*commentString, x->mainStrings[x->value]);
    strcat(*commentString, x->suffixStrings[x->suffix]);
    
    return false;
    
    
}



//creates a string in the notevalue system.  Perhaps these could be precalculated but I'm lazy.
bool mat_notevalues_outStr(t_mat_notevalues *x,char **outStr){
    
    int outInt = pow(2,x->value);
    
    *outStr = malloc(6);

    
    char* suffix="";
    if(x->suffix == 1){
        suffix="d";
    }else if(x->suffix==2){
        suffix="t";
    }
    
    sprintf(*outStr,"%dn%s", outInt,suffix);
    
    return false;
    
    
}
