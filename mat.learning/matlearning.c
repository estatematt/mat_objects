/**
	matlearning - a max object that observes the learning attribute of the nearest live.numbox
*/

#include "ext.h"							// standard Max include, always required
#include "ext_obex.h"						// required for new style Max object
#include "ext_hashtab.h"
////////////////////////// object struct
typedef struct _matlearning
{
	t_object					ob;			// the object itself (must be first)
    t_symbol *classname;
    void* m_out;
} t_matlearning;

///////////////////////// function prototypes
//// standard set
void *matlearning_new(t_symbol *s, long argc, t_atom *argv);
void matlearning_free(t_matlearning *x);
void matlearning_assist(t_matlearning *x, void *b, long m, long a, char *s);
void matlearning_attach(t_matlearning *x, t_symbol *s, long ac, t_atom *av);
void matlearning_setClassName(t_matlearning *x, t_symbol *s, long ac, t_atom *av);

void matlearning_notify(t_matlearning *x, t_symbol *s, t_symbol *msg, void *sender, void *data);
t_object* getFistTobjectOfClassName(t_matlearning *x,char* classname,t_object **boxOut);




//////////////////////// global class pointer variable
void *matlearning_class;


void ext_main(void *r)
{
	t_class *c;

	c = class_new("mat.learning", (method)matlearning_new, (method)matlearning_free, (long)sizeof(t_matlearning),
				  0L /* leave NULL!! */, A_GIMME, 0);

	/* you CAN'T call this from the patcher */
	class_addmethod(c, (method)matlearning_assist,			"assist",		A_CANT, 0);
    
    class_addmethod(c, (method)matlearning_attach, "attach",A_GIMME,0);
    class_addmethod(c, (method)matlearning_notify, "notify", A_CANT, 0);
    class_addmethod(c, (method)matlearning_setClassName, "setclassname", A_GIMME, 0);

	class_register(CLASS_BOX, c); /* CLASS_NOBOX */
	matlearning_class = c;
}

void matlearning_assist(t_matlearning *x, void *b, long m, long a, char *s)
{
	if (m == ASSIST_INLET) { // inlet
		sprintf(s, "I am inlet %ld", a);
	}
	else {	// outlet
		sprintf(s, "I am outlet %ld", a);
	}
}

void matlearning_free(t_matlearning *x)
{
	;
}


void matlearning_setClassName(t_matlearning *x, t_symbol *s, long ac, t_atom *av){
    if(atom_gettype(av)!=A_SYM){
        return;
    }
    x->classname=atom_getsym(av);
}


void *matlearning_new(t_symbol *s, long argc, t_atom *argv)
{
	t_matlearning *x = NULL;

	if ((x = (t_matlearning *)object_alloc(matlearning_class))) {
        x->classname = gensym("live.numbox");
        x->m_out=outlet_new(x,NULL);

    }else{
        error("bogus?");
    }
    
    
	return (x);
}



t_object* getFistTobjectOfClassName(t_matlearning *x,char* classname,t_object **boxOut){
    t_object* inspectMe =NULL;
    
    t_object *patcher, *box, *obj;
    object_obex_lookup(x, gensym("#P"), &patcher);

    
    for (box = jpatcher_get_firstobject(patcher); box; box = jbox_get_nextobject(box)) {
        *boxOut=box;
        obj = jbox_get_object(box);
        if (obj){
            if(strcmp(object_classname(obj)->s_name,classname)==0){
                inspectMe=obj;
                break;
            }
        }else{
            post("box with NULL object");
        }
    }
    if(inspectMe==NULL){
        error("none found");
        return NULL;
    }
    
    return inspectMe;
}




void matlearning_attach(t_matlearning *x, t_symbol *s, long ac, t_atom *av)
{
    
    t_object* inspectMe;
    t_object* box=NULL;

    inspectMe =getFistTobjectOfClassName(x,x->classname->s_name,&box);
    if(inspectMe==NULL){
        return;
    }

    t_max_err err= object_attach_byptr(x,inspectMe);
    
    if(err!=MAX_ERR_NONE){
        object_error((t_object*)x, "mat.learning attached to nothing");
        return;
    }
    
    
    
    long attrC = 0;
    t_atom *attrV = NULL;
    
    err= object_attr_getvalueof(inspectMe, gensym("learning"), &attrC, &attrV);
    
    if(err==MAX_ERR_NONE){

        
        outlet_atoms(x->m_out, 1, attrV);
    }
    
    
}


void matlearning_notify(t_matlearning *x, t_symbol *s, t_symbol *msg, void *sender, void *data){
    
    
    t_object *inspectMe =  (t_object*)data;
    
    if(inspectMe==NULL){
        return;
    }
    
    t_symbol *name = object_method(inspectMe,gensym("getname"));
    
    if(name!=gensym("learning")){
        return;
    }
    
    long attrC = 0;
    t_atom *attrV = NULL;
    t_max_err err;
    
    
    err= object_attr_getvalueof(sender, gensym("learning"), &attrC, &attrV);
    
    if(err==MAX_ERR_NONE){
        outlet_atoms(x->m_out, 1, attrV);
    }
    
    
}
