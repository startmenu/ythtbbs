#include "bbslib.h"
#include <openssl/des.h>

static des_cblock iv;

int key_fail;

static unsigned char magic[2][8];
static des_key_schedule ks[2][3];

static int
load_key(char *file, int key)
{
	FILE *fp;
	des_cblock k;
	int i;
	fp = fopen(file, "r");
	if (NULL == fp)
		return -1;
	fread(magic[key], 1, 8, fp);
	for (i = 0; i < 3; i++) {
		fread(&k, 1, sizeof (k), fp);
		des_set_key_checked(&k, ks[key][i]);
	}
	memcpy(&iv, &k, sizeof (k));
	fclose(fp);
	return 0;
}

int
load_all_key(void)
{
	if (load_key("etc/my_key", 0))
		return -1;
	if (load_key("etc/service_key", 1))
		return -1;
	return 0;
}

char *
des3_encode(char *id, int key)
{
	unsigned char buf[24], out[24];
	static char result[65];
	des_cblock this_iv;
	memcpy(&this_iv, &iv, sizeof (iv));
	memcpy(buf, &now_t, 4);
	memcpy(buf + 4, id, IDLEN);
	memcpy(buf + 16, magic[key], 8);
	des_ede3_cbc_encrypt(buf, out, 24, ks[key][0], ks[key][1],
			     ks[key][2], &iv, 1);
	hex_encode(this_iv, 8, result);
	hex_encode(out, 24, result + 16);
	return result;
}

char *
des3_decode(char *buf, int key)
{
	des_cblock this_iv;
	char tmp[24], out[16];
	static char id[IDLEN + 1];
	id[0] = 0;
	if (strlen(buf) != 64)
		return id;
	strsncpy(tmp, buf, 17);
	hex_decode(tmp, this_iv);
	hex_decode(buf + 16, tmp);
	des_ede3_cbc_encrypt(tmp, out, 24, ks[key][0], ks[key][1],
			     ks[key][2], &this_iv, 0);
	if (memcmp(out + 16, magic[key], 8))
		return id;
	if (now_t < *((time_t *) out) || now_t > *((time_t *) out) + 86400)
		return id;
	strsncpy(id, out + 4, IDLEN + 1);
	return id;
}

int
bbslpassport_main()
{
	int uid, infochanged = 0;
	char id[IDLEN + 1], pw[PASSLEN], site[256], md5pass[MD5LEN], buf[384];
	struct userec *x, tmpu;
	time_t t, dtime;
	html_header(3);
	strsncpy(id, strtrim(getparm("id")), IDLEN + 1);
	strsncpy(pw, getparm("pw"), PASSLEN);
	strsncpy(site, getparm("site"), 256);

	if (!id[0])
		http_fatal("�������ʺ�");
	if (!site[0])
		http_fatal("no...");
	if (key_fail)
		http_fatal("�ڲ�����, ��ϵά��!\n");
	if ((uid = getuser(id, &x)) <= 0) {
		printf("%s<br>", id);
		http_fatal("�����ʹ�����ʺ�");
	}
	strcpy(id, x->userid);
	if (!strcasecmp(id, "guest"))
		http_fatal("�����ʹ�����ʺ�");

	if (checkbansite(fromhost)) {
		http_fatal
		    ("�Բ���, ��վ����ӭ���� [%s] �ĵ�¼. <br>��������, ����SYSOP��ϵ.",
		     fromhost);
	}
	if (userbansite(x->userid, fromhost))
		http_fatal("��ID�����ý�ֹ��%s��¼", fromhost);
	if (!checkpasswd(x->passwd, x->salt, pw)) {
		logattempt(x->userid, fromhost, "PASSPORT", now_t);
		http_fatal
		    ("�������������������ϵվ���飬�ṩע�������һ�����");
	}
#if 0
	if (!user_perm(x, PERM_BASIC))
		http_fatal
		    ("���ڱ��ʺ����Ʋ������ʺŹ���취���Ѿ�������Ա��ֹ������վ��<br>���������ʺŵ�¼�� <font color=red>"
		     DEFAULTBOARD "</font> ��ѯ��.");
	if (file_has_word(MY_BBS_HOME "/etc/prisonor", x->userid)) {
		if (x->inprison == 0) {
			memcpy(&tmpu, x, sizeof (tmpu));
			tmpu.inprison = 1;
			tmpu.dieday = 2;
			updateuserec(&tmpu, 0);
		}
		http_fatal("���ĸ��죬��Ҫ����");
	}
	if (x->dieday)
		http_fatal("����?��Ҫ��ʲô? :)");
#endif
	t = x->lastlogin;
	memcpy(&tmpu, x, sizeof (tmpu));
	if (tmpu.salt == 0) {
		tmpu.salt = getsalt_md5();
		genpasswd(md5pass, tmpu.salt, pw);
		memcpy(tmpu.passwd, md5pass, MD5LEN);
		infochanged = 1;
	}
#if 1
	if (count_uindex(uid) == 0) {
		if (now_t - t > 1800)
			tmpu.numlogins++;
		infochanged = 1;
		tmpu.lastlogin = now_t;
		dtime = t - 4 * 3600;
		t = localtime(&dtime)->tm_mday;
		dtime = now_t - 4 * 3600;
		if (t < localtime(&dtime)->tm_mday && x->numdays < 60000) {
			tmpu.numdays++;
		}
	}
#endif
	if (abs(t - now_t) < 20) {
		http_fatal("���ε�¼�������!");
	}

	if (x->lasthost != from_addr.s_addr) {
		tmpu.lasthost = from_addr.s_addr;
		infochanged = 1;
	}
	if (infochanged)
		updateuserec(&tmpu, 0);
	tracelog("%s enter %s passport %d %s", x->userid, fromhost, infochanged,
		 getsenv("HTTP_X_FORWARDED_FOR"));
	printf
	    ("<script>exDate = new Date; exDate.setMonth(exDate.getMonth()+9);"
	     "document.cookie='pp=%s;path=/;expires=' + exDate.toGMTString();</script>",
	     des3_encode(id, 0));
	snprintf(buf, sizeof (buf), "http://%s?q=%s", site, des3_encode(id, 1));
	redirect(buf);
	http_quit();
	return 0;
}
