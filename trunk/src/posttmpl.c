#include "bbs.h"
#include "bbstelnet.h"
#include "edit.h"

static int tmpl_setnote(char *notefn);
static int tmpl_setcontent(char *contentfn);
static int tmpl_setquestion(char *questionfn);
static char *tmpl_selectdoent(int num, struct posttmplheader *ent,
			      char buf[512]);
static int tmpl_selecttitle(void);
static int tmpl_selectposttmpl(void);
static int tmpl_deleteposttmpl(int ent, struct posttmplheader *posttmplinfo,
			       char *direct);
static int tmpl_postwithtmpl(int ent, struct posttmplheader *posttmplinfo,
			     char *direct);
static int tmpl_out_mem(char *buf, char *out);
static int tmpl_out_file(char *buf, FILE *out);
static int tmpl_preview_content(int ent, struct posttmplheader *posttmplinfo, char *direct);
static int tmpl_preview_question(int ent, struct posttmplheader *posttmplinfo, char *direct);
static int tmpl_edit_posttmpl(int ent, struct posttmplheader *posttmplinfo, char *direct);

struct one_key selectposttmpl_comms[] = {
	{'r', tmpl_postwithtmpl, "ģ�巢��"},
	{'L', show_allmsgs, "�鿴��Ϣ"},
	{'!', Q_Goodbye, "������վ"},
	{'S', s_msg, "����ѶϢ"},
	{'c', t_friends, "�鿴����"},
	{'a', new_posttmpl, "������ģ��"},
	{'d', tmpl_deleteposttmpl, "ɾ��ģ��"},
	{'l', tmpl_preview_content, "Ԥ��ģ��"},
	{'C', tmpl_preview_question, "Ԥ������"},
	{'E', tmpl_edit_posttmpl, "�޸�ģ��"},
	{'h', posttmplhelp, "�鿴����"},
	{'\0', NULL, ""}
};

int
into_posttmpl()
{
	int savemode = uinfo.mode;
	tmpl_selectposttmpl();
	modify_user_mode(savemode);
	return 999;
}

static int
tmpl_selectposttmpl()
{
	char posttmpldir[PATH_MAX];

	setbfile(posttmpldir, currboard, ".TMPL");
	i_read(POSTTMPL, posttmpldir, tmpl_selecttitle,
	       (void *) tmpl_selectdoent, selectposttmpl_comms,
	       sizeof (struct posttmplheader));
	return 0;
}

static int
tmpl_selecttitle()
{

	if (chkmail())
		showtitle("ѡ��ģ��", "[�����ż�,�밴 w �鿴�ż�]");
	else
		showtitle("ѡ��ģ��", MY_BBS_NAME);
	prints
	    ("�뿪[\033[1;32m��\033[m,\033[1;32mq\033[m]  ѡ��[\033[1;32m��\033[m,\033[1;32m��\033[m]  ����[\033[1;32m��\033[m,\033[1;32mRtn\033[m]  ������ģ��[\033[1;32ma\033[m]  ����[\033[1;32mh\033[m]\033[m\n");
	prints("\033[1;44m��� %-12s %-48s %8s   \033[m\n", "������",
	       "��  ��", "���Ĵ���");
	clrtobot();
	return 0;
}

static char *
tmpl_selectdoent(int num, struct posttmplheader *ent, char buf[512])
{
	snprintf(buf, 512, " \033[m%3d %-12.12s �� %-45.45s %6d\033[m",
		 num, ent->userid, ent->name, ent->num_visitors);
	return buf;
}

