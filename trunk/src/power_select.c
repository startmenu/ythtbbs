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
#include "regular.h"

typedef void (*power_dofunc) (int, struct fileheader *, char *);
static void sure_markdel(int ent, struct fileheader *fileinfo, char *direct);
static void sure_unmarkdel(int ent, struct fileheader *fileinfo, char *direct);
static void power_dir(int ent, struct fileheader *fileinfo, char *direct);
static int power_range(char *filename, int id1, int id2, char *select,
		       power_dofunc function, int *shoot);
static int titlehas(char *buf);
static int idis(char *buf);
static int idhas(char *buf);
static int checkmark(char *buf);
static int counttextlt(char *num);
static int checktext(char *query);
static int checkattr(char *buf);
static int checkstar(char *buf);
static int checkevas(char *buf);

static void
sure_markdel(ent, fileinfo, direct)
int ent;
struct fileheader *fileinfo;
char *direct;
{
	if (fileinfo->owner[0] == '-')
		return;
	change_dir(direct, fileinfo,
		   (void *) DIR_do_suremarkdel, ent, digestmode, 0, NULL);
	return;
}

static void
sure_unmarkdel(ent, fileinfo, direct)
int ent;
struct fileheader *fileinfo;
char *direct;
{
	if (fileinfo->owner[0] == '-')
		return;
	change_dir(direct, fileinfo,
		   (void *) DIR_do_sureunmarkdel, ent, digestmode, 0, NULL);
	return;
}

static struct fileheader *select_cur;
static void
power_dir(ent, fileinfo, direct)
int ent;
struct fileheader *fileinfo;
char *direct;
{
	extern char currdirect[STRLEN];
//For hide the article is deleted by who, ugly code....
	if (!strncmp(fileinfo->title + 33, "- ", 2))
		fileinfo->title[33] = 0;
	append_record(currdirect, fileinfo, sizeof (struct fileheader));
}

int
mkpowersymlink(char *direct)
{
	char *ptr, buf[256];
	ptr = strrchr(direct, '/');
	if (!ptr || strncmp(ptr + 1, ".POWER.", 7))
		return -1;
	sprintf(buf, MY_BBS_HOME "/bbstmpfs/tmp/POWER.%s.%d",
		currentuser->userid, uinfo.pid);
	close(creat(buf, 0600));
	symlink(buf, direct);
	return 0;
}

void
rmpower()
{
	rmpowersymlink(NULL);
}

int
rmpowersymlink(char *direct)
{
	char *ptr, buf[256];
	sprintf(buf, MY_BBS_HOME "/bbstmpfs/tmp/POWER.%s.%d",
		currentuser->userid, uinfo.pid);
	unlink(buf);
	if (!direct)
		return 0;
	ptr = strrchr(direct, '/');
	if (!ptr || strncmp(ptr + 1, ".POWER.", 7))
		return -1;
	unlink(direct);
	return 0;
}

static int
power_range(filename, id1, id2, select, function, shoot)
char *filename;
int id1, id2;
char *select;
power_dofunc function;
int *shoot;
{
	struct fileheader *buf;
	int fd, bufsize, i, n, ret;
	struct stat st;
	*shoot = 0;
	if ((fd = open(filename, O_RDONLY)) == -1) {
		return -1;
	}
	fstat(fd, &st);
	if (-1 == id2)
		id2 = st.st_size / sizeof (struct fileheader);
	bufsize = sizeof (struct fileheader) * (id2 + 1 - id1);
	buf = malloc(bufsize);
	if (buf == NULL) {
		close(fd);
		return -4;
	}

	lseek(fd, (id1 - 1) * sizeof (struct fileheader), SEEK_SET);
	n = read(fd, buf, bufsize);
	close(fd);
	for (i = id1; i < id1 + n / sizeof (struct fileheader); i++) {
		select_cur = buf + (i - id1);
		ret = checkf(select);
		if (ret > 0) {
			(*function) (i, select_cur, filename);
			(*shoot)++;
		} else if (ret < 0) {
			free(buf);
			return -ret;
		}
	}
	free(buf);
	return 0;
}

int
power_action(filename, id1, id2, select, action)
char *filename;
int id1, id2;
char *select;
int action;
{
	ExtStru myextstru[] = {
		{titlehas, Pair("标题", "含")},
		{idis, Pair("作者", "是")},
		{idhas, Pair("作者", "含")},
		{counttextlt, Pair("灌水字数", "少于")},
		{checkmark, Pair("标记", "含")},
		{checktext, Pair("内容", "含")},
		{checkattr, Pair("属性", "是")},
		{checkstar, Pair("推荐星级", "高于")},
		{checkevas, Pair("推荐人数", "高于")},
		{NULL, NULL, NULL}
	};
	power_dofunc function;
	int shoot;
	int ret;
	extern char currdirect[STRLEN];
	extstru = myextstru;
	switch (action) {
	case 1:
		function = sure_markdel;
		break;
	case 2:
		function = sure_unmarkdel;
		break;
	case 9:
		digestmode = 3;
		setbdir(currdirect, currboard, digestmode);
		unlink(currdirect);
		mkpowersymlink(currdirect);
		function = power_dir;
		break;
	case 0:
	default:
		return FULLUPDATE;
	}
	ret = power_range(filename, id1, id2, select, function, &shoot);
	tracelog("%s select %s %d %d", currentuser->userid, currboard, id1,
		 id2);
	if (ret < 0) {
		prints("无法执行超级操作:%d,请联系系统维护.\n", ret);
		pressreturn();
		if (action == 9) {
			digestmode = NA;
			setbdir(currdirect, currboard, digestmode);
		}
		return FULLUPDATE;
	} else if (ret > 0) {
		prints
		    ("限制条件语法错误,从第%d个字符起我就觉得不对劲,一个汉字算两个字符啊!",
		     ret);
		if (action == 9) {
			digestmode = NA;
			setbdir(currdirect, currboard, digestmode);
		}
		pressreturn();
		return FULLUPDATE;
	}

	limit_cpu();
	if (action == 9)
		return NEWDIRECT;
	fixkeep(filename, (id1 <= 0) ? 1 : id1, (id2 <= 0) ? 1 : id2);

	prints("操作完成,有%d篇文章满足条件\n", shoot);
	pressreturn();
	return FULLUPDATE;
}

