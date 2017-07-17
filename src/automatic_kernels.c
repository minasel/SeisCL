/*------------------------------------------------------------------------
 * Copyright (C) 2016 For the list of authors, see file AUTHORS.
 *
 * This file is part of SeisCL.
 *
 * SeisCL is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.0 of the License only.
 *
 * SeisCL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SeisCL. See file COPYING and/or
 * <http://www.gnu.org/licenses/gpl-3.0.html>.
 --------------------------------------------------------------------------*/




#include "F.h"


int kernel_varout(struct varcl * vcl,
                  struct variable * vars,
                  struct clprogram * prog){
 
    int state=0;
    int i;
    
    char * temp=NULL;
    GMALLOC(temp, sizeof(char)*MAX_KERN_STR);
    char * p=(char*)temp;
    
    strcat(temp, "__kernel void varsout(int nt, __global float * rec_pos, ");
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].to_output){
            strcat(temp, "__global float * ");
            strcat(temp, vars[i].name);
            strcat(temp, ", ");
            strcat(temp, "__global float * ");
            strcat(temp, vars[i].name);
            strcat(temp, "out, ");
        }
    }
    while (*p)
        p++;
    p[-2]='\0';
    strcat(temp, "){\n\n");

    
    strcat(temp,"    int gid = get_global_id(0);\n"
           "    int i=(int)(rec_pos[0*8+gid]/DH)+FDOH;\n"
           "    int j=(int)(rec_pos[0*8+gid]/DH)+FDOH;\n"
           "    int k=(int)(rec_pos[0*8+gid]/DH)+FDOH;\n"
           "\n"
           "    if ( (i-OFFSET)>=FDOH && (i-OFFSET)<(NX-FDOH) ){\n\n");
    
    char * posstr=NULL;
    if (vcl->NDIM==2){
        posstr="[(i-OFFSET)*(NZ)+k];\n";
    }
    else{
        posstr="[(i-OFFSET)*NY*(NZ)+j*(NZ)+k];\n";
    }
    
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].to_output){
            strcat(temp, "        ");
            strcat(temp, vars[i].name);
            strcat(temp, "out[nt*8+gid]=");
            strcat(temp, vars[i].name);
            strcat(temp, posstr);
        }
    }
    
    strcat(temp, "\n    }");
    strcat(temp, "\n}");
    
    
//    printf("%s\n\n%lu\n",temp, strlen(temp));
    
    
    (*prog).src=temp;
    
    __GUARD assign_prog_source(prog, "varsout", (*prog).src);
    
    return state;

}


int kernel_varoutinit(struct varcl * vcl,
                      struct variable * vars,
                      struct clprogram * prog){
    
    int state=0;
    int i;
    
    char * temp=NULL;
    GMALLOC(temp, sizeof(char)*MAX_KERN_STR);
    char * p=(char*)temp;
    
    strcat(temp, "__kernel void varsoutinit(");
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].to_output){
            strcat(temp, "__global float * ");
            strcat(temp, vars[i].name);
            strcat(temp, "out, ");
        }
    }
    while (*p)
        p++;
    p[-2]='\0';
    strcat(temp, "){\n\n");
    
    
    strcat(temp,"    int gid = get_global_id(0);\n\n");
    
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].to_output){
            strcat(temp, "    ");
            strcat(temp, vars[i].name);
            strcat(temp, "out[gid]=0;\n");
        }
    }

    strcat(temp, "\n}");
    
    
//    printf("%s\n\n%lu\n",temp, strlen(temp));
    
    
    (*prog).src=temp;
    
    __GUARD assign_prog_source(prog, "varsoutinit", (*prog).src);
    
    return state;
    
}

int kernel_varinit(struct varcl * vcl,
                   struct variable * vars,
                   struct clprogram * prog){
    
    int state=0;
    int i;
    
    char * temp=NULL;
    GMALLOC(temp, sizeof(char)*MAX_KERN_STR);
    char * p=(char*)temp;
    char ptemp[50];
    
    int maxsize=0;
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].num_ele>maxsize){
            maxsize=vars[i].num_ele;
        }
    }
    
    
    strcat(temp, "__kernel void vars_init(");
    for (i=0;i<vcl->nvars;i++){
            strcat(temp, "__global float * ");
            strcat(temp, vars[i].name);
            strcat(temp, ", ");
    }
    while (*p)
        p++;
    p[-2]='\0';
    strcat(temp, "){\n\n");
    
    
    strcat(temp,"    int gid = get_global_id(0);\n\n");
    
    
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].num_ele<maxsize){
            sprintf(ptemp,"    if (gid<%d)\n", vars[i].num_ele);
            strcat(temp,ptemp);
            strcat(temp, "    ");
        }
        strcat(temp, "    ");
        strcat(temp, vars[i].name);
        strcat(temp, "[gid]=0;\n");
    }
    
    strcat(temp, "\n}");
    
    
