#include "bbslib.h"
#include "sec_hand.h"

void
short_stamp(str, chrono)
char *str;
time_t *chrono;
{
	struct tm *ptime;
	ptime = localtime(chrono);
	sprintf(str, "%2d/%02d", ptime->tm_mon + 1, ptime->tm_mday);
}

int
showheader(char *grp)
{
	char fpath[STRLEN];
	int fd;
	int valid = 0;
	SLOT tempslot;
	sprintf(fpath, "2nd/" FN_GRP);
	fd = open(fpath, O_RDONLY);
	if (strlen(grp))
		printf(" <a href=bbssechand>���г��ſ�</a> ");
	else
		printf(" [<a href=bbssechand>���г��ſ�</a>] ");
	if (fd > 0) {
		while (read(fd, &tempslot, sizeof (SLOT)) > 0) {
			if (!(tempslot.prop & PROP_IS_CANCEL))
				if (!strcmp(grp, tempslot.fn)) {
					printf
					    (" [<a href=bbssechand?grp=%s>%s</a>] ",
					     tempslot.fn,
					     void1(titlestr(tempslot.title)));
					valid = 1;
				} else
					printf
					    (" <a href=bbssechand?grp=%s>%s</a> ",
					     tempslot.fn,
					     void1(titlestr(tempslot.title)));
		}
		close(fd);
	}
	printf("<hr>");
	return valid;
}

void
showwelcome()
{
	showcon("etc/2ndhand_wwwdecl");
	printf("</center><br><font class=f2>");
	printf
	    ("��ذ��棺<br><li><a href=bbshome?board=secondhand>������̯��(secondhand)</a><br>"
	     "<li><a href=bbshome?board=Shopping>�����(Shopping)</a>");
}

void
showgroup(char *grp)
{
	char fpath[STRLEN];
	char grpdir[STRLEN];
	int fd;
	int i = 1;
	SLOT tempslot;
	sprintf(grpdir, "2nd/%s", grp);
	sprintf(fpath, "2nd/%s/%s", grp, FN_ITEM);
	if (!file_isdir(grpdir))
		http_fatal("���鲻���ڻ��ѱ�ɾ��");
	printf("<a href=bbssechand?grp=%s&mode=1&sell=0>��Ҫ������Ʒ</a> ",
	       grp);
	printf("<a href=bbssechand?grp=%s&mode=1&sell=1>��Ҫ������Ʒ</a>", grp);
	printhr();
	fd = open(fpath, O_RDONLY);
	if (fd > 0) {
		printf
		    ("<table><tr><td>���</td><td>Ŀ��</td><td>����</td><td>����</td><td>����</td><td>��������</td></tr>");
		while (read(fd, &tempslot, sizeof (SLOT)) > 0) {
			if (tempslot.prop & PROP_IS_CANCEL)
				continue;
			printf("<tr>");
			printf("<td>%d</td>", i);
			printf("<td>%s</td>",
			       (tempslot.prop & PROP_I_SELL) ?
			       "<font color=green>ת��</font>" :
			       "<font color=red>��</font>");
			printf("<td><a href=bbsqry?userid=%s>%s</a></td>",
			       tempslot.userid, tempslot.userid);
			printf("<td>%s</td>", tempslot.date);
			printf
			    ("<td><a href=bbssechand?grp=%s&item=%s>%s</a></td>",
			     grp, tempslot.fn, void1(titlestr(tempslot.title)));
			printf("<td>%d</td>", tempslot.reply);
			printf("</tr>");
			i++;
		}
		close(fd);
		printf("</table>");
	}
	printhr();
}

