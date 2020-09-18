/**
 matjsuiplus
Matthew Aidekman matthewaudio  attt  gmai1 com
 
 
 clear overlay that sends extra UI messages to jsui (or LCD).
 
 I'd love to introduce some sort of parameter mode but it's undocumented and couldn't nail down a solution.
 */

#include "ext.h"
#include "ext_obex.h"
#include "jgraphics.h"
#include "ext_parameter.h"
#include "ext_boxstyle.h"
#include "jpatcher_utils.h"
t_class	*s_matjsuiplus_class = 0;

typedef struct _matjsuiplus
{
    t_jbox j_box;
    int deltamode;
    bool reportIdle;
    t_pt  runningMousePosition;
    long runningModifiers;
    t_jrgba u_outline;
    float fval;
    
} t_matjsuiplus;

void matjsuiplus_assist(t_matjsuiplus *x, void *b, long m, long a, char *s);
t_matjsuiplus *matjsuiplus_new(t_symbol *s, long argc, t_atom *argv);
void matjsuiplus_int(t_matjsuiplus *x, long n);
void matjsuiplus_float(t_matjsuiplus *x, float n);

void matjsuiplus_free(t_matjsuiplus *x);

void matjsuiplus_paint(t_matjsuiplus *x, t_object *view);

t_max_err matjsuiplus_notify(t_matjsuiplus *x, t_symbol *s, t_symbol *msg, void *sender, void *data);

void matjsuiplus_mouseenter(t_matjsuiplus *x, t_object *patcherview, t_pt pt, long modifiers);
void matjsuiplus_mouseleave(t_matjsuiplus *x, t_object *patcherview, t_pt pt, long modifiers);
void matjsuiplus_mousemove(t_matjsuiplus *x, t_object *patcherview, t_pt pt, long modifiers);
void matjsuiplus_mousewheel(t_matjsuiplus *x, t_object *patcherview,  t_pt pt, long modifiers, double x_inc, double y_inc);
void matjsuiplus_mousedoubleclick(t_matjsuiplus *x, t_object *patcherview, t_pt pt, long modifiers);
void matjsuiplus_mousedragdelta(t_matjsuiplus *x, t_object *patcherview, t_pt pt, long modifiers);
void matjsuiplus_mousedown(t_matjsuiplus *x, t_object *patcherview, t_pt pt, long modifiers);
void matjsuiplus_mouseup(t_matjsuiplus *x, t_object *patcherview, t_pt pt, long modifiers);
long matjsuiplus_key(t_matjsuiplus *x, t_object *patcherview, long keycode, long modifiers, long textcharacter);
void matjsuiplus_focusgained(t_matjsuiplus *x, t_object *patcherview);
void matjsuiplus_focuslost(t_matjsuiplus *x, t_object *patcherview);
long matjsuiplus_hittest(t_matjsuiplus *x, t_object *patcherview, t_pt pt);


t_max_err matjsuiplus_setvalueof(t_matjsuiplus *x, long ac, t_atom *av);
t_max_err matjsuiplus_getvalueof(t_matjsuiplus *x, long *ac, t_atom **av);
void matjsuiplus_set(t_matjsuiplus *x, t_symbol *s, long ac, t_atom *av);
void matjsuiplus_bang(t_matjsuiplus *x);

t_max_err matjsuiplus_getValType(t_matjsuiplus *x,int *type);
void matjsuiplus_getdrawparams(t_matjsuiplus *x, t_object *patcherview, t_jboxdrawparams *params);
void postargss(long argc, t_atom *argv);

