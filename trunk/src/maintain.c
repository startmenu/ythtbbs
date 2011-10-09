/*
    Pirate Bulletin Board System
    Copyright (C) 1990, Edward Luke, lush@Athena.EE.MsState.EDU
    Eagles Bulletin Board System
    Copyright (C) 1992, Raymond Rocker, rocker@rock.b11.ingr.com
                        Guy Vega, gtvega@seabass.st.usm.edu
                        Dominic Tynes, dbtynes@seabass.st.usm.edu
    Firebird Bulletin Board System
    Copyright (C) 1996, Hsien-Tsung Chang, Smallpig.bbs@bbs.cs.ccu.edu.tw
                        Peng Piaw Foong, ppfoong@csie.ncu.edu.tw
    
    Copyright (C) 1999, KCN,Zhou Lin, kcn@cic.tsinghua.edu.cn

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 1, or (at your option)
    any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
*/
#include "bbs.h"
#include "bbstelnet.h"

char cexplain[STRLEN];
char lookgrp[30];

static int valid_brdname(char *brd);
static char *chgrp(void);
static int freeclubnum(void);
static int setsecstr(char *buf, int ln);
static void anno_title(char *buf, struct boardheader *bh);
//static void domailclean(struct fileheader *fhdrp);
//static int cleanmail(struct userec *urec);
static void trace_state(int flag, char *name, int size);
static int touchfile(char *filename);
static int scan_register_form(char *regfile);

int
check_systempasswd()
{
	FILE *pass;
	char passbuf[20], prepass[STRLEN];

	clear();
	if ((pass = fopen("etc/.syspasswd", "r")) != NULL) {
		fgets(prepass, STRLEN, pass);
		fclose(pass);
		prepass[strlen(prepass) - 1] = '\0';
		getdata(1, 0, "������ϵͳ����: ", passbuf, 19, NOECHO, YEA);
		if (passbuf[0] == '\0' || passbuf[0] == '\n')
			return NA;
		if (!checkpasswd_des(prepass, passbuf)) {
			move(2, 0);
			prints("�����ϵͳ����...");
			securityreport("ϵͳ�����������...",
				       "ϵͳ�����������...");
			pressanykey();
			return NA;
		}
	}
	return YEA;
}

static void
admin_b_report(char *title, char *content, struct boardheader *fh)
{
	char class;
	int i;

	strcpy(currboard, fh->filename);
	deliverreport(title, content);
	if (!normal_board(fh->filename))
		return;
	strcpy(currboard, "Board");
	if(0 != strcmp(currboard, fh->filename))
		deliverreport(title, content);
	for (i = 0, class = fh->sec1[0];
	     i < 2 && !(i == 1 && class == fh->sec1[0]);
	     i++, class = fh->sec2[0]) {
		if (!class)
			break;
		sprintf(currboard, "%coffice", class);
		if(0 != strcmp(currboard, fh->filename))
			deliverreport(title, content);
	}
}

int
setsystempasswd()
{
	FILE *pass;
	char passbuf[20], prepass[20];

	modify_user_mode(ADMIN);
	if (strcmp(currentuser->userid, "SYSOP"))
		return -1;
	if (!check_systempasswd())
		return -1;
	getdata(2, 0, "�������µ�ϵͳ����: ", passbuf, 19, NOECHO, YEA);
	getdata(3, 0, "ȷ���µ�ϵͳ����: ", prepass, 19, NOECHO, YEA);
	if (strcmp(passbuf, prepass))
		return -1;
	if (passbuf[0] == '\0' || passbuf[0] == '\n')
		return NA;
	if ((pass = fopen("etc/.syspasswd", "w")) == NULL) {
		move(4, 0);
		prints("ϵͳ�����޷��趨....");
		pressanykey();
		return -1;
	}
	fprintf(pass, "%s\n", genpasswd_des(passbuf));
	fclose(pass);
	move(4, 0);
	prints("ϵͳ�����趨���....");
	pressanykey();
	return 0;
}

void
penaltyreport(title, str)
char *title;
char *str;
{
	FILE *se;
	char fname[STRLEN];
	int savemode;

	savemode = uinfo.mode;
	sprintf(fname, "bbstmpfs/tmp/penalty.%s.%05d", currentuser->userid,
		uinfo.pid);
	if ((se = fopen(fname, "w")) != NULL) {
		fprintf(se, "%s", str);
		fclose(se);
		postfile(fname, "Penalty", title, 1);
		unlink(fname);
		modify_user_mode(savemode);
	}
}

void
deliverreport(title, str)
char *title;
char *str;
{
	FILE *se;
	char fname[STRLEN];
	int savemode;

	snprintf(fname, sizeof (fname), "boards/%s", currboard);
	if (!file_isdir(fname))
		return;
	savemode = uinfo.mode;
	sprintf(fname, "bbstmpfs/tmp/deliver.%s.%05d", currentuser->userid,
		uinfo.pid);
	if ((se = fopen(fname, "w")) != NULL) {
		fprintf(se, "%s", str);
		fclose(se);
		postfile(fname, currboard, title, 1);
		unlink(fname);
		modify_user_mode(savemode);
	}
}

void
securityreport(str, content)
char *str;
char *content;
{
	FILE *se;
	char fname[STRLEN];
	int savemode;

	savemode = uinfo.mode;
	//report(str);
	sprintf(fname, "bbstmpfs/tmp/security.%s.%05d", currentuser->userid,
		uinfo.pid);
	if ((se = fopen(fname, "w")) != NULL) {
		fprintf(se, "ϵͳ��ȫ��¼ϵͳ\nԭ��\n%s\n", content);
		fprintf(se, "�����ǲ��ָ�������\n");
		fprintf(se, "������ٻ���: %s", realfromhost);
		fclose(se);
		postfile(fname, "syssecurity", str, 2);
		unlink(fname);
		modify_user_mode(savemode);
	}
}

int
get_grp(seekstr)
char seekstr[STRLEN];
{
	FILE *fp;
	char buf[STRLEN];
	char *namep;

	if ((fp = fopen("0Announce/.Search", "r")) == NULL)
		return 0;
	while (fgets(buf, STRLEN, fp) != NULL) {
		namep = strtok(buf, ": \n\r\t");
		if (namep != NULL && strcasecmp(namep, seekstr) == 0) {
			fclose(fp);
			strtok(NULL, "/");
			namep = strtok(NULL, "/");
			if (strlen(namep) < 30) {
				strcpy(lookgrp, namep);
				return 1;
			} else
				return 0;
		}
	}
	fclose(fp);
	return 0;
}

void
stand_title(title)
char *title;
{
	clear();
	//standout();
	prints(title);
	//standend();
}

int
m_info()
{
	struct userec urec;
	int id;
	struct userec *lookupuser;

	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return -1;
	}
	clear();
	stand_title("�޸�ʹ���ߴ���");
	move(1, 0);
	usercomplete("������ʹ���ߴ���: ", genbuf);
	if (*genbuf == '\0') {
		clear();
		return -1;
	}

	if (!(id = getuser(genbuf, &lookupuser))) {
		move(3, 0);
		prints("�����ʹ���ߴ���");
		clrtoeol();
		pressreturn();
		clear();
		return -1;
	}
	memcpy(&urec, lookupuser, sizeof (urec));
	sprintf(genbuf, "�޸� %s ������", lookupuser->userid);
	securityreport(genbuf, genbuf);

	move(1, 0);
	clrtobot();
	disply_userinfo(&urec, 1);
	uinfo_query(&urec, 1);
	return 0;
}

static int
valid_brdname(brd)
char *brd;
{
	char ch;

	ch = *brd++;
	if (!isalnum(ch) && ch != '_')
		return 0;
	while ((ch = *brd++) != '\0') {
		if (!isalnum(ch) && ch != '_')
			return 0;
	}
	return 1;
}

static char *
chgrp()
{
	int i, ch;
	static char buf[STRLEN];
	char ans[6];

	clear();
	move(2, 0);
	prints("ѡ�񾫻�����Ŀ¼\n\n");
	for (i = 0; i < sectree.nsubsec; i++) {
		prints("\033[1;32m%2d\033[m. %-20s                GROUP_%c\n",
		       i, sectree.subsec[i]->title,
		       sectree.subsec[i]->basestr[0]);
	}
	sprintf(buf, "���������ѡ��(0~%d): ", --i);
	while (1) {
		getdata(i + 6, 0, buf, ans, 4, DOECHO, YEA);
		if (!isdigit(ans[0]))
			continue;
		ch = atoi(ans);
		if (ch < 0 || ch > i || ans[0] == '\r' || ans[0] == '\0')
			continue;
		else
			break;
	}
	strcpy(cexplain, sectree.subsec[ch]->title);
	snprintf(buf, sizeof (buf), "GROUP_%c", sectree.subsec[ch]->basestr[0]);
	return buf;
}

static int
freeclubnum()
{
	FILE *fp;
	int club[CLUB_SIZE];
	int i;
	struct boardheader rec;
	bzero(club, sizeof (club));
	if ((fp = fopen(BOARDS, "r")) == NULL) {
		return -1;
	}
	while (!feof(fp)) {
		fread(&rec, sizeof (struct boardheader), 1, fp);
		if (rec.clubnum != 0)
			club[rec.clubnum / (8 * sizeof (int))] |=
			    (1 << (rec.clubnum % (8 * sizeof (int))));
	}
	fclose(fp);
	for (i = 1; i < 8 * sizeof (int) * CLUB_SIZE; i++)
		if ((~club[i / (8 * sizeof (int))]) &
		    (1 << (i % (8 * sizeof (int))))) {
			return i;
		}
	return -1;
}

static int
setsecstr(char *buf, int ln)
{
	const struct sectree *sec;
	int i = 0, ch, len, choose = 0;
	sec = getsectree(buf);
	move(ln, 0);
	clrtobot();
	while (1) {
		prints("[��ǰ����ѡ��]\n");
		move(ln, 30);
		prints("==>   ");
		if (!sec->parent)
			prints("BBS ϵͳ������\n\n");
		else
			prints("\033[32m[%s]\033[0m %s\033[m\n\n",
		     sec->basestr, sec->title);
		if (sec->parent) {
			prints("[\033[4;32m#\033[0m] ���ϼ�����\n");
			prints("[\033[4;32m%%\033[0m] �ͷ�������\n");
		}
		prints
		    ("[\033[4;32m*\033[0m] ����ԭ���趨(���ûس�ѡ������)\n");
		len = strlen(sec->basestr);
		for (i = 0; i < sec->nsubsec; i++) {
			if (i && !(i % 3))
				prints("\n");
			ch = sec->subsec[i]->basestr[len];
			prints
			    ("[\033[4;32m%c\033[0m] \033[0m%s ",
			     ch, sec->subsec[i]->title);
		}
		if (i && (i + 1) % 3)
			prints("\n");
		prints("\n�밴�����ڵ���ĸѡ��[���߰�Q�˳�]");
		while (1) {
			ch = igetkey();
			if (ch == '\n' || ch == '\r')
				ch = '*';
			if (sec->parent == NULL && (ch == '#' || ch == '%'))
				continue;
			for (i = 0; i < sec->nsubsec; i++) {
				if (sec->subsec[i]->basestr[len] == ch) {
					choose = i;
					break;
				}
			}
			if (ch != '#' && ch != '*' && ch != '%' && ch !='Q' && ch !='q'
			    && i == sec->nsubsec)
				continue;
			break;
		}
		move(ln, 0);
		clrtobot();
		switch (ch) {
		case '#':
			sec = sec->parent;
			break;
		case '%':
			strcpy(buf, sec->basestr);
			return 0;
		case '*':
			sec = getsectree(buf);
			strcpy(buf, sec->basestr);
			return 0;
		case 'Q':
		case 'q':
			return -1;
		default:
			sec = sec->subsec[choose];
		}
	}
}

