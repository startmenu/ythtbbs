#include "bbslib.h"

static int tmpl_view(struct boardmem *x);
static int tmpl_post(struct boardmem *x, int num);
static int tmpl_send(struct boardmem *x, int num);
static int tmpl_check_BM(void);
static int tmpl_add1(struct boardmem *x);
static int tmpl_add2(struct boardmem *x);
static int tmpl_del(struct boardmem *x, int num);
static int tmpl_out_mem(char *buf, char *out);
static int tmpl_out_file(char *buf, FILE *out);

static int IScurrBM;

int
bbspsttmpl_main()
{
	char action[80], board[80];
	int num;
	struct boardmem *x;
	html_header(1);
	//check_msg();
//�޸Ŀ�ܣ��������͵ײ�
	printf("<body topmargin=0 leftMargin=1 MARGINWIDTH=1 MARGINHEIGHT=0>" WWWLEFT_DIV);

	if (!loginok || isguest)
		http_fatal("����δ��¼, ���ȵ�¼");
	strsncpy(action, getparm("action"), sizeof (action));
	getparmboard(board, sizeof (board));
	num = atoi(getparm("num"));
	if (!(x = getboard(board)))
		http_fatal("�����������");
	if (!has_post_perm(currentuser, x) && !isguest)
		http_fatal("�������������������Ȩ�ڴ���������������");
	if (noadm4political(board))
		http_fatal("�Բ���,��Ϊû�а��������Ա����,������ʱ���.");
	if (x->ban == 2)
		http_fatal("�Բ���, ��Ϊ�������³���, ������ʱ�ر�.");
	IScurrBM = has_BM_perm(currentuser, x);
	changemode(POSTTMPL);
	if (!strcmp(action, "view")) {
		tmpl_view(x);
	} else if (!strcmp(action, "post")) {
		tmpl_post(x, num);
	} else if (!strcmp(action, "send")) {
		tmpl_send(x, num);
	} else if (!strcmp(action, "add1")) {
		tmpl_add1(x);
	} else if (!strcmp(action, "add2")) {
		tmpl_add2(x);
	} else if (!strcmp(action, "del")) {
		tmpl_del(x, num);
	} else {
		http_fatal("����Ĳ���");
	}
	printf(WWWFOOT_DIV "</body></html>\n");
	http_quit();
	return 0;
}

static int
tmpl_view(struct boardmem *x)
{
	char dir[PATH_MAX];
	FILE *fp;
	int total, i = 1;
	struct posttmplheader bn;
	setbfile(dir, x->header.filename, ".TMPL");
	total = file_size(dir) / sizeof (struct posttmplheader);
	if (total < 0 || total > 30000)
		http_fatal("too many posttmpl");
	if (!total) {
		printf("<p>Ŀǰ��û���κ�ģ��</p>");
		if (IScurrBM) {
			printf("<p><a href=psttmpl?B=%d&action=add1>"
			       "�½�ģ��</a></p>", getbnumx(x));
		}
		http_quit();
		return 0;
	}
	printf("<center><h1>����ģ��ѡ��</h1></center>");
	//table head
	printf
	    ("<table align=center border=1 width=80%%><tr><th>���</th>"
	     "<th>������</th><th>ģ������</th><th>������</th><th>����</th>");
	if (IScurrBM) {
		printf("<th>����</th>");
	}
	printf("</tr>\n");
	//table head end
	if ((fp = fopen(dir, "r")) == NULL)
		http_fatal("�޷���Ŀ¼�ļ�, ��֪ͨϵͳά��");
	while (1) {
		if (fread(&bn, sizeof (struct posttmplheader), 1, fp) <= 0)
			break;
		printf
		    ("<tr><td>%d</td><td>%s</td><td>%s</td><td>%d</td>"
		     "<td><a href=psttmpl?B=%d&action=post&num=%d>"
		     "�ô�ģ�巢��</a></td>",
		     i, bn.userid, bn.name, bn.question_num, getbnumx(x), i);
		if (IScurrBM) {
			printf("<td><a href=psttmpl?B=%d&action=del"
			       "&num=%d>ɾ��</a></td>", getbnumx(x), i);
		}
		printf("</tr>\n");
		i++;
	}
	fclose(fp);
	printf("</table>");
	if (IScurrBM) {
		printf("<p><a href=psttmpl?B=%d&action=add1>"
		       "�½�ģ��</a></p>", getbnumx(x));
	}
	return 0;
}