void
showitem(char *grp, char *item)
{
	char fpath[STRLEN];
	char dotitem[STRLEN];
	char prev[STRLEN], next[STRLEN];
	int fd;
	time_t t = atoi(item + 2);
	SLOT tempslot;
	sprintf(dotitem, "2nd/%s/%s", grp, FN_ITEM);
	sprintf(fpath, "2nd/%s/%s", grp, item);
	prev[0] = next[0] = '\0';
	fd = open(dotitem, O_RDONLY);
	if (fd > 0) {
		while (read(fd, &tempslot, sizeof (SLOT)) > 0) {
			if (!strcmp(tempslot.fn, item)
			    && !(tempslot.prop & PROP_IS_CANCEL)) {
				printf
				    ("<font size=+1>%s</font><br></center><font style='font-size:14px'>"
				     "����ʱ�䣺%s<br>������<br>",
				     void1(titlestr(tempslot.title)),
				     ctime(&t));
				showcon(fpath);
				printf("ϣ���۸�: %s<br>", tempslot.price);
				printf
				    ("<a href=bbssechand?grp=%s&item=%s&mode=3>д�Ÿ�%s</a><br>",
				     grp, tempslot.fn, tempslot.userid);
				printf("�������緽ʽ: %s<br>",
				       tempslot.contact);
				while (read(fd, &tempslot, sizeof (SLOT)) > 0) {
					if (!(tempslot.prop & PROP_IS_CANCEL)) {
						strcpy(next, tempslot.fn);
						break;
					}
				}
				break;
			}
			if (!(tempslot.prop & PROP_IS_CANCEL))
				strcpy(prev, tempslot.fn);
		}
		close(fd);
	}
	printf("<hr>");
	if (prev[0] != '\0')
		printf("<a href=bbssechand?grp=%s&item=%s>��һƪ</a> ",
		       grp, prev);
	printf("<a href=bbssechand?grp=%s>����������Ϣ</a> ", grp);
	if (next[0] != '\0')
		printf("<a href=bbssechand?grp=%s&item=%s>��һƪ</a> ",
		       grp, next);
}

void
postnewslot(char *grp)
{
	printf("%s -- �����г� [ʹ����: %s]<hr>\n", BBSNAME,
	       currentuser->userid);
	printf("<table border=1>\n");
	printf("<tr><td>");
	printf("<font color=green>����ע������: <br>\n");
	printf
	    ("����ʱӦ���ؿ������������Ƿ��ʺϹ������Ϸ������������ˮ��лл���ĺ�����<br>"
	     "�ڴ˷���Ĺ��ͬʱ��������Secondhand����棬���Բ����ظ����ġ�\n</font>");
	printf
	    ("<tr><td><form name=form1 method=post action=bbssechand?grp=%s&mode=2>\n",
	     grp);
	printf("ʹ�ñ���: <input type=text name=title size=30 maxlength=30>");
	printf(" ���ߣ�%s", currentuser->userid);
	printf
	    ("<br>����ϣ���ļ۸�<input type=text name=price size=10 maxlength=10>");
	printf
	    (" ���������������緽ʽ��<input type=text name=contact size=20 maxlength=20>");
	if (atoi(getparm("sell")) == 0) {
		printf("<input type=radio name=sell value=0 checked>�� "
		       "<input type=radio name=sell value=1>��");
	} else {
		printf("<input type=radio name=sell value=0>�� "
		       "<input type=radio name=sell value=1 checked>��");
	}
	printf
	    ("<br>\n<textarea  onkeydown='if(event.keyCode==87 && event.ctrlKey) {document.form1.submit(); return false;}'  onkeypress='if(event.keyCode==10) return document.form1.submit()' name=text rows=20 cols=76 wrap=physical class=f2>\n\n");
	printf("</textarea>\n");
	printf("<tr><td>");
	printf
	    ("<tr><td class=post align=center><input type=submit value=����> ");
	printf("<input type=reset value=���></form>\n");
	printf("</table>");
}