unsigned short int connect_board_to_discuzx(char* title)
{
	MYSQL *mysql = NULL;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char boardcnameutf8[24];
	char sqlbuf[256];
	unsigned short int discuzboardid;

	if(code_convert("gbk","utf8", title, strlen(title), boardcnameutf8, 24 )==-1)  // 24 is the length of title
	{
		prints("����ת��Ϊutf8����");
		return 0;
	}
	mysql = mysql_init(mysql);
    mysql = mysql_real_connect(mysql,"localhost",SQLUSER, SQLPASSWD, SQLDB,0, NULL,0);
	if (!mysql) {
		prints("�޷������ݿ�\n");
		return 0;
	}
	mysql_query(mysql, "set names utf8");

	sprintf(sqlbuf,"select fid from %sforum_forum where name = \'%s\'; " , DISCUZ_DATABASE_PRE, boardcnameutf8 );
	mysql_query(mysql, sqlbuf);
	res = mysql_store_result(mysql);

	if (mysql_num_rows(res)!=0)
	{
		row = mysql_fetch_row(res);
		discuzboardid=(unsigned short int)atoi(row[0]);
		prints("discuzx��ͬ�����İ����ҵ���fid����%d", discuzboardid);
	}
	else
	{
		discuzboardid = 0;
		prints("��Ϊ��discuzx��ͨ��ȴδ���ҵ�ͬ����������");
	}
	return discuzboardid;

}

int
m_newbrd()
{
	struct boardheader newboard;
	char vbuf[100], title[STRLEN];
	char info[256 * 20], *group;
	int ln = 2;	//Ŀǰ���ڵ�����
	int bid, now, mode, savemode;
	FILE *fp;

	if (!USERPERM(currentuser, PERM_BLEVELS) && 
			!USERPERM(currentuser, PERM_SPECIAL4))
		return -1;
	mode = USERPERM(currentuser, PERM_BLEVELS)?0:1;

	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return -1;
	}
	clear();
	stand_title("������������ѡ��");
	memset(&newboard, 0, sizeof (newboard));
	
	while (1) {
		move(ln, 0);
		prints("\033[1;33m������������Ӣ������\033[m");
		getdata(ln, 30, NULL, newboard.filename, 18, DOECHO, YEA);
		if (newboard.filename[0] != 0) {
			struct boardheader dh;
			if (new_search_record
			    (BOARDS, &dh, sizeof (dh), (void *) cmpbnames,
			     newboard.filename)) {
				prints("\n�Ѵ���ͬ����������");
				pressanykey();
				return -1;
			}
		} else
			return -1;
		if (valid_brdname(newboard.filename))
			break;
		prints("\n���������ƺ��зǷ��ַ���");
	}
	sprintf(info, "%-14s\033[1;32m%s\033[m\n", "Ӣ������", 
			newboard.filename);
	move(2, 0);
	clrtobot();
	prints("%s", info);
	
	move(++ln, 0);
	prints("\033[1;33m��������������������\033[m");
	getdata(ln, 30, NULL, newboard.title,
		sizeof (newboard.title), DOECHO, YEA);
	if (newboard.title[0] == '\0')
		return -1;
	strcpy(vbuf, "vote/");
	strcat(vbuf, newboard.filename);
	setbpath(genbuf, newboard.filename);
	if (getbnum(newboard.filename) > 0 || mkdir(genbuf, 0777) == -1
	    || mkdir(vbuf, 0777) == -1) {
		prints("\n��������������ơ�\n����ϵϵͳά�����"
			" %s �� ftphome/root/boards/%s �Ƿ��Ѵ��ڡ�\n", 
			vbuf, newboard.filename);
		pressreturn();
		rmdir(vbuf);
		rmdir(genbuf);
		clear();
		return -1;
	}
	sprintf(genbuf, "ftphome/root/boards/%s", newboard.filename);
	mkdir(genbuf, 0777);
	sprintf(info, "%s%-14s\033[1;32m%s\033[m\n", 
			info, "��������", newboard.title);
	
	move(2, 0);
	clrtobot();
	prints("%s", info);
	move(++ln, 0);
	prints("\033[1;33m��ѡ����������������\033[m");
	while (1) {
		genbuf[0] = 0;
		if (setsecstr(genbuf, 10) == -1) {
			rmdir(vbuf);
			setbpath(genbuf, newboard.filename);
			rmdir(genbuf);
			clear();
			return -1;
		}
		if (mode && !issecm(genbuf, currentuser->userid)) {
			//��������ӷ���������
			move(ln, 30);
			prints("\033[1;31m��ѡ�������������ӷ�������\033[m");
			continue;
		}
		if (genbuf[0] != '\0')
			break;
	}
	newboard.secnumber1 = genbuf[0];
	strsncpy(newboard.sec1, genbuf, sizeof (newboard.sec1));
	sprintf(info, "%s%-14s\033[1;32m%-16s\033[m", 
			info, "�� �� ��", genbuf);

	move(2, 0);
	clrtobot();
	prints("%s", info);
	move(++ln, 0);
	prints("\033[1;33m��ѡ����������ӳ�����\033[m");
	genbuf[0] = 0;
	if (!mode)
		if (setsecstr(genbuf, 10) == -1) {
			rmdir(vbuf);
			setbpath(genbuf, newboard.filename);
			rmdir(genbuf);
			clear();
			return -1;
		}
	genbuf[0] = (genbuf[0] == newboard.secnumber1)?0:genbuf[0];
		//��������һ���������ӷ�������
	newboard.secnumber2 = genbuf[0];
	strsncpy(newboard.sec2, genbuf, sizeof (newboard.sec2));
	sprintf(info, "%s%-14s\033[1;32m%s\033[m\n", 
			info, "ӳ�����", genbuf[0] == 0 ? "(��)": genbuf);

	move(2, 0);
	clrtobot();
	prints("%s", info);
	move(++ln, 0);
	prints("\033[1;33m��������������������\033[m(4�ַ�)");
	ansimore2("etc/boardref", NA, ln + 4, 11);
	while (1) {
		getdata(ln, 30, "", newboard.type,
			sizeof (newboard.type), DOECHO, YEA);
		if (strlen(newboard.type) == 4)
			break;
	}
	move(ln, 0);
	sprintf(info, "%s%-14s\033[1;32m%-16s\033[m", 
			info, "��������", newboard.type);
	clrtobot();

	move(2, 0);
	clrtobot();
	prints("%s", info);
	move(++ln, 0);
	prints("\033[1;33m���������վ�������\033[m(Ĭ��3000)");
	if (!mode)
		getdata(ln, 30, NULL, genbuf, 5, DOECHO, YEA);
	newboard.limitchar = atoi(genbuf) / 100;
	move(ln, 0);
	sprintf(info, "%s%-14s\033[1;32m%d\033[m\n", info, "��������",  
			(newboard.limitchar == 0) ? 
			3000 : (newboard.limitchar * 100));

	move(2, 0);
	clrtobot();
	prints("%s", info);

	move(++ln, 0);
	prints("\033[1;33m��ѡ�������������\033[m");
	move(ln + 2, 4);
	prints("[\033[0;4;32m0\033[m] ��ͨ����(Ĭ��)  "
		"[\033[0;4;32m1\033[m] o���ֲ�  "
		"[\033[0;4;32m2\033[m] c���ֲ�  ");
	if (!mode) {
		prints("\n    [\033[0;4;32m3\033[m] C���ֲ�  "
			"[\033[0;4;32m4\033[m] �����ư���  "
			"[\033[0;4;32m5\033[m] д���ư���  "
			"[\033[0;4;32m6\033[m] ��������");
	}
	getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
	switch(genbuf[0]) {
		case '1':
			sprintf(info, "%s%-14s\033[1;32m%-16s\033[m", 
					info, "��������", "o���ֲ�");
			newboard.flag &= ~ANONY_FLAG;
			newboard.level = 0;
			newboard.flag |= CLUB_FLAG;
			newboard.flag &= ~CLOSECLUB_FLAG;
			newboard.clubnum = 0;
			break;
		case '2':
			sprintf(info, "%s%-14s\033[1;32m%-16s\033[m", 
					info, "��������", "c���ֲ�");
			newboard.flag &= ~ANONY_FLAG;
			newboard.level = 0;
			newboard.flag |= CLOSECLUB_FLAG;
			newboard.flag &= ~CLUBLEVEL_FLAG;
			newboard.clubnum = 0;
			break;
		case '3':
			if (mode) {
				sprintf(info, "%s%-14s\033[1;32m%-16s\033[m", 
					info, "��������", "��ͨ����");
				break;
			}
			sprintf(info, "%s%-14s\033[1;32m%-16s\033[m", 
					info, "��������", "C���ֲ�");
			newboard.flag &= ~ANONY_FLAG;
			newboard.level = 0;
                	newboard.flag |= CLUBLEVEL_FLAG;
                	newboard.flag |= CLOSECLUB_FLAG;
			if ((newboard.clubnum = freeclubnum()) == -1) {
				move(ln + 4, 0);
        	            	prints("\033[1;31mû�пյľ��ֲ�λ����"
						"\033[m\n");
				rmdir(vbuf);
				setbpath(genbuf, newboard.filename);
				rmdir(genbuf);
        	            	pressreturn();
			return -1;
			}
			break;
		case '4':
			if (mode) {
				sprintf(info, "%s%-14s\033[1;32m%-16s\033[m", 
					info, "��������", "��ͨ����");
				break;
			}
			sprintf(info, "%s%-14s\033[1;32m%-16s\033[m", 
					info, "��������", "�����ư���");
			newboard.level = 0;
			clear();
			prints("�趨 %s ��������ȡ����ѡ��\n", 
					newboard.filename);
			newboard.level = setperms(newboard.level, 
					"Ȩ��", NUMPERMS, showperminfo, 0);
			clear();
			stand_title("������������ѡ��");
			break;
		case '5':
			if (mode) {
				sprintf(info, "%s%-14s\033[1;32m%-16s\033[m", 
					info, "��������", "��ͨ����");
				break;
			}
			sprintf(info, "%s%-14s\033[1;32m%-16s\033[m", 
					info, "��������", "д���ư���");
			clear();
			prints("�趨 %s ������дȡ����ѡ��\n", 
			newboard.filename);
			newboard.level = setperms(newboard.level, 
					"Ȩ��", NUMPERMS, showperminfo, 0);
			newboard.level &= PERM_POSTMASK;
			clear();
			stand_title("������������ѡ��");
			break;
		case '6':
			if (mode) {
				sprintf(info, "%s%-14s\033[1;32m%-16s\033[m", 
					info, "��������", "��ͨ����");
				break;
			}
			sprintf(info, "%s%-14s\033[1;32m%-16s\033[m", 
					info, "��������", "��������");
			newboard.flag |= ANONY_FLAG;
			newboard.level = 0;
			break;
		case '0':
		default:
			sprintf(info, "%s%-14s\033[1;32m%-16s\033[m", 
					info, "��������", "��ͨ����");
			newboard.level = 0;
			newboard.flag = 0;
	}
	if (newboard.level & PERM_NOZAP)
		newboard.flag |= NOZAP_FLAG;
	move(2, 0);
	clrtobot();
	prints("%s", info);
	if (newboard.level) {
		permtostr(newboard.level, genbuf);
		prints("\n%-14s\033[1;32m%s\033[m", "��дȨ��", genbuf);
		ln++;
	}

	move(++ln, 0);
	prints("\033[1;33m��ѡ��ת������\033[m");
	move(ln + 2, 4);
	prints("[\033[0;4;32m0\033[m] ��ת��  "
			"[\033[0;4;32m1\033[m] ��Ե�ת��  "
			"[\033[0;4;32m2\033[m] cn.bbsת��"
			"[\033[0;4;32m3\033[m] DiscuzX���ݿ�ͬ�� ");
	getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
	switch(genbuf[0]) {
		case '1':
			sprintf(info, "%s%-16s\033[1;32m%s\033[m\n", 
					info, "ת������", "��Ե�ת��");
			newboard.flag2 |= NJUINN_FLAG;
			break;
		case '2':
			sprintf(info, "%s%-14s\033[1;32m%s\033[m\n", 
					info, "ת������", "cn.bbsת��");
			newboard.flag |= INNBBSD_FLAG;
			break;
		case '3':
			newboard.flag2 |= DISCUZWEB_FLAG;
			// find the discuzx board with the same cname
			newboard.discuzxfid = connect_board_to_discuzx(newboard.title);
			sprintf(info, "%s%-14s\033[1;32m%s%d\033[m\n",
					info, "ת������", "DX���", newboard.discuzxfid);
			break;
		case '0':
		default:
			sprintf(info, "%s%-14s\033[1;32m%s\033[m\n", 
					info, "ת������", "��ת��");
	}

	if (newboard.level) {
		permtostr(newboard.level, genbuf);
		sprintf(info, "%s%-14s\033[1;32m%s\033[m\n", 
				info, "��дȨ��", genbuf);
	}

	move(2, 0);
	clrtobot();
	prints("%s", info);

	if (!mode) {
		move(++ln, 0);
		prints("\033[1;33m��ѡ���Ƿ�ʱ����\033[m (y/N)");
		getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
		if (genbuf[0] == 'y' || genbuf[0] == 'Y') {
			sprintf(info, "%s%-14s\033[1;32m%-16s\033[m", 
					info, "��ʱ����", "��");
			newboard.flag2 |= WATCH_FLAG;
		} else {
			sprintf(info, "%s%-14s\033[1;32m%-16s\033[m", 
					info, "��ʱ����", "��");
			newboard.flag2 &= ~WATCH_FLAG;
		}

		move(2, 0);
		clrtobot();
		prints("%s", info);
		move(++ln, 0);
		prints("\033[1;33m��ѡ���Ƿ�Ҫ���¼��\033[m (y/N)");
		getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
		if (genbuf[0] == 'y' || genbuf[0] == 'Y') {
			sprintf(info, "%s%-14s\033[1;32m%s\033[m\n", 
					info, "�������", "��");
			newboard.flag |= IS1984_FLAG;
		} else {
			sprintf(info, "%s%-14s\033[1;32m%s\033[m\n", 
					info, "�������", "��");
			newboard.flag &= ~IS1984_FLAG;
		}

		move(2, 0);
		clrtobot();
		prints("%s", info);
		move(++ln, 0);
		prints("\033[1;33m��ѡ���Ƿ��������԰���\033[m (y/N)");
		getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
		if (genbuf[0] == 'y' || genbuf[0] == 'Y') {
			sprintf(info, "%s%-14s\033[1;32m%s\033[m\n", 
					info, "���ΰ���", "��");
			newboard.flag |= POLITICAL_FLAG;
		} else {
			sprintf(info, "%s%-14s\033[1;32m%s\033[m\n", 
					info, "���ΰ���", "��");
			newboard.flag &= ~POLITICAL_FLAG;
		}
	} else
		sprintf(info, "%s%-14s\033[1;32m%-16s\033[m%-14s\033[1;32m"
				"��\033[m\n%-14s\033[1;32m��\033[m", 
				info, "��ʱ����", "��", 
				"�������", "���ΰ���");
		
	
	now = time(NULL);
	newboard.board_mtime = now;
	newboard.board_ctime = now;

	move(2, 0);
	clrtobot();
	prints("%s", info);

	move(++ln, 0);
	prints("\033[1;33mȷ��������������\033[m (y/n)");
	while (1) {
		getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
		if (genbuf[0] == 'y' || genbuf[0] == 'Y')
			break;
		if (genbuf[0] == 'n' || genbuf[0] == 'N') {
			setbpath(genbuf, newboard.filename);
			rmdir(vbuf);
			rmdir(genbuf);
			sprintf(genbuf, "ftphome/root/boards/%s", 
					newboard.filename);
			rmdir(genbuf);
			move(ln + 2, 0);
			prints("����������������...\n");
			pressreturn();
			return -1;
		}
	}

	reload_boards();
	if ((bid = getbnum("")) > 0) {
		substitute_record(BOARDS, &newboard, sizeof (newboard), bid);
	} else if (append_record(BOARDS, &newboard, sizeof (newboard)) == -1) {
		rmdir(vbuf);
		sprintf(genbuf, "ftphome/root/boards/%s", newboard.filename);
		rmdir(genbuf);
		pressreturn();
		clear();
		return -1;
	}

        update_postboards();

	group = chgrp();
	sprintf(vbuf, "%-38.38s", newboard.title);
	//sprintf(group, "GROUP_%c", newboard.sec1[0]);
	//sprintf(vbuf, "%-38.38s", newboard.title);
	if (group != NULL) {
		if (add_grp(group, cexplain, newboard.filename, vbuf) == -1)
			prints("\n����������ʧ��...\n");
		else
			prints("�Ѿ����뾫����...\n");
	}

	prints("\n������������\n");
	
	sprintf(title, "�����°棺%s", newboard.filename);
	securityreport(title, info);
	pressreturn();
	clear();
	
	sprintf(genbuf, "boards/%s", newboard.filename);
	if (!file_isdir(genbuf))
		return 0;
	savemode = uinfo.mode;
	sprintf(genbuf, "bbstmpfs/tmp/deliver.%s.%05d", currentuser->userid, 
			uinfo.pid);
	if (strstr(info, "��ʱ����"))
		*(strstr(info, "��ʱ����")) = 0;
	if ((fp = fopen(genbuf, "w")) != NULL) {
		fprintf(fp, "\n\n    \033[1;32m�Ⱦ����ڱ�վ %c ��"
				"���� %s �棬�ش��������˼������"
				"�ѱ�ʾף�ء�\033[m\n\n\n\n"
				"����������ϸ��Ϣ\n\n%s\n\n", 
				newboard.sec1[0], newboard.filename, info);
		fclose(fp);
		postfile(genbuf, newboard.filename, title, 1);
		unlink(genbuf);
		modify_user_mode(savemode);
	}
	return 0;
}

