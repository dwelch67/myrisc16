
//-------------------------------------------------------------------
//-------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int show_fetches;
int show_mem_writes;
int show_mem_reads;
int show_reg_writes;
int show_reg_reads;
int show_diss;

unsigned int fetch_count;
unsigned int write_count;
unsigned int read_count;
unsigned int fetch_limit;

//-------------------------------------------------------------------
unsigned short mem[0x10000];
//-------------------------------------------------------------------
unsigned short pc;
unsigned short reg[8];


//-------------------------------------------------------------------
// stuff for reading csv (binary) file.
FILE *fpin;
char section[512];
unsigned int line;
char newline[1024];
unsigned int maxaddr;
//-------------------------------------------------------------------
//-------------------------------------------------------------------
unsigned short fetch_mem ( unsigned short addr )
{
    unsigned short data;

    fetch_count++;
    data=mem[addr];
    if(show_fetches)
    {
        printf("fetch(0x%04X)=0x%04X\n",addr,data);
    }
    return(data);
}
//-------------------------------------------------------------------
void write_mem ( unsigned short addr, unsigned short data )
{
    write_count++;
    if(show_mem_writes)
    {
        printf("write_mem(0x%04X,0x%04X)\n",addr,data);
    }
    mem[addr]=data;
}
//-------------------------------------------------------------------
void write_reg ( unsigned char r, unsigned short data )
{
    r&=7;
    if(show_reg_writes)
    {
        printf("write_reg(r%u,0x%04X)\n",r,data);
    }
    if(r==0)
    {
        printf("cant write to r0\n");
    }
    else
    {
        reg[r]=data;
    }
}
//-------------------------------------------------------------------
unsigned short read_mem ( unsigned char addr )
{
    unsigned short data;

    data=mem[addr];
    if(show_mem_reads)
    {
        printf("read_mem(r%u)=0x%04X\n",addr,data);
    }
    return(data);
}
//-------------------------------------------------------------------
unsigned short read_reg ( unsigned char r )
{
    unsigned short data;
    r&=7;

    data=reg[r];
    if(show_reg_reads)
    {
        printf("read_reg(r%u)=0x%04X\n",r,data);
    }
    return(data);
}
//-------------------------------------------------------------------
void reset_sim ( void )
{
    unsigned int ra;
    pc=0;
    for(ra=0;ra<8;ra++) reg[ra]=0;
    fetch_count=0;
    write_count=0;
    read_count=0;
}
//-------------------------------------------------------------------
int sim_one ( void )
{
    unsigned short inst;
    unsigned short op;
    unsigned short imm,simm;
    unsigned short addr;
    unsigned short data;
    unsigned short adata,bdata,cdata;
    unsigned short brdest;
    unsigned char ra,rb,rc;

    inst=fetch_mem(pc);
    pc=pc+1;
    op=(inst>>13)&7;
    ra=(inst>>10)&7;
    rb=(inst>> 7)&7;
    rc=(inst>> 0)&7;
    imm=inst&0x3FF;
    simm=inst&0x7F;
    if(simm&0x40) simm|=(~0)<<7;
    switch(op)
    {
        case 0: //ADD
        {
            if(inst&0x0078)
            {
                printf("undefined instruction [0x%04X] 0x%04X\n",pc-1,inst);
                return(1);
            }
            if(show_diss) printf("[0x%04X] 0x%04X add r%u,r%u,r%u\n",pc-1,inst,ra,rb,rc);
            bdata=read_reg(rb);
            cdata=read_reg(rc);
            write_reg(ra,bdata+cdata);
            break;
        }
        case 1: //ADDI
        {
            if(show_diss) printf("[0x%04X] 0x%04X addi r%u,r%u,0x%04X (%d)\n",pc-1,inst,ra,rb,simm,(short)simm);
            bdata=read_reg(rb);
            write_reg(ra,bdata+simm);
            break;
        }
        case 2: //NAND
        {
            if(inst&0x0078)
            {
                printf("undefined instruction [0x%04X] 0x%04X\n",pc-1,inst);
                return(1);
            }
            if(show_diss) printf("[0x%04X] 0x%04X nand r%u,r%u,r%u\n",pc-1,inst,ra,rb,rc);
            bdata=read_reg(rb);
            cdata=read_reg(rc);
            write_reg(ra,~(bdata&cdata));
            break;
        }
        case 3: //LUI
        {
            if(show_diss) printf("[0x%04X] 0x%04X lui r%u,0x%03X (0x%04X)\n",pc-1,inst,ra,imm,imm<<6);
            write_reg(ra,imm<<6);
            break;
        }
        case 4: //SW
        {
            data=reg[ra];
            addr=reg[rb]+simm;
            if(show_diss) printf("[0x%04X] 0x%04X sw r%u,[r%u%-d] ([0x%04X]<=0x%04X) \n",pc-1,inst,ra,rb,simm,addr,data);
            data=read_reg(ra);
            addr=read_reg(rb)+simm;
            write_mem(addr,data);
            break;
        }
        case 5: //LW
        {
            addr=reg[rb]+simm;
            data=mem[addr];
            if(show_diss) printf("[0x%04X] 0x%04X lw r%u,[r%u%-d] ([0x%04X]=>0x%04X) \n",pc-1,inst,ra,rb,simm,addr,data);
            addr=read_reg(rb)+simm;
            data=read_mem(addr);
            write_reg(ra,data);
            break;
        }
        case 6: //BEQ
        {
            adata=reg[ra];
            bdata=reg[rb];
            brdest=pc+simm;
            if(show_diss) printf("[0x%04X] 0x%04X beq r%u,r%u,0x%04X (0x%04X 0x%04X)\n",pc-1,inst,ra,rb,brdest,adata,bdata);
            adata=read_reg(ra);
            bdata=read_reg(rb);
            if(adata==bdata) pc=brdest;
            break;
        }
        case 7: //JALR
        {
            if(inst==0xFFFF)
            {
                if(show_diss) printf("[0x%04X] 0x%04X halt\n",pc-1,inst);
                return(1);
            }
            if(inst&0x007F)
            {
                printf("undefined instruction [0x%04X] 0x%04X\n",pc-1,inst);
                return(1);
            }
            brdest=reg[rb];
            if(show_diss) printf("[0x%04X] 0x%04X jalr r%u,r%u (0x%04X)\n",pc-1,inst,ra,rb,brdest);
            write_reg(ra,pc);
            brdest=read_reg(rb);
            pc=brdest;
            break;
        }
    }
    return(0);
}
//-------------------------------------------------------------------
void sim ( void )
{
    while(1)
    {
        if(sim_one()) break;
        if(fetch_limit)
        {
            if(fetch_count>=fetch_limit) break;
        }
    }
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------
unsigned int get_section ( char d[], unsigned int len, char s[], unsigned int off )
{
    unsigned int dx;
    //files generated with mr16as do not have spaces or other characters
    //requiring fields to have quotes
    dx=0;
    while(1)
    {
        if(s[off]==0)
        {
            d[dx]=0;
            break;
        }
        if(s[off]==',')
        {
            d[dx]=0;
            off++;
            break;
        }
        d[dx++]=s[off++];
    }
    return(off);
}
//-------------------------------------------------------------------
int read_csv ( void )
{
    unsigned int ra;
    unsigned int off;

    unsigned int addr;
    unsigned int data;

    memset(mem,0xFF,sizeof(mem));

    maxaddr=0;
    line=0;
    while(fgets(newline,sizeof(newline)-1,fpin))
    {
        line++;
        off=0;
        off=get_section(section,sizeof(section)-1,newline,off);
        if(section[0]==0) continue; //warning/error?
        if(strcmp(section,"REM")==0) continue;
        if(strcmp(section,"COMMENT")==0) continue;
        if(strcmp(section,"END")==0) break;
        if(strcmp(section,"MEM")==0)
        {
            //address
            off=get_section(section,sizeof(section)-1,newline,off);
            ra=0;
            if(section[0]==0x30) ra++;
            if(section[1]=='x') ra++;
            if(section[1]=='X') ra++;
            if(ra!=2)
            {
                printf("<%u> Error: invalid format for address, expecting 0x1234\n",line);
                return(1);
            }
            addr=strtoul(section,NULL,16);
            if(addr&(~0xFFFF))
            {
                printf("<%u> Error: invalid address\n",line);
                return(1);
            }
            //data
            off=get_section(section,sizeof(section)-1,newline,off);
            ra=0;
            if(section[0]==0x30) ra++;
            if(section[1]=='x') ra++;
            if(section[1]=='X') ra++;
            if(ra!=2)
            {
                printf("<%u> Error: invalid format for data, expecting 0x1234\n",line);
                return(1);
            }
            data=strtoul(section,NULL,16);
            if(data&(~0xFFFF))
            {
                printf("<%u> Error: invalid data\n",line);
                return(1);
            }
            //END
            off=get_section(section,sizeof(section)-1,newline,off);
            if(strcmp(section,"END")!=0)
            {
                printf("<%u> Error: no END\n",line);
                return(1);
            }
            //printf("MEM,0x%04X,0x%04X,END,,\n",addr,data);
            if(addr>maxaddr) maxaddr=addr;
            mem[addr]=data;
        }
    }
    //for(ra=0;ra<=maxaddr;ra++) printf("[0x%04X] 0x%04X\n",ra,mem[ra]);
    return(0);
}
//-------------------------------------------------------------------
int main ( int argc, char *argv[] )
{
    int ret;

    show_fetches=0;
    show_mem_writes=0;
    show_mem_reads=0;
    show_reg_writes=1;
    show_reg_reads=0;
    show_diss=1;
    fetch_limit=100;

    if(argc<2)
    {
        printf("mr16sim filename.csv\n");
        return(1);
    }

    fpin=fopen(argv[1],"rt");
    if(fpin==NULL)
    {
        printf("Error opening file [%s]\n",argv[1]);
        return(1);
    }
    ret=read_csv();
    fclose(fpin);
    if(ret) return(1);

    reset_sim();
    sim();
    printf("fetch_count %u\n",fetch_count);
    printf("write_count %u\n",write_count);
    printf("read_count %u\n",read_count);
    return(0);
}
//-------------------------------------------------------------------
//-------------------------------------------------------------------


