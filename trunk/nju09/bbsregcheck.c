#include "bbslib.h"
#define isletter(a)	(a>= 'A' && a <= 'Z')?1:((a>= 'a' && a <= 'z')?1:0)

int
bbsregcheck_main()
{
	char file[STRLEN];
	html_header(1);
	printf("<body>");
  char userid[IDLEN+2];
  int r;
  //char email[42],email2[42];
  strsncpy(userid, getparm("userid"), sizeof (userid));
  //strsncpy(email, getparm("email"), sizeof (email));
  //strsncpy(email2, getparm("email2"), sizeof (email2));

	if (strlen(userid) < 2 || (strlen(userid) == 2 && !(isletter(userid[0]) && isletter(userid[1])) ) ) {
  		printf("<li>��Ǹ���ʺų���̫�̻��ʺ���Ӣ����ĸ��������(2-12����Ӣ���ַ�),�޷�ע�ᣬ������ѡ��");
		  return 0;
		}
	if (strlen(userid)>12) {
	   printf("<li>��Ǹ���ʺų���̫��(2-12����Ӣ���ַ�)���޷�ע�ᣬ������ѡ��");
	   return 0;
	}
	if (is_bad_id(userid)) {
    printf("<li>��Ǹ��%sΪϵͳ�����ʺţ��޷�ע�ᣬ����ѡһ��",userid);
    return 0;
  }
	if (!goodgbid(userid)) {
		printf("<li>��Ǹ���ʺ�ֻ����Ӣ����ĸ�ͱ�׼������ɣ�������ѡ��");
		return 0;
	}
	r=user_registered(userid);
	if (r==0) {
      printf("<li>��ϲ��%sδ��ע�ᣬ������ʹ�ø��ʺ�",userid);
      return 0;
  }
	if (r > 0) {
			printf("<li>%s�Ѿ�����ʹ��,������ѡ��",userid);
			return 0;
	}
	if (r < 0) {
			printf("%s�ո�����������������������������ѡ��id",userid);
			return 0;
	}
	errlog("www reg error: %s", userid);
	http_fatal("�ڲ�����");
//printf("%s,%s,%s",userid,email,email2);
	showfile(MY_BBS_HOME "/wwwtmp/googleanalytics");
  printf("</body></html>");
  return 0;
}