static void
anno_title(buf, bh)
char *buf;
struct boardheader *bh;
{
	char bm[IDLEN * 4 + 4];	//���ĸ�����
	sprintf(buf, "%-38.38s", bh->title);
	if (bh->bm[0][0] == 0)
		return;
	else {
		//fixme, �����sizeof�������κ�����
		strncat(buf, "(BM: ", STRLEN - sizeof (buf) - 2);
		bm2str(bm, bh);
		strncat(buf, bm, STRLEN - sizeof (buf) - 2);
	}
	strcat(buf, ")");
	return;
}

int
m_editbrd()
{
	char bname[STRLEN], oldtitle[STRLEN], vbuf[256], *group;
	char oldpath[STRLEN], newpath[STRLEN], tmp_grp[30], *ptr;
	int pos, mode, ln;
	struct boardheader fh, newfh;
	char info[256 * 10], report[256 * 21];
	char tempbuf[20];
	
	if (!USERPERM(currentuser, PERM_BLEVELS) && 
			!USERPERM(currentuser, PERM_SPECIAL4))
		return -1;
	mode = USERPERM(currentuser, PERM_BLEVELS) ? 0 : 1;

	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return -1;
	}
	clear();
	stand_title("�޸���������Ѷ");
	move(2, 0);
	make_blist_full();
	namecomplete("��������������: ", bname);
	FreeNameList();
	if (*bname == '\0') {
		move(4, 0);
		prints("\033[1;31m���������������\033[m");
		pressreturn();
		clear();
		return -1;
	}
	pos =
	    new_search_record(BOARDS, &fh, sizeof (fh), (void *) cmpbnames,
			      bname);
	if (!pos) {
		move(4, 4);
		prints("\033[1;31m��Ȼû���ҵ�-__-||\033[m");
		pressreturn();
		clear();
		return -1;
	}
	if (mode && !issecm(fh.sec1, currentuser->userid)) {
		move(4, 4);
		prints("\033[1;31m�������Ĺ������-__-||\033[m");
		pressreturn();
		clear();
		return -1;
	}
	sprintf(info, "%-14s\033[1;32m%-32s\033[m\n"
			"%-14s\033[1;32m%-32s\033[m\n"
			"%-14s\033[1;32m%-16s\033[m"
			"%-14s\033[1;32m%-16s\033[m\n"
			"%-14s\033[1;32m%-16s\033[m"
			"%-14s\033[1;32m%-16d\033[m\n",
			"Ӣ������", fh.filename, 
			"��������", fh.title,
			"�� �� ��", fh.sec1,
			"ӳ�����", fh.sec2[0] ? fh.sec2 : "(��)",
			"��������", fh.type,
			"��������", fh.limitchar ? fh.limitchar * 100 : 3000);
	if (fh.flag & ANONY_FLAG)
		strcpy(genbuf, "��������");
	else if (fh.flag & CLUBLEVEL_FLAG && !(fh.flag & CLUBLEVEL_FLAG))
		strcpy(genbuf, "o���ֲ�");
	else if (!(fh.flag & CLUBLEVEL_FLAG) && fh.flag & CLOSECLUB_FLAG)
		strcpy(genbuf, "c���ֲ�");
	else if (fh.flag & CLUBLEVEL_FLAG && fh.flag & CLOSECLUB_FLAG )
		strcpy(genbuf, "C���ֲ�");
	else if (fh.level) {
		if (fh.level & PERM_POSTMASK)
			strcpy(genbuf, "д���ư���");
		else
			strcpy(genbuf, "�����ư���");
	} else
		strcpy(genbuf, "��ͨ����");
	sprintf(info, "%s%-14s\033[1;32m%-16s\033[m", 
			info, "��������", genbuf);
	if (fh.flag & INNBBSD_FLAG)
		strcpy(genbuf, "cn.bbsת��");
	else if (fh.flag2 & NJUINN_FLAG)
		strcpy(genbuf, "��Ե�ת��");
	else if (fh.flag2 & DISCUZWEB_FLAG)
	{
		// show discuzx board id
//		strcpy(genbuf, "DX WEB��ͨ");
		sprintf(genbuf, "DX��ͨ���%d", fh.discuzxfid);
	}
	else
		strcpy(genbuf, "��ת��");
	sprintf(info, "%s%-14s\033[1;32m%-16s\033[m\n", 
			info, "ת������", genbuf);
	permtostr(fh.level, genbuf);
	sprintf(info, "%s%-14s\033[1;32m%-36s\033[m\n", 
			info, "��дȨ��", genbuf);
	sprintf(info, "%s%-14s\033[1;32m%-16s\033[m"
			"%-14s\033[1;32m%-16s\033[m\n"
			"%-14s\033[1;32m%-16s\033[m\n", 
			info,
			"��ʱ����", fh.flag2 & WATCH_FLAG ? "��" : "��",
			"�������", fh.flag & IS1984_FLAG ? "��" : "��",
			"���ΰ���", fh.flag & POLITICAL_FLAG ? "��" : "��");

	sprintf(report, "�޸�ǰ��\n\n%s\n", info);

	move(2, 0);
	clrtobot();
	prints("%s", info);
	
	ln = 13;
	move (ln, 0);
	prints("\033[1;33mȷ��Ҫ�޸İ�����Ϣ��\033[m (y/n)");
	getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
	while (1) {
		if (genbuf[0] == 'N' || genbuf[0] == 'n') {
			move(ln + 4, 4);
			prints("\033[1;31mԭ�������ת����...\033[m");
			pressreturn();
			return 1;
		} else if (genbuf[0] == 'Y' || genbuf[0] == 'y')
			break;
		getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
		continue;
	}

	memcpy(&newfh, &fh, sizeof(struct boardheader));
	
	move(ln - 2, 0);
	prints("[\033[1;32m��ʾ\033[m] ֱ�ӻس���ʾ���޸ĵ�ǰ��");
	move(ln, 0);
	clrtoeol();
	prints("\033[1;33m���������Ӣ������\033[m");
	getdata(ln, 30, NULL, genbuf, 18, DOECHO, YEA);
	if (*genbuf) {
		struct boardheader dh;

		while (1) {

			if (new_search_record(BOARDS, &dh, sizeof(dh), 
						(void *) cmpbnames, genbuf)) {
				move(ln + 4, 4);
				prints("\033[1;31m�Ѵ���ͬ��������\033[m");
				getdata(ln, 30, NULL, genbuf, 
						18, DOECHO , YEA);
				continue;
			}
			if (!valid_brdname(genbuf)) {
				move(ln + 4, 4);
				prints("\033[1;31m���������ƺ��Ƿ��ַ�\033[m");
				getdata(ln, 30, NULL, genbuf, 
						18, DOECHO, YEA);
				continue;
			}
			break;
		}

		strsncpy(newfh.filename, genbuf, sizeof(newfh.filename));
		if (strcmp(newfh.filename, fh.filename)) {
			ptr = info + 19;
			*ptr = '3';
			sprintf(genbuf, "%-32s", newfh.filename);
			strncpy(ptr + 2, genbuf, 31);
		}
	}

	move(2, 0);
	clrtobot();
	prints("%s", info);
	move(ln - 2, 0);
	prints("[\033[1;32m��ʾ\033[m] ֱ�ӻس���ʾ���޸ĵ�ǰ��");
	move(ln, 0);
	clrtoeol();
	prints("\033[1;33m�����������������\033[m");
	getdata(ln, 30, NULL, genbuf, 24, DOECHO, YEA);
	if (*genbuf) {
		strsncpy(newfh.title, genbuf, sizeof(newfh.title));
		if (strcmp(newfh.title, fh.title)) {
			ptr = strstr(info, "��������");
			ptr += 19;
			*ptr = '3';
			sprintf(genbuf, "%-32s", newfh.title);
			strncpy(ptr + 2, genbuf, 31);
		}
	}

	move(2, 0);
	clrtobot();
	prints("%s", info);
	move(ln - 2, 0);
	prints("[\033[1;32m��ʾ\033[m] ֱ�ӻس���ʾ���޸ĵ�ǰ��");
	move(ln, 0);
	clrtoeol();
	prints("\033[1;33m��ѡ�����������\033[m");
	strncpy(genbuf, newfh.sec1, sizeof(genbuf));
	if (setsecstr(genbuf, 12) == -1) {
		move(ln, 0);
		clrtobot();
		move(ln + 4, 4);
		prints("\033[1;32m�����е�������...\033[m");
		pressreturn();
		return 0;
	}
	if (*genbuf) {
		strsncpy(newfh.sec1, genbuf, sizeof(newfh. sec1));
		if (strcmp(newfh.sec1, fh.sec1)) {
			ptr = strstr(info, "�� �� ��");
			ptr += 19;
			*ptr = '3';
			sprintf(genbuf, "%-16s", newfh.sec1);
			strncpy(ptr + 2, genbuf, 15);
			newfh.secnumber1 = newfh.sec1[0];
		}
	}
	
	move(2, 0);
	clrtobot();
	prints("%s", info);
	move(ln - 2, 0);
	prints("[\033[1;32m��ʾ\033[m] ֱ�ӻس���ʾ���޸ĵ�ǰ��"
			"(��ӳ���������������ͬʱ��ȡ��ӳ��)");
	move(ln, 0);
	clrtoeol();
	prints("\033[1;33m��ѡ�����ӳ�����\033[m");
	strncpy(genbuf, newfh.sec2, sizeof(genbuf));
	if (setsecstr(genbuf, 12) == -1) {
		move(ln, 0);
		clrtobot();
		move(ln + 4, 4);
		prints("\033[1;31m�����е�������...\033[m");
		pressreturn();
		return 0;
	}
	if (*genbuf) {
		strsncpy(newfh.sec2, genbuf, sizeof(newfh.sec2));
		if (!strcmp(newfh.sec2, newfh.sec1)) {
			*newfh.sec2 = 0;
			newfh.secnumber2 = 0;
			ptr = strstr(info, "ӳ�����");
			ptr += 19;
			*ptr = '3';
			sprintf(genbuf, "%-16s", "(��)");
			strncpy(ptr + 2, genbuf, 15);
		} else if (strcmp(newfh.sec2, fh.sec2)) {
			ptr = strstr(info, "ӳ�����");
			ptr += 19;
			*ptr = '3';
			sprintf(genbuf, "%-16s", newfh.sec2);
			strncpy(ptr + 2, genbuf, 15);
			newfh.secnumber2 = newfh.sec2[0];
		}
	}

	move(2, 0);
	clrtobot();
	prints("%s", info);
	move(ln - 2, 0);
	prints("[\033[1;32m��ʾ\033[m] ֱ�ӻس���ʾ���޸ĵ�ǰ��");
	move(ln, 0);
	clrtoeol();
	prints("\033[1;33m��������������������\033[m (4�ֽ�)");
	ansimore2("/etc/boardref", NA, ln + 2, 7);
	getdata(ln, 30, NULL, genbuf, 5, DOECHO, YEA);
	if (*genbuf) {
		strsncpy(newfh.type, genbuf, sizeof(newfh.type));
		if (strcmp(newfh.type, fh.type)) {
			ptr = strstr(info, "��������");
			ptr += 19;
			*ptr = '3';
			sprintf(genbuf, "%-16s", newfh.type);
			strncpy(ptr + 2, genbuf, 15);
		}
	}

	move(2, 0);
	clrtobot();
	prints("%s", info);
	move(ln - 2, 0);
	prints("[\033[1;32m��ʾ\033[m] ֱ�ӻس���ʾ���޸ĵ�ǰ��");
	move(ln, 0);
	clrtoeol();
	prints("\033[1;33m���������վ���������\033[m");
	getdata(ln, 30, NULL, genbuf, 5, DOECHO, YEA);
	if (*genbuf) {
		newfh.limitchar = atoi(genbuf) / 100;
		if (newfh.limitchar != fh.limitchar) {
			ptr = strstr(info, "��������");
			ptr += 19;
			*ptr = '3';
			sprintf(genbuf, "%-16d", newfh.limitchar * 100);
			strncpy(ptr + 2, genbuf, 15);
		}
	}

	move(2, 0);
	clrtobot();
	prints("%s", info);
	move(ln - 2, 0);
	prints("[\033[1;32m��ʾ\033[m] ֱ�ӻس���ʾ���޸ĵ�ǰ��");
	move(ln, 0);
	clrtoeol();
	prints("\033[1;33m��ѡ���������\033[m");
	move(ln + 2 , 4);
	prints("[\033[0;4;32m0\033[m] ��ͨ����(Ĭ��)  "
		"[\033[0;4;32m1\033[m] o���ֲ�  "
		"[\033[0;4;32m2\033[m] c���ֲ�  ");
	if (!mode) {
		prints("\n    [\033[0;4;32m3\033[m] C���ֲ�  "
			"[\033[0;4;32m4\033[m] �����ư���  "
			"[\033[0;4;32m5\033[m] д���ư���  "
			"[\033[0;4;32m6\033[m] ��������");
	}
	getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
	if (*genbuf) {
		switch(genbuf[0]) {
			case '0': 
				newfh.flag &= ~ANONY_FLAG;
				newfh.flag &= ~CLUB_FLAG;
				newfh.level = 0;
				sprintf(genbuf, "%-16s", "��ͨ����");
				break;
			case '1':
				newfh.flag &= ~ANONY_FLAG;
				newfh.flag |= CLUB_FLAG;
				newfh.flag &= ~CLOSECLUB_FLAG;
				newfh.level = 0;
				newfh.clubnum = 0;
				sprintf(genbuf, "%-16s", "o���ֲ�");
				break;
			case '2':
				newfh.flag &= ~ANONY_FLAG;
				newfh.flag |= CLOSECLUB_FLAG;
				newfh.flag &= ~CLUBLEVEL_FLAG;
				newfh.level = 0;
				newfh.clubnum = 0;
				sprintf(genbuf, "%-16s", "c���ֲ�");
				break;
			case '3':
				if (mode)
					break;
				if (fh.flag & CLUBLEVEL_FLAG && 
						fh.flag & CLOSECLUB_FLAG)
					break;
				if ((newfh.clubnum = freeclubnum()) == -1) {
					move(ln + 4, 4);
					prints("\033[1;31mû�пյ�"
						"���ֲ�λ����\033[m");
					newfh.clubnum = fh.clubnum;
					break;
				}
				newfh.flag &= ~ANONY_FLAG;
				newfh.flag |= CLUBLEVEL_FLAG;
				newfh.flag |= CLOSECLUB_FLAG;
				newfh.level = 0;
				sprintf(genbuf, "%-16s", "C���ֲ�");
				break;
			case '4':
				if (mode)
					break;
				newfh.flag &= ~ANONY_FLAG;
				newfh.flag &= ~CLUB_FLAG;
				move(2, 0);
				clrtobot();
				newfh.level = setperms(newfh.level, 
						"Ȩ��", NUMPERMS, 
						showperminfo, 0);
				sprintf(genbuf, "%-16s", "�����ư���");
				break;
			case '5':
				if (mode)
					break;
				newfh.flag &= ~ANONY_FLAG;
				newfh.flag &= ~CLUB_FLAG;
				move(2, 0);
				clrtobot();
				newfh.level = setperms(newfh.level, 
						"Ȩ��", NUMPERMS, 
						showperminfo, 0);
				newfh.level |= PERM_POSTMASK;
				sprintf(genbuf, "%-16s", "д���ư���");
				break;
			case '6':
				if (mode)
					break;
				newfh.flag |= ANONY_FLAG;
				newfh.flag &= ~CLUB_FLAG;
				newfh.level = 0;
				sprintf(genbuf, "%-16s", "��������");
				break;
		}
		if (newfh.flag != fh.flag) {
			ptr = strstr(info, "��������");
			ptr += 19;
			*ptr = '3';
			strncpy(ptr + 2, genbuf, 15);
		}
		if (newfh.level != fh.level) {
			ptr = strstr(info, "��дȨ��");
			ptr += 19;
			*ptr = '3';
			permtostr(newfh.level, genbuf);
			sprintf(genbuf, "%-35s", genbuf);
			strncpy(ptr + 2, genbuf, 35);
		}
		if (fh.clubnum != 0 && newfh.clubnum == 0) {
			if (clubmember_num(fh.filename) != 0) {
				move(ln + 2, 4);
				clrtobot();
				prints("\033[1;31m����վ��ֲ�"
						"��Ա��...\033[m");
				pressreturn();
				clear();
				return 0;
			}
		}
	}

	if (!(newfh.level & PERM_NOZAP))
		newfh.flag &= ~NOZAP_FLAG;
	else
		newfh.flag |= NOZAP_FLAG;

	move(2, 0);
	clrtobot();
	prints("%s", info);
	move(ln - 2, 0);
	prints("[\033[1;32m��ʾ\033[m] ֱ�ӻس���ʾ���޸ĵ�ǰ��");
	move(ln, 0);
	clrtoeol();
	prints("\033[1;33m�����ð���ת������\033[m");
	move(ln + 2, 4);
	prints("[\033[0;4;32m0\033[m] ��ת��  "
			"[\033[0;4;32m1\033[m] ��Ե�ת��  "
			"[\033[0;4;32m2\033[m] cn.bbsת�� "
			"[\033[0;4;32m3\033[m] DiscuzX���ݿ�ͬ��");
	getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
	if (*genbuf) {
		switch(genbuf[0]) {
			case '0':
				newfh.flag &= ~INNBBSD_FLAG;
				newfh.flag2 &= ~NJUINN_FLAG;
				newfh.flag2 &= ~DISCUZWEB_FLAG;
				newfh.discuzxfid = 0;
				sprintf(genbuf, "%-16s", "��ת��");
				break;
			case '1':
				newfh.flag &= ~INNBBSD_FLAG;
				newfh.flag2 |= NJUINN_FLAG;
				newfh.flag2 &= ~DISCUZWEB_FLAG;
				sprintf(genbuf, "%-16s", "��Ե�ת��");
				break;
			case '2':
				newfh.flag |= INNBBSD_FLAG;
				newfh.flag2 &= ~NJUINN_FLAG;
				newfh.flag2 &= ~DISCUZWEB_FLAG;
				sprintf(genbuf, "%-16s", "cn.bbsת��");
				break;
			case '3':
				newfh.flag &= ~INNBBSD_FLAG;
				newfh.flag2 &= ~NJUINN_FLAG;
				newfh.flag2 |= DISCUZWEB_FLAG;
				// connect to discuzx board
				newfh.discuzxfid = connect_board_to_discuzx(newfh.title);
				sprintf(tempbuf, "%s%d", "DX��ͨ���", newfh.discuzxfid);
				sprintf(genbuf, "%-16s", tempbuf);
				break;
		}
		if ((newfh.flag & INNBBSD_FLAG) != (fh.flag & INNBBSD_FLAG) 
			|| (newfh.flag2 & NJUINN_FLAG) != (fh.flag2 & NJUINN_FLAG)
			|| (newfh.flag2 & DISCUZWEB_FLAG) != (fh.flag2 & DISCUZWEB_FLAG)) {
			ptr = strstr(info, "ת������");
			ptr += 19;
			*ptr = '3';
			strncpy(ptr + 2, genbuf, 15);
		}
	}
	
	move(2, 0);
	clrtobot();
	prints("%s", info);
	if (!mode) {
		move(ln - 2, 0);
		prints("[\033[1;32m��ʾ\033[m] ֱ�ӻس���ʾ���޸ĵ�ǰ��");
		move(ln, 0);
		clrtoeol();
		prints("\033[1;33m�������Ƿ�ʱ����\033[m (y/n)");
		getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
		if (*genbuf) {
			if (genbuf[0] == 'y' || genbuf[0] == 'Y')
				newfh.flag2 |= WATCH_FLAG;
			else if (genbuf[0] == 'n' || genbuf[0] == 'N')
				newfh.flag2 &= ~WATCH_FLAG;
			if ((newfh.flag2 & WATCH_FLAG) != 
					(fh.flag2 & WATCH_FLAG)) {
				ptr = strstr(info, "��ʱ����");
				ptr += 19;
				*ptr = '3';
				strncpy(ptr + 2, newfh.flag2 & WATCH_FLAG ? 
						"�� " : "�� ", 2);
			}
		}
		
		move(2, 0);
		clrtobot();
		prints("%s", info);
		move(ln - 2, 0);
		prints("[\033[1;32m��ʾ\033[m] ֱ�ӻس���ʾ���޸ĵ�ǰ��");
		move(ln, 0);
		clrtoeol();
		prints("\033[1;33m�������Ƿ��������\033[m (y/n)");
		getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
		if (*genbuf) {
			if (genbuf[0] == 'y' || genbuf[0] == 'Y') 
				newfh.flag |= IS1984_FLAG;
			else if (genbuf[0] == 'n' || genbuf[0] == 'N')
				newfh.flag &= ~IS1984_FLAG;
			if ((newfh.flag & IS1984_FLAG)
					!= (fh.flag & IS1984_FLAG)) {
				ptr = strstr(info, "�������");
				ptr += 19;
				*ptr = '3';
				strncpy(ptr + 2, newfh.flag & IS1984_FLAG ? 
						"�� " : "�� ", 2);
			}
		}

		move(2, 0);
		clrtobot();
		prints("%s", info);
		move(ln - 2, 0);
		prints("[\033[1;32m��ʾ\033[m] ֱ�ӻس���ʾ���޸ĵ�ǰ��");
		move(ln, 0);
		clrtoeol();
		prints("\033[1;33m�������Ƿ������ΰ���\033[m (y/n)");
		getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
		if (*genbuf) {
			if (genbuf[0] == 'y' || genbuf[0] == 'Y') 
				newfh.flag |= POLITICAL_FLAG;
			else if (genbuf[0] == 'n' || genbuf[0] == 'N')
				newfh.flag &= ~POLITICAL_FLAG;
			if ((newfh.flag & POLITICAL_FLAG) != 
					(fh.flag & POLITICAL_FLAG)) {
				ptr = strstr(info, "���ΰ���");
				ptr += 19;
				*ptr = '3';
				strncpy(ptr + 2, newfh.flag & POLITICAL_FLAG ?
						"�� " : "�� ", 2);
			}
		}
	}

	move(2, 0);
	clrtobot();
	prints("%s", info);
	move(ln, 0);
	prints("\033[1;33m���Ҫ��ô��һ�ģ�\033[m (y/n)");
	getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
	while (1) {
		if (genbuf[0] == 'y' || genbuf[0] == 'Y')
			break;
		else if (genbuf[0] == 'n' || genbuf[0] == 'N') {
			move(ln + 4, 4);
			prints("\033[1;31mԭ�������ת����...\033[m");
			pressreturn();
			return 1;
		}
		getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
	}
	
	newfh.board_mtime = time(NULL);

	if (strcmp(fh.filename, newfh.filename)) {
		//��������
		char old[256], tar[256];
		setbpath(old, fh.filename);
		setbpath(tar, newfh.filename);
		if (file_exist(tar)) {
			move (ln + 4, 0);
			clrtobot();
			prints("\033[1;31m�ƺ����˽����ȵ���...\033[m");
			pressreturn();
			return 0;
		}
		rename(old, tar);
		sprintf(old, "vote/%s", fh.filename);
		sprintf(tar, "vote/%s", newfh.filename);
		rename(old, tar);
		sprintf(old, "boards/.backnumbers/%s", fh.filename);
		sprintf(tar, "boards/.backnumbers/%s", newfh.filename);
		rename(old, tar);
		if (seek_in_file("etc/junkboards", fh.filename)) {
			del_from_file("etc/junkboards", fh.filename);
			addtofile("etc/junkboards", newfh.filename);
		}
		if (seek_in_file("etc/noboaboards", fh.filename)) {
			del_from_file("etc/noboaboards", fh.filename);
			addtofile("etc/noboaboards", newfh.filename);
		}
	}
	get_grp(fh.filename);
	anno_title(vbuf, &newfh);
	edit_grp(fh.filename, lookgrp, oldtitle, vbuf);
	group = chgrp();
	//sprintf(group, "GROUP_%c", newfh.sec1[0]);
	get_grp(fh.filename);
	strcpy(tmp_grp, lookgrp);
	if (strcmp(tmp_grp, group) || strcmp(fh.filename, newfh.filename)) {
		del_from_file("0Announce/.Search", fh.filename);
		if (group != NULL) {
			if (add_grp(group, cexplain, 
						newfh.filename, vbuf) == -1) {
				move(ln + 4, 4);
				clrtobot();
				prints("\033[1;31m���뾫����ʧ��...\033[m");
			} else {
				move(ln + 4, 4);
				clrtobot();
				prints("\033[1;32m�ɹ����뾫����\033[m");
			}
			sprintf(newpath, "0Announce/groups/%s/%s", 
					group, newfh.filename);
			sprintf(oldpath, "0Announce/groups/%s/%s", 
					tmp_grp, fh.filename);
			if (dashd(oldpath)) {
				deltree(newpath);
			}
		}
		rename(oldpath, newpath);
		del_grp(tmp_grp, fh.filename, fh.title);
	}
	substitute_record(BOARDS, &newfh, sizeof(newfh), pos);
	reload_boards();
	update_postboards();
	move(ln + 4, 4);
	clrtoeol();
	prints("\033[1;32m�޸ĳɹ���\033[m");

	sprintf(vbuf, "�޸������� %s (%s)", fh.filename, newfh.filename);
	sprintf(report, "%s\n\n\n�޸ĺ�\n\n%s", report, info);
	securityreport(vbuf, report);

	move(2, 0);
	clrtobot();
	prints("%s", info);
	move(ln, 0);
	prints("\033[1;33m�Ƿ�����淢�����飿\033[m (y/n)");
	getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
	while(1) {
		if (genbuf[0] == 'y' || genbuf[0] == 'Y') {
			strcpy(bname, currboard);
			strcpy(currboard, newfh.filename);
			*(strstr(report, "��ʱ����")) = 0;
			*(strstr(info, "��ʱ����")) = 0;
			sprintf(report, "%s\n\n\n�޸ĺ�\n\n%s", 
					report, info);
			strcpy(vbuf, "�޸ı�����������");
			deliverreport(vbuf, report);
			move(ln + 4, 4);
			clrtoeol();
			prints("\033[1;32m���ͳɹ�...\033[m");
			strcpy(currboard, bname);
			break;
		} else if (genbuf[0] == 'n' || genbuf[0] == 'N') {
			break;
		}
		getdata(ln, 30, NULL, genbuf, 2, DOECHO, YEA);
	}
	pressreturn();
	clear();
	return 1;
}