int
new_posttmpl()
{
	char posttmpldir[PATH_MAX];
	char notefn[PATH_MAX], questionfn[PATH_MAX], contentfn[PATH_MAX];
	char filepath[STRLEN];
	struct posttmplheader bn;
	if (!IScurrBM) {
		return DONOTHING;
	}
	bzero(&bn, sizeof (struct posttmplheader));
	getdata(t_lines - 1, 0, "������ģ������: ", bn.name, 50, DOECHO, YEA);
	if (bn.name[0] == '\0')
		return FULLUPDATE;
	setbfile(filepath, currboard, "");
	bn.filetime = trycreatefile(filepath, "T.%d.Q", now_t, 100);
	if (bn.filetime < 0)
		return FULLUPDATE;
	do {
		clear();
		move(4, 0);
		prints("��Ҫ�滻������ʹ�� [$n] (n = 1, 2 ...)");
		getdata(3, 0, "���±���: ", bn.title, 50, DOECHO, YEA);
	} while (!bn.title[0]);
	tmpl_set_filename(notefn, sizeof (notefn), currboard, bn.filetime, 'N');
	tmpl_set_filename(contentfn, sizeof (contentfn), currboard, bn.filetime,
			  'C');
	tmpl_set_filename(questionfn, sizeof (questionfn), currboard,
			  bn.filetime, 'Q');
	if (!tmpl_setnote(notefn)
	    || !tmpl_setcontent(contentfn)
	    || !(bn.question_num = tmpl_setquestion(questionfn))) {
		unlink(notefn);
		unlink(contentfn);
		unlink(questionfn);
		return FULLUPDATE;
	}
	if (askyn("ȷ��Ҫ�����µķ���ģ����", YEA, NA) == NA) {
		unlink(notefn);
		unlink(contentfn);
		unlink(questionfn);
		return FULLUPDATE;
	}
	strsncpy(bn.userid, currentuser->userid, sizeof (bn.userid));
	setbfile(posttmpldir, currboard, ".TMPL");
	append_record(posttmpldir, &bn, sizeof (struct posttmplheader));

	return FULLUPDATE;
}

static int
tmpl_setnote(char *notefn)
{
	clear();
	move(2, 0);
	prints("���������ʼ���뱾ģ���\033[1m�û�����¼\033[m");
	pressanykey();
	if (vedit(notefn, NA, YEA) == REAL_ABORT)
		return 0;
	return 1;
}

static int
tmpl_setcontent(char *contentfn)
{
	clear();
	move(2, 0);
	prints("���������ʼ���뱾ģ�������\n"
	       "��Ҫ�滻������ʹ�� [$n] (n = 1, 2 ...)");
	pressanykey();
	if (vedit(contentfn, NA, YEA) == REAL_ABORT)
		return 0;
	return 1;
}

static int
tmpl_setquestion(char *questionfn)
{
	int i, n;
	struct posttmplquestion qn;
	clear();
	move(2, 0);
	prints("�����趨(������ʾ��Ϊ�ձ�ʾ����):\n");

	for (i = 0; i < QUESTION_MAX_NUM; i++) {
		bzero(&qn, sizeof (struct posttmplquestion));
		move(5, 0);
		clrtobot();
		prints("��%d������(���%d��):\n", i + 1, QUESTION_MAX_NUM);
		getdata(6, 0, "������ʾ��: ", genbuf, 50, DOECHO, YEA);
		if(!genbuf[0]) {
			if(i == 0) {
				move(7, 0);
				prints("��������Ҫ��һ�����⡣");
				pressanykey();
				i --;
				continue;
			}
			break;
		}
		strsncpy(qn.str, genbuf, sizeof (qn.str));
		move(8, 0);
		prints("(1: �����ı�Ƭ�ϻ����ı�   2: �����ı�)");
		qn.type = askone(7, 0, "�������� (1/2)? [1]", "12", '1') - '0';
		move(9, 0);
		prints("(���ֵ%d)", ANSWER_MAXLENGTH);
		do {
			getdata(8, 0, "����󳤶�(\033[1m�ַ�\033[m��): ", genbuf, 5,
				DOECHO, YEA);
			qn.answer_maxlen = atoi(genbuf);
		} while (qn.answer_maxlen < 1
			 || qn.answer_maxlen > ANSWER_MAXLENGTH);
		n = append_record(questionfn, &qn,
				  sizeof (struct posttmplquestion));
		if (n == -1) {
			move(9, 0);
			prints("����������");
			pressanykey();
			return 0;
		}
	}
	return i;
}