int
savenewslot(char *grp)
{

	FILE *fp, *fp2;
	char filename[80], title[30], buf[256], buf3[1024], *content;
	int i, sell, fd;
	struct SLOT tosave;
	time_t t = 0;		//to make gcc happy,let t has a initial value.
	bzero(&tosave, sizeof (tosave));
	strsncpy(title, getparm("title"), 30);
	sell = atoi(getparm("sell"));
	strsncpy(tosave.price, getparm("price"), 10);
	strsncpy(tosave.contact, getparm("contact"), 20);
	for (i = 0; i < strlen(title); i++)
		if (title[i] <= 27 && title[i] >= -1)
			title[i] = ' ';
	i = strlen(title) - 1;
	while (i > 0 && isspace(title[i]))
		title[i--] = 0;
	content = getparm("text");
	if (title[0] == 0)
		http_fatal("���±���Ҫ�б���");
	if ((now_t - w_info->lastposttime) < 6) {
		w_info->lastposttime = now_t;
		http_fatal("���η��ļ������, ����Ϣ���������");
	}
	w_info->lastposttime = now_t;
	sprintf(filename, "bbstmpfs/tmp/%d.tmp", thispid);
	f_write(filename, content);
	for (i = 0; i < 100; i++) {
		t = now_t + i;
		sprintf(buf3, "2nd/%s/M.%ld.A", grp, t);
		if (!file_exist(buf3))
			break;
	}
	if (i >= 100)
		return -1;
	strsncpy(tosave.title, title, 30);
	fp = fopen(buf3, "w");
	fp2 = fopen(filename, "r");
	fprintf(fp,
		"%s(%s), ������Ϣʱip: \033[35m%s\033[m\n\n",
		currentuser->userid, currentuser->username, fromhost);
	if (fp2 != 0) {
		while (1) {
			if (fgets(buf3, 1000, fp2) <= 0)
				break;
			fprintf2(fp, buf3);
		}
		fclose(fp2);
	}
	fclose(fp);
	sprintf(buf3,
		"\nϣ���۸�: %s\n��ϵ��ʽ: %s\n(����Ϊ WWW �����г����ĸ���)",
		tosave.price, tosave.contact);
	f_append(filename, buf3);
	post_article("SecondHand", title, filename, currentuser->userid,
		     currentuser->username, fromhost, 0, 0, 0,
		     currentuser->userid, -1);
	unlink(filename);
	tosave.chrono = t;
	sprintf(tosave.fn, "M.%ld.A", t);
	short_stamp(tosave.date, &(tosave.chrono));
	if (!sell)
		tosave.prop = PROP_I_WANT;
	else
		tosave.prop = PROP_I_SELL;
	tosave.reply = 0;
	strcpy(tosave.userid, currentuser->userid);
	sprintf(buf3, "2nd/%s/.ITEM", grp);
	if (insert_record(buf3, &tosave, sizeof (SLOT), 0, 1))
		return -1;	// the newer the upper
	sprintf(buf3, "2nd/" FN_GRP);
	fd = open(buf3, O_RDWR);
	if (fd > 0) {
		while (read(fd, &tosave, sizeof (SLOT)) > 0) {
			if (!strcmp(tosave.fn, grp)) {
				tosave.reply++;
				lseek(fd, -sizeof (SLOT), SEEK_CUR);
				write(fd, &tosave, sizeof (SLOT));
				break;
			}
		}
		close(fd);
	}
	sprintf(buf, "bbssechand?grp=%s", grp);
	redirect(buf);
	return 0;
}