FILE *cleanlog;
char curruser[IDLEN + 2];

static void
trace_state(flag, name, size)
int flag, size;
char *name;
{
	char buf[STRLEN];

	if (flag != -1) {
		sprintf(buf, "ON (size = %d)", size);
	} else {
		strcpy(buf, "OFF");
	}
	prints("%s��¼ %s\n", name, buf);
}

static int
touchfile(filename)
char *filename;
{
	int fd;

	if ((fd = open(filename, O_RDWR | O_CREAT, 0600)) > 0) {
		close(fd);
	}
	return fd;
}

int
m_trace()
{
	struct stat ostatb, cstatb;
	int otflag, ctflag, done = 0;
	char ans[3];
	char *msg;

	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return -1;
	}
	clear();
	stand_title("Set Trace Options");
	while (!done) {
		move(2, 0);
		otflag = stat("trace", &ostatb);
		ctflag = stat("trace.chatd", &cstatb);
		prints("Ŀǰ�趨:\n");
		trace_state(otflag, "һ��", ostatb.st_size);
		trace_state(ctflag, "����", cstatb.st_size);
		move(9, 0);
		prints("<1> �л�һ���¼\n");
		prints("<2> �л������¼\n");
		getdata(12, 0, "��ѡ�� (1/2/Exit) [E]: ", ans, 2, DOECHO, YEA);

		switch (ans[0]) {
		case '1':
			if (otflag) {
				touchfile("trace");
				msg = "һ���¼ ON";
			} else {
				rename("trace", "trace.old");
				msg = "һ���¼ OFF";
			}
			break;
		case '2':
			if (ctflag) {
				touchfile("trace.chatd");
				msg = "�����¼ ON";
			} else {
				rename("trace.chatd", "trace.chatd.old");
				msg = "�����¼ OFF";
			}
			break;
		default:
			msg = NULL;
			done = 1;
		}
		move(t_lines - 2, 0);
		if (msg) {
			prints("%s\n", msg);
			//report(msg);
		}
	}
	clear();
	return 0;
}