static int
tmpl_deleteposttmpl(int ent, struct posttmplheader *posttmplinfo, char *direct)
{
	char notefn[PATH_MAX], questionfn[PATH_MAX], contentfn[PATH_MAX];
	struct posttmplheader tmphdr;
	if (!IScurrBM)
		return DONOTHING;
	if (askyn("ȷ��ɾ��", NA, YEA) == NA)
		return FULLUPDATE;
	tmpl_set_filename(notefn, sizeof (notefn), currboard,
			  posttmplinfo->filetime, 'N');
	tmpl_set_filename(contentfn, sizeof (contentfn), currboard,
			  posttmplinfo->filetime, 'C');
	tmpl_set_filename(questionfn, sizeof (questionfn), currboard,
			  posttmplinfo->filetime, 'Q');
	unlink(notefn);
	unlink(contentfn);
	unlink(questionfn);
	if(new_search_record(direct, &tmphdr, sizeof(struct posttmplheader),
			(void *)tmpl_cmp_psttmpls, &(posttmplinfo->filetime)) == ent) {
		delete_record(direct, sizeof (struct posttmplheader), ent);
	}
	return FULLUPDATE;
}

static int
tmpl_postwithtmpl(int ent, struct posttmplheader *posttmplinfo, char *direct)
{
	char titlebuf[STRLEN], *p;
	FILE *fin, *fout;
	char notefn[PATH_MAX], questionfn[PATH_MAX], contentfn[PATH_MAX];
	int i, n, pos;
	struct posttmplquestion qlist[QUESTION_MAX_NUM];
	char *alist[QUESTION_MAX_NUM];
	char psttmpfn[PATH_MAX];
	struct posttmplheader tmphdr;
	if (!post_check_perm(currboard)) {
		return FULLUPDATE;
	}
	tmpl_set_filename(notefn, sizeof (notefn), currboard,
			  posttmplinfo->filetime, 'N');
	tmpl_set_filename(contentfn, sizeof (contentfn), currboard,
			  posttmplinfo->filetime, 'C');
	tmpl_set_filename(questionfn, sizeof (questionfn), currboard,
			  posttmplinfo->filetime, 'Q');
	ansimore(notefn, YEA);
	n = get_records(questionfn, qlist, sizeof (struct posttmplquestion), 1,
			posttmplinfo->question_num);
	if (n != posttmplinfo->question_num) {
		clear();
		move(2, 0);
		prints("��ȡ���������ȷ��ģ���Ƿ��Ѿ���ɾ����");
		pressanykey();
		return FULLUPDATE;
	}

	bzero(alist, sizeof (char *) * posttmplinfo->question_num);
	for (i = 0; i < posttmplinfo->question_num; i++) {
		alist[i] = (char *) malloc(qlist[i].answer_maxlen + 1);
		if (!alist[i]) {
			for (i = 0; i < posttmplinfo->question_num; i++) {
				if (alist[i]) {
					free(alist[i]);
				}
			}
			return FULLUPDATE;
		}
		clear();
		move(2, 0);
		prints("����: %s\n����󳤶�: \033[1m%5d\033[m\t���س�����\t%s",
		       qlist[i].str, qlist[i].answer_maxlen,
		       (qlist[i].type == 1) ? "" : "ctrl+Q����");
		if (qlist[i].type == 1)
			getdata(4, 0, NULL, alist[i],
				qlist[i].answer_maxlen + 1, DOECHO, YEA);
		else
			multi_getdata(4, 0, 79, NULL, alist[i],
				      qlist[i].answer_maxlen + 1, 11, 1);
	}
	clear();
	move(2, 0);
	if (askyn("ȷ��Ҫ������ƪ������", YEA, NA) == NA) {
		for (i = 0; i < posttmplinfo->question_num; i++) {
			free(alist[i]);
		}
		return FULLUPDATE;
	}
	if ((fin = fopen(contentfn, "r")) == NULL) {
		for (i = 0; i < posttmplinfo->question_num; i++) {
			free(alist[i]);
		}
		clear();
		move(2, 0);
		prints("��ȡģ�����ݴ�����ȷ��ģ���Ƿ��Ѿ���ɾ����");
		pressanykey();
		return FULLUPDATE;
	}
	snprintf(psttmpfn, sizeof (psttmpfn), "bbstmpfs/tmp/tmplpst.%s.%05d",
		 currentuser->userid, uinfo.pid);
	if ((fout = fopen(psttmpfn, "w")) == NULL) {
		for (i = 0; i < posttmplinfo->question_num; i++) {
			free(alist[i]);
		}
		fclose(fin);
		clear();
		move(2, 0);
		prints("������ʱ�ļ�����");
		pressanykey();
		return FULLUPDATE;
	}
	while (fgets(genbuf, sizeof (genbuf), fin)) {
		tmpl_replacetxt(genbuf, alist,
				posttmplinfo->question_num,
				(void *) tmpl_out_file, fout);
	}
	if (!(uinfo.signature == 0 || header.chk_anony == 1)) {
		addsignature(fout, 1);
	}
	fclose(fin);
	fclose(fout);
	add_loginfo(psttmpfn);
	*titlebuf = 0;
	tmpl_replacetxt(posttmplinfo->title, alist,
			posttmplinfo->question_num, (void *) tmpl_out_mem,
			titlebuf);
	p = strchr(titlebuf, '\n');
	if (p)
		*p = 0;
	i = strlen(titlebuf) - 1;
	while (i > 0 && isspace(titlebuf[i]))
		titlebuf[i--] = 0;
	if (titlebuf[0] == 0)
		strcpy(titlebuf, "û�б���");
	titlebuf[49] = 0;
	postfile(psttmpfn, currboard, titlebuf, 2);
	unlink(psttmpfn);
	for (i = 0; i < posttmplinfo->question_num; i++) {
		free(alist[i]);
	}
	// add num_visitors
	pos = new_search_record(direct, &tmphdr, sizeof(struct posttmplheader),
				(void *)tmpl_cmp_psttmpls, &(posttmplinfo->filetime));
	if(pos <= 0) {
		return FULLUPDATE;
	}
	tmphdr.num_visitors ++;
	substitute_record(direct, &tmphdr, sizeof(struct posttmplheader), pos);
	// add num_visitors end
	return DIRCHANGED;//because num_visitors changed
}

