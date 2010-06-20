#include "bbslib.h"
static char *vote_type[] = { "�Ƿ�", "��ѡ", "��ѡ", "����", "�ʴ�" };
static FILE *sug = NULL;
unsigned int result[33];
int addtofile();

void
show_vote_list(char *board, struct boardmem *x, int num)
{
	FILE *fp;
	char controlfile[STRLEN], flagname[STRLEN];
	char *date;
	int i, num_voted;
	struct votebal ent;
	struct stat st;

	sprintf(controlfile, "vote/%s/%s", board, "control");
	fp = fopen(controlfile, "r");
	printf("<center><table border=1><tr>");
	printf("<td>���</td>");
	printf("<td>����ͶƱ����</td>");
	printf("<td>������</td>");
	printf("<td>ͶƱ����</td>");
	printf("<td>���</td>");
	printf("<td>����</td>");
	printf("<td>����</td>");
	printf("<td>��ʱ���</td>");
	printf("</tr>");
	for (i = 1; i <= num; i++) {
		fread(&ent, sizeof (struct votebal), 1, fp);
		sprintf(flagname, "vote/%s/flag.%d", board, (int) ent.opendate);
		num_voted = (stat(flagname, &st) ==
		     	-1) ? 0 : st.st_size / sizeof (struct ballot);
		date = ctime(&ent.opendate) + 4;
		printf("<tr>");
		printf("<td>%d</td>", i);
		printf("<td><a href=bbsqry?userid=%s>%s</a></td>", ent.userid,
		       ent.userid);
		printf("<td>%.24s</td>", date);
		printf("<td><a href=bbsvote?B=%d&voteidx=%d>%s<a></td>",
		       getbnumx(x), i, ent.title);
		printf("<td>%s</td>", vote_type[ent.type - 1]);
		printf("<td>%d</td>", ent.maxdays);
		printf("<td>%d</td>", num_voted);
		printf("<td><a href=vote?B=%d&voteidx=%d&poll=%d>%s</a></td>", 
			getbnum(board), i, ent.flag & VOTE_FLAG_POLL,
			ent.flag & VOTE_FLAG_POLL ? "�鿴" : "");
		printf("</tr>");
	}
	printf("</table></center>");
	fclose(fp);
	printf("<p><a href=javascript:history.go(-1);>������һҳ</a>");
}

void
show_vote_form(char *board, struct votebal *currvote, int num_voted,
	       struct ballot *uservote)
{
	FILE *fp;
	char *date;
	time_t closedate;
	char buf[256], buf1[512];
	unsigned int i, j, multiroll = 0;

	date = ctime(&currvote->opendate) + 4;
	closedate = currvote->opendate + currvote->maxdays * 86400;

	printf("ͶƱ������ %s<br>", void1(nohtml(currvote->title)));
	printf("ͶƱ������ %s<br>", vote_type[currvote->type - 1]);
	printf("ͶƱ���� %s ����<br>", ctime(&closedate));
	printf("ͶƱ��ID��%s<br>",
	       (currvote->flag & VOTE_FLAG_OPENED) ? "����" : "������");
	printf("ͶƱ��IP��%s<br>", 
		(currvote->flag & VOTE_FLAG_SHOWIP) ? "����" : "������");
	if (currvote->type != VOTE_ASKING)
		printf("������Ͷ%dƱ<br>", currvote->maxtkt);
	printf("<hr>ͶƱ˵��:<br>");
	sprintf(buf, "vote/%s/desc.%d", board, (int) currvote->opendate);
	fp = fopen(buf, "r");
	if (fp == 0)
		http_fatal("ͶƱ˵����ʧ");
	while (1) {
		if (fgets(buf1, sizeof (buf1), fp) == 0)
			break;
		fhhprintf(stdout, "%s", buf1);
	}
	fclose(fp);
	if ((currvote->type != VOTE_ASKING)
	    && (currvote->type != VOTE_VALUE))
		multiroll = (num_voted + now_t) % currvote->totalitems;

