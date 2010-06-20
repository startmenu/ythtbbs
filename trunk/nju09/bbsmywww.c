#include "bbslib.h"

static int
save_set(int t_lines, int link_mode, int def_mode, int att_mode, int doc_mode,
	 int edit_mode, int mypic_mode, int myreply_mode, int mybrd_mode);

int
bbsmywww_main()
{
	char *ptr, buf[256];
	int t_lines = 20, link_mode = 0, def_mode = 0, att_mode = 0;
	int doc_mode = 0, edit_mode = 0, mypic_mode = 0, type;
	int myreply_mode=0, mybrd_mode = 0;
	html_header(1);
	//check_msg();
	printf("<body>");
	if (!loginok || isguest)
		http_fatal("�Ҵҹ��Ͳ��ܶ��ƽ���");
	changemode(GMENU);
	if (readuservalue(currentuser->userid, 
				"t_lines", buf, sizeof (buf)) >= 0)
		t_lines = atoi(buf);
	if (readuservalue(currentuser->userid, 
				"link_mode", buf, sizeof (buf)) >= 0)
		link_mode = atoi(buf);
	if (readuservalue(currentuser->userid, 
				"def_mode", buf, sizeof (buf)) >= 0)
		def_mode = atoi(buf);
	if (readuservalue(currentuser->userid, 
				"mypic_mode", buf, sizeof (buf)) >= 0)
		mypic_mode = atoi(buf);
//      if (readuservalue(currentuser->userid, "att_mode", buf, sizeof(buf)) >= 0) att_mode=atoi(buf);
	if (readuservalue(currentuser->userid, 
				"myreply_mode", buf, sizeof (buf)) >= 0)
		myreply_mode = atoi(buf);
	if (readuservalue(currentuser->userid,
				"mybrd_mode", buf, sizeof (buf)) >= 0)
		mybrd_mode = atoi(buf);

	att_mode = w_info->att_mode;
	doc_mode = w_info->doc_mode;
	edit_mode = w_info->edit_mode;
	type = atoi(getparm("type"));
	ptr = getparm("t_lines");
	if (ptr[0])
		t_lines = atoi(ptr);
	ptr = getparm("link_mode");
	if (ptr[0])
		link_mode = atoi(ptr);
	ptr = getparm("def_mode");
	if (ptr[0])
		def_mode = atoi(ptr);
	ptr = getparm("att_mode");
	if (ptr[0])
		att_mode = atoi(ptr);
	ptr = getparm("doc_mode");
	if (ptr[0])
		doc_mode = atoi(ptr);
	ptr = getparm("edit_mode");
	if (ptr[0])
		edit_mode = atoi(ptr);
	ptr = getparm("mypic_mode");
	if (ptr[0])
		mypic_mode = atoi(ptr);
	ptr = getparm("myreply_mode");
	if (ptr[0])
		myreply_mode = atoi(ptr);
	ptr = getparm("mybrd_mode");
	if (ptr[0])
		mybrd_mode = atoi(ptr);
	printf("<h2>[%s] WWW�������˶��� [ʹ����: %s]</h2><hr noshade>", BBSNAME,
	       currentuser->userid);
//      if (type > 0)
//              return save_set(t_lines, link_mode, def_mode, att_mode);
	if (t_lines < DOC_MIN_LINES || t_lines > DOC_MAX_LINES)
		t_lines = 20;
	if (link_mode < 0 || link_mode > 1)
		link_mode = 0;
	if (att_mode < 0 || att_mode > 1)
		att_mode = 0;
	if (doc_mode < 0 || doc_mode > 1)
		doc_mode = 0;
	if (edit_mode < 0 || edit_mode > 1)
		edit_mode = 0;
	if (mypic_mode < 0 || mypic_mode > 1)
		mypic_mode = 0;

	if (type > 0)
		return save_set(t_lines, link_mode, def_mode, att_mode,
				doc_mode, edit_mode, mypic_mode, 
				myreply_mode, mybrd_mode);
	printf("<table><form action=bbsmywww>\n");
	printf("<tr class=\"tr_1\"><td class=\"td_1_1\">\n");
	printf("<input type=hidden name=type value=1>");
	printf
	    ("һ����ʾ����������</td><td class=\"td_1_2\">"
	     "<input name=t_lines type=radio value=10%s>10��"
	     "<input name=t_lines type=radio value=20%s>20��"
	     "<input name=t_lines type=radio value=30%s>30</td></tr>\n",
	     t_lines == 10 ? " checked" : "", 
	     t_lines == 20 ? " checked" : "",
	     t_lines == 30 ? " checked" : "");
	printf
	    ("<tr class=\"tr_2\"><td class=\"td_2_1\">����ʶ��</td>\n"
	     "<td class=\"td_2_2\"><input name=link_mode type=radio value=0%s>��ʶ��"
	     "<input name=link_mode type=radio value=1%s>ʶ��</td></tr>\n",
	     link_mode ? " checked" : "",
	     !link_mode ? " checked" : "");
	printf
	    ("<tr class\"tr_1\"><td class=\"td_1_1\">"
	     "����ģʽ</td><td class=\"td_1_2\">"
	     "<input name=def_mode type=radio value=0%s>һ�㡡"
	     "<input name=def_mode type=radio value=1%s>����</td></tr>\n",
	     def_mode == 0 ? " checked" : "",
	     def_mode == 1 ? " checked" : "");
	printf
	    ("<tr class=\"tr_2\"><td class=\"td_2_1\">"
	     "�༭ģʽ</td><td class=\"td_2_2\">"
	     "<input name=edit_mode type=radio value=0%s>�򵥡�"
	     "<input name=edit_mode type=radio value=1%s>"
	     "����������</td></tr>",
	     edit_mode == 0 ? " checked" : "",
	     edit_mode == 1 ? " checked" : "");
	printf
	    ("<tr class=\"tr_1\"><td class=\"td_1_1\">"
	     "����ģʽ</td><td class=\"td_1_2\">"
	     "<input name=att_mode type=radio value=0%s>��ͨ��"
	     "<input name=att_mode type=radio value=1%s>����</td></tr>\n"
	     "<tr class=\"tr_1\"><td class=\"td_1_1\"><td class=\"td_1_2\">"
	     "����Ϊ��ͨģʽ�ٶȽϿ죬��ͼƬ��ʾ�������볢������Ϊ����ģʽ��</td></tr>\n",
	     att_mode == 0 ? " checked" : "",
	     att_mode == 1 ? " checked" : "");
	printf
	    ("<tr class=\"tr_2\"><td class=\"td_2_1\">"
	     "����ģʽ</td><td class=\"td_2_1\">"
	     "<input name=doc_mode type=radio value=0%s>��ͨ��"
	     "<input name=doc_mode type=radio value=1%s>����</td></tr>"
	     "<tr class=\"tr_2\"><td class=\"td_2_1\"></td><td class=\"td_2_2\">"
	     "����Ϊ��ͨģʽ�ٶȽϿ죬��������ʾ�������볢������Ϊ����ģʽ��</td></tr>\n",
	     doc_mode == 0 ? " checked" : "",
	     doc_mode == 1 ? " checked" : "");
	printf
	    ("<tr class=\"tr_1\"><td class=\"td_1_1\">"
	     "��ʾͷ��</td><td class=\"td_1_2\">"
	     "<input name=mypic_mode type=radio value=0%s>��ʾͷ��"
	     "<input name=mypic_mode type=radio value=1%s>����ͷ��</td></tr>\n",
	     mypic_mode == 0 ? " checked" : "",
	     mypic_mode == 1 ? " checked" : "");
	printf
	    ("<tr class=\"tr_2\"><td class=\"td_2_1\">"
	     "�ظ�ģʽ</td><td class=\"td_2_2\">"
	     "<input name=myreply_mode type=radio value=0%s>���ٻظ�"
	     "<input name=myreply_mode type=radio value=1%s>�����༭</td></tr>\n",
	     myreply_mode == 0 ? " checked" : "",
	     myreply_mode == 1 ? " checked" : "");
	printf("<tr class=\"tr_2\"><td class=\"td_1_1\">"
			"�ղؼ�</td><td class=\"td_1_2\">"
			"<input name=\"mybrd_mode\" type=radio value=0%s>"
			"���İ���"
			"<input name=\"mybrd_mode\" type=radio value=1%s>"
			"Ӣ�İ���</td></tr>\n",
			mybrd_mode == 0 ? " checked" : "", 
			mybrd_mode == 1 ? " checked" : "");
	printf
	    ("<tr class=\"tr_2\"><td class=\"td_2_1\"></td><td class=\"td_2_2\">"
	     "<input type=submit value=ȷ��>��<input type=reset value=��ԭ>\n");
	printf("</td></tr></form></table>");
	showfile(MY_BBS_HOME "/wwwtmp/googleanalytics");
	printf("</body></html>\n");
	return 0;
}