static int
tmpl_out_mem(char *buf, char *out)
{
	int len;
	len = strlen(out);
	if (len >= 49)
		return 0;
	strncat(out, buf, 49 - len);
	return 1;
}

static int
tmpl_out_file(char *buf, FILE * out)
{
	fputs(buf, out);
	return 1;
}

static int
tmpl_preview_content(int ent, struct posttmplheader *posttmplinfo, char *direct)
{
	char contentfn[PATH_MAX];
	tmpl_set_filename(contentfn, sizeof (contentfn), currboard,
			  posttmplinfo->filetime, 'C');
	clear();
	move(2, 0);
	prints("ģ����⣺%s", posttmplinfo->title);
	move(3, 0);
	prints("ģ������Ԥ����");
	ansimore2stuff(contentfn, YEA, 4, 22);
	return FULLUPDATE;
}

static int
tmpl_preview_question(int ent, struct posttmplheader *posttmplinfo, char *direct)
{
	char questionfn[PATH_MAX];
	FILE *fquestion;
	struct posttmplquestion qn;
	int i;
	tmpl_set_filename(questionfn, sizeof (questionfn), currboard,
			  posttmplinfo->filetime, 'Q');
	clear();
	move(2, 0);
	prints("ģ������Ԥ����");
	if ((fquestion = fopen(questionfn, "r")) == NULL) {
		move(3, 0);
		prints("��ȡ���������ȷ��ģ���Ƿ��Ѿ���ɾ����");
		pressanykey();
		return FULLUPDATE;
	}
	for (i = 0; i < posttmplinfo->question_num; i++) {
		if (fread(&qn, sizeof (struct posttmplquestion), 1, fquestion) != 1) {
			fclose(fquestion);
			move(i + 3, 0);
			prints("��ȡ�������2����ȷ��ģ���Ƿ��Ѿ���ɾ����");
			pressanykey();
			return FULLUPDATE;
		}
		move(i + 3, 0);
		prints("%2d: %s (�����%d�ֽ�)", i + 1, qn.str, qn.answer_maxlen);
	}
	fclose(fquestion);
	pressanykey();
	return FULLUPDATE;
}

