#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "translator1.h"
#define is64(p) (p[0] == 'X')
#define is32(p) (p[0] == 'W')
#define isnum(p) (p >= '0' && p <= '9')
#define isalpha(p) (p >= 'a' && p <= 'z' || p >= 'A' && p <= 'Z')
#define isalphanum(p) (isalpha(p) || isnum(p))
#define isValidRangeCondition(p) ((p[1] >= '0' && p[1] <= '9') && (p[2] == '\0')) || ((p[1] == '2' && p[2] >= '0' && p[2] <= '8') && (p[3] == '\0')) || ((p[1] == '1' && p[2] >= '0' && p[2] <= '9') && (p[3] == '\0'))
#define isnull(p) (p == '\0')
#define op(p) (p == '+' || p == '-' || p == '*')
#define ismem(p) (p[0] == 'm' && p[1] == 'e' && p[2] == 'm' && isnull(p[3]))
#define isArrow(p) (p[0] == '<' && p[1] == '-' && isnull(p[2]))
#define isOpArrow(p) ((op(p[0]) && p[1] == '<' && p[2] == '-' && isnull(p[3])) || ((isrshift(p) || islshift(p)) && p[2] == '<' && p[3] == '-' && isnull(p[4])))
#define isrshift(p) (p[0] == '>' && p[1] == '>')
#define islshift(p) (p[0] == '<' && p[1] == '<')
#define issp(p) (p[0] == 'S' && p[1] == 'P' && isnull(p[2]))
#define isSpace(p) (p == ' ')
#define space " "
#define isHash(p) (p == '#')
#define isNL(p) (p[0] == '\n')
#define isTab(p) (p == '\t')
#define lblstart(p) (p[0] == ':')
#define sbrstart(p) (p[0] == '(')
#define sbrnamestart(p) (p == '$')
#define sbrend(p) (p[0] == ')')
#define islblBranch(p) (p[0] == 'b' && p[1] == '\0')
#define issbrBranch(p) (p[0] == 'b' && p[1] == 'l' && p[2] == '\0')
#define cmpop(p) ((p[0] == '<' || p[0] == '>' && p[1] == '\0') || ((p[0] == '>' || p[1] == '=') || (p[0] == '<' || p[1] == '=') || (p[0] == '!' || p[1] == '=') || (p[0] == '=' || p[1] == '=') && p[2] == '\0'))
#define isCB(p) (p[0] == 'C' && p[1] == 'B' && p[2] == '\0')
#define RETN "return"
struct sbrlbl
{
	char start[100];
	int fstart;
	char sbrname[100];
	int sbr;
	int rsbr;
	int lbl;
	int branch;
	char *strlblcall;
	char *strlbldecl;
	char *strsbrcall;
	char *strsbrdecl;
};
typedef struct trans trans;
struct trans
{
	int index;
	ts ts;
	struct sbrlbl sbrlbl;
	char *tarray;
	int cur;
	int ln;
	char nw[100];
	int wrd;
};

int nextword(trans *t);
int chk64reg(trans *t);
int chk32reg(trans *t);
int chkimdvalue(trans *t);
int chkmemimdvalue(trans *t);
int chkmem(trans *t);
int valid64Reginstrn(trans *t);
int valid32Reginstrn(trans *t);
int validinstrn(trans *t);
int chkdefsbrlbl(trans *t);
int chknw(trans *t);
int noofpara(trans *t);
int length(char t[]);
int strlblcall(trans *t);
int strlbldecl(trans *t);
int strsbrcall(trans *t);
int strsbrdecl(trans *t);
int chkempty(char *a, char *i);
int cmpstr(char *a, char *b);
int chkbranchsbrlbl(trans *t);
int cmpBranch(trans *t);
int appendStr(char *a, char *s);

typedef struct sbrlbl sbrlbl;