//    printf("%s\n\n%lu\n",temp, strlen(temp));
    
    
    (*prog).src=temp;
    
    __GUARD assign_prog_source(prog, "vars_init", (*prog).src);
    
    return state;
    
}

int kernel_residuals(struct varcl * vcl,
                     struct variable * vars,
                     struct clprogram * prog){
    
    int state=0;
    int i;
    
    char * temp=NULL;
    GMALLOC(temp, sizeof(char)*MAX_KERN_STR);
    char * p=(char*)temp;

    
   
    
    strcat(temp, "__kernel void residuals(int nt, __global float * rec_pos,");
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].to_output){
            strcat(temp, "__global float * ");
            strcat(temp, vars[i].name);
            strcat(temp, ", ");
            strcat(temp, "__global float * ");
            strcat(temp, vars[i].name);
            strcat(temp, "out, ");
        }
    }
    while (*p)
        p++;
    p[-2]='\0';
    strcat(temp, "){\n\n");
    
    
    strcat(temp,"    int gid = get_global_id(0);\n"
           "    int i=(int)(rec_pos[0*8+gid]/DH)+FDOH;\n"
           "    int j=(int)(rec_pos[0*8+gid]/DH)+FDOH;\n"
           "    int k=(int)(rec_pos[0*8+gid]/DH)+FDOH;\n"
           "\n"
           "    if ( (i-OFFSET)>=FDOH && (i-OFFSET)<(NX-FDOH) ){\n\n");
    
    char * posstr=NULL;
    if (vcl->NDIM==2){
        posstr="[(i-OFFSET)*NZ+k]";
    }
    else{
        posstr="[(i-OFFSET)*NY*NZ+j*(NZ)+k]";
    }
    
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].to_output){
            strcat(temp, "        ");
            strcat(temp, vars[i].name);
            strcat(temp, posstr);
            strcat(temp, "+=");
            strcat(temp, vars[i].name);
            strcat(temp, "out[nt*8+gid];\n");

        }
    }
    
    strcat(temp, "\n    }");
    strcat(temp, "\n}");
    
    (*prog).src=temp;
    
    __GUARD assign_prog_source(prog, "residuals", (*prog).src);

//    printf("%s\n\n%lu\n",temp, strlen(temp));
    
    return state;
    
}

int kernel_gradinit(struct varcl * vcl,
                    struct parameter * pars,
                    struct clprogram * prog){
    
    int state=0;
    int i;
    
    char * temp=NULL;
    GMALLOC(temp, sizeof(char)*MAX_KERN_STR);
    char * p=(char*)temp;
    

    strcat(temp, "__kernel void gradinit(");
    for (i=0;i<vcl->npars;i++){
        if (pars[i].to_grad){
            strcat(temp, "__global float * grad");
            strcat(temp, pars[i].name);
            strcat(temp, ", ");
        }
    }
    while (*p)
        p++;
    p[-2]='\0';
    strcat(temp, "){\n\n");
    
    
    strcat(temp,"    int gid = get_global_id(0);\n\n");
    
    
    for (i=0;i<vcl->npars;i++){
        if (pars[i].to_grad){
            strcat(temp, "    grad");
            strcat(temp, pars[i].name);
            strcat(temp, "[gid]=0;\n");
        }
    }

    strcat(temp, "\n}");
    
   (*prog).src=temp;
    
    __GUARD assign_prog_source(prog, "gradinit", (*prog).src);
    
//    printf("%s\n\n%lu\n",temp, strlen(temp));
    
    return state;
    
}