void ext_main(void *r)
{
    t_class *c;
    
    c = class_new("mat.jsuiplus",
                  (method)matjsuiplus_new,
                  (method)matjsuiplus_free,
                  sizeof(t_matjsuiplus),
                  (method)NULL,
                  A_GIMME,
                  0L);
    
    c->c_flags |= CLASS_FLAG_NEWDICTIONARY;
    jbox_initclass(c, JBOX_COLOR);
    
    /*
     max messages
     */
    class_addmethod(c, (method) matjsuiplus_bang, "bang", A_CANT, 0);
    class_addmethod(c, (method) matjsuiplus_int, "int", A_LONG, 0);
    class_addmethod(c, (method) matjsuiplus_float, "float", A_FLOAT, 0);
    class_addmethod(c, (method) matjsuiplus_set, "set", A_GIMME, 0);
    
    class_addmethod(c, (method) matjsuiplus_getdrawparams, "getdrawparams", A_CANT, 0);
    class_addmethod(c, (method) matjsuiplus_paint, "paint", A_CANT, 0);
    class_addmethod(c, (method) matjsuiplus_assist, "assist", A_CANT, 0);
    class_addmethod(c, (method) jbox_notify, "notify", A_CANT, 0);
    
    class_addmethod(c, (method)matjsuiplus_getvalueof, "getvalueof", A_CANT, 0);
    class_addmethod(c, (method)matjsuiplus_setvalueof, "setvalueof", A_CANT, 0);
    
    /*
     INTERACTIONS
     */
    
    class_addmethod(c, (method) matjsuiplus_mousedoubleclick, "mousedoubleclick", A_CANT, 0);
    class_addmethod(c, (method) matjsuiplus_mouseenter, "mouseenter", A_CANT, 0);
    class_addmethod(c, (method) matjsuiplus_mouseleave, "mouseleave", A_CANT, 0);
    class_addmethod(c, (method) matjsuiplus_mousewheel, "mousewheel", A_CANT, 0);
    class_addmethod(c, (method) matjsuiplus_mousemove, "mousemove", A_CANT, 0);
    class_addmethod(c, (method) matjsuiplus_mousedown, "mousedown", A_CANT, 0);
    class_addmethod(c, (method) matjsuiplus_mousedragdelta, "mousedragdelta", A_CANT, 0);
    class_addmethod(c, (method) matjsuiplus_mouseup, "mouseup", A_CANT, 0);
    class_addmethod(c, (method) matjsuiplus_hittest, "hittest", A_CANT, 0);
    class_addmethod(c, (method) matjsuiplus_key, "key", A_CANT, 0);
    class_addmethod(c, (method) matjsuiplus_focusgained, "focusgained", A_CANT, 0);
    class_addmethod(c, (method) matjsuiplus_focuslost, "focuslost", A_CANT, 0);
    
    
    CLASS_ATTR_DEFAULT(c, "patching_rect", 0, "0 0 60 60");
    CLASS_ATTR_DEFAULT(c, "color", 0, "0.8 0.5 0.2 1");
    
    CLASS_ATTR_LONG(c,"deltamode",0, t_matjsuiplus, deltamode);
    CLASS_ATTR_STYLE_LABEL(c,"deltamode",0,"onoff","deltamode");
    CLASS_ATTR_CATEGORY(c,"deltamode",0,"Tweaks");
    CLASS_ATTR_DEFAULT_SAVE(c,"deltamode",0,"1");
    
    CLASS_ATTR_LONG(c,"reportidle",0, t_matjsuiplus, reportIdle);
    CLASS_ATTR_STYLE_LABEL(c,"reportidle",0,"onoff","reportidle");
    CLASS_ATTR_CATEGORY(c,"reportidle",0,"Tweaks");
    CLASS_ATTR_DEFAULT_SAVE(c,"reportidle",0,"1");
    
    CLASS_ATTR_RGBA(c, "bordercolor", 0, t_matjsuiplus, u_outline);
    CLASS_ATTR_DEFAULTNAME_SAVE_PAINT(c, "bordercolor", 0, "0 0 0 0");
    CLASS_ATTR_STYLE_LABEL(c,"bordercolor",0,"rgba","Border Color");
    
    class_parameter_init(c);
    
    s_matjsuiplus_class = c;
    class_register(CLASS_BOX, s_matjsuiplus_class);
    //return 0;
}



