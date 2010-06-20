#include "bbslib.h"

#define	MY_BBS_SLOGAN	"������ʶ�����!"
static int
printInviteForm()
{
	printf("<center><div style='width:600' align=left>"
	       "<b>�����Լ������ѵ�" MY_BBS_NAME " ��</b><i>  -----���Ѿ����%d�㸽�Ӿ���ֵ-----</i><br><br>"
	       "��д���µı��ϵͳ���Զ�����һ�������Ÿ�������ѣ�"
	       "�����/���յ����ţ����ҽ���ע�ᣬϵͳ���Զ��Ѹ��û����뵽��ĺ��������"
	       "ͬʱ��ľ���ֵҲ������ 20 �㣡<br>&nbsp;"
	       "<form action=bbsinvite?doit=1 method=post>"
	       "<table><tr><td align=right><nobr>��/���� email��</nobr></td>"
	       "<td align=left><input type=text name=ivemail maxlength=55 value=''></td></tr>"
	       "<tr><td align=right>������֣�</td>"
	       "<td align=left><input type=text name=myname value=''></td></tr>"
	       "<tr><td align=right>��/�������֣�</td>"
	       "<td align=left><input type=text name=ivname value=''></td></tr>"
	       "<tr><td align=right>�������ԣ�</td>"
	       "<td align=left><textarea name=note cols=65 wrap=virtual>"
	       MY_BBS_SLOGAN "����" MY_BBS_NAME " �������û���~"
	       "</textarea></td></tr>"
	       "</table><input type=submit value='ȷ��'></form>"
	       "</div></center>",currentuser->extraexp1 * 20);
	return 0;
}

int
bbsinvite_main()
{
	char myname[40], ivemail[60], ivname[40], *note;
	char tmpfn[256];
	int retv;
	html_header(1);
	//check_msg();
	printf("<body>");
	if (!loginok || isguest
	    || (currentuser->userlevel & PERM_DEFAULT) != PERM_DEFAULT)
		http_fatal("�Ҵҹ����޷���������, ���ȵ�¼");
	changemode(MMENU);
	if (atoi(getparm("doit")) == 0) {
		printInviteForm();
		http_quit();
		return 0;
	}

	strsncpy(myname, getparm("myname"), sizeof (myname));
	strsncpy(ivemail, getparm("ivemail"), sizeof (ivemail));
	strsncpy(ivname, getparm("ivname"), sizeof (ivname));
	note = getparm("note");
	if (!*ivemail || !*ivname)
		http_fatal("�������˵� email �����ֶ���Ҫ��д");
	if (!trustEmail(ivemail))
		http_fatal("�� email ��Ч��������ʹ�����Ѿ��ڱ�վע��");
	sprintf(tmpfn, "bbstmpfs/tmp/bbsinvite.%d",  getpid());
	f_write(tmpfn, note);
	retv = sendInvitation(currentuser->userid, myname, ivemail, ivname, tmpfn);
	unlink(tmpfn);
	if (retv < 0) {
		http_fatal("���� email ʧ�ܣ����� %d", retv);
	}
	tracelog("%s invite %s %s", currentuser->userid, ivemail, ivname);

	printf("<br>�ɹ��� %s �������룡<br>", ivname);
	printf("<a href=bbsinvite>�ٷ��ͼ�������</a>");
	http_quit();
	return 0;
}