int kernel_initsavefreqs(struct varcl * vcl,
                         struct variable * vars,
                         struct clprogram * prog){
    
    int state=0;
    int i;
    
    char * temp=NULL;
    GMALLOC(temp, sizeof(char)*MAX_KERN_STR);
    char * p=(char*)temp;
    char ptemp[50];
    
    int maxsize=0;
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].num_ele>maxsize && vars[i].for_grad){
            maxsize=vars[i].num_ele;
        }
    }
    
    strcat(temp, "__kernel void initsavefreqs(");
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].for_grad){
            strcat(temp, "__global float * f");
            strcat(temp, vars[i].name);
            strcat(temp, ", ");
        }
    }
    while (*p)
        p++;
    p[-2]='\0';
    strcat(temp, "){\n\n");
    
    
    strcat(temp,"    int gid = get_global_id(0);\n\n");
    
    
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].for_grad){
            if (vars[i].num_ele<maxsize){
                sprintf(ptemp,"    if (gid<%d)\n", vars[i].num_ele);
                strcat(temp,ptemp);
                strcat(temp, "    ");
            }
            strcat(temp, "    f");
            strcat(temp, vars[i].name);
            strcat(temp, "[gid]=0;\n");
        }
    }
    
    strcat(temp, "\n}");
    
    (*prog).src=temp;
    
    __GUARD assign_prog_source(prog, "initsavefreqs", (*prog).src);
    
//    printf("%s\n\n%lu\n",temp, strlen(temp));
    
    return state;
    
}

int kernel_savefreqs(struct varcl * vcl,
                     struct variable * vars,
                     struct clprogram * prog){
    
    int state=0;
    int i;
    
    char * temp=NULL;
    GMALLOC(temp, sizeof(char)*MAX_KERN_STR);
    char * p=(char*)temp;
    char ptemp[50];
    
    int maxsize=0;
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].num_ele>maxsize && vars[i].for_grad){
            maxsize=vars[i].num_ele;
        }
    }
    
    strcat(temp,"__kernel void savefreqs(__constant float *gradfreqs, int nt, ");
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].for_grad){
            strcat(temp, "__global float * ");
            strcat(temp, vars[i].name);
            strcat(temp, ", ");
            strcat(temp, "__global float2 * f");
            strcat(temp, vars[i].name);
            strcat(temp, ", ");
        }
    }
    while (*p)
        p++;
    p[-2]='\0';
    strcat(temp, "){\n\n");
    
    strcat(temp,"    int freq,l;\n"
                "    float2 fact[NFREQS]={0};\n");
    
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].for_grad){
            strcat(temp, "    float  l");
            strcat(temp, vars[i].name);
            strcat(temp, ";\n");
        }
    }
    while (*p)
        p++;
    p[-2]='\0';
    strcat(temp, ";\n\n");

    strcat(temp,"    int gid = get_global_id(0);\n"
                "    int gsize=get_global_size(0);\n\n" );
    
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].for_grad){
            if (vars[i].num_ele<maxsize){
                sprintf(ptemp,"    if (gid<%d)\n", vars[i].num_ele);
                strcat(temp,ptemp);
                strcat(temp, "    ");
            }
            strcat(temp, "    l");
            strcat(temp, vars[i].name);
            strcat(temp, "=");
            strcat(temp, vars[i].name);
            strcat(temp, "[gid];\n");
        }
    }

    
    strcat(temp,"\n"
        "    for (freq=0;freq<NFREQS;freq++){\n"
        "        fact[freq].x = DTNYQ*DT*cospi(2.0*gradfreqs[freq]*nt/NTNYQ);\n"
        "        fact[freq].y =-DTNYQ*DT*sinpi(2.0*gradfreqs[freq]*nt/NTNYQ);\n"
        "    }\n\n"
           );
        
        
    
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].for_grad){
            if (vars[i].num_ele<maxsize){
                sprintf(ptemp,"    if (gid<%d){\n", vars[i].num_ele);
                strcat(temp,ptemp);
            }
            strcat(temp, "    #pragma unroll\n");
            strcat(temp, "    for (freq=0;freq<NFREQS;freq++){\n");
            strcat(temp, "        f");
            strcat(temp, vars[i].name);
            strcat(temp, "[gid+freq*gsize]+=fact[freq]*l");
            strcat(temp, vars[i].name);
            strcat(temp, ";\n");
            strcat(temp, "    }\n");
            if (vars[i].num_ele<maxsize){
                strcat(temp,"    }\n");
            }
        }
    }
    
    strcat(temp, "\n}");
    
    (*prog).src=temp;
    __GUARD assign_prog_source(prog, "savefreqs", (*prog).src);
    
//    printf("%s\n\n%lu\n",temp, strlen(temp));
    
    return state;
    
}