	if(currvote->flag & VOTE_FLAG_IMAGE) {
		printf("<style type=\"text/css\">\n"
			".votegroup {\n"
			"height:200px; width:600px;}\n"
			".voteeach {\n"
			"text-align:center; width:200px; \n"
			"margin:10px; overflow:hidden;}\n"
			".votesingle {\n"
			"text-align:center; width:600px; margin-left:100px; overflow:hidden;}\n"
			"</style>");
	}	
	printf("<hr><form name=voteform method=post>");
	switch (currvote->type) {
	case VOTE_SINGLE:
		j = (uservote->voted >> multiroll) +
		    (uservote->voted << (currvote->totalitems - multiroll));
		for (i = 0; i < currvote->totalitems; i++) {
			if(currvote->flag & VOTE_FLAG_IMAGE) {
				if(i % 2 == 0)
					printf("<div class=votegroup>");
				sprintf(buf, "/vote/%d/%c.jpg", 
					currvote->imagedir, (i + multiroll) % currvote->totalitems + 'A');
				printf("<div class=voteeach style=\"float:%s;\">"
					"<a href='%s' target=_blank><img src='%s' width=200px></a>",
					(i % 2) == 0 ? "left" : "right", buf, buf);
			}
			printf("<input type=radio name=votesingle value=%d %s> %s <br>",
			     (i + multiroll) % currvote->totalitems + 1, 
			     (j & 1) ? "checked" : "",
			     nohtml(void1(currvote->items[(i + multiroll) % currvote->totalitems]))
			     );
			if(currvote->flag & VOTE_FLAG_IMAGE) {
				printf("</div>");
				if(i % 2 == 1 || i == currvote->totalitems-1)
					printf("</div>");
			}
			j >>= 1;
		}
		if(currvote->flag & VOTE_FLAG_IMAGE)
			printf("<div style=\"clear:both;\">");
		printf("<input type=hidden name=procvote value=2>");
		break;
	case VOTE_MULTI:
		j = (uservote->voted >> multiroll) +
		    (uservote->voted << (currvote->totalitems - multiroll));
		for (i = 0; i < currvote->totalitems; i++) {
			if(currvote->flag & VOTE_FLAG_IMAGE) {
				if(i % 2 == 0)
					printf("<div class=votegroup>");
				sprintf(buf, "/vote/%d/%c.jpg", 
					currvote->imagedir, (i + multiroll) % currvote->totalitems + 'A');
				printf("<div class=voteeach style=\"float:%s;\">"
					"<a href='%s' target=_blank><img src='%s' width=200px></a>",
					(i % 2) == 0 ? "left" : "right", buf, buf);
			}
			printf("<input type=checkbox name=votemulti%d value=%d %s> %s <br>",
			     (i + multiroll) % currvote->totalitems +
			     1, 1, (j & 1) ? "checked" : "",
			     nohtml(void1(currvote->items[(i + multiroll) % currvote->totalitems]))
			     );
			if(currvote->flag & VOTE_FLAG_IMAGE) {
				printf("</div>");
				if(i % 2 == 1 || i == currvote->totalitems-1)
					printf("</div>");
			}
			j >>= 1;
		}
		if(currvote->flag & VOTE_FLAG_IMAGE)
			printf("<div style=\"clear:both;\">");
		printf("<input type=hidden name=procvote value=3>");
		break;
	case VOTE_YN:
		j = (uservote->voted >> multiroll) +
		    (uservote->voted << (currvote->totalitems - multiroll));
		if(currvote->flag & VOTE_FLAG_IMAGE) {
			printf("<div class=votesingle>"
				"<img src='/vote/%d/A.jpg' width=400px></div>", currvote->imagedir);
		}		
		for (i = 0; i < currvote->totalitems; i++) {
			printf
			    ("<input type=radio name=voteyn value=%d %s> %s <br>",
			     (i +
			      multiroll) % currvote->totalitems +
			     1, (j & 1) ? "checked" : "",
			     nohtml(void1
				    (currvote->items
				     [(i +
				       multiroll) % currvote->totalitems])));
			j >>= 1;
		}
		if(currvote->flag & VOTE_FLAG_IMAGE)
			printf("<div style=\"clear:both;\">");
		printf("<input type=hidden name=procvote value=1>");
		break;
	case VOTE_VALUE:
		if(currvote->flag & VOTE_FLAG_IMAGE) {
			printf("<div class=votesingle>"
				"<img src='/vote/%d/A.jpg' width=400px></div>", currvote->imagedir);
		}		
		printf("������һ��ֵ��");
		printf("<input type=text name=votevalue value=%d><br>", uservote->voted);
		if(currvote->flag & VOTE_FLAG_IMAGE) {
			printf("</div>");
		}
		if(currvote->flag & VOTE_FLAG_IMAGE)
			printf("<div style=\"clear:both;\">");
		printf("<input type=hidden name=procvote value=4>");
		break;
	case VOTE_ASKING:
		if(currvote->flag & VOTE_FLAG_IMAGE) {
			printf("<div class=votesingle>"
				"<img src='/vote/%d/A.jpg' width=400px></div>", currvote->imagedir);
		}		
		if(currvote->flag & VOTE_FLAG_IMAGE)
			printf("<div style=\"clear:both;\">");
		printf("<input type=hidden name=procvote value=5>");
		break;
	default:
		http_fatal("û���������͵�ͶƱ��");
	}
	printf("<hr>���������Ľ���(���������Ч)<br>");
	printf("<textarea name=sug rows=3 cols=79 wrap=off>");
	printf("%s\n", nohtml(void1(uservote->msg[0])));
	printf("%s\n", nohtml(void1(uservote->msg[1])));
	printf("%s\n", nohtml(void1(uservote->msg[2])));
	printf("</textarea><br>");
	printf("<input type=submit name=Submit value=Ͷ��ȥ>");
	printf("<input type=reset name=Submit2 value=���ٸĸ�>");
	if(currvote->flag & VOTE_FLAG_IMAGE)
		printf("</div>");
	printf("</form>");
}