t_matjsuiplus *matjsuiplus_new(t_symbol *s, long argc, t_atom *argv)
{
    t_matjsuiplus *x = NULL;
    t_max_err err = MAX_ERR_GENERIC;
    t_dictionary *d;
    long flags;
    
    if (!(d=object_dictionaryarg(argc,argv)))
        return NULL;
    
    x = (t_matjsuiplus *) object_alloc(s_matjsuiplus_class);
    
    x->runningMousePosition.x=0;
    x->runningMousePosition.y=0;
    x->runningModifiers=0;
    flags = 0
    | JBOX_DRAWFIRSTIN
    | JBOX_NODRAWBOX
    | JBOX_DRAWINLAST
    | JBOX_TRANSPARENT
    | JBOX_GROWBOTH
    | JBOX_HILITE
    | JBOX_BACKGROUND
    | JBOX_DRAWBACKGROUND
    | JBOX_MOUSEDRAGDELTA
    ;
    
    if(argc>0){
        
        if(atom_gettype(argv)==A_OBJ){
            
            
            t_dictionary * dictarg = atom_getobj(argv);
            
            t_atom_long deltaMode = 0;
            dictionary_getlong(dictarg, gensym("deltamode"), &deltaMode);
            if(deltaMode>0){
                flags=flags|JBOX_MOUSEDRAGDELTA;
            }
            
        }
    }
    
   // postdictionary((t_object*)d);
    
    // I HAVE NO CLUE WHAT I'M SUPPOSED TO DO HERE TO MAKE PARAMETER MODE WORK CORRECTLY
    object_parameter_init((t_object*)x,PARAM_TYPE_FLOAT);
    err = jbox_new(&x->j_box, flags, argc, argv);
    outlet_new((t_object *)x,NULL);
    x->j_box.b_firstin = (t_object *) x;
    attr_dictionary_process(x, d);
    jbox_ready(&x->j_box);
    object_parameter_dictionary_process((t_object *)x,d);
    return x;
}


void matjsuiplus_assist(t_matjsuiplus *x, void *b, long m, long a, char *s)
{
    if (m == ASSIST_INLET)
        sprintf(s,"bang Says \"bang\"");
}

void matjsuiplus_int(t_matjsuiplus *x, long n){
    
    x->fval = (float)n;
    matjsuiplus_bang(x);
}

void matjsuiplus_float(t_matjsuiplus *x, float n){
    
    x->fval = n;
    matjsuiplus_bang(x);
}

void matjsuiplus_free(t_matjsuiplus *x){
    
    object_parameter_free((t_object *)x);
    jbox_free(&x->j_box);
}

t_max_err matjsuiplus_notify(t_matjsuiplus *x, t_symbol *s, t_symbol *msg, void *sender, void *data){
    
    return MAX_ERR_NONE;
}

void matjsuiplus_paint(t_matjsuiplus *x, t_object *patcherview)
{
    return;
}


void matjsuiplus_bang(t_matjsuiplus *x)
{
    int type;
    t_max_err err= matjsuiplus_getValType(x,&type);
    
    if(err!=MAX_ERR_NONE){
        error("error");
        return;
    }
    
    if(type==PARAM_TYPE_FLOAT){
        t_atom modi[1];
        atom_setfloat(&modi[0], x->fval);
        outlet_anything(x->j_box.b_ob.o_outlet, gensym("_val"), 1, modi);
    }else{
        t_atom modi[1];
        atom_setlong(&modi[0], x->fval);
        outlet_anything(x->j_box.b_ob.o_outlet, gensym("_val"), 1, modi);
    }
    
}

void matjsuiplus_mouseenter(t_matjsuiplus *x, t_object *patcherview, t_pt pt, long modifiers){
    
    if(!x->reportIdle){
        return;
    }
    x->runningMousePosition = pt;
    x->runningModifiers = modifiers;
    
    t_atom modi[7];
    atom_setlong(&modi[0], (long)pt.x);
    atom_setlong(&modi[1], (long)pt.y);
    atom_setlong(&modi[2], (modifiers&eCommandKey)==eCommandKey?1:0);
    atom_setlong(&modi[3], (modifiers&eShiftKey)==eShiftKey?1:0);
    atom_setlong(&modi[4], (modifiers&eCapsLock)==eCapsLock?1:0);
    atom_setlong(&modi[5], (modifiers&eAltKey)==eAltKey?1:0);
    atom_setlong(&modi[6], (modifiers&eControlKey)==eControlKey?1:0);
    outlet_anything(x->j_box.b_ob.o_outlet, gensym("_mouseenter"), 7, modi);
}