static int
tmpl_edit_posttmpl(int ent, struct posttmplheader *posttmplinfo, char *direct)
{
	char ans[3];
	int type, dir_changed = 0, pos;
	struct posttmplheader bn, tmphdr;
	char notefn[PATH_MAX], questionfn[PATH_MAX], contentfn[PATH_MAX];
	char tempfn[PATH_MAX];

	if (!IScurrBM)
		return DONOTHING;

	ans[0] = '\0';
	getdata(t_lines - 1, 0,
		"��ѡ���޸���Ŀ:0)ȡ�� 1)ģ������ 2)���±��� 3)���� 4)����¼ 5)��������[0]:",
		ans, 2, DOECHO, NA);
	type = atoi(ans);
	if(ans[0] == '\0') {
		type = 0;
	}
	if(type < 1 || type > 5) {
		return FULLUPDATE;
	}
	memcpy(&bn, posttmplinfo, sizeof(struct posttmplheader));
	switch(type) {
	case 1:
		getdata(t_lines - 1, 0, "��ģ������:", bn.name, 50, DOECHO, NA);
		dir_changed = 1;
		break;
	case 2:
		getdata(t_lines - 1, 0, "�����±���:", bn.title, 50, DOECHO, NA);
		dir_changed = 1;
		break;
	case 3:
		tmpl_set_filename(contentfn, sizeof (contentfn), currboard, bn.filetime, 'C');
		tmpl_setcontent(contentfn);
		return FULLUPDATE;
	case 4:
		tmpl_set_filename(notefn, sizeof (notefn), currboard, bn.filetime, 'N');
		tmpl_setnote(notefn);
		return FULLUPDATE;
	case 5:
		// 'T' means temp file
		tmpl_set_filename(tempfn, sizeof (tempfn), currboard, bn.filetime, 'T');
		tmpl_set_filename(questionfn, sizeof (questionfn), currboard, bn.filetime, 'Q');
		if(file_exist(tempfn)) {
			unlink(tempfn);
		}
		bn.question_num = tmpl_setquestion(tempfn);
		if(bn.question_num == 0) {
			return FULLUPDATE;
		}
		if (askyn("ȷ��Ӧ����������ô", YEA, NA) == NA) {
			unlink(tempfn);
			return FULLUPDATE;
		}
		rename(tempfn, questionfn);
		dir_changed = 1;
		break;
	}
	if(!dir_changed) {
		return FULLUPDATE;
	}
	pos = new_search_record(direct, &tmphdr, sizeof(struct posttmplheader),
				(void *)tmpl_cmp_psttmpls, &(bn.filetime));
	if(pos <= 0) {
		return FULLUPDATE;
	}
	substitute_record(direct, &bn, sizeof(struct posttmplheader), pos);
	return DIRCHANGED;
}
