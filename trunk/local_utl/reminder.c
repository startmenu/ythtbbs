//by ecnegrevid 2001.9.29
//������Ϣ������Ը�������, ���ӵ�����д��etc/reminder.extra��
#include "../include/bbs.h"
#include "ythtlib.h"
#include "ythtbbs.h"

int
sendreminder(struct userec *urec)
{
	char cmd[1024], *ptr;
	time_t t;
	FILE *fp;
	struct userdata udata;
	static struct mmapfile mf = { ptr:NULL };
	if (!mf.ptr)
		mmapfile(MY_BBS_HOME "/etc/reminder.extra", &mf);
	loaduserdata(urec->userid, &udata);
	if (strchr(udata.email, '@') == NULL)
		return 0;
	ptr = udata.email;
	while (*ptr) {
		if (!isalnum(*ptr) && !strchr(".@", *ptr))
			return 0;
		ptr++;
	}
	if (strcasestr(udata.email, ".bbs@ytht.") != NULL)
		return 0;
	sprintf(cmd, "/usr/lib/sendmail -f SYSOP.bbs@%s %s", MY_BBS_DOMAIN,
		udata.email);
	fp = popen(cmd, "w");
	if (fp == NULL)
		return 0;
	fprintf(fp, "Return-Path: SYSOP.bbs@%s\n", MY_BBS_DOMAIN);
	fprintf(fp, "Reply-To: SYSOP.bbs@%s\n", MY_BBS_DOMAIN);
	fprintf(fp, "From: SYSOP.bbs@%s\n", MY_BBS_DOMAIN);
	fprintf(fp, "To: %s\n", udata.email);
	fprintf(fp, "Subject: ϵͳ���ѣ�" MY_BBS_NAME "��\n\n");
	fprintf(fp,
		"    " MY_BBS_NAME "(" MY_BBS_DOMAIN
		")���û����ã����ڱ�վע���\n"
		"�ʺ� %s �����������Ѿ����͵� 10����Ҫ����\n"
		"���ʺŵ�½һ�β���ʹ�������ָ���������ʻ�������\n"
		"��ע��ģ���������žͿ����ˡ�\n\n" "������������˵��:\n"
		"    ��BBSϵͳ�ϣ�ÿ���ʺŶ���һ�������������û���\n"
		"��¼������£�������ÿ�����1�������������ٵ�0��ʱ\n"
		"���ʺžͻ��Զ���ʧ���ʺ�ÿ�ε�¼���������ͻָ���\n"
		"һ���̶�ֵ������ͨ��ע������Ѿ���¼4�ε��û������\n"
		"�̶�ֵ������120������ͨ��ע�ᵫ��¼����4�ε��û���\n"
		"����̶�ֵ��30������δͨ��ע����û�������̶�ֵ��15��\n\n",
		urec->userid);
	if (mf.ptr)
		fwrite(mf.ptr, 1, mf.size, fp);
	fputs("\n.\n", fp);
	pclose(fp);
	if ((fp = fopen(MY_BBS_HOME "/reminder.log", "a")) != NULL) {
		t = time(NULL);
		ptr = ctime(&t);
		ptr[strlen(ptr) - 1] = 0;
		fprintf(fp, "%s %s %s\n", ptr, urec->userid, udata.email);
		fclose(fp);
	}
	sleep(2);
	return 0;
}

int
main(int argc, char *argv[])
{
	int fd1;
	struct userec rec;
	int size1 = sizeof (rec);

	if ((fd1 = open(PASSFILE, O_RDONLY, 0660)) == -1) {
		perror("open PASSWDFILE");
		return -1;
	}

	while (read(fd1, &rec, size1) == size1) {
		if (!rec.userid[0])
			continue;
		if (countlife(&rec) == 10||!strcmp(rec.userid,"lepton")) {
			sendreminder(&rec);
		}
	}
	close(fd1);
	return 0;
}