void
process_vote(char *board, struct votebal *currvote, struct ballot *uservote, int pos)
{
	unsigned int votevalue;
	int i, j, v, fd;
	char buf[STRLEN], buf1[512];
	char flagname[STRLEN], logname[STRLEN];
	char *tmp1, *tmp2;
	int aborted;
	struct votelog log;
	
	if(!currvote || !uservote || pos < 0)
		http_fatal("ft");

	aborted = NA;
	switch (currvote->type) {
	case VOTE_SINGLE:
		v = atoi(getparm("votesingle"));
		if (v  > currvote->totalitems + 1)
			http_fatal("��������");
		votevalue = 1;
		votevalue <<= (v - 1);
		aborted = (votevalue == uservote->voted);
		break;
	case VOTE_MULTI:
		votevalue = 0;
		j = 0;
		for (i = currvote->totalitems - 1; i >= 0; i--) {
			votevalue <<= 1;
			sprintf(buf, "votemulti%d", i + 1);
			v = atoi(getparm(buf));
			votevalue += v;
			j += v ;
		}
		aborted = (votevalue == uservote->voted);
		if (j > currvote->maxtkt) {
			sprintf(buf, "�����ֻ��Ͷ%dƱ", currvote->maxtkt);
			http_fatal(buf);
		}
		break;
	case VOTE_YN:
		votevalue = 1;
		votevalue <<= atoi(getparm("voteyn")) - 1;
		if (atoi(getparm("voteyn")) > currvote->totalitems + 1)
			http_fatal("��������");
		aborted = (votevalue == uservote->voted);
		break;
	case VOTE_VALUE:
		votevalue = atoi(getparm("votevalue"));
		aborted = (votevalue == uservote->voted);
		if (votevalue > currvote->maxtkt) {
			sprintf(buf, "���ֵ���ܳ���%d", currvote->maxtkt);
			http_fatal(buf);
		}
		break;
	case VOTE_ASKING:
	default:
		return;
	}
	if (aborted == YEA) {
		printf("���� ��%s��ԭ���ĵ�ͶƱ��<p>", currvote->title);
		goto END;
	}
	sprintf(flagname, "vote/%s/flag.%ld", board, currvote->opendate);
	fd = open(flagname, O_RDWR | O_CREAT, 0660);
	if(fd < 0)
		http_fatal("����ͶƱʧ��");

	strcpy(uservote->uid, currentuser->userid);
	uservote->voted = votevalue;
	strsncpy(buf1, getparm("sug"), 500);
	tmp2 = buf1;
	if (pos > 0)
		uservote->msg[0][0] = uservote->msg[1][0] = uservote->msg[2][0] = '\0';
	for (i = 0; i < 3; i++) {
		tmp1 = strchr(tmp2, '\n');
		if (tmp1)
			*tmp1 = '\0';
		strsncpy(uservote->msg[i], tmp2, 70);
		if (!tmp1)
			break;
		tmp2 = ++tmp1;
	}
	flock(fd, LOCK_EX);
	if (pos > 0) // substitute vote record
		lseek(fd, (pos-1)*sizeof(struct ballot), SEEK_SET);
	else
		lseek(fd, 0, SEEK_END);
	write(fd, uservote, sizeof(struct ballot));
	flock(fd, LOCK_UN);
	close(fd);

	if (currvote->flag & VOTE_FLAG_OPENED) { // log vote ID/IP
		strcpy(log.uid, currentuser->userid);
		if (currvote->flag & VOTE_FLAG_SHOWIP)
			strsncpy(log.ip, realfromhost, sizeof (log.ip));
		else
			strsncpy(log.ip, fromhost, sizeof (log.ip));
		log.votetime = now_t;
		log.voted = uservote->voted;
		sprintf(logname, "vote/%s/newlog.%ld", board, 
				currvote->opendate);
		append_record(logname, &log, sizeof(log));
	}

	printf("<p>�Ѿ�����Ͷ��Ʊ���С�</p>");
	if (!strcasecmp(board, "SM_Election")) { // for sm election
		sprintf(buf1, "%s %s %s", currentuser->userid, 
			inet_ntoa(from_addr), Ctime(now_t));
		addtofile(MY_BBS_HOME "/vote/smvote.log", buf1);
	}
END:
	printf("<a href=javascript:history.go(-2);>����ͶƱ�б�</a>");
	return;
}