void matjsuiplus_mouseleave(t_matjsuiplus *x, t_object *patcherview, t_pt pt, long modifiers){
    if(!x->reportIdle){
        return;
    }
    t_atom modi[7];
    atom_setlong(&modi[0], (long)pt.x);
    atom_setlong(&modi[1], (long)pt.y);
    atom_setlong(&modi[2], (modifiers&eCommandKey)==eCommandKey?1:0);
    atom_setlong(&modi[3], (modifiers&eShiftKey)==eShiftKey?1:0);
    atom_setlong(&modi[4], (modifiers&eCapsLock)==eCapsLock?1:0);
    atom_setlong(&modi[5], (modifiers&eAltKey)==eAltKey?1:0);
    atom_setlong(&modi[6], (modifiers&eControlKey)==eControlKey?1:0);
    outlet_anything(x->j_box.b_ob.o_outlet, gensym("_mouseleave"), 7, modi);
}



void matjsuiplus_mousemove(t_matjsuiplus *x, t_object *patcherview, t_pt pt, long modifiers){
    if(!x->reportIdle){
        return;
    }
    
    if(pt.x!=x->runningMousePosition.x || pt.y!=x->runningMousePosition.y ||modifiers!=x->runningModifiers){
        x->runningMousePosition = pt;
        x->runningModifiers = modifiers;
        
        t_atom modi[7];
        atom_setlong(&modi[0], (long)pt.x);
        atom_setlong(&modi[1], (long)pt.y);
        atom_setlong(&modi[2], (modifiers&eCommandKey)==eCommandKey?1:0);
        atom_setlong(&modi[3], (modifiers&eShiftKey)==eShiftKey?1:0);
        atom_setlong(&modi[4], (modifiers&eCapsLock)==eCapsLock?1:0);
        atom_setlong(&modi[5], (modifiers&eAltKey)==eAltKey?1:0);
        atom_setlong(&modi[6], (modifiers&eControlKey)==eControlKey?1:0);
        outlet_anything(x->j_box.b_ob.o_outlet, gensym("_mousemove"), 7, modi);
    }
}



void matjsuiplus_mousewheel(t_matjsuiplus *x, t_object *patcherview,  t_pt pt, long modifiers, double x_inc, double y_inc){
    
    t_atom modi[9];
    atom_setfloat(&modi[0], x_inc);
    atom_setfloat(&modi[1], y_inc);
    atom_setlong(&modi[2], (long)pt.x);
    atom_setlong(&modi[3], (long)pt.y);
    atom_setlong(&modi[4], (modifiers&eCommandKey)==eCommandKey?1:0);
    atom_setlong(&modi[5], (modifiers&eShiftKey)==eShiftKey?1:0);
    atom_setlong(&modi[6], (modifiers&eCapsLock)==eCapsLock?1:0);
    atom_setlong(&modi[7], (modifiers&eAltKey)==eAltKey?1:0);
    atom_setlong(&modi[8], (modifiers&eControlKey)==eControlKey?1:0);
    outlet_anything(x->j_box.b_ob.o_outlet, gensym("_mousewheel"), 9, modi);
}



void matjsuiplus_mousedoubleclick(t_matjsuiplus *x, t_object *patcherview, t_pt pt, long modifiers){
    
    t_atom modi[7];
    atom_setlong(&modi[0], (long)pt.x);
    atom_setlong(&modi[1], (long)pt.y);
    atom_setlong(&modi[2], (modifiers&eCommandKey)==eCommandKey?1:0);
    atom_setlong(&modi[3], (modifiers&eShiftKey)==eShiftKey?1:0);
    atom_setlong(&modi[4], (modifiers&eCapsLock)==eCapsLock?1:0);
    atom_setlong(&modi[5], (modifiers&eAltKey)==eAltKey?1:0);
    atom_setlong(&modi[6], (modifiers&eControlKey)==eControlKey?1:0);
    outlet_anything(x->j_box.b_ob.o_outlet, gensym("_dblclick"), 7, modi);
}