static int
scan_register_form(regfile)
char *regfile;
{
	static char *const field[] = { "usernum", "userid", "realname", "dept",
		"addr", "phone", "assoc", "rereg", NULL
	};
	static char *const finfo[] =
	    { "�ʺ�λ��", "�����ʺ�", "��ʵ����", "ѧУϵ��",
		"Ŀǰסַ", "����绰", "��ҵѧУ", "���״���д", NULL
	};
	static char *const reason[] = { "��������д����������ʵ������",
		"��������дѧУϵ���������š�",
		"��������д������ס��ַ��",
		"���϶���ϸ���������д��",
		"��д���д���ʲ�������ϸ",
		"�����email������Ϊ��Ч��ַ��",
		"������ƴ����Ӣ����дע�ᵥ��", NULL
	};
	struct userec urec;
	struct userdata udata;
	FILE *fn, *fout, *freg, *scanreglog, *fp;
	struct stat st;
	time_t dtime;
	struct tm *t;
	char fdata[8][STRLEN];
	char fname[STRLEN], buf[STRLEN], logname[STRLEN];
	char ans[5], dellog[STRLEN * 8], *ptr, *uid;
	int n, unum, lockfd, numline = 0, j, percent, loglength;
	time_t now;
	int npass = 0, nreject = 0, nskip = 0, ndel = 0, nresult = 0;
	const char *blk[] = {
		"��", "��", "��", "��", "��", "��", "��", "��", " "
	};
	struct userec *lookupuser;

	uid = currentuser->userid;
	stand_title("�����趨������ע������");
	sprintf(fname, "%s.tmp", regfile);
	move(2, 0);
	lockfd = openlockfile(".lock_new_register", O_RDONLY, LOCK_EX);
	if (dashf(fname)) {
		close(lockfd);
		if (stat(fname, &st) != -1 && st.st_atime > time(0) - 86400) {
			prints
			    ("���� SYSOP ���ڲ鿴ע�����뵥, ����ʹ����״̬.\n");
			pressreturn();
			return -1;
		} else {
			prints
			    ("���� SYSOP ���ڲ鿴ע�����뵥, ����ʹ����״̬.\n");
			pressreturn();
			return -1;
		}
	} else {
		rename(regfile, fname);
		close(lockfd);
	}
	if ((fn = fopen(fname, "r")) == NULL) {
		move(2, 0);
		prints("ϵͳ����, �޷���ȡע�����ϵ�, ����ϵϵͳά��\n");
		pressreturn();
		return -1;
	}

	sprintf(logname, "bbstmpfs/tmp/telnetscanreglog.%d.%s", uinfo.pid,
		currentuser->userid);
	if ((scanreglog = fopen(logname, "w")) != NULL) {
		fprintf(scanreglog, "ɾ�����������У�\n");
	} else {
		errlog("open scanreg tmpfile error.");
	}
	memset(fdata, 0, sizeof (fdata));
	while (fgets(genbuf, STRLEN, fn) != NULL) {
		if ((ptr = (char *) strstr(genbuf, ": ")) != NULL) {
			*ptr = '\0';
			for (n = 0; field[n] != NULL; n++) {
				if (strcmp(genbuf, field[n]) == 0) {
					strcpy(fdata[n], ptr + 2);
					if ((ptr =
					     (char *) strchr(fdata[n],
							     '\n')) != NULL)
						*ptr = '\0';
				}
			}
			numline++;
			continue;
		}
		if (numline == 0)
			continue;
		numline = 0;

		if ((unum = getuser(fdata[1], &lookupuser)) == 0) {
			move(2, 0);
			clrtobot();
			prints("ϵͳ����, ���޴��ʺ�.\n\n");
			for (n = 0; field[n] != NULL; n++)
				prints("%s     : %s\n", finfo[n], fdata[n]);
			pressreturn();
			memset(fdata, 0, sizeof (fdata));
			continue;
		}
		memcpy(&urec, lookupuser, sizeof (urec));
		loaduserdata(urec.userid, &udata);
		move(1, 0);
		prints("�ʺ�λ��     : %d\n", unum);
		disply_userinfo(&urec, 1);
		move(15, 0);
		printdash(NULL);
		clrtobot();
		sprintf(dellog, "ע����Ϣ����: \n");
		loglength = 0;
		for (n = 0; field[n] != NULL; n++) {
			if (loglength >= sizeof (dellog))
				break;
			loglength +=
			    snprintf(dellog + loglength,
				     sizeof (dellog) - loglength, "%s:%s\n",
				     finfo[n], fdata[n]);
		}
		if (urec.userlevel & PERM_LOGINOK) {
			move(t_lines - 1, 0);
			prints("���ʺŲ�������дע�ᵥ.\n");
			igetkey();
			ans[0] = 0;
		} else {
			getdata(t_lines - 1, 0,
				"�Ƿ���ܴ����� (Y/N/Q/Del/Skip)? [S]: ", ans,
				3, DOECHO, YEA);
		}
		move(1, 0);
		clrtobot();
		switch (ans[0]) {
		case 0:
			break;
		case 'D':
		case 'd':
			prints
			    ("���ٽ������Դ��û�����ע��,���û����������վȨ��.\n");
			urec.userlevel &= ~PERM_DEFAULT;
			updateuserec(&urec, 0);
			sprintf(genbuf, "%s ɾ�� %s ��ע�ᵥ",
				currentuser->userid, urec.userid);
			securityreport(genbuf, dellog);
			sprintf(buf, "bbstmpfs/tmp/scanregdellog.%d.%s",
				getpid(), currentuser->userid);
			if ((fp = fopen(buf, "w")) != NULL) {
				fprintf(fp, dellog);
				fclose(fp);
			}
			postfile(buf, "IDScanRecord", genbuf, 2);
			unlink(buf);
			ndel++;
			if (scanreglog != NULL)
				fprintf(scanreglog, "DEL %s\n", urec.userid);
			break;
		case 'Y':
		case 'y':
			prints("����ʹ���������Ѿ�����:\n");
			n = strlen(fdata[5]);
			if (n + strlen(fdata[3]) > 60) {
				if (n > 40)
					fdata[5][n = 40] = '\0';
				fdata[3][60 - n] = '\0';
			}
			strsncpy(udata.realname, fdata[2],
				 sizeof (udata.realname));
			strsncpy(udata.address, fdata[4],
				 sizeof (udata.address));
			sprintf(genbuf, "%s$%s@%s", fdata[3], fdata[5], uid);
			strsncpy(udata.realmail, genbuf,
				 sizeof (udata.realmail));
			urec.userlevel |= PERM_DEFAULT;	// by ylsdd
			updateuserec(&urec, 0);

			saveuserdata(urec.userid, &udata);
			sethomefile(buf, urec.userid, "sucessreg");
			if ((fout = fopen(buf, "w")) != NULL) {
				fprintf(fout, "\n");
				fclose(fout);
			}

			sethomefile(buf, urec.userid, "register");
			if (dashf(buf)) {
				sethomefile(genbuf, urec.userid,
					    "register.old");
				rename(buf, genbuf);
			}
			if ((fout = fopen(buf, "w")) != NULL) {
				for (n = 0; field[n] != NULL; n++)
					fprintf(fout, "%s: %s\n", field[n],
						fdata[n]);
				now = time(NULL);
				fprintf(fout, "Date: %s", ctime(&now));
				fprintf(fout, "Approved: %s\n", uid);
				fclose(fout);
			}
			system_mail_file("etc/s_fill", urec.userid,
				  "��ϲ��ͨ�������֤", currentuser->userid);

			system_mail_file("etc/s_fill2", urec.userid,
				  "��ӭ����" MY_BBS_NAME "���ͥ", currentuser->userid);
			sethomefile(buf, urec.userid, "mailcheck");
			unlink(buf);
			sprintf(genbuf, "�� %s ͨ�����ȷ��.", urec.userid);
			securityreport(genbuf, genbuf);
			npass++;
			break;
		case 'Q':
		case 'q':
			if ((freg = fopen(regfile, "a")) != NULL) {
				for (n = 0; field[n] != NULL; n++)
					fprintf(freg, "%s: %s\n", field[n],
						fdata[n]);
				fprintf(freg, "----\n");
				while (fgets(genbuf, STRLEN, fn) != NULL)
					fputs(genbuf, freg);
				fclose(freg);
			}
			break;
		case 'N':
		case 'n':
			for (n = 0; field[n] != NULL; n++)
				prints("%s: %s\n", finfo[n], fdata[n]);
			printdash(NULL);
			move(9, 0);
			prints
			    ("��ѡ��/�����˻������ԭ��, �� <enter> ȡ��.\n\n");
			for (n = 0; reason[n] != NULL; n++)
				prints("%d) %s\n", n + 1, reason[n]);
			getdata(12 + n, 0, "�˻�ԭ��: ", buf, 60, DOECHO, YEA);
			if (buf[0] != '\0') {
				if (buf[0] >= '1' && buf[0] < '1' + n) {
					strcpy(buf, reason[buf[0] - '1']);
				}
				sprintf(genbuf, "<ע��ʧ��>%s", buf);
				system_mail_file("etc/f_fill", urec.userid, genbuf, currentuser->userid);
				nreject++;
				break;
			}
			move(10, 0);
			clrtobot();
			prints("ȡ���˻ش�ע�������.\n");
			nskip++;
			if (scanreglog != NULL)
				fprintf(scanreglog, "SKIP %s\n", urec.userid);
			break;
			/* run default -- put back to regfile */
		default:
			if ((freg = fopen(regfile, "a")) != NULL) {
				for (n = 0; field[n] != NULL; n++)
					fprintf(freg, "%s: %s\n", field[n],
						fdata[n]);
				fprintf(freg, "----\n");
				fclose(freg);
			}
			nskip++;
			if (scanreglog != NULL)
				fprintf(scanreglog, "SKIP %s\n", urec.userid);
		}
		memset(fdata, 0, sizeof (fdata));
	}
	if (scanreglog != NULL) {
		nresult = npass + nreject + nskip + ndel;
		fprintf(scanreglog, "\n������ %3d ��ע�ᵥ\n\n", nresult);
		fprintf(scanreglog, "\033[32mͨ��\033[0m %3d�� Լռ", npass);
		percent = (npass * 100) / ((nresult <= 0) ? 1 : nresult);
		for (j = percent / 16; j; j--)
			fprintf(scanreglog, "%s", blk[0]);
		fprintf(scanreglog, "%s%d%%\n", blk[8 - (percent % 16) / 2],
			percent);
		fprintf(scanreglog, "\033[33m�˻�\033[0m %3d�� Լռ", nreject);
		percent = (nreject * 100) / ((nresult <= 0) ? 1 : nresult);
		for (j = percent / 16; j; j--)
			fprintf(scanreglog, "%s", blk[0]);
		fprintf(scanreglog, "%s%d%%\n", blk[8 - (percent % 16) / 2],
			percent);
		fprintf(scanreglog, "���� %3d�� Լռ", nskip);
		percent = (nskip * 100) / ((nresult <= 0) ? 1 : nresult);
		for (j = percent / 16; j; j--)
			fprintf(scanreglog, "%s", blk[0]);
		fprintf(scanreglog, "%s%d%%\n", blk[8 - (percent % 16) / 2],
			percent);
		fprintf(scanreglog, "\033[31mɾ��\033[0m %3d�� Լռ", ndel);
		percent = (ndel * 100) / ((nresult <= 0) ? 1 : nresult);
		for (j = percent / 16; j; j--)
			fprintf(scanreglog, "%s", blk[0]);
		fprintf(scanreglog, "%s%d%%\n", blk[8 - (percent % 16) / 2],
			percent);
		fclose(scanreglog);
		time(&dtime);
		t = localtime(&dtime);
		sprintf(genbuf,
			"Telnet �ʺ�������¼-%d-%02d-%02d-%02d:%02d:%02d",
			1900 + t->tm_year, 1 + t->tm_mon, t->tm_mday,
			t->tm_hour, t->tm_min, t->tm_sec);
		postfile(logname, "IDScanRecord", genbuf, 2);
		unlink(logname);
		tracelog("%s pass %d %d %d %d", currentuser->userid, npass,
			 nreject, nskip, ndel);
	}
	fclose(fn);
	unlink(fname);
	return (0);
}

