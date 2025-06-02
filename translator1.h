#include <stdio.h>
#include <string.h>
#include <stdlib.h>

enum insttype { UNKNOWN, LDR, STR, MOV, ADD, SADD, SMUL, SSUB, MUL, SUB, SLSL, SLSR, LSL, LSR, B, BL, CBracket, CB, RET, END, START, SBR, LBL, EQ, NE, GE, LT, GT, LE };

typedef struct inst inst;
typedef enum insttype insttype;
struct inst
{
    insttype type;
    int op1;
    int op2;
    int op3;
    char lblname[100];
    char sbrname[100];
};

typedef struct transtore ts;

struct transtore
{
    int start;
    int icount;
    inst instarr[100];
};

void rfc2(char *a)
{
	int i = 0;
	while (a[i] != '\0')
	{
		a[i] = a[i + 1];
		i++;
	}
}




char arr[1000];
int pushStr(char *arr, char *str);

char* getword(char *str, int i)
{
    char *word = (char*)malloc(100 * sizeof(char));
    int j = 0;
    
    while (str[i] != ' ' && str[i] != '\n' && str[i] != '\0')
    {
        word[j] = str[i];
        j++;
        i++;
    }
    word[j] = '\0';
    if(word[0] == ':' || word[0] == '$')
        rfc2(word);
    
    return word;
}


char typeOfCO(char *p)
{
    if (p[0] == '=' && p[1] == '=' && p[2] == '\0')
        return EQ;
    else if (p[0] == '!' && p[1] == '=' && p[2] == '\0')
        return NE;
    else if (p[0] == '>' && p[1] == '=' && p[2] == '\0')
        return GE;
    else if (p[0] == '<' && p[1] == '\0')
        return LT;
    else if (p[0] == '>' && p[1] == '\0')
        return GT;
    else if (p[0] == '<' && p[1] == '=' && p[2] == '\0')
        return LE;
    else
        return -1;
}

int typeOf(char *c)
{
    if (!strcmp(c, "+"))
        return ADD;
    else if(!strcmp(c, "+<-"))
        return SADD;
    else if (!strcmp(c, "*"))
        return MUL;
    else if (!strcmp(c, "*<-"))
        return SMUL;
    else if(!strcmp(c, "-"))
        return SUB;
    else if(!strcmp(c, "-<-"))
        return SMUL;
    else if (!strcmp(c, "<<"))
        return LSL;
    else if (!strcmp(c, ">>"))
        return LSR;
    else if (!strcmp(c, "<<<-"))
        return SLSL;
    else if (!strcmp(c, ">>>-"))
        return SLSR;

    else
        return -1;
}

int pushStr(char *arr, char *str)
{
    int i = 0;
    while (arr[i] != '\0')
    {
        i++;
    }
    int j = 0;
    while (str[j] != '\0')
    {
        arr[i] = str[j];
        i++;
        j++;
    }
    arr[i] = '\0';
}