static int
tmpl_post(struct boardmem *x, int num)
{
	char buf[256], dir[PATH_MAX];
	FILE *fquestion, *fnote;
	char notefn[PATH_MAX], questionfn[PATH_MAX];
	int i, len;
	struct posttmplheader bn;
	struct posttmplquestion qn;
	setbfile(dir, x->header.filename, ".TMPL");
	if (get_record(&bn, sizeof (struct posttmplheader), num - 1, dir) == 0) {
		http_fatal("û�����ģ��");
	}
	tmpl_set_filename(notefn, sizeof (notefn), x->header.filename,
			  bn.filetime, 'N');
	tmpl_set_filename(questionfn, sizeof (questionfn), x->header.filename,
			  bn.filetime, 'Q');
	if ((fnote = fopen(notefn, "r")) != NULL) {
		while (fgets(buf, sizeof (buf), fnote)) {
			fhhprintf(stdout, "%s", buf);
		}
		printf("<hr>");
		fclose(fnote);
	}
	if ((fquestion = fopen(questionfn, "r")) == NULL) {
		http_fatal("��ȡ���������ȷ��ģ���Ƿ��Ѿ���ɾ����");
	}
	printf("<form action=psttmpl?action=send&B=%d&num=%d method=post>",
	       getbnumx(x), num);
	for (i = 0; i < bn.question_num; i++) {
		if (fread(&qn, sizeof (struct posttmplquestion), 1, fquestion)
		    != 1) {
			fclose(fquestion);
			http_fatal("�����������");
		}
		len = qn.answer_maxlen;
		printf("����: %s      (����󳤶�: %5d)<br>", qn.str, len);
		if (qn.type == 1)
			printf
			    ("<input type=text name=text%d size=%d maxlength=%d><br><br><br>",
			     i + 1, (len > 80) ? 80 : len, len);
		else
			printf
			    ("<textarea name=text%d rows=5 cols=80 wrap=virtual></textarea><br><br><br>",
			     i + 1);
	}
	fclose(fquestion);
	printf("<input type=submit name=submit value=\"ȷ��\"></form>");
	return 0;
}

static int
tmpl_send(struct boardmem *x, int num)
{
	int i;
	char questionfn[PATH_MAX], contentfn[PATH_MAX],
	    psttmpfn[PATH_MAX], dir[PATH_MAX];
	char titlebuf[STRLEN], buf[256], *alist[QUESTION_MAX_NUM], *p, tmp[STRLEN];
	FILE *fin, *fout, *fquestion;
	struct posttmplheader bn;
	struct posttmplquestion qn;
	setbfile(dir, x->header.filename, ".TMPL");
	if (get_record(&bn, sizeof (struct posttmplheader), num - 1, dir) == 0) {
		http_fatal("û�����ģ��");
	}
	tmpl_set_filename(contentfn, sizeof (contentfn), x->header.filename,
			  bn.filetime, 'C');
	tmpl_set_filename(questionfn, sizeof (questionfn), x->header.filename,
			  bn.filetime, 'Q');
	if ((fquestion = fopen(questionfn, "r")) == NULL) {
		http_fatal("��ȡ���������ȷ��ģ���Ƿ��Ѿ���ɾ����");
	}

	bzero(alist, sizeof (char *) * bn.question_num);
	for (i = 0; i < bn.question_num; i++) {
		snprintf(tmp, sizeof (tmp), "text%d", i + 1);
		p = getparm(tmp);
		if (fread(&qn, sizeof (struct posttmplquestion), 1, fquestion)
		    != 1) {
			for (i = 0; i < bn.question_num; i++) {
				if (alist[i]) {
					free(alist[i]);
				}
			}
			fclose(fquestion);
			http_fatal("�����������");
		}
		alist[i] = (char *) malloc(qn.answer_maxlen + 1);
		if (!alist[i]) {
			for (i = 0; i < bn.question_num; i++) {
				if (alist[i]) {
					free(alist[i]);
				}
			}
			fclose(fquestion);
			http_fatal("ģ���ڲ�����2");
		}
		strsncpy(alist[i], p, qn.answer_maxlen + 1);
	}
	fclose(fquestion);
	if ((fin = fopen(contentfn, "r")) == NULL) {
		for (i = 0; i < bn.question_num; i++) {
			free(alist[i]);
		}
		http_fatal("��ȡģ�����ݴ�����ȷ��ģ���Ƿ��Ѿ���ɾ����");
	}
	snprintf(psttmpfn, sizeof (psttmpfn), "bbstmpfs/tmp/tmplpst.%s.www",
		 currentuser->userid);
	if ((fout = fopen(psttmpfn, "w")) == NULL) {
		fclose(fin);
		for (i = 0; i < bn.question_num; i++) {
			free(alist[i]);
		}
		http_fatal("������ʱ�ļ�����");
	}
	while (fgets(buf, sizeof (buf), fin))
		tmpl_replacetxt(buf, alist,
				bn.question_num, (void *) tmpl_out_file, fout);
	fclose(fin);
	fclose(fout);
	*titlebuf = 0;
	tmpl_replacetxt(bn.title, alist,
			bn.question_num, (void *) tmpl_out_mem, titlebuf);
	p = strchr(titlebuf, '\n');
	if (p) {
		*p = 0;
	}
	i = strlen(titlebuf) - 1;
	while (i > 0 && isspace(titlebuf[i])) {
		titlebuf[i--] = 0;
	}
	if (titlebuf[0] == 0) {
		strcpy(titlebuf, "û�б���");
	}
	titlebuf[49] = 0;
	printf("<center>%s </center><hr>", titlebuf);
	if ((fout = fopen(psttmpfn, "r")) != NULL) {
		while (fgets(buf, sizeof (buf), fout)) {
			fhhprintf(stdout, "%s", buf);
		}
		fclose(fout);
	}
	printf("<hr>");
	printf("<form name=form1 method=post action=bbssnd?B=%d&th=-1&tmpl=1&tmplfiletime=%d>",
	       getbnumx(x), bn.filetime);
	printselsignature();
	printuploadattach();
	printf
	    ("<input type=hidden name=title size=40 maxlength=100 value='%s '>",
	     titlebuf);
	printf
	    ("<input type=submit value=���� onclick=\"this.value='�����ύ�У����Ժ�...';"
	     "this.disabled=true;form1.submit();\"></form>");
	for (i = 0; i < bn.question_num; i++) {
		free(alist[i]);
	}
	return 0;
}