int
replymail(char *grp, char *item)
{
	char fpath[STRLEN];
	char dotitem[STRLEN];
	int fd;
	SLOT tempslot;
	int found = 0;
	char userid[IDLEN + 1];
	char title[30];
	sprintf(dotitem, "2nd/%s/%s", grp, FN_ITEM);
	sprintf(fpath, "2nd/%s/%s", grp, item);
	fd = open(dotitem, O_RDWR);
	if (fd > 0) {
		while (read(fd, &tempslot, sizeof (SLOT)) > 0) {
			if (!strcmp(tempslot.fn, item)) {
				found = 1;
				strcpy(userid, tempslot.userid);
				strcpy(title, tempslot.title);
				tempslot.reply++;
				lseek(fd, -sizeof (SLOT), SEEK_CUR);
				write(fd, &tempslot, sizeof (SLOT));
				break;
			}
		}
		close(fd);
	}
	if (found) {
		printf("<hr>");
		printf("<center>\n");
		printf("%s -- �����Ÿ� [ʹ����: %s]<hr>\n", BBSNAME,
		       currentuser->userid);
		printf("<table border=1><tr><td>\n");
		printf
		    ("<form name=form1 method=post action=bbssndmail?userid=%s>\n",
		     userid);
		printf
		    ("�ż�����: <input type=text name=title size=40 maxlength=100 value='%s'> ",
		     nohtml(title));
		printf("������: &nbsp;%s<br>\n", currentuser->userid);
		printf
		    ("������: &nbsp;&nbsp;<input type=text name=userid value='%s'> ",
		     nohtml(userid));
		printselsignature();
		printf(" ����<input type=checkbox name=backup>\n");
		printf("<br>\n");
		printf
		    ("<textarea  onkeydown='if(event.keyCode==87 && event.ctrlKey) {document.form1.submit(); return false;}'  onkeypress='if(event.keyCode==10) return document.form1.submit()' name=text rows=20 cols=80 wrap=physical>\n\n");
		{
			int lines = 0, i;
			FILE *fp;
			char buf[500];
			printf("�� �� %s �Ĵ������ᵽ: ��\n", userid);
			fp = fopen(fpath, "r");
			if (fp) {
				for (i = 0; i < 1; i++)
					if (fgets(buf, 500, fp) == 0)
						break;
				while (1) {
					if (fgets(buf, 500, fp) == 0)
						break;
					if (!strncmp(buf, ": ��", 4))
						continue;
					if (!strncmp(buf, ": : ", 4))
						continue;
					if (!strncmp(buf, "--\n", 3)
					    || !strncmp(buf, "begin 644 ", 10))
						break;
					if (buf[0] == '\n')
						continue;;
					printf(": %s", nohtml(void1(buf)));
					if (lines++ >= 20) {
						printf(": (��������ʡ��...)\n");
						break;
					}
				}
				fclose(fp);
			}
			printf("</textarea><br><div align=center>\n");
			printf
			    ("<tr><td align=center><input type=submit value=����> ");
			printf("<input type=reset value=���></form>\n");
			printf("</div></table>");
		}
	}
	http_quit();
	return 0;
}

int
bbssechand_main()
{
	char curr_grp[15], curr_item[15];
	int mode;
	int valid_grp;
	html_header(1);
	printf("<script src=" BBSJS "></script>\n");
	//check_msg();
	changemode(M_2NDHAND);
	strncpy(curr_grp, getparm("grp"), 15);
	strncpy(curr_item, getparm("item"), 15);
	mode = atoi(getparm("mode"));
	printf("<body><center>");
	if (strstr(curr_item, ".GRP") || strstr(curr_item, ".ITEM")
	    || strstr(curr_item, "..") || strstr(curr_item, "SYSHome")
	    || strstr(curr_grp, "..") || strstr(curr_grp, "SYSHome"))
		http_fatal("����Ĳ���");
	valid_grp = showheader(curr_grp);
	if (mode && valid_grp) {
		if (!loginok || isguest)
			http_fatal("�Ҵҹ��Ͳ��ܽ��н��ף����ȵ�¼");
		if (mode == 1)
			postnewslot(curr_grp);
		else if (mode == 2)
			savenewslot(curr_grp);
		else if (mode == 3)
			replymail(curr_grp, curr_item);
	} else if (curr_item[0] == '\0' && valid_grp)
		showgroup(curr_grp);
	else if (valid_grp)
		showitem(curr_grp, curr_item);
	else
		showwelcome();
	showfile(MY_BBS_HOME "/wwwtmp/googleanalytics");
	printf("</center></body>");
	http_quit();
	return 0;
}