int
m_register()
{
	FILE *fn;
	char ans[3], *fname;
	int x, y, wid, len;
	char uident[STRLEN];
	char buf[PASSLEN];
	struct userec *lookupuser;

	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return -1;
	}
	getdata(1, 0, "�������������: ", buf, PASSLEN, NOECHO, YEA);
	if (*buf == '\0'
	    || !checkpasswd(currentuser->passwd, currentuser->salt, buf)) {
		prints("\n\n���Ų���ȷ, ����ִ�С�\n");
		pressreturn();
		clear();
		return -1;
	}
	clear();

	stand_title("�趨ʹ����ע������");
	for (;;) {
		getdata(1, 0,
			"(0)�뿪  (1)�����ע������  (2)��ѯʹ����ע������ ? : ",
			ans, 2, DOECHO, YEA);
		if (ans[0] == '0' || ans[0] == '\n' || ans[0] == '\0')
			return 0;
		if (ans[0] == '1' || ans[0] == '2')
			break;

	}
	if (ans[0] == '1') {
		fname = "new_register";
		if ((fn = fopen(fname, "r")) == NULL) {
			prints("\n\nĿǰ������ע������.");
			pressreturn();
		} else {
			y = 3, x = wid = 0;
			while (fgets(genbuf, STRLEN, fn) != NULL && x < 65) {
				if (strncmp(genbuf, "userid: ", 8) == 0) {
					move(y++, x);
					prints(genbuf + 8);
					len = strlen(genbuf + 8);
					if (len > wid)
						wid = len;
					if (y >= t_lines - 2) {
						y = 3;
						x += wid + 2;
					}
				}
			}
			fclose(fn);
			if (askyn("�趨������", NA, YEA) == YEA) {
				securityreport("��ʼTelnet�趨ʹ����ע������",
					       "��ʼTelnet�趨ʹ����ע������");
				scan_register_form(fname);
			}
		}
	} else {
		move(1, 0);
		usercomplete("������Ҫ��ѯ�Ĵ���: ", uident);
		if (uident[0] != '\0') {
			if (!getuser(uident, &lookupuser)) {
				move(2, 0);
				prints("�����ʹ���ߴ���...");
			} else {
				sprintf(genbuf, "home/%c/%s/register",
					mytoupper(lookupuser->userid[0]),
					lookupuser->userid);
				if ((fn = fopen(genbuf, "r")) != NULL) {
					sprintf(genbuf, "��ѯ %s ��ע����Ϣ.",
						lookupuser->userid);
					securityreport(genbuf, genbuf);

					prints("\nע����������:\n\n");
					for (x = 1; x <= 15; x++) {
						if (fgets(genbuf, STRLEN, fn))
							prints("%s", genbuf);
						else
							break;
					}
					fclose(fn);
				} else
					prints("\n\n�Ҳ�����/����ע������!!\n");
				printdash("���û���������Ϣ");
				prints("�ʺŽ������� : %s",
				       ctime(&lookupuser->firstlogin));
				prints("����������� : %s",
				       ctime(&lookupuser->lastlogin));
				prints("����¼������ : %d\n",
				       lookupuser->numdays);
				prints("��վ��ʱ��   : %d Сʱ %d ����\n",
				       (int) lookupuser->stay / 3600,
				       (int) (lookupuser->stay / 60) % 60);
			}
		}
		pressanykey();
	}
	clear();
	return 0;
}
extern struct UTMPFILE *utmpshm;
int
m_ordainBM()
{
	return do_ordainBM(NULL, NULL);

}