static int
save_set(int t_lines, int link_mode, int def_mode, int att_mode, int doc_mode,
	 int edit_mode, int mypic_mode, int myreply_mode, int mybrd_mode)
{
	char buf[20];
	if (t_lines < 10 || t_lines > 40)
		http_fatal("���������");
	if (link_mode < 0 || link_mode > 1)
		http_fatal("���������ʶ�����");
	if (def_mode < 0 || def_mode > 1)
		http_fatal("�����ȱʡģʽ");
	if (att_mode < 0 || att_mode > 1)
		http_fatal("����ĸ���ģʽ");
	if (doc_mode < 0 || doc_mode > 1)
		http_fatal("���������ģʽ");
	if (mypic_mode < 0 || mypic_mode > 1)
		http_fatal("�����ͷ��ģʽ");
	if (mypic_mode < 0 || mypic_mode > 1)
		http_fatal("����Ļظ�ģʽ");
	if (mybrd_mode < 0 || mybrd_mode > 1)
		http_fatal("������ղؼа���ģʽ");

	sprintf(buf, "%d", t_lines);
	saveuservalue(currentuser->userid, "t_lines", buf);
	sprintf(buf, "%d", link_mode);
	saveuservalue(currentuser->userid, "link_mode", buf);
	sprintf(buf, "%d", def_mode);
	saveuservalue(currentuser->userid, "def_mode", buf);
	sprintf(buf, "%d", att_mode);
	saveuservalue(currentuser->userid, "att_mode", buf);
	sprintf(buf, "%d", doc_mode);
	saveuservalue(currentuser->userid, "doc_mode", buf);
	sprintf(buf, "%d", edit_mode);
	saveuservalue(currentuser->userid, "edit_mode", buf);
	sprintf(buf, "%d", mypic_mode);
	saveuservalue(currentuser->userid, "mypic_mode", buf);
	sprintf(buf, "%d", myreply_mode);
	saveuservalue(currentuser->userid, "myreply_mode", buf);
	sprintf(buf, "%d", mybrd_mode);
	saveuservalue(currentuser->userid, "mybrd_mode", buf);
	w_info->t_lines = t_lines;
	w_info->def_mode = def_mode;
	w_info->link_mode = link_mode;
	w_info->att_mode = att_mode;
	w_info->doc_mode = doc_mode;
	w_info->edit_mode = edit_mode;
	w_info->mypic_mode = mypic_mode;
	printf("WWW���Ʋ����趨�ɹ�.<br>\n");
	printf("[<a href='javascript:history.go(-2)'>����</a>]");
	return 0;
}