static int
tmpl_check_BM()
{
	if (!IScurrBM) {
		http_fatal("û�й���ģ���Ȩ��");
	}
	return 0;
}

static int
tmpl_add1(struct boardmem *x)
{
	int i;
	tmpl_check_BM();

	printf("<h1>���ģ���� ��һ��</h1><hr>\n"
	       "<form name=form1 action=psttmpl?B=%d&action=add2"
	       " method=post>\n"
	       "ģ�����ƣ�<br>\n"
	       "<input type=text name=tmplname size=49 maxlength=49><p><hr>\n"
	       "���±��⣺��Ҫ�滻������ʹ�� [$n] (n = 1, 2 ...)<br>\n"
	       "<input type=text name=tmpltitle size=49 maxlength=49><p><hr>\n"
	       "�û�����¼��<br>\n"
	       "<textarea name=tmplnote rows=5 cols=80 wrap=virtual></textarea><p><hr>\n"
	       "ģ�����ݣ���Ҫ�滻������ʹ�� [$n] (n = 1, 2 ...)<br>\n"
	       "<textarea name=tmplcontent rows=10 cols=80 wrap=virtual></textarea><p><hr>\n",
	       getbnumx(x));

	printf("������������ʾ��(��һ���հ�������ʾ�ʼ������Ľ�������)��");
	for (i = 0; i < QUESTION_MAX_NUM; i++) {
		printf("<p>���� %d ��ʾ�ʣ�"
		       "<input name=question%d type=text size=49"
		       " maxlength=49><br>"
		       "�������ͣ�"
		       "<select name=qtype%d>"
		       "<option value=1>�����ı�Ƭ�ϻ����ı�</option>"
		       "<option value=2>�����ı�</option>"
		       "</select><br>"
		       "����󳤶�(�ַ���) ���ֵ%d��"
		       "<input name=qlen%d type=text size=3 maxlength=3></p><hr>",
		       i + 1, i + 1, i + 1, ANSWER_MAXLENGTH, i + 1);
	}
	printf("<p><input type=submit value=\"�������ģ��\""
	       " onclick=\"this.value='��ģ���ύ�У����Ժ�...';"
	       "this.disabled=true;form1.submit();\"></p>");
	return 0;
}