int kernel_init_gradsrc(struct clprogram * prog){
    
    int state=0;
    
    char * temp=NULL;
    GMALLOC(temp, sizeof(char)*MAX_KERN_STR);
    
    strcat(temp,
           "__kernel void init_gradsrc(__global float *gradsrc)\n"
           "{\n\n"
           "    int gid = get_global_id(0);\n"
           "    gradsrc[gid]=0.0;\n\n"
           "}");
    
    (*prog).src=temp;
    
//    printf("%s\n\n%lu\n",temp, strlen(temp));
    
    __GUARD assign_prog_source(prog, "init_gradsrc", (*prog).src);
    
    return state;
    
}

int kernel_fcom_out(struct varcl * vcl,
                    struct variable * vars,
                    struct clprogram * prog,
                    int upid, int buff12){
    
    int state=0;
    int i,j;
    
    char * temp=NULL;
    GMALLOC(temp, sizeof(char)*MAX_KERN_STR);
    char * p=(char*)temp;
    char ptemp[200];
    
    int maxsize=0;
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].num_ele>maxsize){
            maxsize=vars[i].num_ele;
        }
    }
    
    strcat(temp, "__kernel void fill_transfer_buff_out(");
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].to_comm==upid){
            strcat(temp, "__global float * ");
            strcat(temp, vars[i].name);
            strcat(temp, ", ");
            strcat(temp, "__global float * ");
            strcat(temp, vars[i].name);
            if (buff12==1)
                strcat(temp, "_buf1, ");
            else if (buff12==2)
                strcat(temp, "_buf2, ");
        }
    }
    while (*p)
        p++;
    p[-2]='\0';
    strcat(temp, "){\n\n");
    
    
    //Indice if local memory is used
    if (vcl->LOCAL_OFF==0){
        for (i=0;i<vcl->NDIM;i++){
            sprintf(ptemp,"    int gid%s=get_global_id(%d)+FDOH;\n",
                                                vcl->N_names[i],i );
            strcat(temp, ptemp);
        }
    }
    //if we use directly global memory, with 1 working dim
    else{
        strcat(temp,"    int gid = get_global_id(0);\n");

        sprintf(ptemp,"    int gid%s=gid%%(N%s-2*FDOH);\n",
                           vcl->N_names[0],vcl->N_names[0]);
        strcat(temp, ptemp);
        
        for (i=1;i<vcl->NDIM;i++){
            sprintf(ptemp,"    int gid%s=(gid",vcl->N_names[i]);
            strcat(temp, ptemp);
            for (j=0;j<i;j++){
                sprintf(ptemp,"/(N%s-2*FDOH)",vcl->N_names[j]);
                strcat(temp, ptemp);
            }
            if (i<vcl->NDIM-1){
                sprintf(ptemp,")%%(N%s-2*fdoh);\n",vcl->N_names[i]);
                strcat(temp, ptemp);
            }
            else
                strcat(temp,");\n");
        }
        
        
    }
    strcat(temp,"\n");
    
    strcat(temp,"    int idbuf=");
    for (i=0;i<vcl->NDIM;i++){
        sprintf(ptemp,"gid%s",vcl->N_names[i]);
        strcat(temp, ptemp);
        for (j=0;j<i;j++){
            sprintf(ptemp,"*(N%s-2*FDOH)",vcl->N_names[j]);
            strcat(temp, ptemp);
        }
        if (i!=vcl->NDIM-1){
            strcat(temp, "+");
        }
    }
    strcat(temp,";\n");
    strcat(temp,"    int idvar=");
    for (i=0;i<vcl->NDIM;i++){
        if (i!=vcl->NDIM-1)
            sprintf(ptemp,"(gid%s+FDOH)",vcl->N_names[i]);
        else{
            if (buff12==1)
                sprintf(ptemp,"(gid%s+FDOH)",vcl->N_names[i]);
            else
                sprintf(ptemp,"(gid%s+N%s-2*FDOH)",
                              vcl->N_names[i],vcl->N_names[i]);
        }
        strcat(temp, ptemp);
        for (j=0;j<i;j++){
            sprintf(ptemp,"*N%s",vcl->N_names[j]);
            strcat(temp, ptemp);
        }
        if (i!=vcl->NDIM-1){
            strcat(temp, "+");
        }
    }
    strcat(temp,";\n\n");
    
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].to_comm==upid){
            sprintf(ptemp,"    %s_buf%d[idbuf]=%s[idvar];\n",
                          vars[i].name, buff12, vars[i].name);
            strcat(temp, ptemp);
        }
    }
    
    strcat(temp, "\n}");
    
    