static int
b_suckinfile(fp, fname)
FILE *fp;
char *fname;
{
	char inbuf[256];
	FILE *sfp;
	if ((sfp = fopen(fname, "r")) == NULL)
		return -1;
	while (fgets(inbuf, sizeof (inbuf), sfp) != NULL)
		fputs(inbuf, fp);
	fclose(sfp);
	sfp = 0;
	return 0;
}


static int
count_result(struct ballot *ptr, struct votebal *currvote)
{
	int i;

	if (ptr == NULL) {
		if (sug != NULL) {
			fclose(sug);
			sug = NULL;
		}
		return 0;
	}
	if (ptr->msg[0][0] != '\0') {
		if (currvote->type == VOTE_ASKING) {
			fprintf(sug, "\033[1m%.12s \033[m���������£�\n",
				ptr->uid);
		} else
			fprintf(sug, "\033[1m%.12s \033[m�Ľ������£�\n",
				ptr->uid);
		for (i = 0; i < 3; i++)
			fprintf(sug, "%s\n", ptr->msg[i]);
	}
	result[32]++;
	if (currvote->type == VOTE_ASKING) {
		return 0;
	}
	if (currvote->type != VOTE_VALUE) {
		for (i = 0; i < 32; i++) {
			if ((ptr->voted >> i) & 1)
				(result[i])++;
		}

	} else {
		result[31] += ptr->voted;
		result[(ptr->voted * 10) / (currvote->maxtkt + 1)]++;
	}
	return 0;
}


static void
get_result_title(FILE * fp, char *board, struct votebal *currvote)
{
	char buf[STRLEN], *tktDesc[] =
	    { NULL, NULL, NULL, "��ͶƱ��", "ֵ���ɳ���" };

	fprintf(fp,
		"�� ͶƱ�����ڣ�\033[1m%.24s\033[m  ���\033[1m%s\033[m\n",
		ctime(&currvote->opendate), vote_type[currvote->type - 1]);
	fprintf(fp, "�� ���⣺\033[1m%s\033[m\n", currvote->title);
	if (currvote->type == VOTE_VALUE || currvote->type == VOTE_MULTI)
		fprintf(fp, "�� �˴�ͶƱ��%s��\033[1m%d\033[m\n\n",
			tktDesc[(int) currvote->type], currvote->maxtkt);
	fprintf(fp, "�� Ʊѡ��Ŀ������\n\n");
	sprintf(buf, "vote/%s/desc.%ld", board, currvote->opendate);
	b_suckinfile(fp, buf);
}


