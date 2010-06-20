#include "bbslib.h"
#define ONLINELIMIT 7000
#define SEARCHID "002851670354406323673:j5jyxomeixu"

void
printdiv(int *n, char *str)
{
	printf("<div id=div%da class=r><A href='javascript:changemn(\"%d\");'>",
	       *n, *n);
	printf("<img border=0 id=img%d src=/folder.gif>%s</A></div>\n", *n,
	       str);
	printf("<div id=div%d class=s>\n", (*n)++);
}

void
printsectree(const struct sectree *sec)
{
	int i;
	for (i = 0; i < sec->nsubsec; i++) {
#if 0
		if (sec->subsec[i]->nsubsec)
			continue;
#endif
		printf("&nbsp; <a target=_top href=boa?secstr=%s>"
		       "%s%c</a><br>\n", sec->subsec[i]->basestr,
		       nohtml(sec->subsec[i]->title),
		       sec->subsec[i]->nsubsec ? '+' : ' ');
	}
}

int
bbsleft_main()
{
	int i;
	int div = 0;
//	changemode(MMENU);
	if (0)
		errlog("%s-%s-%s-%s", getsenv("HTTP_ACCEPT_LANGUAGE"),
		       getsenv("Accept"), getsenv("Accept-Charset"),
		       getsenv("Accept-Encoding"));
	html_header(2);
#if 0
	{
		char *ptr;
		char buf[256];
		ptr = getsenv("HTTP_USER_AGENT");
		sprintf(buf, "%-14.14s %.100s", currentuser->userid, ptr);
		addtofile(MY_BBS_HOME "/browser.log", buf);
	}
#endif
	printf("<script src=" BBSLEFTJS "></script>\n"
		"<script src=" BBSJS "></script>\n"
		"<script src=" BBSAJAXJS "></script>\n"
		"<script src=" BBSJSONJS "></script>\n"
		"<script src=" BBSBRDJS "></script>\n"
	       "<body onMouseOver='doMouseOver()' "
	       "onMouseEnter='doMouseOver()' "
	       "onMouseOut='doMouseOut()'>\n<nobr>");
	if (!loginok || isguest) {
		printf("<table width=100%%>\n");
/*����ȥ���˿�ܣ���ʱȥ��https����ת֧��
#ifdef HTTPS_DOMAIN
#ifdef USESESSIONCOOKIE
		printf("<form name=l action=https://%s/" SMAGIC "/bbslogin "
				"method=post target=_top>", 
				getsenv("HTTP_HOST"));
#else
		printf("<form name=l action=https://" HTTPS_DOMAIN
		       "/" SMAGIC "/bbslogin method=post target=_top>");
#endif
		printf("<input type=hidden name=usehost "
				"value='http%s://%s:%s'>",
		       strcasecmp(getsenv("HTTPS"), "ON")?"":"s",
		       getsenv("HTTP_HOST"), getsenv("SERVER_PORT"));
#else
		printf("<form name=l action=bbslogin method=post target=_top>");
#endif
*/
		printf("<form name=l action=bbslogin method=post target=_top>");
		printf("<tr><td>"
		       "<input type=hidden name=lastip1 value=''>"
		       "<input type=hidden name=lastip2 value=''>"
		       "�ʺ�<input type=text name=id maxlength=%d size=11><br>"
		       "����<input type=password name=pw maxlength=%d size=11><br>"
		       "<a href=/ipmask.html target=_blank>��Χ</a>"
			"<select name=ipmask style='width: 78px;'>\n"
		       "<option value=0 selected>��IP</option>\n"
		       "<option value=1>2 IP</option>\n"
		       "<option value=2>4 IP</option>\n"
		       "<option value=3>8 IP</option>\n"
		       "<option value=4>16 IP</option>\n"
		       "<option value=5>32 IP</option>\n"
		       "<option value=6>64 IP</option>\n"
		       "<option value=7>128 IP</option>\n"
		       "<option value=8>256 IP</option>\n"
		       "<option value=15>32K IP</option></select>"
		       "<input type=submit value=��¼>&nbsp;&nbsp;"
		       "<input type=submit value=ע�� onclick=\"{top.location.href='/"
		       SMAGIC "/bbsemailreg';return false}\">\n"
		       "</td></tr></form></table>\n", IDLEN, PASSLEN - 1);
	} else {
		char buf[256] = "δע���û�";
		printf
		    ("�û�: <a href=bbsqry?userid=%s target=_top>%s</a><br>",
		     currentuser->userid, currentuser->userid);
		if (currentuser->userlevel & PERM_LOGINOK)
			strcpy(buf, cuserexp(currentuser->exp_group, countexp(currentuser, 2)));
		if (currentuser->userlevel & PERM_BOARDS)
			strcpy(buf, "����");
		if (currentuser->userlevel & PERM_XEMPT)
			strcpy(buf, "�����ʺ�");
		if (currentuser->userlevel & PERM_SYSOP)
			strcpy(buf, "��վվ��");
		printf("����: %s<br>", buf);
		printf("<a href=bbslogout target=_top>ע�����ε�¼</a><br>\n");
	}
	printf("<hr>");
	check_msg();  //���������Ϣ����print����  
	printf("&nbsp;&nbsp;<a target=_top href=boa?secstr=?>һ·bbs����</a><br>\n");
	printf("&nbsp;&nbsp;<a href=\"/ku\" target=\"_blank\">һ·���������£�</a><br>\n");
	printf("&nbsp;&nbsp;<a target=_top href=bbs0an>����������</a><br>\n");
	printf("&nbsp;&nbsp;<a target=_top href=bbstop10>ʮ�����Ż���</a><br>\n");
	printf("&nbsp;&nbsp;<a target=_top href=digest?C=0>���վ��ʻ���</a><br>\n");
#if ENABLE_BLOG
	printf("&nbsp; <a target=_blank href=blogpage>һ·����</a><br>\n");
#endif
//	printf("&nbsp; <a target==_BLANK href=http://yjrg.net/wiki><font color=brown>���֪ʶ��</font></a><br>\n");
	if (loginok && !isguest) {
		char buf[10];
		unsigned int mybrdmode;
		readuservalue(currentuser->userid, "mybrd_mode", 
				buf, sizeof(buf));
		mybrdmode = atoi(buf);
		printdiv(&div, "����������");
		bbsmybrd_show_left(mybrdmode);
		printf
		    ("&nbsp;&nbsp;<a target=_top href=bbsboa?secstr=*>Ԥ��������</a><br>\n");
		printf
		    ("&nbsp;&nbsp;<a target=_top href=bbsmybrd?mode=1>Ԥ������</a><br>\n");
		printf("</div>\n");
	}
	printdiv(&div, "����������");
	printsectree(&sectree);
	printf("</div>\n");
#if 0
	printf("<div class=r>");
	for (i = 0; i < sectree.nsubsec; i++) {
		const struct sectree *sec = sectree.subsec[i];
		if (!sec->nsubsec)
			continue;
		printf
		    ("--<a target=_top href=bbsboa?secstr=%s>%s</a><br>\n",
		     sec->basestr, sec->title);
	}
	printf("</div>\n");
#endif
	printdiv(&div, "̸��˵��");
	if (loginok && !isguest) {
		printf
		    ("&nbsp;&nbsp;<a href=bbsfriend target=_top>���ߺ���</a><br>\n");
	}
//      printf
//          ("&nbsp;&nbsp;<a href=bbsufind?search=A&limit=20 target=_top>�����ķ�</a><br>\n");
	printf("&nbsp;&nbsp;<a href=bbsqry target=_top>��ѯ����</a><br>\n");
	if (currentuser->userlevel & PERM_PAGE) {
		printf
		    ("&nbsp;&nbsp;<a href=bbssendmsg target=_top>���Ͷ���Ϣ</a><br>\n");
		printf
		    ("&nbsp;&nbsp;<a href=bbsmsg target=_top>�鿴���ж���Ϣ</a><br>\n");
	}
	printf("</div>\n");
	if (loginok && !isguest) {
#ifdef HTTPS_DOMAIN
		char str[STRLEN + 10], *ptr;
		//char taskfile[256];
#endif
		printdiv(&div, "��������");
#ifdef HTTPS_DOMAIN
		strsncpy(str, getsenv("SCRIPT_URL"), STRLEN);
		ptr = strrchr(str, '/');
		if (ptr)
			strcpy(ptr, "/bbspwd");
		printf("&nbsp;&nbsp;<a target=_top href=https://" HTTPS_DOMAIN
		       "%s>�޸�����</a><br>", str);
#else
		printf("&nbsp;&nbsp;<a target=_top href=bbspwd>�޸�����</a><br>");
#endif
		printf("&nbsp;&nbsp;<a target=_top href=bbsinfo>�������Ϻ�ͷ��</a><br>"
		       "&nbsp;&nbsp;<a target=_top href=bbsplan>��˵����</a><br>"
		       "&nbsp;&nbsp;<a target=_top href=bbssig>��ǩ����</a><br>"
		       "&nbsp;&nbsp;<a target=_top href=bbsparm>�޸ĸ��˲���</a><br>"
		       "&nbsp;&nbsp;<a target=_top href=bbsmywww>WWW���˶���</a><br>"
		       "&nbsp;&nbsp;<a target=_top href=bbsmyclass>������ʾ�İ���</a><br>"
		       "&nbsp;&nbsp;<a target=_top href=bbsnick>��ʱ���ǳ�</a><br>"
		       "&nbsp;&nbsp;<a target=_top href=bbsstat>����ͳ��</a><br>"
		       "&nbsp;&nbsp;<a target=_top href=bbsfall>�趨����</a><br>");

		if (currentuser->userlevel & PERM_CLOAK)
			printf("&nbsp;&nbsp;<a target=_top "
			       "onclick='return confirm(\"ȷʵ�л�����״̬��?\")' "
			       "href=bbscloak>�л�����</a><br>\n");
		printf("</div>");
		printdiv(&div, "�����ż�");
		printf("&nbsp;&nbsp;<a target=_top href=bbsnewmail>δ���ʼ�</a><br>"
		       "&nbsp;&nbsp;<a target=_top href=bbsmail>�����ʼ�</a><br>"
		       "&nbsp;&nbsp;<a target=_top href=bbspstmail>�����ʼ�</a><br>"
		       "</div>");
//		       "&nbsp;&nbsp;<a target=_top href=bbsspam>�����ʼ�</a><br>"

	}
	printdiv(&div, "�ر����");
	//printf("&nbsp;&nbsp;<a target=_top href=bbssechand>�����г�</a><br>\n");
	printf("&nbsp;&nbsp;<a target=_top href=/wnl.html>������</a><br>\n");
	//�����ر������ע�͵ĵط���ʱ���˻���û�д�
	//printf("&nbsp;&nbsp;<a target=_top href=/cgi-bin/cgincce>�Ƽ��ʵ�</a><br>\n");
	printf
	    ("&nbsp;&nbsp;<a target=_top href=/scicalc.html>��ѧ������</a><br>\n");
	//printf("&nbsp;&nbsp;<a target=_top href=/periodic/periodic.html>Ԫ�����ڱ�</a><br>\n");
	//printf("&nbsp;&nbsp;<a target=_top href=/cgi-bin/cgiman>Linux�ֲ��ѯ</a><br>\n");
	printf("&nbsp;&nbsp;<a href=bbsfind target=_top>���²�ѯ</a><br>\n");
	//printf("&nbsp;&nbsp;<a target=_top href=/cgi-bin/cgifreeip>IP��ַ��ѯ</a><br>\n");
	//printf("&nbsp;&nbsp;<a target=_top href=bbs0an>����������</a><br>\n");
	//printf("&nbsp;&nbsp;<a target=_top href=bbsx?chm=1>���ؾ�����</a><br>\n");
	//printf("&nbsp;&nbsp;<a target=_top href=http://%s/tools/telnet_tools/index.html>" "Telnet��������</a><br>\n", getsenv("HTTP_HOST"));
	printf("</div>\n");
if (loginok && !isguest &&
                    (currentuser->userlevel & PERM_DEFAULT) == PERM_DEFAULT) {
	printdiv(&div, "�ҵĵ���");
  if (*system_load() >= 1.7 || count_online() > ONLINELIMIT)
	     printf("&nbsp;&nbsp;�ҵ����ӣ�ϵͳ���ع��ߣ���ͣʹ�ã�<br>\n");
  else
  	   printf("&nbsp;&nbsp;<a target=_top href=bbsfind?user=%s&amp;day=180>�ҵ�����</a><br>\n",currentuser->userid);
#ifdef ENABLE_BLOG  	   
  if (currentuser->hasblog)
	printf("&nbsp;&nbsp;<a target=_top href=blog?U=%s>�ҵ�Blog</a><br>\n",currentuser->userid);
#endif
		if (isalpha(currentuser->userid[0]) && (currentuser->userlevel & PERM_SPECIAL8)) {
		printf
		    ("&nbsp;&nbsp;<a target=_top href=bbs0an?path=/groups/GROUP_0/Personal_Corpus/%c/%s>�ҵ��ļ�</a>",
		     toupper(currentuser->userid[0]), currentuser->userid);
	}
}
	printf("</div>\n");
	printf("<div class=r>");
//	printf("&nbsp;&nbsp;<a target=_top href=bbs0an?path=/groups/GROUP_0/Personal_Corpus>�����ļ���</a><br>\n");
	printf("&nbsp;&nbsp;<a target=_top href=bbsall>����������</a><br>\n");
	#ifdef ENABLE_INVITATION
                if (loginok && !isguest &&
                    (currentuser->userlevel & PERM_DEFAULT) == PERM_DEFAULT)
                        printf
                            ("&nbsp;&nbsp;<a target=_top href=bbsinvite>��������</a><br>");
	#endif

	bbsadv_show(2);
	printf("<hr>");
	printf("<table width=100%%><tr><form action=bbssearchboard method=post target=_top><td><div align=center>"
		"<input type=text style='width:100px' name=match maxlength=24 "
		"size=9 value=���������� onclick=\"this.select()\" align=left></div></td></form></tr></table>\n");
	printf("<hr>");
	printf("&nbsp;&nbsp;<a href='telnet:%s'>Telnet��¼</a><br>\n", BBSHOST);
	printf("&nbsp;&nbsp;<a target=_top href=home?B=BBSHelp>�û�����</a>\n");
#if 0	/* ��ȥ�����пոĳ��ٷ���һ��ע��ȷ���� */
	if (loginok && !isguest && !(currentuser->userlevel & PERM_LOGINOK)
	    && !has_fill_form(currentuser->userid))
		printf
		    ("<br>&nbsp;&nbsp;<a target=_top href=bbsform><font color=red>��дע�ᵥ</font></a>\n");
#endif
	if (loginok && !isguest && USERPERM(currentuser, PERM_ACCOUNTS))
		printf
		    ("<br>&nbsp;&nbsp;<a href=bbsscanreg target=_top>SCANREG</a>");
	if (loginok && !isguest && USERPERM(currentuser, PERM_SYSOP))
		printf("<br>&nbsp;&nbsp;<a href=kick target=_top>��www��վ</a>");
	//if(loginok && !isguest) printf("<br>&nbsp;&nbsp;<a href='javascript:openchat()'>bbs���</a>");
	printf
	    ("<br>&nbsp;&nbsp;<a href=bbsselstyle target=_top>�������濴��</a>");
	printf("<br>&nbsp;&nbsp;��ǰ����[%d] ", count_online());
	printf("<hr />\n");
	printf("<!-- SiteSearch Google --><form method=\"get\" action=\"http://www.google.com/custom\" target=\"google_window\"><table border=\"0\"><tr><td nowrap=\"nowrap\" valign=\"top\" align=\"left\" height=\"32\"></td><td nowrap=\"nowrap\"><input type=\"hidden\" name=\"domains\" value=\"yilubbs.com\"></input><label for=\"sbi\" style=\"display: none\">�������������ִ�</label><input type=\"text\" name=\"q\" size=\"15\" maxlength=\"255\" value=\"\" id=\"sbi\"></input></td></tr><tr><td>&nbsp;</td><td nowrap=\"nowrap\"><table><tr><td><input type=\"radio\" name=\"sitesearch\" value=\"yilubbs.com\" id=\"ss1\" checked></input><label for=\"ss1\" title=\"���� yilubbs.com\"><font size=\"-1\" color=\"#000000\">yilubbs.com</font></label></td><td></td></tr></table><label for=\"sbb\" style=\"display: none\">�ύ������</label><input type=\"submit\" name=\"sa\" value=\"Google ����\" id=\"sbb\"></input><input type=\"hidden\" name=\"client\" value=\"pub-7608613947207155\"></input><input type=\"hidden\" name=\"forid\" value=\"1\"></input><input type=\"hidden\" name=\"ie\" value=\"GB2312\"></input><input type=\"hidden\" name=\"oe\" value=\"GB2312\"></input><input type=\"hidden\" name=\"cof\" value=\"GALT:#008000;GL:1;DIV:#FFFFFF;VLC:663399;AH:center;BGC:FFFFFF;LBGC:FFFFFF;ALC:0000FF;LC:0000FF;T:000000;GFNT:0000FF;GIMP:0000FF;LH:50;LW:110;L:http://www.yilubbs.com/ku/images/logo.gif;S:http://www.yilubbs.com;FORID:1\"></input><input type=\"hidden\" name=\"hl\" value=\"zh-CN\"></input></td></tr></table></form><!-- SiteSearch Google -->");
	/*
	printf("<hr />\n"
		"<!-- Google CSE Search Box Begins -->\n"
		"<div align=center>"
		"<form action=\"/fulltext.htm\" id =\"" SEARCHID "\" "
		"target=_blank>\n"
		"<input type=\"hidden\" name=\"cx\" value=\"" SEARCHID "\" />\n"
		"<input type=\"hidden\" name=\"cof\" value=\"FORID:11\" />\n"
		"<input type=\"text\" name=\"q\" size=\"14\" />\n<br>\n"
		"<input type=\"submit\" name=\"sa\" value=\"ȫ������\" />\n"
		"</form>"
		"<script type=\"text/javascript\" src=\"http://"
		"www.google.com/coop/cse/brand?form=searchbox_" SEARCHID 
		"\">\n</script>\n"
		"</div>"
		"<!-- Google CSE Search Box Ends -->");
	*/
	if (1 || strcmp(MY_BBS_ID, "YTHT"))
		printf("<br><br><center><img src=/coco.gif>");
	else {
		printf
		    ("<br><center><a href=http://www.cbe-amd.com target=_blank><img src=/cbe-amd.gif border=0></a>");
		printf
		    ("<br><center><a href=http://www.amdc.com.cn/products/cpg/amd64/ target=_blank><img src=/AMD64_logo.gif border=0></a>");
	}
	printf ("<br><a target=_top href=bug ><strong>����Bug</strong></a>");
	printf("</div>");
	printf("<script>if(isNS4) arrange();if(isOP)alarrangeO();</script>");
	if (loginok && !isguest) {
		if (USERPERM(currentuser, PERM_LOGINOK)
		    && !USERPERM(currentuser, PERM_POST))
			printf
			    ("<script>alert('���������ȫվ�������µ�Ȩ��, ��ο�sysop�湫��, ��������sysop��������. ��������, ���Ÿ�arbitration�ʺ�Ͷ��)</script>\n");
		mails(currentuser->userid, &i);
		if (i > 0)
			printf("<script>alert('�������ż�!')</script>\n");
	}
	// if(loginok&&currentuser.userdefine&DEF_ACBOARD)
	//              printf("<script>window.open('bbsmovie','','left=200,top=200,width=600,height=240');</script>"); 
	//virusalert();
	if (isguest && 0)
		printf
		    ("<script>setTimeout('open(\"regreq\", \"winREGREQ\", \"width=600,height=460\")', 1800000);</script>");
	if (loginok && !isguest) {
		char filename[80];
		sethomepath(filename, currentuser->userid);
		mkdir(filename, 0755);
		sethomefile(filename, currentuser->userid, BADLOGINFILE);
		if (file_exist(filename)) {
			printf("<script>"
			       "window.open('bbsbadlogins', 'badlogins', 'toolbar=0, scrollbars=1, location=0, statusbar=1, menubar=0, resizable=1, width=450, height=300');"
			       "</script>");
		}
	}
	bbsadv_show(0);
	if (!via_proxy && wwwcache->text_accel_port
	    && wwwcache->text_accel_addr.s_addr)
		printf("<script src=http://proxy.%s:%d/testdoc.js></script>",
		       MY_BBS_DOMAIN,
		       wwwcache->text_accel_port);
	else if (via_proxy)
		w_info->doc_mode = 0;
	//printf("<script src=/testdoc.js></script>");
	if (!loginok || isguest)
		setlastip();
	printf("</body></html>");
	return 0;
}

/*
 * void
virusalert()
{
	if (file_has_word("virusalert.txt", realfromhost)) {
		printf
		    ("<script>window.open('/virusalert.html','','left=200,top=200,width=250,height=80');</script>");
	}
}
*/