//    printf("%s\n\n%lu\n",temp, strlen(temp));
    
    
    (*prog).src=temp;
    
    __GUARD assign_prog_source(prog, "fill_transfer_buff_out", (*prog).src);
    
    return state;
    
}

int kernel_fcom_in(struct varcl * vcl,
                   struct variable * vars,
                   struct clprogram * prog,
                   int upid, int buff12){
    
    int state=0;
    int i,j;
    
    char * temp=NULL;
    GMALLOC(temp, sizeof(char)*MAX_KERN_STR);
    char * p=(char*)temp;
    char ptemp[200];
    
    int maxsize=0;
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].num_ele>maxsize){
            maxsize=vars[i].num_ele;
        }
    }
    
    strcat(temp, "__kernel void fill_transfer_buff_in(");
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].to_comm==upid){
            strcat(temp, "__global float * ");
            strcat(temp, vars[i].name);
            strcat(temp, ", ");
            strcat(temp, "__global float * ");
            strcat(temp, vars[i].name);
            if (buff12==1)
                strcat(temp, "_buf1, ");
            else if (buff12==2)
                strcat(temp, "_buf2, ");
        }
    }
    while (*p)
        p++;
    p[-2]='\0';
    strcat(temp, "){\n\n");
    
    
    //Indice if local memory is used
    if (vcl->LOCAL_OFF==0){
        for (i=0;i<vcl->NDIM;i++){
            sprintf(ptemp,"    int gid%s=get_global_id(%d)+FDOH;\n",
                                                vcl->N_names[i],i );
            strcat(temp, ptemp);
        }
    }
    //if we use directly global memory, with 1 working dim
    else{
        strcat(temp,"    int gid = get_global_id(0);\n");
        
        sprintf(ptemp,"    int gid%s=gid%%(N%s-2*FDOH);\n",
                          vcl->N_names[0],vcl->N_names[0]);
        strcat(temp, ptemp);
        
        for (i=1;i<vcl->NDIM;i++){
            sprintf(ptemp,"    int gid%s=(gid",vcl->N_names[i]);
            strcat(temp, ptemp);
            for (j=0;j<i;j++){
                sprintf(ptemp,"/(N%s-2*FDOH)",vcl->N_names[j]);
                strcat(temp, ptemp);
            }
            if (i<vcl->NDIM-1){
                sprintf(ptemp,")%%(N%s-2*fdoh);\n",vcl->N_names[i]);
                strcat(temp, ptemp);
            }
            else
                strcat(temp,");\n");
        }
        
        
    }
    strcat(temp,"\n");
    
    strcat(temp,"    int idbuf=");
    for (i=0;i<vcl->NDIM;i++){
        sprintf(ptemp,"gid%s",vcl->N_names[i]);
        strcat(temp, ptemp);
        for (j=0;j<i;j++){
            sprintf(ptemp,"*(N%s-2*FDOH)",vcl->N_names[j]);
            strcat(temp, ptemp);
        }
        if (i!=vcl->NDIM-1){
            strcat(temp, "+");
        }
    }
    strcat(temp,";\n");
    strcat(temp,"    int idvar=");
    for (i=0;i<vcl->NDIM;i++){
        if (i!=vcl->NDIM-1)
            sprintf(ptemp,"(gid%s+FDOH)",vcl->N_names[i]);
        else{
            if (buff12==1)
                sprintf(ptemp,"(gid%s)",vcl->N_names[i]);
            else
                sprintf(ptemp,"(gid%s+N%s-FDOH)",
                             vcl->N_names[i],vcl->N_names[i]);
        }
        strcat(temp, ptemp);
        for (j=0;j<i;j++){
            sprintf(ptemp,"*N%s",vcl->N_names[j]);
            strcat(temp, ptemp);
        }
        if (i!=vcl->NDIM-1){
            strcat(temp, "+");
        }
    }
    strcat(temp,";\n\n");
    
    for (i=0;i<vcl->nvars;i++){
        if (vars[i].to_comm==upid){
            sprintf(ptemp,"    %s[idvar]=%s_buf%d[idbuf];\n",
                               vars[i].name, vars[i].name, buff12);
            strcat(temp, ptemp);
        }
    }
    
    strcat(temp, "\n}");
    
    
        printf("%s\n\n%lu\n",temp, strlen(temp));
    
    
    (*prog).src=temp;
    
    __GUARD assign_prog_source(prog, "fill_transfer_buff_in", (*prog).src);
    
    
    return state;
    
}