void matjsuiplus_mousedragdelta(t_matjsuiplus *x, t_object *patcherview, t_pt pt, long modifiers)
{
    x->runningMousePosition.x+=pt.x;
    x->runningMousePosition.y+=pt.y;
    t_atom modi[7];
    atom_setlong(&modi[0], x->runningMousePosition.x);
    atom_setlong(&modi[1], x->runningMousePosition.y);
    atom_setlong(&modi[2], (modifiers&eCommandKey)==eCommandKey?1:0);
    atom_setlong(&modi[3], (modifiers&eShiftKey)==eShiftKey?1:0);
    atom_setlong(&modi[4], (modifiers&eCapsLock)==eCapsLock?1:0);
    atom_setlong(&modi[5], (modifiers&eAltKey)==eAltKey?1:0);
    atom_setlong(&modi[6], (modifiers&eControlKey)==eControlKey?1:0);
    outlet_anything(x->j_box.b_ob.o_outlet, gensym("_mousedragdelta"), 7, modi);
}

void matjsuiplus_mousedown(t_matjsuiplus *x, t_object *patcherview, t_pt pt, long modifiers)
{
    
    
    x->runningMousePosition.x=pt.x;
    x->runningMousePosition.y=pt.y;
    
    t_atom modi[7];
    atom_setlong(&modi[0], x->runningMousePosition.x);
    atom_setlong(&modi[1], x->runningMousePosition.y);
    atom_setlong(&modi[2], (modifiers&eCommandKey)==eCommandKey?1:0);
    atom_setlong(&modi[3], (modifiers&eShiftKey)==eShiftKey?1:0);
    atom_setlong(&modi[4], (modifiers&eCapsLock)==eCapsLock?1:0);
    atom_setlong(&modi[5], (modifiers&eAltKey)==eAltKey?1:0);
    atom_setlong(&modi[6], (modifiers&eControlKey)==eControlKey?1:0);
    outlet_anything(x->j_box.b_ob.o_outlet, gensym("_onclick"), 7, modi);
    
    
    
}

void matjsuiplus_mouseup(t_matjsuiplus *x, t_object *patcherview, t_pt pt, long modifiers)
{
    x->runningMousePosition.x+=pt.x;
    x->runningMousePosition.y+=pt.y;
    
    
    t_atom modi[7];
    atom_setlong(&modi[0], x->runningMousePosition.x);
    atom_setlong(&modi[1], x->runningMousePosition.y);
    atom_setlong(&modi[2], (modifiers&eCommandKey)==eCommandKey?1:0);
    atom_setlong(&modi[3], (modifiers&eShiftKey)==eShiftKey?1:0);
    atom_setlong(&modi[4], (modifiers&eCapsLock)==eCapsLock?1:0);
    atom_setlong(&modi[5], (modifiers&eAltKey)==eAltKey?1:0);
    atom_setlong(&modi[6], (modifiers&eControlKey)==eControlKey?1:0);
    outlet_anything(x->j_box.b_ob.o_outlet, gensym("_mouseup"), 7, modi);
    
}

long matjsuiplus_key(t_matjsuiplus *x, t_object *patcherview, long keycode, long modifiers, long textcharacter)
{
    
    t_atom modi[6];
    atom_setlong(&modi[0], textcharacter);
    atom_setlong(&modi[1], (modifiers&eCommandKey)==eCommandKey?1:0);
    atom_setlong(&modi[2], (modifiers&eShiftKey)==eShiftKey?1:0);
    atom_setlong(&modi[3], (modifiers&eCapsLock)==eCapsLock?1:0);
    atom_setlong(&modi[4], (modifiers&eAltKey)==eAltKey?1:0);
    atom_setlong(&modi[5], (modifiers&eControlKey)==eControlKey?1:0);
    outlet_anything(x->j_box.b_ob.o_outlet, gensym("_key"), 6, modi);
    
    return 0;	// returns 1 if you want to filter it from the key object
}

void matjsuiplus_focusgained(t_matjsuiplus *x, t_object *patcherview)
{
    outlet_anything(x->j_box.b_ob.o_outlet, gensym("_focusgained"), 0, NULL);
}

void matjsuiplus_focuslost(t_matjsuiplus *x, t_object *patcherview)
{
    outlet_anything(x->j_box.b_ob.o_outlet, gensym("_focuslost"), 0, NULL);
}

long matjsuiplus_hittest(t_matjsuiplus *x, t_object *patcherview, t_pt pt)
{
    t_rect	rect;
    long	rv = 1;	// 1 means that we will respond to user clicks
    
    jbox_get_rect_for_view((t_object *)x, patcherview, &rect);
    
    if (pt.x < rect.width &&pt.x>=0 &&pt.y>0 &&pt.y<rect.height) {
        rv = 1;
    }else{
        rv=0;
    }
    return rv;
}