static void
mk_result(char *board, struct votebal *currvote)
{
	char fname[STRLEN], nname[STRLEN];
	char sugname[STRLEN];
	int i, j, percent;
	unsigned int total = 0;
	char *blk[] = { "��", "��", "��", "��", "��", "��", "��", "��", " " };

	count_result(NULL, currvote);
	sprintf(sugname, "bbstmpfs/tmp/votetmp.%s.%d", board, getpid());
	if ((sug = fopen(sugname, "w")) == NULL) {
		errlog("open vote tmp file error %d", errno);
		return;
	}
	memset(result, 0, sizeof (result));
	sprintf(fname, "vote/%s/flag.%ld", board, currvote->opendate);
	if (new_apply_record(fname, sizeof (struct ballot), (void *) count_result, currvote)
	    == -1) {
		errlog("Vote apply flag error");
	}
	fprintf(sug,
		"\033[1;44;36m������������������������������ʹ����%s������������������������������\033[m\n\n\n",
		(currvote->type != VOTE_ASKING) ? "��������" : "�˴ε�����");
	fclose(sug);
	sug = NULL;
	sprintf(nname, "bbstmpfs/tmp/voteresults.%s.%d", board, getpid());
	if ((sug = fopen(nname, "w")) == NULL) {
		errlog("open vote newresult file error %d", errno);
		return;
	}
	get_result_title(sug, board, currvote);

	fprintf(sug, "** ͶƱ���:\n\n");
	if (currvote->type == VOTE_VALUE) {
		total = result[32];
		for (i = 0; i < 10; i++) {
			fprintf(sug,
				"\033[1m  %4d\033[m �� \033[1m%4d\033[m ֮���� \033[1m%4d\033[m Ʊ  Լռ ",
				(i * currvote->maxtkt) / 10 + ((i == 0) ? 0 : 1),
				((i + 1) * currvote->maxtkt) / 10, result[i]);
			percent = 0.5 +
			    (result[i] * 100.) / ((total <= 0) ? 1 : total);
			fprintf(sug, "%3d%%", percent);
			fprintf(sug, "\033[3%dm", 1 + i % 7);
			for (j = percent / 16; j; j--)
				fprintf(sug, "%s", blk[0]);
			fprintf(sug, "%s\033[m\n", blk[8 - (percent % 16) / 2]);
		}
		fprintf(sug, "�˴�ͶƱ���ƽ��ֵ��: \033[1m%d\033[m\n",
			result[31] / ((total <= 0) ? 1 : total));
	} else if (currvote->type == VOTE_ASKING) {
		total = result[32];
	} else {
		for (i = 0; i < currvote->totalitems; i++) {
			total += result[i];
		}
		for (i = 0; i < currvote->totalitems; i++) {
			fprintf(sug, "(%c) %-40s  %4d Ʊ  Լռ ",
				'A' + i, currvote->items[i], result[i]);
			percent =
			    (result[i] * 100) / ((total <= 0) ? 1 : total);
			fprintf(sug, "%3d%%", percent);
			fprintf(sug, "\033[3%dm", 1 + i % 7);
			for (j = percent / 16; j; j--)
				fprintf(sug, "%s", blk[0]);
			fprintf(sug, "%s\033[m\n", blk[8 - (percent % 16) / 2]);
		}
	}
	fprintf(sug, "\nͶƱ������ = \033[1m%d\033[m ��\n", result[32]);
	fprintf(sug, "ͶƱ��Ʊ�� =\033[1m %d\033[m Ʊ\n\n", total);
	fprintf(sug,
		"\033[1;44;36m������������������������������ʹ����%s������������������������������\033[m\n\n\n",
		(currvote->type != VOTE_ASKING) ? "��������" : "�˴ε�����");
	b_suckinfile(sug, sugname);
	unlink(sugname);
	fclose(sug);
	sug = NULL;
	showcon(nname);
	unlink(nname);
	return;
}