static int
titlehas(char *buf)
{
	if (strstr(select_cur->title, buf))
		return 1;
	else
		return 0;
}

static int
idis(char *buf)
{
	if (strcasecmp(select_cur->owner, buf))
		return 0;
	else
		return 1;
}

static int
idhas(char *buf)
{
	if (strcasestr(select_cur->owner, buf))
		return 1;
	else
		return 0;
}

static int
checkmark(char *buf)
{
	if (!strcasecmp(buf, "m")) {
		if (select_cur->accessed & FH_MARKED)
			return 1;
		else
			return 0;
	} else if (!strcasecmp(buf, "g")) {
		if (select_cur->accessed & FH_DIGEST)
			return 1;
		else
			return 0;
	} else if (!strcasecmp(buf, "@")) {
		if (select_cur->accessed & FH_ATTACHED)
			return 1;
		else
			return 0;
	} else
		return 0;
}

static int
counttextlt(char *num)
{
	int n, size;
	if (select_cur->sizebyte)
		size = bytenum(select_cur->sizebyte);
	else {
		char buf[256];
		snprintf(buf, sizeof (buf), "boards/%s/%s",
			 currboard, fh2fname(select_cur));
		size = eff_size(buf);
	}
	n = atoi(num);
	if (size < n)
		return 1;
	else
		return 0;
}

static int
checktext(char *query)
{
	char buf[256];
	snprintf(buf, sizeof (buf), "boards/%s/%s",
		 currboard, fh2fname(select_cur));
	return searchpattern(buf, query);
}

static int
checkattr(char *buf)
{
	if (!strcmp(buf, "未读")) {
		if (UNREAD(select_cur, &brc))
			return 1;
		else
			return 0;
	} else if (!strcmp(buf, "原作")) {
		if (strncmp(select_cur->title, "Re: ", 4))
			return 1;
		else
			return 0;
	} else
		return 0;
}

static int
checkstar(char *buf)
{
	if (select_cur->staravg50 >= atoi(buf) * 50)
		return 1;
	else
		return 0;
}

static int
checkevas(char *buf)
{
	if (select_cur->hasvoted >= atoi(buf))
		return 1;
	else
		return 0;
}

int
power_select(ent, fileinfo, direct)
int ent;
struct fileheader *fileinfo;
char *direct;
{
	char num[8];
	static char select[STRLEN];
	int inum1, inum2, answer;
	char dir[STRLEN];
	if (uinfo.mode != READING || digestmode != NA)
		return DONOTHING;
	snprintf(dir, STRLEN, "%s", direct);
	clear();
	prints("                  超强文章选择\n\n");
	prints("请选择操作范围\n");
	getdata(3, 0, "首篇文章编号: ", num, 7, DOECHO, YEA);
	inum1 = atoi(num);
	if (inum1 <= 0) {
		prints("错误编号\n");
		pressreturn();
		return FULLUPDATE;
	}
	getdata(4, 0, "末篇文章编号: ", num, 7, DOECHO, YEA);
	inum2 = atoi(num);
	if (inum2 - inum1 <= 1) {
		prints("错误编号\n");
		pressreturn();
		return FULLUPDATE;
	}
	move(6, 0);
	prints("例子:\n"
	       "   例子一: 找auto的所有文章\n"
	       "   请输入限制条件: 作者是 auto\n"
	       "   例子二: 找auto写的灌水文章\n"
	       "   请输入限制条件: 作者是 auto 且 灌水字数少于 40\n"
	       "   例子三: 找auto写的灌水文章,而且被标记为m的\n"
	       "   请输入限制条件: 作者是 auto 且 灌水字数少于 40 且 标记含 m\n"
	       "   例子四: 找所有标题包含一路BBS或者yilubbs的文章\n"
	       "   请输入限制条件: 标题含 一路BBS 或 标题含 yilubbs\n"
	       "   例子五: 找所有不是auto也不是deliver写的文章\n"
	       "   请输入限制条件: 非 (作者是 auto 或 作者是 deliver)\n"
	       "   (或者)请输入限制条件: 作者不是 auto 且 作者不是 deliver\n"
	       "   例子六: 找所有有附件的文章\n"
	       "   请输入限制条件: 标记含 @\n"
	       "   例子七: 找所有推荐星级在3星以上的文章\n"
	       "   请输入限制条件: 推荐星级高于 3\n"
	       "   例子八: 找所有推荐人数在10人以上的文章\n"
	       "   请输入限制条件: 推荐人数高于 10\n");

	getdata(5, 0, "请输入限制条件: ", select, 60, DOECHO, NA);
	getdata(6, 0,
		"请输入你希望的操作: (0)取消 (1)标记删除 (2)去掉标记删除 (9)阅读:",
		num, 2, DOECHO, YEA);
	answer = atoi(num);
	if (answer > 0 && answer < 9 && !IScurrBM) {
		clrtoeol();
		prints("您不是版主\n");
		pressreturn();
		return FULLUPDATE;
	}
	return power_action(dir, inum1, inum2, select, answer);
}