static int
tmpl_add2(struct boardmem *x)
{
	int i;
	char dir[PATH_MAX];
	char notefn[PATH_MAX], questionfn[PATH_MAX], contentfn[PATH_MAX];
	char filepath[STRLEN];
	char *tmplcontent, *tmplnote;
	char buf[80];
	struct posttmplheader bn;
	struct posttmplquestion qn;
	tmpl_check_BM();

	bzero(&bn, sizeof (struct posttmplheader));
	setbfile(filepath, x->header.filename, "");
	bn.filetime = trycreatefile(filepath, "T.%d.Q", now_t, 100);
	if (bn.filetime < 0)
		return -1;
	tmpl_set_filename(notefn, sizeof (notefn), x->header.filename,
			  bn.filetime, 'N');
	tmpl_set_filename(contentfn, sizeof (contentfn), x->header.filename,
			  bn.filetime, 'C');
	tmpl_set_filename(questionfn, sizeof (questionfn), x->header.filename,
			  bn.filetime, 'Q');
	strsncpy(bn.name, getparm("tmplname"), sizeof (bn.name));
	if (bn.name[0] == '\0') {
		http_fatal("ģ�����Ʋ���Ϊ��");
	}
	strsncpy(bn.title, getparm("tmpltitle"), sizeof (bn.title));
	if (bn.title[0] == '\0') {
		http_fatal("���±��ⲻ��Ϊ��");
	}
	for (i = 0; i < QUESTION_MAX_NUM; i++) {
		bzero(&qn, sizeof (struct posttmplquestion));
		snprintf(buf, sizeof (buf), "question%d", i + 1);
		strsncpy(qn.str, getparm(buf), sizeof (qn.str));
		if (qn.str[0] == '\0') {
			if(i == 0) {
				unlink(questionfn);
				http_fatal("����Ҫ��һ������");
			}
			break;
		}
		snprintf(buf, sizeof (buf), "qtype%d", i + 1);
		qn.type = atoi(getparm(buf));
		if (qn.type != 1 && qn.type != 2) {
			unlink(questionfn);
			http_fatal("ģ���ڲ�����3");
		}
		snprintf(buf, sizeof (buf), "qlen%d", i + 1);
		qn.answer_maxlen = atoi(getparm(buf));
		if (qn.answer_maxlen < 1 || qn.answer_maxlen > ANSWER_MAXLENGTH) {
			unlink(questionfn);
			http_fatal("����󳤶ȴ���");
		}
		if (append_record
		    (questionfn, &qn, sizeof (struct posttmplquestion)) == -1) {
			unlink(questionfn);
			http_fatal("ģ���ڲ�����4");
		}
	}
	bn.question_num = i;
	tmplnote = getparm("tmplnote");
	if (f_write(notefn, tmplnote) == -1) {
		unlink(questionfn);
		http_fatal("ģ���ڲ�����5");
	}
	tmplcontent = getparm("tmplcontent");
	if (f_write(contentfn, tmplcontent) == -1) {
		unlink(questionfn);
		unlink(notefn);
		http_fatal("ģ���ڲ�����6");
	}
	strsncpy(bn.userid, currentuser->userid, sizeof (bn.userid));
	setbfile(dir, x->header.filename, ".TMPL");
	append_record(dir, &bn, sizeof (struct posttmplheader));
	// add finished
	printf("<h1>���ģ���� �ڶ���</h1><hr>\n"
	       "<p>ģ�������� <a href=psttmpl?B=%d&action=view>"
	       "����ģ��ѡ��</a></p>", getbnumx(x));
	return 0;
}

static int
tmpl_del(struct boardmem *x, int num)
{
	char dir[PATH_MAX];
	char notefn[PATH_MAX], questionfn[PATH_MAX], contentfn[PATH_MAX];
	struct posttmplheader bn;
	tmpl_check_BM();
	setbfile(dir, x->header.filename, ".TMPL");
	if (get_record(&bn, sizeof (struct posttmplheader), num - 1, dir) == 0) {
		http_fatal("û�����ģ��");
	}
	tmpl_set_filename(notefn, sizeof (notefn), x->header.filename,
			  bn.filetime, 'N');
	tmpl_set_filename(contentfn, sizeof (contentfn), x->header.filename,
			  bn.filetime, 'C');
	tmpl_set_filename(questionfn, sizeof (questionfn), x->header.filename,
			  bn.filetime, 'Q');
	unlink(notefn);
	unlink(contentfn);
	unlink(questionfn);
	delete_record(dir, sizeof (struct posttmplheader), num);
	// del finished
	printf("<h1>ɾ��ģ��</h1><hr>\n"
	       "<p>ģ��ɾ����� <a href=psttmpl?B=%d&action=view>"
	       "����ģ��ѡ��</a></p>", getbnumx(x));
	return 0;
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