int
bbsvote_main()
{
	FILE *fp;
	struct votebal currvote;
	struct ballot uservote;
	int i, pos;
	unsigned int j;
	char board[STRLEN];
	char controlfile[STRLEN];
	char flagname[STRLEN];
	int voted_flag;		//�û��Ƿ�Ͷ������Ʊ
	int num_voted;		//�ж�����Ͷ��Ʊ
	int num_of_vote;	//������ͶƱ��
	int voteidx;		//�û�ѡ����еڼ���ͶƱ
	int procvote;
	struct stat st;
	struct boardmem *x;

	html_header(1);
	getparmboard(board, sizeof (board));
	voteidx = atoi(getparm("voteidx"));
	procvote = atoi(getparm("procvote"));
	printf("<body><center>");
	if (!loginok || isguest) {
		printf("���ȵ�¼!<br><br>");
		http_quit();
	}
	x = getboard(board);
	if (!x)
		http_fatal("����İ���");
	printf("<a href=bbsdoc?B=%d><h2>%s������</h2></a></center>",
	       getbnumx(x), board);
	//check_msg();
	changemode(VOTING);

	sprintf(controlfile, "vote/%s/%s", board, "control");
	num_of_vote = (stat(controlfile, &st) == -1) ? 0 : st.st_size / sizeof (struct votebal);
	if (num_of_vote == 0)
		http_fatal("��Ǹ, Ŀǰ��û���κ�ͶƱ����");
	if (voteidx == 0) {	// list current votes of a board
		show_vote_list(board, x, num_of_vote);
		goto END;
	}
	
	sprintf(controlfile, "vote/%s/%s", board, "control");
	if (voteidx > num_of_vote)
		http_fatal("��������");
	fp = fopen(controlfile, "r"); //!!!
	if(fp == NULL)
		http_fatal("���ܴ��ļ�");
	
	printf("<table width=600px>");
	fseek(fp, sizeof (struct votebal) * (voteidx - 1), 0);
	fread(&currvote, sizeof (struct votebal), 1, fp);
//	if(!strcmp(currentuser->userid, "bg")) {
//		currvote.maxdays = 5;
//		fseek(fp, sizeof (struct votebal) * (voteidx - 1), 0);
//		fwrite(&currvote, sizeof (struct votebal), 1, fp);
//	}	
	fclose(fp);

	if(atoi(getparm("poll"))) {
		if(currvote.flag & VOTE_FLAG_POLL) {	//��ʱ���
			mk_result(board, &currvote);
		} else
			printf("��ͶƱ������ʵʱ�鿴�����");
		goto END;
	}
	
	// vote
	if (currvote.flag & VOTE_FLAG_LIMITED) { // for sm election 
		int retv = valid_voter(board, currentuser->userid);
		if (retv == 0 || retv == -1) {
			http_fatal("�Բ���,��û��ѡ��Ȩ");
		}
	}
/*	if (currentuser->firstlogin >= currvote.opendate - 7 * 86400)
		http_fatal
		    ("�Բ���, ��ע�ỹû��7�����ͶƱ�Ϳ���, û���ʸ�ͶƱ\n");
	*/
	if ((currvote.flag & VOTE_FLAG_LIMITIP) && invalid_voteIP(realfromhost))
		http_fatal("�Բ���,���Ӵ���վ����,û��ͶƱȨ\n");
	pos = 0;
	voted_flag = NA;
	sprintf(flagname, "vote/%s/flag.%d", board, (int) currvote.opendate);
	num_voted = (stat(flagname, &st) ==  -1) ? 0 : st.st_size / sizeof (struct ballot);
	if(isguest)
		goto DO_VOTE;
	fp = fopen(flagname, "r");
	if (fp) {
		j = strlen(currentuser->userid) + 1;
		if (j > IDLEN)
			j = IDLEN;
		for (i = 1; i <= num_voted; i++) {
			fread(&uservote, sizeof (struct ballot), 1, fp);
			if (!memcmp(uservote.uid, currentuser->userid, j)) {
				voted_flag = YEA;
				pos = i;
				break;
			}
		}
		fclose(fp);
	}
DO_VOTE:
	if (!voted_flag)
		memset(&uservote, 0, sizeof (uservote));
	if (procvote == 0) {	// show vote form
		show_vote_form(board, &currvote, num_voted, &uservote);
		goto END;
	}
	// process vote
	process_vote(board, &currvote, &uservote, pos);
      END:
	http_quit();
	return 0;
}

int
valid_voter(char *board, char *name)
{
	FILE *in;
	char buf[100];
	int i;

/*	in = fopen(MY_BBS_HOME "/etc/untrust", "r");
	while (fgets(buf, 80, in)) {
		i = strlen(buf);
		if (buf[i - 1] == '\n')
			buf[i - 1] = 0;
		if (!strcmp(buf, currentuser.lasthost)) {
			fclose(in);
			return -1;
		}
	}
	fclose(in);*/
	sprintf(buf, "%s/%s.validlist", MY_BBS_HOME, board);
	in = fopen(buf, "r");
	if (in != NULL) {
		while (fgets(buf, 80, in)) {
			i = strlen(buf);
			if (buf[i - 1] == '\n')
				buf[i - 1] = 0;
			if (!strcmp(buf, name)) {
				fclose(in);
				return 1;
			}
		}
		fclose(in);
	}
	return 0;
}
