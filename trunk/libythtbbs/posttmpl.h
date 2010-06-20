#ifndef __POSTTMPL_H
#define __POSTTMPL_H

#define ANSWER_MAXLENGTH 880
#define QUESTION_MAX_NUM 20

struct posttmplheader {
	char name[60];		//ģ������
	char title[STRLEN];	//ģ�����±���
	int filetime;
	int question_num;
	char userid[IDLEN + 2];	//������
	char unused1[2];
	int num_visitors;
	char unused2[180 - IDLEN - STRLEN];
};

struct posttmplquestion {
	char str[60];
	int type;		//1: �����ı�Ƭ�ϻ����ı�   2: �����ı�
	int answer_maxlen;
	char unused[60];
};

int tmpl_replacetxt(char *buf, char **alist, int answer_num, int (*outputfun) (char *, void *), void *out);
int tmpl_set_filename(char *o_buf, int i_size, char *i_board, int i_num, char i_suffix);
int get_num_psttmpls(char *board);
int tmpl_cmp_psttmpls(struct posttmplheader *phdr, int *pfiletime);
#endif