int
do_ordainBM(const char *userid, const char *abname)
{
	int id, pos, oldbm = 0, i, bm = 0, bigbm, bmpos, minpos, maxpos;
	struct boardheader fh;
	char bname[STRLEN], tmp[256], buf[5][STRLEN];
	char content[1024], title[STRLEN];
	struct userec *lookupuser;
	modify_user_mode(ADMIN);
	if (!check_systempasswd()) {
		return -1;
	}
	clear();
	stand_title("��������\n");
	clrtoeol();
	move(2, 0);
	if (userid)
		strsncpy(genbuf, userid, sizeof (genbuf));
	else
		usercomplete("������������ʹ�����ʺ�: ", genbuf);
	if (genbuf[0] == '\0') {
		clear();
		return 0;
	}
	if (!(id = getuser(genbuf, &lookupuser))) {
		move(4, 0);
		prints("��Ч��ʹ�����ʺ�");
		clrtoeol();
		pressreturn();
		clear();
		return 0;
	}
	if (abname)
		strsncpy(bname, abname, sizeof (bname));
	else {
		make_blist_full();
		namecomplete("�����ʹ���߽����������������: ", bname);
		FreeNameList();
	}
	if (*bname == '\0') {
		move(5, 0);
		prints("���������������");
		pressreturn();
		clear();
		return -1;
	}
	pos =
	    new_search_record(BOARDS, &fh, sizeof (fh), (void *) cmpbnames,
			      bname);
	if (!pos) {
		move(5, 0);
		prints("���������������");
		pressreturn();
		clear();
		return -1;
	}
	oldbm = getbmnum(lookupuser->userid);
	if (oldbm >= 5 && strcmp(lookupuser->userid, "SYSOP")
	    && normal_board(bname)) {
		move(5, 0);
		prints(" %s �Ѿ��������İ�����", lookupuser->userid);
		pressanykey();
		clear();
		return -1;
	}
	if (askyn("����Ϊ�����ô? (������������)", NA, NA) == YEA) {
		bigbm = 1;
		minpos = 0;
		maxpos = 3;
	} else {
		bigbm = 0;
		minpos = 4;
		maxpos = BMNUM - 1;
	}
	bmpos = -1;
	for (i = 0; i < BMNUM; i++) {
		if (fh.bm[i][0] == 0 && (i >= minpos) && (i <= maxpos)
		    && (bmpos == -1)) {
			bmpos = i;
		}
		if (!strncmp(fh.bm[i], lookupuser->userid, IDLEN)) {
			prints(" %s �Ѿ��Ǹð����", lookupuser->userid);
			pressanykey();
			clear();
			return -1;
		}
	}
	if (bmpos == -1) {
		prints(" %s û�п������λ��", bname);
		pressanykey();
		clear();
		return -1;
	}
	prints("\n�㽫���� %s Ϊ %s �����.\n", lookupuser->userid, bname);
	if (askyn("��ȷ��Ҫ������?", NA, NA) == NA) {
		prints("ȡ����������");
		pressanykey();
		clear();
		return -1;
	}
	for (i = 0; i < 5; i++)
		buf[i][0] = '\0';
	move(8, 0);
	prints("��������������(������У��� Enter ����)");
	for (i = 0; i < 5; i++) {
		getdata(i + 9, 0, ": ", buf[i], STRLEN - 5, DOECHO, YEA);
		if (buf[i][0] == '\0')
			break;
	}

	if (!oldbm) {
		char secu[STRLEN];
		struct userec tmpu;
		memcpy(&tmpu, lookupuser, sizeof (tmpu));
		tmpu.userlevel |= PERM_BOARDS;
		updateuserec(&tmpu, 0);
		sprintf(secu, "��������, ���� %s �İ���Ȩ��",
			lookupuser->userid);
		securityreport(secu, secu);
		move(14, 0);
		system_mail_file("etc/bmhelp", lookupuser->userid, "��������ֲ�", currentuser->userid);
		prints("%s", secu);
	}
	strncpy(fh.bm[bmpos], lookupuser->userid, IDLEN);
	fh.bm[bmpos][IDLEN] = 0;
	fh.hiretime[bmpos] = time(NULL);
	if (bigbm) {
		anno_title(tmp, &fh);
		get_grp(fh.filename);
		edit_grp(fh.filename, lookgrp, fh.title, tmp);
	}
	substitute_record(BOARDS, &fh, sizeof (fh), pos);
	if (fh.flag & CLOSECLUB_FLAG) {
		setclubmember(fh.filename, lookupuser->userid, 1);
	}
	reload_boards();
	sprintf(genbuf, "���� %s Ϊ %s ����������", lookupuser->userid,
		fh.filename);
	securityreport(genbuf, genbuf);
	move(15, 0);
	prints("%s", genbuf);
	sprintf(title, "[����]���� %s ��%s %s ", bname, (!bm) ? "����" : "�渱",
		lookupuser->userid);
	sprintf(content,
		"\n\t\t\t�� ���� ��\n\n\t���� %s Ϊ %s ��%s��\n",
		lookupuser->userid, bname, (!bm) ? "����" : "�渱");
	for (i = 0; i < 5; i++) {
		if (buf[i][0] == '\0')
			break;
		if (i == 0)
			strcat(content, "\n\n�������ԣ�\n");
		strcat(content, buf[i]);
		strcat(content, "\n\n");
	}
	admin_b_report(title, content, &fh);
	mail_buf(content, strlen(content), lookupuser->userid, title, "deliver");
	bbsinfo.utmpshm->syncbmonline = 1;
	pressanykey();
	return 0;
}

int
m_retireBM()
{
	return do_retireBM(NULL, NULL);
}

int
do_retireBM(const char *userid, const char *abname)
{
	int id, pos, bmpos, right = 0, oldbm = 0, i;
	int bm = 1;
	struct boardheader fh;
	char buf[5][STRLEN];
	char bname[STRLEN];
	char content[1024], title[256];
	char tmp[256];
	struct userec *lookupuser;
	modify_user_mode(ADMIN);
	if (!check_systempasswd())
		return -1;

	clear();
	stand_title("������ְ\n");
	clrtoeol();
	if (userid)
		strsncpy(genbuf, userid, sizeof (genbuf));
	else
		usercomplete("���������ε�ʹ�����ʺ�: ", genbuf);
	if (genbuf[0] == '\0') {
		clear();
		return 0;
	}
	if (!(id = getuser(genbuf, &lookupuser))) {
		move(4, 0);
		prints("��Ч��ʹ�����ʺ�");
		clrtoeol();
		pressreturn();
		clear();
		return 0;
	}
	if (abname)
		strsncpy(bname, abname, sizeof (bname));
	else {
		make_blist_full();
		namecomplete("�����ʹ���߽����������������: ", bname);
		FreeNameList();
	}
	if (*bname == '\0') {
		move(5, 0);
		prints("���������������");
		pressreturn();
		clear();
		return -1;
	}
	pos =
	    new_search_record(BOARDS, &fh, sizeof (fh), (void *) cmpbnames,
			      bname);
	if (!pos) {
		move(5, 0);
		prints("���������������");
		pressreturn();
		clear();
		return -1;
	}
	bmpos = -1;
	for (i = 0; i < BMNUM; i++) {
		if (!strcasecmp(fh.bm[i], lookupuser->userid)) {
			bmpos = i;
			if (i < 4)
				bm = 1;
			else
				bm = 0;
		}
	}

	oldbm = getbmnum(lookupuser->userid);
	if (bmpos == -1) {
		move(5, 0);
		prints(" ����������û��%s�����д�����֪ͨϵͳά����",
		       lookupuser->userid);
		pressanykey();
		clear();
		return -1;
	}
	prints("\n�㽫ȡ�� %s �� %s ��%s����ְ��.\n",
	       lookupuser->userid, bname, bm ? "��" : "");
	if (askyn("��ȷ��Ҫȡ�����ĸð����ְ����?", NA, NA) == NA) {
		prints("\n�Ǻǣ���ı������ˣ� %s �������� %s �����ְ��",
		       lookupuser->userid, bname);
		pressanykey();
		clear();
		return -1;
	}
	anno_title(title, &fh);
	fh.bm[bmpos][0] = 0;	//�������, ���������
	fh.hiretime[bmpos] = 0;
	for (i = bmpos; i < (bm ? 4 : BMNUM); i++) {
		if (i == (bm ? 3 : BMNUM - 1)) {	//���һ��BM
			fh.bm[i][0] = 0;
			fh.hiretime[i] = 0;
		} else {
			strcpy(fh.bm[i], fh.bm[i + 1]);
			fh.bm[i][strlen(fh.bm[i + 1])] = 0;
			fh.hiretime[i] = fh.hiretime[i + 1];
		}
	}
	if (bm) {
		anno_title(tmp, &fh);
		get_grp(fh.filename);
		edit_grp(fh.filename, lookgrp, title, tmp);
	}
	substitute_record(BOARDS, &fh, sizeof (fh), pos);
	reload_boards();
	sprintf(genbuf, "ȡ�� %s �� %s ����������ְ��", lookupuser->userid,
		fh.filename);
	securityreport(genbuf, genbuf);
	move(8, 0);
	prints("%s", genbuf);
	if (!(oldbm - 1)) {
		char secu[STRLEN];
		struct userec tmpu;
		if (!(lookupuser->userlevel & PERM_OBOARDS)
		    && !(lookupuser->userlevel & PERM_SYSOP)) {
			memcpy(&tmpu, lookupuser, sizeof (tmpu));
			tmpu.userlevel &= ~PERM_BOARDS;
			updateuserec(&tmpu, 0);
			sprintf(secu, "����жְ, ȡ�� %s �İ���Ȩ��",
				lookupuser->userid);
			securityreport(secu, secu);
			move(9, 0);
			prints(secu);
		}
	}
	prints("\n\n");
	if (askyn("��Ҫ����ذ��淢��ͨ����?", YEA, NA) == NA) {
		pressanykey();
		return 0;
	}
	prints("\n");
	if (askyn("���������밴 Enter ��ȷ�ϣ���ְ�ͷ��� N ��", YEA, NA) == YEA)
		right = 1;
	else
		right = 0;
	if (right)
		sprintf(title, "[����]%s ��%s %s ����", bname,
			bm ? "�����" : "����", lookupuser->userid);
	else
		sprintf(title, "[����]���� %s ��%s %s ", bname,
			bm ? "�����" : "����", lookupuser->userid);
	strcpy(currboard, bname);
	if (right) {
		sprintf(content, "\n\t\t\t�� ���� ��\n\n"
	//		"\t��վ�������ۣ�\n"
			"\tͬ�� %s ��ȥ %s ���%sְ��\n"
			"\t�ڴˣ����������� %s �������������ʾ��л��\n\n"
			"\tϣ�����Ҳ��֧�ֱ���Ĺ���.",
			lookupuser->userid, bname, bm ? "�����" : "����",
			bname);
	} else {
		sprintf(content, "\n\t\t\t����ְ���桿\n\n"
	//		"\t��վ�������۾�����\n"
			"\t���� %s ��%s %s ��%sְ��\n",
			bname, bm ? "�����" : "����", lookupuser->userid,
			bm ? "�����" : "����");
	}
	for (i = 0; i < 5; i++)
		buf[i][0] = '\0';
	move(14, 0);
	prints("������%s����(������У��� Enter ����)",
	       right ? "��������" : "������ְ");
	for (i = 0; i < 5; i++) {
		getdata(i + 15, 0, ": ", buf[i], STRLEN - 5, DOECHO, YEA);
		if (buf[i][0] == '\0')
			break;
		if (i == 0)
			strcat(content,
			       right ? "\n\n���θ��ԣ�\n" : "\n\n��ְ˵����\n");
		strcat(content, buf[i]);
		strcat(content, "\n");
	}
	admin_b_report(title, content, &fh);
	mail_buf(content, strlen(content), lookupuser->userid, title, "deliver");
	prints("\nִ����ϣ�");
	bbsinfo.utmpshm->syncbmonline = 1;
	pressanykey();
	return 0;
}