void matjsuiplus_getdrawparams(t_matjsuiplus *x, t_object *patcherview, t_jboxdrawparams *params)
{
    /*
    params->d_borderthickness=0;
    params->d_bordercolor.alpha = 0;
    params->d_boxfillcolor.red = 0;
    params->d_boxfillcolor.green = 0;
    params->d_boxfillcolor.blue = 0;
    params->d_boxfillcolor.alpha = 0;
    */
}


void postargss(long argc, t_atom *argv)
{
    long textsize = 0;
    char *text = NULL;
    t_max_err err;
    
    err = atom_gettext(argc, argv, &textsize, &text, OBEX_UTIL_ATOM_GETTEXT_DEFAULT);
    if (err == MAX_ERR_NONE && textsize && text) {
        post(text);
    }
    if (text) {
        sysmem_freeptr(text);
    }
}


void matjsuiplus_set(t_matjsuiplus *x, t_symbol *s, long ac, t_atom *av){
    
    if (ac && av) {
        if (av->a_type == A_FLOAT || av->a_type == A_LONG){
            x->fval = (float)atom_getfloat(av);
        }
    }
}


t_max_err matjsuiplus_setvalueof(t_matjsuiplus *x, long ac, t_atom *av)
{
    if (ac && av) {
        
        int xValType =-1;
        t_max_err err = matjsuiplus_getValType(x,&xValType);
        if(err!=MAX_ERR_NONE || xValType<0){
            return MAX_ERR_GENERIC;
        }
        
        if(xValType==PARAM_TYPE_FLOAT){
            
            if (av->a_type == A_FLOAT){
                x->fval = (float)atom_getfloat(av);
                
            } else if(av->a_type == A_LONG){
                x->fval = (float)atom_getlong(av);
            }else{
                return MAX_ERR_GENERIC;
            }
            
        }else if(xValType==PARAM_TYPE_INT||xValType==PARAM_TYPE_ENUM){
            
            if (av->a_type == A_FLOAT){
                x->fval = (long)atom_getfloat(av);
                
            } else if(av->a_type == A_LONG){
                x->fval = (long)atom_getlong(av);
            }else{
                return MAX_ERR_GENERIC;
            }
            
        }else{
            return MAX_ERR_GENERIC;
        }
        
        t_atom modi[1];
        atom_setfloat(&modi[0], x->fval);
        outlet_anything(x->j_box.b_ob.o_outlet, gensym("_val"), ac, av);
        
        
        
    }
    return MAX_ERR_NONE;
}

t_max_err matjsuiplus_getvalueof(t_matjsuiplus *x, long *ac, t_atom **av)
{
    t_atom a;
    
    if (ac && av) {
        if (*ac && *av) {
            //memory passed in use it
        } else {
            *av = (t_atom *)getbytes(sizeof(t_atom));
        }
        int xValType =-1;
        t_max_err err = matjsuiplus_getValType(x,&xValType);
        if(err!=MAX_ERR_NONE || xValType<0){
            return MAX_ERR_GENERIC;
        }
        
        switch(xValType){
            case PARAM_TYPE_FLOAT:
                atom_setfloat(&a,x->fval);
                break;
            case PARAM_TYPE_INT:
            case PARAM_TYPE_ENUM:
                atom_setlong(&a,(long)x->fval);
                break;
                
        }
        *ac = 1;
        **av = a;
    }
    return MAX_ERR_NONE;
}



t_max_err matjsuiplus_getValType(t_matjsuiplus *x,int *type){
    
    
    /*
     *type = object_attr_getlong(x, gensym("_parameter_type"));
     
     return MAX_ERR_NONE;
     */
    
    long ac = 0;
    t_atom *av = NULL;
    
    object_parameter_getinfo(
                             (t_object *)x,
                             PARAM_DATA_TYPE_TYPE,
                             &ac,
                             &av);
    
    
    if(ac&&av){
        if(av->a_type==A_LONG){
            *type = (int)atom_getlong(av);
        }else{
            return MAX_ERR_GENERIC;
        }
    }else{
        return MAX_ERR_GENERIC;
    }
    return MAX_ERR_NONE;
}