char* translate(ts ts , char *tarray)
{
    // printf("in tarray\n%s \n", getword(tarray,ts.instarr[3].op1));
    int cb = 0;
    for (int i = 0; i < ts.icount; i++)
    {
        switch (ts.instarr[i].type)
        {
        case LDR:
            // printf("\tLDR %s,#[%s,#-%s]!\n", getword(tarray,ts.instarr[i].op1), getword(tarray,ts.instarr[i].op2), getword(tarray,ts.instarr[i].op3));
            pushStr(arr, "\tldr ");
            pushStr(arr, getword(tarray,ts.instarr[i].op1));
            pushStr(arr, "[");
            pushStr(arr, getword(tarray,ts.instarr[i].op2));
            pushStr(arr, "],#");
            pushStr(arr, getword(tarray,ts.instarr[i].op3));
            pushStr(arr, "\n");
            break;
        case STR:
            // printf("\tSTR %s,#[%s,#-%s]!\n", getword(tarray,ts.instarr[i].op3), getword(tarray,ts.instarr[i].op1), getword(tarray,ts.instarr[i].op2));
            pushStr(arr, "\tldr ");
            pushStr(arr, getword(tarray,ts.instarr[i].op3));
            pushStr(arr, ",[");
            pushStr(arr, getword(tarray,ts.instarr[i].op1));
            pushStr(arr, ",#-");
            pushStr(arr, getword(tarray,ts.instarr[i].op2));
            pushStr(arr, "]!");
            pushStr(arr, "\n");
            break;
        case MOV:
            // printf("\tMOV %s,%s\n", getword(tarray,ts.instarr[i].op1), getword(tarray,ts.instarr[i].op2));
            pushStr(arr, "\tmov ");
            pushStr(arr, getword(tarray,ts.instarr[i].op1));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op2));
            pushStr(arr, "\n");
            break;
        case ADD:
            // printf("\tADD %s,%s,%s\n", getword(tarray,ts.instarr[i].op1), getword(tarray,ts.instarr[i].op2), getword(tarray,ts.instarr[i].op3));
            pushStr(arr, "\tadd ");
            pushStr(arr, getword(tarray,ts.instarr[i].op1));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op2));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op3));
            pushStr(arr, "\n");
            break;
        case SADD:
            pushStr(arr, "\tadd ");
            pushStr(arr, getword(tarray,ts.instarr[i].op1));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op1));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op2));
            pushStr(arr, "\n");
            break;
        case SMUL:
            pushStr(arr, "\tmul ");
            pushStr(arr, getword(tarray,ts.instarr[i].op1));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op1));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op2));
            pushStr(arr, "\n");
            break;
        case SSUB:
            pushStr(arr, "\tsub ");
            pushStr(arr, getword(tarray,ts.instarr[i].op1));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op1));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op2));
            pushStr(arr, "\n");
            break;
        case SUB:
            pushStr(arr, "\tsub ");
            pushStr(arr, getword(tarray,ts.instarr[i].op1));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op2));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op3));
            pushStr(arr, "\n");
            break;
        case MUL:
            // printf("\tMUL %s,%s,%s\n", getword(tarray,ts.instarr[i].op1), getword(tarray,ts.instarr[i].op2), getword(tarray,ts.instarr[i].op3));
            pushStr(arr, "\tmul ");
            pushStr(arr, getword(tarray,ts.instarr[i].op1));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op2));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op3));
            pushStr(arr, "\n");
            break;
        case LSL:
            // printf("\tLSL %s,%s,%s\n", getword(tarray,ts.instarr[i].op1), getword(tarray,ts.instarr[i].op2), getword(tarray,ts.instarr[i].op3));
            pushStr(arr, "\tlsl ");
            pushStr(arr, getword(tarray,ts.instarr[i].op1));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op2));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op3));
            pushStr(arr, "\n");
            break;
        case LSR:
            // printf("\tLSR %s,%s,%s\n", getword(tarray,ts.instarr[i].op1), getword(tarray,ts.instarr[i].op2), getword(tarray,ts.instarr[i].op3));
            pushStr(arr, "\tlsr ");
            pushStr(arr, getword(tarray,ts.instarr[i].op1));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op2));
            pushStr(arr, ",");
            pushStr(arr, getword(tarray,ts.instarr[i].op3));
            pushStr(arr, "\n");
            break;
        case B:
            if (cb == 0)
            {
                // printf("\tB __%s_%s\n", ts.instarr[i].sbrname, getword(tarray,ts.instarr[i].op1));
                pushStr(arr, "\tb ");
                pushStr(arr, "__");
                pushStr(arr, ts.instarr[i].sbrname);
                pushStr(arr, "_");
                pushStr(arr, getword(tarray,ts.instarr[i].op1));
                pushStr(arr, "\n");

            }
            else if (cb == 1)
            {
                // printf(" __%s_%s\n", ts.instarr[i - 1].sbrname, getword(tarray,ts.instarr[i].op1));
                cb = 0;
                pushStr(arr, " __");
                pushStr(arr, ts.instarr[i - 1].sbrname);
                pushStr(arr, "_");
                pushStr(arr, getword(tarray,ts.instarr[i].op1));
                pushStr(arr, "\n");
            }
            break;
        case BL:
            // printf("\tBL __SBR_%s\n", getword(tarray,ts.instarr[i].op1));
            pushStr(arr, "\tbl ");
            pushStr(arr, "__sbr_");
            pushStr(arr, getword(tarray,ts.instarr[i].op1));
            pushStr(arr, "\n");
            break;
        case CBracket:
            // printf("\tBL __RET_RET\n");
            pushStr(arr, "\tbl ");
            pushStr(arr, "__ret_ret");
            pushStr(arr, "\n\n");
            break;
        case LE:
            if (cb == 0)
            {
                // printf("\tCMP %s,%s\n\tBLE", getword(tarray,ts.instarr[i].op1), getword(tarray,ts.instarr[i].op2));
                cb = 1;
                pushStr(arr, "\tcmp ");
                pushStr(arr, getword(tarray,ts.instarr[i].op1));
                pushStr(arr, ",");
                pushStr(arr, getword(tarray,ts.instarr[i].op2));
                pushStr(arr, "\n\tble ");
            }
            break;
        case GE:
            if (cb == 0)
            {
                // printf("\tCMP %s,%s\n\tBGE", getword(tarray,ts.instarr[i].op1), getword(tarray,ts.instarr[i].op2));
                cb = 1;
                pushStr(arr, "\tcmp ");
                pushStr(arr, getword(tarray,ts.instarr[i].op1));
                pushStr(arr, ",");
                pushStr(arr, getword(tarray,ts.instarr[i].op2));
                pushStr(arr, "\n\tbge ");
            }
            break;
        case LT:
            if (cb == 0)
            {
                // printf("\tCMP %s,%s\n\tBLT", getword(tarray,ts.instarr[i].op1), getword(tarray,ts.instarr[i].op2));
                cb = 1;
                pushStr(arr, "\tcmp ");
                pushStr(arr, getword(tarray,ts.instarr[i].op1));
                pushStr(arr, ",");
                pushStr(arr, getword(tarray,ts.instarr[i].op2));
                pushStr(arr, "\n\tblt ");
            }
            break;
        case GT:
            if (cb == 0)
            {
                // printf("\tCMP %s,%s\n\tBGT", getword(tarray,ts.instarr[i].op1), getword(tarray,ts.instarr[i].op2));
                cb = 1;
                pushStr(arr, "\tcmp ");
                pushStr(arr, getword(tarray,ts.instarr[i].op1));
                pushStr(arr, ",");
                pushStr(arr, getword(tarray,ts.instarr[i].op2));
                pushStr(arr, "\n\tbgt ");
            }
            break;
        case EQ:
            if (cb == 0)
            {
                // printf("\tCMP %s,%s\n\tBEQ", getword(tarray,ts.instarr[i].op1), getword(tarray,ts.instarr[i].op2));
                cb = 1;
                pushStr(arr, "\tcmp ");
                pushStr(arr, getword(tarray,ts.instarr[i].op1));
                pushStr(arr, ",");
                pushStr(arr, getword(tarray,ts.instarr[i].op2));
                pushStr(arr, "\n\tbeq ");
            }
            break;
        case NE:
            if (cb == 0)
            {
                // printf("\tCMP %s,%s\n\tBNE", getword(tarray,ts.instarr[i].op1), getword(tarray,ts.instarr[i].op2));
                cb = 1;
                pushStr(arr, "\tcmp ");
                pushStr(arr, getword(tarray,ts.instarr[i].op1));
                pushStr(arr, ",");
                pushStr(arr, getword(tarray,ts.instarr[i].op2));
                pushStr(arr, "\n\tbne ");
            }
            break;
        case RET:
            // printf("\tRET\n");
            pushStr(arr, "\tret");
            pushStr(arr, "\n");
            break;
        case END:
            // printf("__RET_RET:\n\tMOV SP,FP\n\tLDR LR,[SP],#8\n\tLDR FP,[SP],#8\n\tRET\n");
            pushStr(arr, "_start:\n\tmov X0, #1\n\tbl __sbr_");
            pushStr(arr, getword(tarray,ts.start));
            pushStr(arr, "\n\tmov W8, #93\n\tsvc #0\n\tret\n__ret_ret:\n\tmov SP,FP\n\tldr LR,[SP],#8\n\tldr FP,[SP],#8\n\tret\n");
            break;
        case START:
            // printf("START\n");
            pushStr(arr, ".section .text\n.global _start\n");
            pushStr(arr, "\n\n");
            break;
        case SBR:
            // printf("__SBR_%s: \n\tSTR FP,#[SP,#-8]! \n\tSTR LR,#[SP,#-8]! \n\tMOV FP,SP \n", ts.instarr[i].sbrname);
            pushStr(arr, "__sbr_");
            pushStr(arr, ts.instarr[i].sbrname);
            pushStr(arr, ": \n\tstr FP,[SP,#-8]! \n\tstr LR,[SP,#-8]! \n\tmov FP,SP \n");
            break;
        case LBL:
            // printf("\t__%s_%s:\n", ts.instarr[i].sbrname, ts.instarr[i].lblname);
            pushStr(arr, "__");
            pushStr(arr, ts.instarr[i].sbrname);
            pushStr(arr, "_");
            pushStr(arr, ts.instarr[i].lblname);
            pushStr(arr, ":\n");
            break;
        default:
            break;
        }
    }
    return arr;
}