int
retireBM(uid, bname)
char *uid;
char *bname;
{
	char tmp[STRLEN];
	char content[1024], title[STRLEN];
	int i, oldbm, id, pos, bmpos = -1, bm = 0;
	struct boardheader fh;
	struct userec *lookupuser;
	if (!(id = getuser(uid, &lookupuser)))
		return -1;
	pos =
	    new_search_record(BOARDS, &fh, sizeof (fh), (void *) cmpbnames,
			      bname);
	if (!pos)
		return -2;
	oldbm = getbmnum(lookupuser->userid);
	for (i = 0; i < BMNUM; i++) {
		if (!strcasecmp(fh.bm[i], lookupuser->userid)) {
			bmpos = i;
			if (i < 4)
				bm = 1;
			else
				bm = 0;
		}
	}
	if (bmpos == -1)
		return -3;
	anno_title(title, &fh);
	fh.bm[bmpos][0] = 0;	//�������, ���������
	fh.hiretime[bmpos] = 0;
	for (i = bmpos; i < (bm ? 4 : BMNUM); i++) {
		if (i == bm ? 3 : BMNUM - 1) {	//���һ��BM
			fh.bm[i][0] = 0;
			fh.hiretime[i] = 0;
		} else {
			strcpy(fh.bm[i], fh.bm[i + 1]);
			fh.hiretime[i] = fh.hiretime[i + 1];
		}
	}
	if (bm) {
		anno_title(tmp, &fh);
		get_grp(fh.filename);
		edit_grp(fh.filename, lookgrp, title, tmp);
	}
	substitute_record(BOARDS, &fh, sizeof (fh), pos);
	reload_boards();
	sprintf(genbuf, "ȡ�� %s �� %s ����������ְ��", lookupuser->userid,
		fh.filename);
	securityreport(genbuf, genbuf);
	if (!(oldbm - 1)) {
		char secu[STRLEN];
		struct userec tmpu;
		if (!(lookupuser->userlevel & PERM_OBOARDS)
		    && !(lookupuser->userlevel & PERM_SYSOP)) {
			memcpy(&tmpu, lookupuser, sizeof (tmpu));
			tmpu.userlevel &= ~PERM_BOARDS;
			updateuserec(&tmpu, 0);
			sprintf(secu, "����жְ, ȡ�� %s �İ���Ȩ��",
				lookupuser->userid);
			securityreport(secu, secu);
		}
	}
	sprintf(title, "[����]���� %s ��%s %s ", bname,
		bm ? "����" : "�渱", lookupuser->userid);
	strcpy(currboard, bname);
	sprintf(content, "\n\t\t\t����ְ���桿\n\n"
		"\tϵͳ��ְ��\n"
		"\t����ID���������� %s ��%s %s ��%sְ��\n",
		bname, bm ? "����" : "�渱", lookupuser->userid,
		bm ? "����" : "�渱");
	deliverreport(title, content);
	if (normal_board(currboard)) {
		strcpy(currboard, "Board");
		deliverreport(title, content);
	}
	bbsinfo.utmpshm->syncbmonline = 1;
	return 0;
}

int
retire_allBM(uid)
char *uid;
{
	struct boardheader bh;
	int fd, size;
	size = sizeof (bh);
	if ((fd = open(BOARDS, O_RDONLY, 0)) == -1)
		return -1;
	while (read(fd, &bh, size) > 0)
		retireBM(uid, bh.filename);
	close(fd);
	bbsinfo.utmpshm->syncbmonline = 1;
	return 0;
}

int
m_addpersonal()
{
	FILE *fn;
	char digestpath[80] = "0Announce/groups/GROUP_0/Personal_Corpus";
	char personalpath[80], title[STRLEN];
	char firstchar[2];
	struct userec *lookupuser;
	int id;
	modify_user_mode(DIGEST);
	if (!check_systempasswd()) {
		return 1;
	}
	clear();
	if (!dashd(digestpath)) {
		prints("���Ƚ��������ļ���������Personal_Corpus");
		pressanykey();
		return 1;
	}
	stand_title("���������ļ�");
	clrtoeol();
	move(2, 0);
	if (USERPERM(currentuser, PERM_SYSOP) || USERPERM(currentuser, PERM_ACCOUNTS))
		usercomplete("������ʹ���ߴ���: ", genbuf);
	else
		strncpy(genbuf, currentuser->userid, strlen(genbuf));
	if (*genbuf == '\0') {
		clear();
		return 1;
	}
	if (!(id = getuser(genbuf, &lookupuser))) {
		prints("%s�Ǵ����ʹ���ߴ���", genbuf);
		clrtoeol();
		pressreturn();
		clear();
		return 1;
	}
	if (!isalpha(lookupuser->userid[0])) {
		move(5, 0);
		prints("��ID������ID�������ʵ�ʷ�������ƴ��������ĸ�������������ܵ����ļ��޷������ʡ�");
		getdata(3, 0, "��Ӣ��ID��������ƴ������ĸ:", firstchar, 2,
			DOECHO, YEA);
		firstchar[0] = toupper(firstchar[0]);
		move(5, 0);
		clrtoeol();
	} else
		firstchar[0] = toupper(lookupuser->userid[0]);
	firstchar[1] = '\0';
	//prints("%c", firstchar[0]);
	sprintf(personalpath, "%s/%c", digestpath, firstchar[0]);
	if (!dashd(personalpath)) {
		mkdir(personalpath, 0755);
		sprintf(personalpath, "%s/.Names", personalpath);
		if ((fn = fopen(personalpath, "w")) == NULL) {
			return -1;
		}
		fprintf(fn, "#\n");
		fprintf(fn, "# Title=%s\n", firstchar);
		fprintf(fn, "#\n");
		fclose(fn);
		linkto(digestpath, firstchar, firstchar);
		sprintf(personalpath, "%s/%c", digestpath, firstchar[0]);
	}
	sprintf(personalpath, "%s/%c/%s", digestpath,
		firstchar[0], lookupuser->userid);
	if (dashd(personalpath) || USERPERM(lookupuser, PERM_SPECIAL8)) {
		prints("��ID�ĸ����ļ�Ŀ¼�Ѵ��ڣ��������ȡ��..");
		pressanykey();
		return 1;
	}
	if (lookupuser->stay / 60 / 60 < 24) {
		prints("��ID��վʱ�䲻��������ʱ��������Ҫ24Сʱ���������ļ������Ƕ����߿�����...");
		pressanykey();
		return 1;
	}
	move(4, 0);
	if (askyn("ȷ��ҪΪ��ID����һ�������ļ���? ", YEA, NA) == NA) {
		prints("��ѡ��ȡ������. �������ȡ��...");
		pressanykey();
		return 1;
	}
	mkdir(personalpath, 0755);
	chmod(personalpath, 0755);

	move(7, 0);
	prints("[ֱ�Ӱ� ENTER ��, �����ȱʡΪ: \033[32m%s�ļ�\033[m]",
	       lookupuser->userid);
	getdata(6, 0, "����������ļ�֮����: ", genbuf, 39, DOECHO, YEA);
	if (genbuf[0] == '\0')
		sprintf(title, "%s�ļ�", lookupuser->userid);
	else
		sprintf(title, "%s�ļ�����%s", lookupuser->userid, genbuf);
	sprintf(title, "%-38.38s(BM: %s _Personal)", title, lookupuser->userid);
	sprintf(digestpath, "%s/%c", digestpath, firstchar[0]);
	linkto(digestpath, lookupuser->userid, title);
	sprintf(personalpath, "%s/.Names", personalpath);
	if ((fn = fopen(personalpath, "w")) == NULL) {
		return -1;
	}
	fprintf(fn, "#\n");
	fprintf(fn, "# Title=%s\n", title);
	fprintf(fn, "#\n");
	fclose(fn);
	if (!(lookupuser->userlevel & PERM_SPECIAL8)) {
		char secu[STRLEN];
		struct userec tmpu;
		memcpy(&tmpu, lookupuser, sizeof (tmpu));
		tmpu.userlevel |= PERM_SPECIAL8;
		updateuserec(&tmpu, 0);
		sprintf(secu, "���������ļ�, ���� %s �ļ�����Ȩ��",
			lookupuser->userid);
		securityreport(secu, secu);
		sprintf(secu, "���� \033[1;32m%s\033[m �ĸ����ļ���\n\n\t\033[1;33m%s\033[m\n\n��ϲ��ϲ\n", lookupuser->userid, title);
		strcpy(currboard, "Personal_Corpus");
		sprintf(genbuf, "���� %s �ĸ����ļ�", lookupuser->userid);
		deliverreport(genbuf, secu);

	}
	system_mail_file("etc/s_personal", lookupuser->userid, "�����ļ������ֲ�", currentuser->userid);
	prints("�Ѿ����������ļ�, �밴���������...");
	clrtoeol();
	pressanykey();
	return 0;
}