int cmpstr(char *str1, char *str2)
{
	while (*str1 && (*str1 == *str2))
	{
		str1++;
		str2++;
	}
	return (unsigned char)*str1 - (unsigned char)*str2;
}
void rfc(char *a)
{
	int i = 0;
	while (a[i] != '\0')
	{
		a[i] = a[i + 1];
		i++;
	}
}
int indexOf(char *a, char *s) // checks if the label is declared or not
{
	int i = 0;
	int addr = i;
	int output = 1;
	char temp1[100];
	if (a != NULL)
	{

		while (a[i] != '\0')
		{
			int j = 0;
			addr = i;
			while (!isSpace(a[i]) && !isnull(a[i]) && !isTab(a[i]) && a[i] != '\0')
			{
				temp1[j] = a[i];
				i++;
				j++;
			}
			temp1[j] = '\0';
			if (cmpstr(s, temp1) == 0) // if label is declared
			{
				return addr; // returns the index of the label
				break;
			}
			while (isSpace(a[i]) || isTab(a[i]))
			{
				i++;
			}
			addr = 0;
		}
	}
	else
	{
		return -1;
	}
	return -1; //  -1 means not declared
}
int appendStr(char *a, char *s)
{
	int i = 0;
	while (a[i] != '\0')
	{
		i++;
	}
	a[i] = ' ';
	i = i + 1;

	int j = 0;
	while (s[j] != '\0')
	{
		a[i] = s[j];
		i++;
		j++;
	}
	a[i] = '\0';
}
int removeStr(char *a, int p)
{
	int len = length(a);
	int i = p;
	while (i < len && (isSpace(a[i]) || isnull(a[i]) || isTab(a[i])))
	{
		i++;
	}
	int start = i;
	if (start >= len)
		return 0;
	while (i < len && !isSpace(a[i]) && !isnull(a[i]) && !isTab(a[i]))
	{
		i++;
	}
	i++;
	int j = start;
	while (i < len)
	{
		a[j++] = a[i++];
	}
	a[j] = '\0';
	return start;
}
int strlblcall(trans *t)
{
	if (indexOf(t->sbrlbl.strlbldecl, t->nw) == -1 && indexOf(t->sbrlbl.strlblcall, t->nw) == -1)
	{

		t->sbrlbl.strlblcall = realloc(t->sbrlbl.strlblcall, length(t->sbrlbl.strlblcall) + length(t->nw) + 1);
		appendStr(t->sbrlbl.strlblcall, t->nw);
		return 1;
	}
}
int strsbrcall(trans *t)
{
	if (indexOf(t->sbrlbl.strsbrdecl, t->nw) == -1 && indexOf(t->sbrlbl.strsbrcall, t->nw) == -1)
	{
		t->sbrlbl.strsbrcall = realloc(t->sbrlbl.strsbrcall, length(t->sbrlbl.strsbrcall) + length(t->nw) + 1);
		appendStr(t->sbrlbl.strsbrcall, t->nw);
		return 1;
	}
}
int strsbrdecl(trans *t)
{
	int p;
	if (indexOf(t->sbrlbl.strsbrdecl, t->nw) != -1 && !isnum(t->nw[0]))
	{
		printf("ERROR : redeclration");
		return 0;
	}
	else if ((p = indexOf(t->sbrlbl.strsbrcall, t->nw)) != -1 && !isnum(t->nw[0]))
	{
		removeStr(t->sbrlbl.strsbrcall, p);
		t->sbrlbl.strsbrcall = realloc(t->sbrlbl.strsbrcall, length(t->sbrlbl.strsbrcall) - length(t->nw) - 1 - 1);
	}

	t->sbrlbl.strsbrdecl = realloc(t->sbrlbl.strsbrdecl, length(t->sbrlbl.strsbrdecl) + length(t->nw) + 1);
	if (appendStr(t->sbrlbl.strsbrdecl, t->nw))
		return 1;

	return 0;
}
int strlbldecl(trans *t)
{
	// printf("in strlbldecl\n");
	int p;
	if (indexOf(t->sbrlbl.strlbldecl, t->nw) != -1)
	{
		printf("ERROR : redeclration\n");
		return 0;
	}
	else if ((p = indexOf(t->sbrlbl.strlblcall, t->nw)) != -1)
	{
		removeStr(t->sbrlbl.strlblcall, p);
		t->sbrlbl.strlblcall = realloc(t->sbrlbl.strlblcall, length(t->sbrlbl.strlblcall) - length(t->nw) - 1);
	}
	t->sbrlbl.strlbldecl = realloc(t->sbrlbl.strlbldecl, length(t->sbrlbl.strlbldecl) + length(t->nw) + 1);
	if (appendStr(t->sbrlbl.strlbldecl, t->nw))
		return 1;

	return 0;
}
int chkempty(char *p, char *i)
{
	if (length(p) > 0 && i == "l")
	{
		// printf("in chksbrlbl\n");
		printf("Following labels are not declared:\n %s\n", p);
		return 0;
	}
	else if (length(p) > 0 && i == "s")
	{

		// printf("in chksbrlbl\n");
		printf("Following sbr are not declared:\n %s\n", p);
		return 0;
	}
	else
	{
		return 1;
	}
}
int length(char a[])
{
	int i = 0;
	if (a != NULL)
	{
		while (!isnull(a[i]))
		{
			i++;
		}
	}
	else
	{
		return 0;
	}
	return i;
}
int nextword(trans *t) // gives the word separeted by delimiters
{
	int i = 0;
	int output = 0;
	t->index = 0;
	int p=0;
	// printf("getword %s at index %d\n",getword(t->tarray,t->index),t->index);
	while (!isSpace(t->tarray[t->cur]) && !isnull(t->tarray[t->cur]) && !isTab(t->tarray[t->cur]))
	{
		if(t->index == 0)
			p = t->cur;
		t->index = t->cur;
		t->nw[i] = t->tarray[t->cur];
		i++;
		t->cur++;
	}
	t->nw[i] = '\0';
	while (isSpace(t->tarray[t->cur]) || isTab(t->tarray[t->cur]))
	{
		t->cur++;
	}
	t->wrd++;
	t->index = p;
	output = 1;
	// printf("nw: %s\n", t->nw);
	return output;
}
int chk64reg(trans *t) // checks if the word is a 64 bit register	// X0...X31
{
	if (is64(t->nw))
	{
		if (isValidRangeCondition(t->nw))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}
int chk32reg(trans *t) // checks if the word is a 32 bit register	// W0...W31
{
	if (is32(t->nw))
	{
		if (isValidRangeCondition(t->nw))
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return 0;
}
int chkimdvalue(trans *t) // checks if the word is an immediate value	#[0-9]
{
	int output = 0;
	int i = 1;
	int num = 0, temp = 1;
	if (isHash(t->nw[0]))
	{
		while (!isnull(t->nw[i]))
		{
			if (isnum(t->nw[i]))
			{
				temp = t->nw[i] - '0';
				num += temp;
				i++;
				output = 1;
			}
			else
			{
				output = 0;
				break;
			}
			num = num * 10;
		}
	}
	return output;
}
int chkmemimdvalue(trans *t) // checks if the word is an immediate value	#[0-9]
{
	int output = 0;
	int i = 1;
	int num = 0, temp = 1;
	if (isHash(t->nw[0]))
	{
		while (!isnull(t->nw[i]))
		{
			if (isnum(t->nw[i]))
			{
				temp = t->nw[i] - '0';
				num += temp;
				i++;
				output = 1;
			}
			else
			{
				output = 0;
				break;
			}
			num = num * 10;
		}
		if (num % 8 == 0)
			return 1;
		else
			return 0;
	}
	return output;
}
int chkmeminstrn(trans *t) // checks if the word is a memory instruction
{
	nextword(t);
	if (chk64reg(t) || issp(t->nw))
	{
		t->ts.instarr[t->ts.icount].op1 = t->index;
		nextword(t);
		if (chkmemimdvalue(t))
		{
			rfc(t->nw);
			t->ts.instarr[t->ts.icount].op2 = t->index;

			nextword(t);
			if ((isArrow(t->nw)))
			{
				nextword(t);
				if (chk64reg(t) || issp(t->nw))
				{
					t->ts.instarr[t->ts.icount].op3 = t->index;
					t->ts.instarr[t->ts.icount].type = STR;
					t->ts.icount++;
					nextword(t);
					if (isNL(t->nw) || isnull(t->nw[0]))
					{
						nextword(t);
						t->ln++;
						t->wrd = 0;
						return 1;
					}
					else
					{
						return 0;
					}
				}
				else
					return 0;
			}
			else
				return 0;
		}
		else
			return 0;
	}
}
int chkmem(trans *t) // checks if the word is a memory instruction in the form of mem X5 #6
{
	nextword(t);
	if (chk64reg(t) || issp(t->nw))
	{
		t->ts.instarr[t->ts.icount].op2 = t->index;
		nextword(t);
		if (chkmemimdvalue(t))
		{
			rfc(t->nw);
			t->ts.instarr[t->ts.icount].op3 = t->index;
			return 1;
		}
		else
			return 0;
	}
	else
		return 0;

	return 0;
}
int valid64Reginstrn(trans *t) // checks if the instruction is valid for 64 bit registers
{
	if (chk64reg(t) || issp(t->nw))
	{
		t->ts.instarr[t->ts.icount].op1 = t->index;
		nextword(t);
		if (isArrow(t->nw))
		{
			nextword(t);
			if (chk64reg(t) || issp(t->nw))
			{
				t->ts.instarr[t->ts.icount].op2 = t->index;
				nextword(t);
				if (op(t->nw[0]) || isrshift(t->nw) || islshift(t->nw))
				{
					t->ts.instarr[t->ts.icount].type = typeOf(t->nw);
					nextword(t);
					if (chk64reg(t) || issp(t->nw))
					{
						t->ts.instarr[t->ts.icount].op3 = t->index;
						t->ts.icount++;
						nextword(t);
						if (isNL(t->nw) || isnull(t->nw[0]))
						{

							nextword(t);
							t->ln++;
							t->wrd = 0;
							return 1;
						}
						else
						{
							return 0;
						}
					}
					else if (chkimdvalue(t) || issp(t->nw))
					{
						t->ts.instarr[t->ts.icount].op3 = t->index;
						t->ts.icount++;
						nextword(t);
						if (isNL(t->nw) || isnull(t->nw[0]))
						{
							nextword(t);
							t->ln++;
							t->wrd = 0;
							return 1;
						}
						else
						{
							return 0;
						}
					}
					else
						return 0;
				}
				else
				{
					t->ts.instarr[t->ts.icount].type = MOV;
					t->ts.icount++;
					if (isNL(t->nw) || isnull(t->nw[0]))
					{
						nextword(t);
						t->ln++;
						t->wrd = 0;
						return 1;
					}
					else
					{
						return 0;
					}
				}
			}
			else if (ismem(t->nw))
			{
				if (chkmem(t))
				{
					t->ts.instarr[t->ts.icount].type = LDR;
					t->ts.icount++;
					nextword(t);
					if (isNL(t->nw) || isnull(t->nw[0]))
					{
						nextword(t);
						t->ln++;
						t->wrd = 0;
						return 1;
					}
					else
					{
						return 0;
					}
				}
				else
					return 0;
			}
			else if (chkimdvalue(t) || issp(t->nw))
			{
				t->ts.instarr[t->ts.icount].op2 = t->index;
				t->ts.instarr[t->ts.icount].type = MOV;
				t->ts.icount++;
				nextword(t);
				if (isNL(t->nw) || isnull(t->nw[0]))
				{
					nextword(t);
					t->ln++;
					t->wrd = 0;
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else if (isOpArrow(t->nw) || isrshift(t->nw) || islshift(t->nw))
		{
			t->ts.instarr[t->ts.icount].type = typeOf(t->nw);
			nextword(t);
			if (chk64reg(t) || issp(t->nw))
			{
				t->ts.instarr[t->ts.icount].op2 = t->index;
				t->ts.instarr[t->ts.icount].op3 = t->index;
				t->ts.icount++;
				nextword(t);
				if (isNL(t->nw) || isnull(t->nw[0]))
				{
					nextword(t);
					t->ln++;
					t->wrd = 0;
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else if (chkimdvalue(t))
			{
				t->ts.instarr[t->ts.icount].op2 = t->index;
				t->ts.instarr[t->ts.icount].op3 = t->index;
				t->ts.icount++;
				nextword(t);
				if (isNL(t->nw) || isnull(t->nw[0]))
				{
					nextword(t);
					t->ln++;
					t->wrd = 0;
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else
				return 0;
		}
		else
			return 0;
	}
	return 0;
}
int valid32Reginstrn(trans *t) // checks if the instruction is valid for 64 bit registers
{

	if (chk32reg(t))
	{
		t->ts.instarr[t->ts.icount].op1 = t->index;
		nextword(t);
		if (isArrow(t->nw))
		{
			nextword(t);
			if (chk32reg(t))
			{
				t->ts.instarr[t->ts.icount].op2 = t->index;
				nextword(t);
				if (op(t->nw[0]) || isrshift(t->nw) || islshift(t->nw))
				{
					t->ts.instarr[t->ts.icount].type = typeOf(t->nw);
					nextword(t);
					if (chk32reg(t))
					{
						t->ts.instarr[t->ts.icount].op3 = t->index;
						t->ts.icount++;
						nextword(t);
						if (isNL(t->nw) || isnull(t->nw[0]))
						{
							nextword(t);
							t->ln++;
							t->wrd = 0;
							return 1;
						}
						else
						{
							return 0;
						}
					}
					else if (chkimdvalue(t))
					{
						t->ts.instarr[t->ts.icount].op3 = t->index;
						t->ts.icount++;
						nextword(t);
						if (isNL(t->nw) || isnull(t->nw[0]))
						{
							nextword(t);
							t->ln++;
							t->wrd = 0;
							return 1;
						}
						else
						{
							return 0;
						}
					}
					else
						return 0;
				}
				else
				{
					t->ts.instarr[t->ts.icount].type = MOV;
					t->ts.icount++;
					if (isNL(t->nw) || isnull(t->nw[0]))
					{
						nextword(t);
						t->ln++;
						t->wrd = 0;
						return 1;
					}
					else
					{
						return 0;
					}
				}
			}
			else if (ismem(t->nw))
			{
				if (chkmem(t))
				{
					t->ts.instarr[t->ts.icount].type = LDR;
					t->ts.icount++;
					nextword(t);
					if (isNL(t->nw) || isnull(t->nw[0]))
					{
						nextword(t);
						t->ln++;
						t->wrd = 0;
						return 1;
					}
					else
					{
						return 0;
					}
				}
				else
					return 0;
			}
			else if (chkimdvalue(t) || issp(t->nw))
			{
				t->ts.instarr[t->ts.icount].op2 = t->index;
				t->ts.instarr[t->ts.icount].type = MOV;
				t->ts.icount++;
				nextword(t);
				if (isNL(t->nw) || isnull(t->nw[0]))
				{
					nextword(t);
					t->ln++;
					t->wrd = 0;
					return 1;
				}
				else
				{
					return 0;
				}
			}
		}
		else if (isOpArrow(t->nw))
		{
			t->ts.instarr[t->ts.icount].type = typeOf(t->nw);

			nextword(t);
			if (chk32reg(t))
			{
				t->ts.instarr[t->ts.icount].op2 = t->index;
				t->ts.instarr[t->ts.icount].op3 = t->index;
				t->ts.icount++;
				nextword(t);
				if (isNL(t->nw) || isnull(t->nw[0]))
				{
					nextword(t);
					t->ln++;
					t->wrd = 0;
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else if (chkimdvalue(t))
			{
				t->ts.instarr[t->ts.icount].op2 = t->index;
				t->ts.instarr[t->ts.icount].op3 = t->index;
				t->ts.icount++;
				nextword(t);
				if (isNL(t->nw) || isnull(t->nw[0]))
				{
					nextword(t);
					t->ln++;
					t->wrd = 0;
					return 1;
				}
				else
				{
					return 0;
				}
			}
			else
				return 0;
		}
		else
			return 0;
	}
	return 0;
}
int validinstrn(trans *t) // checks if the instruction is valid for both 32 and 64 bit registers
{
	if (is64(t->nw))
	{
		if (valid64Reginstrn(t))
		{
			return 1;
		}
		else
			return 0;
	}
	else if (is32(t->nw))
	{
		if (valid32Reginstrn(t))
		{
			return 1;
		}
		else
			return 0;
	}
	return 0;
}
int chkdefsbrlbl(trans *t)
{
	int output = 0;
	int i = 0;

	if (lblstart(t->nw) && isalpha(t->nw[1]) && t->sbrlbl.lbl == 0)
	{
		i = 2;

		while (t->nw[i] != '\0')
		{
			if (isalphanum(t->nw[i]))
			{
				i++;
				output = 1;
				t->sbrlbl.lbl = 1;
			}
			else
			{
				break;
			}
		}
		rfc(t->nw);
		if (strlbldecl(t))
		{
			strcpy(t->ts.instarr[t->ts.icount].lblname, t->nw);
			t->ts.instarr[t->ts.icount].type = LBL;
			t->ts.icount++;
			nextword(t);
			return 1;
		}
		else
		{
			printf("Cant store label already exists\n");
			return 0;
		}
	}
	else if (!sbrnamestart(t->nw[i]))
	{
		nextword(t);
		i = 0;
	}
	if (sbrnamestart(t->nw[i]) && isalpha(t->nw[i + 1]))
	{
		i++;
		if (!isnull(t->nw[i]))
		{
			while (t->nw[i] != '\0')
			{
				if (isalphanum(t->nw[i]))
				{
					output = 1;
				}
				i++;
			}
		}
		else
		{
			output = 1;
		}
	}
	else
	{
		return 0;
	}
	rfc(t->nw);
	if (t->sbrlbl.sbr != 0)
	{
		if (strsbrdecl(t))
		{
			strcpy(t->ts.instarr[t->ts.icount].sbrname, t->nw);
			strcpy(t->sbrlbl.sbrname, t->nw);
			t->ts.instarr[t->ts.icount].type = SBR;
			t->ts.icount++;
			nextword(t);
		}
	}
	else if (t->sbrlbl.lbl == 0)
	{
		strcpy(t->sbrlbl.start, t->nw);
		nextword(t);
		if (isNL(t->nw))
		{
			nextword(t);
			t->ln++;
			t->wrd = 0;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
	if (noofpara(t) && t->sbrlbl.sbr != 0)
	{
		if (strsbrdecl(t))
			nextword(t);
		if (isNL(t->nw))
		{
			nextword(t);
			t->ln++;
			t->wrd = 0;
			return 1;
		}
		else
		{
			return 0;
		}
	}

	else
	{
		return 0;
	}
	return output;
}
int chkbranchsbrlbl(trans *t)
{
	nextword(t);
	int output = 0;
	int i = 1;
	if (sbrnamestart(t->nw[0]) && isalpha(t->nw[i]))
	{
		i++;
		while (t->nw[i] != '\0')
		{
			if (isalphanum(t->nw[i]))
			{
				i++;
				output = 1;
			}
			else
			{
				break;
			}
		}
		rfc(t->nw);
		if (t->sbrlbl.sbr != 0)
		{
			t->ts.instarr[t->ts.icount].op1 = t->index;
			t->ts.instarr[t->ts.icount].type = BL;
			t->ts.icount++;
			strsbrcall(t);
			nextword(t);
		}
		else
		{
			strcpy(t->sbrlbl.start, t->nw);
			return 1;
		}
		if (isNL(t->nw))
		{
			nextword(t);
			t->ln++;
			t->wrd = 0;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else if (lblstart(t->nw) && isalpha(t->nw[i]))
	{
		i++;
		while (t->nw[i] != '\0')
		{
			if (isalphanum(t->nw[i]))
			{
				i++;
				output = 1;
			}
			else
			{
				break;
			}
		}
		rfc(t->nw);
		t->ts.instarr[t->ts.icount].op1 = t->index;
		t->ts.instarr[t->ts.icount].type = B;
		t->ts.icount++;
		if (strlblcall(t))
			nextword(t);
		if (isNL(t->nw))
		{
			nextword(t);
			t->ln++;
			t->wrd = 0;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	return output;
}
int noofpara(trans *t)
{
	int i = 0;
	int output = 0;
	while (!isnull(t->nw[i]))
	{
		if (isnum(t->nw[i]))
		{
			i++;
			output = 1;
		}
		else
		{
			output = 0;
			break;
		}
	}
	return output;
}
int cmpBranch(trans *t)
{
	int temp;
	nextword(t);
	if (chk32reg(t))
	{
		t->ts.instarr[t->ts.icount].op1 = t->index;
		temp = 0;
		nextword(t);
	}
	else if (chk64reg(t))
	{
		t->ts.instarr[t->ts.icount].op1 = t->index;
		temp = 1;
		nextword(t);
	}
	else
		return 0;
	if (cmpop(t->nw))
	{
		t->ts.instarr[t->ts.icount].type = typeOfCO(t->nw);
		nextword(t);
		if ((chk32reg(t) && temp == 0) || (chk64reg(t) && temp == 1) || chkimdvalue(t))
		{
			t->ts.instarr[t->ts.icount].op2 = t->index;
			t->ts.icount++;
			if (chkbranchsbrlbl(t))
				return 1;
			else
				return 0;
		}
		else
			return 0;
	}
	else
		return 0;
}
int chknw(trans *t) // checks word and call the specific function to check the instruction
{
	strcpy(t->ts.instarr[t->ts.icount].sbrname, t->sbrlbl.sbrname); // saving the sbrname
	if (sbrstart(t->nw) && t->sbrlbl.sbr == 0)						// checks if the word is a subroutine or not
	{
		if (chkbranchsbrlbl(t))
		{
			t->ts.start = t->index;
			t->ts.instarr[t->ts.icount].type = START;
			t->ts.icount++;
			t->sbrlbl.sbr = 1;
			nextword(t);
			chknw(t);
		}
		else
		{

			return 0;
		}
	}
	else if (is64(t->nw) || is32(t->nw) && t->sbrlbl.rsbr == 1) // Checks the word is any reg instruction or not
	{
		t->sbrlbl.lbl = 0;
		if (validinstrn(t))
			chknw(t);
		else
			return 0;
	}
	else if (ismem(t->nw) && t->sbrlbl.rsbr == 1) // Checks if word is any mem instruction or not
	{
		t->sbrlbl.lbl = 0;
		if (chkmeminstrn(t))
			chknw(t);
		else
			return 0;
	}
	else if (lblstart(t->nw) || sbrstart(t->nw) && t->sbrlbl.sbr != 0) // Checks if word is any label or not
	{
		if (sbrstart(t->nw) && t->sbrlbl.rsbr == 0)
		{
			if (!cmpstr(t->sbrlbl.start, t->nw))
				t->sbrlbl.fstart = 1; //
			t->sbrlbl.sbr += 1;
			t->sbrlbl.rsbr = 1;
		}
		if (chkdefsbrlbl(t))
		{
			t->sbrlbl.lbl = 1;
			chknw(t);
		}
		else
			return 0;
	}
	else if (islblBranch(t->nw) || issbrBranch(t->nw) && t->sbrlbl.rsbr == 1) // Checks if word is any subroutine or not
	{
		t->sbrlbl.lbl = 0;
		if (chkbranchsbrlbl(t))
			chknw(t);
		else
			return 0;
	}
	else if (isCB(t->nw) && t->sbrlbl.rsbr == 1)
	{
		t->sbrlbl.lbl = 0;
		if (cmpBranch(t))
			chknw(t);
		else
			return 0;
	}
	else if (sbrend(t->nw) && t->sbrlbl.rsbr == 1) // checks if the word is end of the subroutine
	{
		t->sbrlbl.sbr -= 1;
		t->sbrlbl.rsbr = 0;
		if (chkempty(t->sbrlbl.strlblcall, "l"))
		{
			t->ts.instarr[t->ts.icount].type = CBracket;
			t->ts.icount++;
			t->sbrlbl.strlblcall = realloc(t->sbrlbl.strlblcall, 1);
			t->sbrlbl.strlbldecl = realloc(t->sbrlbl.strlbldecl, 1);
			t->sbrlbl.strlblcall[0] = '\0';
			t->sbrlbl.strlbldecl[0] = '\0';
			nextword(t);
			if (isNL(t->nw))
			{
				nextword(t);
				t->ln++;
				t->wrd = 0;
				chknw(t);
			}
		}
		else
		{
			return 0;
		}
	}
	else if (!cmpstr(t->nw, RETN))
	{
		t->ts.instarr[t->ts.icount].type = RET;
		t->ts.icount++;
		nextword(t);
		chknw(t);
	}
	else if (sbrend(t->nw) && t->sbrlbl.rsbr == 0) // checks if the word is end of the subroutine
	{
		t->sbrlbl.sbr -= 1;
		if (t->sbrlbl.sbr == 0)
		{
			t->ts.instarr[t->ts.icount].type = END;
			t->ts.icount++;
			nextword(t);
			chknw(t);
		}
		else
		{
			printf("ERROR : no subroutine to end\n");
			return 0;
		}
	}
	else if (isnull(t->nw[0])) // If no more word is remained to check then its end of the file an no more code to check
	{
		if (chkempty(t->sbrlbl.strsbrcall, "s") && t->sbrlbl.sbr == 0)
		{

			t->sbrlbl.sbr = 0;
			t->sbrlbl.lbl = 0;
			return 1;
		}
		else if (t->sbrlbl.fstart != 1)
		{
			printf("Main subroutine '%s' not found\n", t->sbrlbl.start);
			return 0;
		}
		else
		{
			return 0;
		}
	}
	else if (isNL(t->nw)) // checks if the word is new line
	{
		nextword(t);
		t->ln++;
		t->wrd = 0;
		chknw(t);
	}
	else
	{
		printf("ERROR : invalid Program!!!\n");
		return 0;
	}
}
int main()
{
	trans *t = (trans *)malloc(sizeof(trans)); // Allocate memory for the structure
	ts ts;
	FILE *file;
	int i = 0, size = 10;
	char ch;
	t->tarray = malloc(size); // Allocate memory for the array
	t->sbrlbl.strlblcall = malloc(size);
	t->sbrlbl.strlbldecl = malloc(size);
	t->sbrlbl.strsbrcall = malloc(size);
	t->sbrlbl.strsbrdecl = malloc(size);
	if (!t->tarray || !(file = fopen("input.txt", "r"))) // If memory allocation fails or file not found
	{
		printf("Error");
		return 1;
	}
	while ((ch = fgetc(file)) != EOF)
	{
		if (i >= size - 1)
		{
			size *= 2;
			t->tarray = realloc(t->tarray, size); // Reallocate memory if the size of the array exceeds
			if (!t->tarray)						  // If memory reallocation fails
			{
				printf("Error reallocating memory");
				fclose(file);
				return 1;
			}
		}
		t->tarray[i++] = ch;
	}

	t->tarray[i] = '\0'; // Null-terminate the string
	fclose(file);
	nextword(t); // Initially called the function to get the first word
	if (chknw(t))
	{
		printf("SUCCESS!\n");
		char *arr = translate(t->ts,t->tarray);
		FILE *fp;
		fp = fopen("test.s", "w");
		// printf("getword %s\n",getword(t->tarray,2) );
		if (fp != NULL)
		{
			fprintf(fp,"%s", arr);
			fclose(fp);
			printf("Translation in file test.s\n");
		}
		// printf("%s", arr);
	}
	else
	{
		printf("FAILED!\n");
		printf("error near: %s\n", t->nw);
		printf("line no.%d\nword no.:%d\n", t->ln+1, t->wrd+1);
		printf("cur word is: %s\n", t->nw);
	}
	free(t->tarray);
	return 0;
}
