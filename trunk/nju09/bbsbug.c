#include "bbslib.h"

#define REQUEST_TITLE_LEN  64
#define REQUEST_MODULE_LEN 12
#define REQUEST_LIST ".request_list"
#define REQUEST_LOG_DIR "request_log"
#define REQUEST_BOARD "YJRG_Development"

typedef enum {
	REQUEST_ACTIVE=1,
	REQUEST_FIXED,
	REQUEST_CLOSED,
} request_status;

static char * const request_status_str[]={
	"��Ծ",
	"���",
	"�ر�",
	NULL
};

typedef enum {
	REASON_NEW=1,
	REASON_ACCEPT,
	REASON_RENEW,
	REASON_DONTACCEPT,
	REASON_ERROR_FIX,
	REASON_FIXED,
	REASON_AS_DESIGN,
	REASON_DUPLICATE,
	REASON_CANT_REPRODUCE,
	REASON_ACCEPT_FIX
} request_reason;

static char * const request_reason_str[]={
	"�½�",
	"���ܽ������",
        "���¼���",
	"����������������",
	"������޸�",
	"���޸�",
	"������Ϊ�������",
	"�ظ��ύ�˴���",
	"�޷����ִ˴���",
	"ȷ���޸�",
	NULL
};

typedef enum {
	REQUEST_TASK=1,
	REQUEST_BUG=2,
} request_class;

static char * const request_class_str[]={
	"����",
	"����"
};

static const char * request_owner_str[]={
	"lepton",
	"��������",
	"tnds",
	"testor",
	"tongban",
	"IZIZ",
	"ivy",
	"ygxz",
	"Fishingsnow",
	"wuqq", 
	"pibroch",
	NULL
};

static const char * request_module_str[]={
	"web_bbs", 
	"telnet/ssh", 
	"blog",
	"wiki",
	"e-bussiness",
	"others",
	NULL
};

static char * const request_priority_str[]={
	"��",
	"����",
	"һ��",
	"����",
	"Զ��",
	NULL
};

#define REQUEST_MAX_PRIORITY  (sizeof(request_priority_str)/sizeof(char *))

struct request {
	time_t new_time; //4
	time_t last_time; //8
	unsigned char class;
	unsigned char priority;
	unsigned char status;
	unsigned char reason; //12
	unsigned int no; //16
	unsigned int unused[2]; //24
	char title[REQUEST_TITLE_LEN+1]; //89
	char module[REQUEST_MODULE_LEN+1]; //102
	char reporter[IDLEN+1]; //115
	char owner[IDLEN+1]; //128
};

unsigned int filter_class;
unsigned int filter_priority;
unsigned int filter_status;
char filter_owner[IDLEN+1];
char filter_reporter[IDLEN+1];
static int sort_type;

int filter_request(struct request *r){
	if(filter_class && filter_class != r->class)
		return 1;
	if(filter_priority && !((1<<(r->priority-1))&filter_priority))
		return 1;
	if(filter_status && !((1<<(r->status-1))&filter_status))
		return 1;
	if(filter_owner[0] && strcmp(filter_owner, r->owner))
		return 1;
	if(filter_reporter[0] && strcmp(filter_reporter, r->reporter))
		return 1;
	return 0;
}

static char *
request_date(time_t time)
{
	struct tm *t;
	static char buf[32];
	t=localtime(&time);
	sprintf(buf, "%04d-%02d-%02d", t->tm_year+1900, t->tm_mon + 1, t->tm_mday);
	return buf;
}

static char *
request_time(time_t time)
{
	struct tm *t;
	static char buf[32];
	t=localtime(&time);
	sprintf(buf, "%04d-%02d-%02d %02d:%02d:%02d", t->tm_year+1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
	return buf;
}



static inline int str_in_strs(char *str, const char **strs)
{
	int i;
	for(i=0; strs[i]; i++){
		if(!strcmp(strs[i],str))
			return (i+1);
	}
	return 0;
}

int valid_request_owner(char *user)
{
	return str_in_strs(user, request_owner_str);
}

/* all logical code  is here */

char ** request_status_options(struct request *r)
{
	static char * op[256];
	memset(&op, 0, sizeof(op));
	if(!r->no){
		//new request;
		op[0]=request_status_str[REQUEST_ACTIVE-1];
		return op;
	}
	if(r->class == REQUEST_TASK){
		op[0]=request_status_str[REQUEST_ACTIVE-1];
		op[1]=request_status_str[REQUEST_CLOSED-1];
		return op;
	}
	if(r->class == REQUEST_BUG){
		if(r->status == REQUEST_ACTIVE){
			op[0]=request_status_str[REQUEST_ACTIVE-1];
			op[1]=request_status_str[REQUEST_FIXED-1];
			return op;
		}
		if(r->status == REQUEST_FIXED){
			op[0]=request_status_str[REQUEST_ACTIVE-1];
			op[1]=request_status_str[REQUEST_FIXED-1];
			op[2]=request_status_str[REQUEST_CLOSED-1];
			return op;
		}
		if(r->status == REQUEST_CLOSED){
			op[0]=request_status_str[REQUEST_CLOSED-1];
			return op;
		}
	}
	return op;
}

char * request_status_default(struct request *r)
{
	static char buf[256];
	buf[0]=0;
	if(!r->no){
		//new request;
		strsncpy(buf, request_status_str[REQUEST_ACTIVE-1], sizeof(buf));
	}
	return buf;
}

char ** request_reason_options(struct request *r)
{
	static char * op[256];
	memset(&op, 0, sizeof(op));
	if(!r->no){
		//new request;
		op[0]=request_reason_str[REASON_NEW-1];
		return op;
	}
	op[0]=request_reason_str[r->reason - 1];
	if(r->class == REQUEST_TASK){
		if(r->status ==  REQUEST_ACTIVE)
			op[1]=request_reason_str[REASON_ACCEPT - 1];
		return op;
	}
	if(r->class == REQUEST_BUG){
		if(r->status == REQUEST_ACTIVE){
			op[1]=request_reason_str[REASON_FIXED - 1];
			op[2]=request_reason_str[REASON_AS_DESIGN - 1];
			op[3]=request_reason_str[REASON_DUPLICATE - 1];
			op[4]=request_reason_str[REASON_CANT_REPRODUCE - 1];
			return op;
		}
		if(r->status == REQUEST_FIXED){
			if(r->reason == REASON_FIXED){
				op[1]=request_reason_str[REASON_RENEW - 1];
				op[2]=request_reason_str[REASON_ACCEPT_FIX - 1];
				op[3]=request_reason_str[REASON_DONTACCEPT - 1];
				op[4]=request_reason_str[REASON_ERROR_FIX - 1];
				return op;
			}
			op[1]=request_reason_str[REASON_RENEW - 1];
			op[2]=request_reason_str[REASON_DONTACCEPT - 1];
			op[3]=request_reason_str[REASON_ERROR_FIX - 1];
			return op;
		}
		if(r->status == REQUEST_CLOSED)
			return op;
	}
	return op;
}

char * request_reason_default(struct request *r)
{
	static char buf[256];
	buf[0]=0;
	if(!r->no){
		//new request;
		strsncpy(buf, request_reason_str[REASON_NEW-1], sizeof(buf));
	}
	return buf;
}

static inline int verify_request(int no, struct request *r)
{
	if(r->no!=no)
		return -1;
	if(r->reporter[IDLEN])
		return -2;
	if(r->owner[IDLEN])
		return -3;
	if(r->module[REQUEST_MODULE_LEN])
		return -4;
	if(r->title[REQUEST_TITLE_LEN])
		return -5;
	if(r->no!=no)
		return -6;
	return 0;
}

void log_request_change(struct request *old, struct request *new, char *content)
{
	char buf[1024];
	int fd, len=0;
	sprintf(buf, REQUEST_LOG_DIR "/%d", new->no);
        fd = open(buf, O_WRONLY | O_CREAT | O_APPEND, 0660);
	if(fd<0)
		return;
	if(old){
		len+=sprintf(buf, "\n====== %s �� %s �����˱�����======\n", currentuser->userid, request_time(new->last_time));
		if(strcmp(new->owner, old->owner))
			len+=sprintf(buf+len, "������: %s -> %s\n",
					old->owner,
					new->owner);
		if(new->status!=old->status)
			len+=sprintf(buf+len, "״̬: %s -> %s\n", 
				request_status_str[old->status-1], 
				request_status_str[new->status-1]);
		if(new->reason!=old->reason)
			len+=sprintf(buf+len, "ԭ��: %s\n", 
				request_reason_str[new->reason - 1]);
		if(strcmp(new->module, old->module))
			len+=sprintf(buf+len, "ģ����: %s -> %s\n",
					old->module,
					new->module);
		if(strcmp(new->title, old->title))
			len+=sprintf(buf+len, "����: %s -> %s\n",
					old->title,
					new->title);
		if(new->priority!=old->priority)
			len+=sprintf(buf+len, "���ȼ�: %s -> %s\n", 
				request_priority_str[old->priority-1], 
				request_priority_str[new->priority-1]);
	} else 
		len+=sprintf(buf, "====== %s �� %s ����˱�����======\n", currentuser->userid, request_time(new->new_time));
	write(fd, buf, len);
	write(fd, content, strlen(content));
	close(fd);
	return;
}

int add_request(struct request *r, char *content)
{
	int fd, ret;
	struct stat st;
	char buf[1024], tmp[256];
	fd=open(REQUEST_LIST, O_CREAT|O_WRONLY, 0600);
	if(fd<0)
		http_fatal("���ܴ��ļ�");
	flock(fd, LOCK_EX);
	ret=fstat(fd, &st);
	if(ret<0)
		goto error;
	if(st.st_size%sizeof(*r))
		goto error;
	r->no = st.st_size/sizeof(*r)+1;
	ret=lseek(fd, 0, SEEK_END);
	if(ret<0)
		goto error;
	r->new_time = time(NULL);
	r->last_time = 0;
	write(fd, r, sizeof(*r));
	log_request_change(NULL, r, content);
	close(fd);
	ret=snprintf(buf, sizeof(buf), "������: %s\n"
			"URL: https://%s/HT/bug?id=%d\n"
			"���ȼ�:%s\n"
			"ģ��:%s\n"
			"\n%s",
			r->title,
			MY_BBS_DOMAIN, r->no,
			request_priority_str[r->priority-1],
			r->module, content);
	if(ret>sizeof(buf)-1)
		ret=sizeof(buf)-1;
	if(strcmp(r->owner, currentuser->userid)){
		snprintf(tmp, sizeof(tmp), "ι,������!(%s)", r->title);
		mail_buf(buf, ret, r->owner, tmp, currentuser->userid);
	}
	return 0;
error:
	close(fd);
	printf("�������ݿ�ʧ��");
	return -1;
}

int new_request(void)
{
	struct request b;
	char * content;
	if(!valid_request_owner(currentuser->userid))
		http_fatal("��Ǹ, ��ֻ�в鿴Ȩ��");
	memset(&b, 0, sizeof(b));
	b.class=atoi((getparm("class")));
	if(b.class<REQUEST_TASK||b.class>REQUEST_BUG)
		http_fatal("����ʶ����������");
	b.priority=atoi((getparm("priority")));
	if(!b.priority||b.priority>=REQUEST_MAX_PRIORITY)
		http_fatal("��֧�ֵ����󼶱�");
	strsncpy(b.reporter, currentuser->userid, IDLEN+1);
	strsncpy(b.owner, getparm("owner"), IDLEN+1);
	if(!valid_request_owner(b.owner))
		http_fatal("������ύ����");
	strsncpy(b.module, getparm("module"), REQUEST_MODULE_LEN+1);
	if(!str_in_strs(b.module, request_module_str))
		http_fatal("�����ģ��");
	b.status=REQUEST_ACTIVE;
	b.reason=REASON_NEW;
	strsncpy(b.title, getparm("title"), REQUEST_TITLE_LEN+1);
	content=getparm("content");
	if(!b.title[0]||!content[0])
		http_fatal("�������д�����˵��");
	return add_request(&b, content);
}

int verify_change(struct request *old, struct request *new, char * content)
{
	new->new_time=old->new_time;
	new->last_time=time(NULL);
	new->class=old->class;
	strcpy(new->reporter, old->reporter);
	if(!str_in_strs(request_status_str[new->status-1],request_status_options(old)))
		return -1;
	if(!str_in_strs(request_reason_str[new->reason-1],request_reason_options(old)))
		return -1;
	if(new->status==old->status)
		new->reason=old->reason;
	else {
		if(new->reason==old->reason &&
			old->status!=REQUEST_FIXED)
		return -1;
	}
	if(new->priority==old->priority &&
	   new->status==old->status &&
	   !strcmp(new->title, old->title) &&
	   !strcmp(new->module, old->module) &&
	   !strcmp(new->owner, old->owner) &&
	   !content[0])
		return -2;
	return 0;
}

int change_request(struct request * r, char *content)
{
	int fd, ret;
	struct stat st;
	struct request old;
	char buf[1024], title[256];
	fd=open(REQUEST_LIST, O_RDWR);
	if(fd<0)
		return -1;
	flock(fd, LOCK_EX);
	ret=fstat(fd, &st);
	if(ret<0)
		goto error;
	if(st.st_size % sizeof(*r))
		goto error;
	if(st.st_size < r->no * sizeof(*r))
		goto error;
	lseek(fd, (r->no-1)*sizeof(*r), SEEK_SET);
	ret=read(fd, &old, sizeof(old));
	if(ret!=sizeof(old))
		goto error;
	if(verify_request(r->no, &old)<0||old.status == REQUEST_CLOSED)
		goto error;
	if((ret=verify_change(&old, r, content))<0){
		close(fd);
		if(ret==-2)
			return 0;
		return -1;
	}
	lseek(fd, (r->no-1)*sizeof(*r), SEEK_SET);
	write(fd, r, sizeof(*r));
	log_request_change(&old, r, content);
	close(fd);
	if(r->status==REQUEST_CLOSED && r->class==REQUEST_TASK){
		sprintf(buf, REQUEST_LOG_DIR "/%d", r->no);
		sprintf(title, "����%d %s (%s/%s) �ѽ���", r->no, r->title, 
				r->module, request_priority_str[r->priority-1]);
		post_article(REQUEST_BOARD, title, buf, "deliver",
		     "�Զ�����ϵͳ", "127.0.0.1", -1, 0, 0, "deliver", -1);
	}
	if(strcmp(old.owner, r->owner)&&strcmp(r->owner, currentuser->userid)){
		ret=snprintf(buf, sizeof(buf), "����: %s\n"
			"URL: https://%s/HT/bug?id=%d\n"
			"���ȼ�:%s\n"
			"ģ��:%s\n"
			"\n%s",
			r->title,
			MY_BBS_DOMAIN, r->no,
			request_priority_str[r->priority-1],
			r->module, content);
		if(ret>sizeof(buf)-1)
			ret=sizeof(buf)-1;
		sprintf(title, "ι,������!(%s)", r->title );
		mail_buf(buf, ret, r->owner, title, currentuser->userid);
	}
	return 0;
error:
	close(fd);
	return -1;
}

struct request * load_all_request(int * count)
{
	struct request *r;
	int fd, ret, i;
	struct stat st;
	fd=open(REQUEST_LIST, O_RDONLY);
	if(fd<0)
		return NULL;
	flock(fd, LOCK_SH);
	ret=fstat(fd, &st);
	if(ret<0)
		goto error;
	if(st.st_size % sizeof(*r))
		goto error;
	r=malloc(st.st_size);
	if(NULL==r)
		goto error;
	read(fd, r, st.st_size);
	close(fd);
	*count=st.st_size/sizeof(*r);
	for(i=0;i<*count;i++){
		ret=verify_request(i+1, r+i);
		if(ret<0){
			printf("internal error:%d\n", ret);
			free(r);
			return NULL;
		}
	}
	return r;
error:
	close(fd);
	return NULL;
}

int load_single_request(int id, struct request *r)
{
	int fd, ret;
	struct stat st;
	fd=open(REQUEST_LIST, O_RDONLY);
	if(fd<0)
		return -1;
	flock(fd, LOCK_SH);
	ret=fstat(fd, &st);
	if(ret<0)
		goto error;
	if(st.st_size % sizeof(*r))
		goto error;
	if(st.st_size < id * sizeof(*r))
		goto error;
	lseek(fd, (id-1)*sizeof(*r), SEEK_SET);
	ret=read(fd, r, sizeof(*r));
	close(fd);
	if(ret!=sizeof(*r))
		return -1;
	if(verify_request(id, r)<0)
		return -1;
	return 0;
error:
	close(fd);
	return -1;
}

void show_selected_status(struct request *r)
{
	int i;
	char **value, *def;
	printf("<select name=status onChange=\""
		"fill_reason(this.form.reason, %d, '%s', this.value, '%s');"
		"fill_owner(this.form.owner, '%s', this.value, '%s', '%s');\""
		">\n",
		r->class-1, request_status_str[r->status-1],
		request_reason_str[r->reason-1],
		request_status_str[r->status-1],
		r->owner, r->reporter);

	value=request_status_options(r);
	def=request_status_str[r->status-1];
	for(i=0; value[i]; i++){
		if(strcmp(value[i],def))
      			printf("<option value=\"%s\">%s</option>",
				value[i], value[i]);
		else
			printf("<option selected value=\"%s\">%s</option>",
				value[i], value[i]);
	}
	printf("</select>\n");
}

void show_selected(int use_num_index, char *name, char **value, char *def)
{
	int i;
 	printf("<select name=%s>\n", name);
	if(use_num_index){
		for(i=0; value[i]; i++){
			if(strcmp(value[i],def))
      				printf("<option value=%d>%s</option>",
					i+1, value[i]);
			else
				printf("<option selected value=%d>%s</option>",
					i+1, value[i]);
		}
	} else {
		for(i=0; value[i]; i++){
			if(strcmp(value[i],def))
      				printf("<option value=\"%s\">%s</option>",
					value[i], value[i]);
			else
				printf("<option selected value=\"%s\">%s</option>",
					value[i], value[i]);
		}
	}
	printf("</select>\n");
}

void
show_request(struct request *r){
	char buf[256];
	puts("<script src=/bug.js></script>");
	if(r->no){
		printf("<p>���� %d��%s %s</p>", r->no, request_class_str[r->class-1], r->title);
		printf("<form action=bug?edit_request=%d METHOD=POST>", r->no);
	}
	else{
		printf("<p>��%s����</p>", request_class_str[r->class-1]);
		puts("<form action=bug?send_request=1 METHOD=POST>");
	}
	puts("<table border=1 align=center>");
  	puts("<tr>");
    	puts("<td width=18% height=30 align=center>�ύ����");
	show_selected(0, "owner", request_owner_str, r->owner);
	puts("</td><td width=18% height=30 align=center>ģ�飺");
	show_selected(0, "module", request_module_str, r->module);
	puts("</td><td width=18% height=30 align=center>���ȼ���");
	show_selected(1, "priority", request_priority_str, request_priority_str[r->priority-1]);
	puts("</td><td width=18% height=30 align=center>״̬��");
	show_selected_status(r);
	puts("</td><td width=28% height=30 align=center>���ɣ�");
	printf("<select name=reason><option value=%s selected>%s</option></select>", request_reason_str[r->reason-1], request_reason_str[r->reason-1]);
	puts("</td></tr><tr><td width=18% height=30 align=right>���⣺</td>");
        printf("<td colspan=4 width=72%%>" 
             "<input name=title type=text size=64 maxlength=64 value=\"%s\"></td></tr>", void1(nohtml(r->title)));
	if(r->status!=REQUEST_CLOSED){
		printf("<tr><td width=18%% align=right>˵����<input type=hidden name=class value=%d></td>", r->class);
		puts("<td colspan=4 width=72%>"
             	"<textarea name=content rows=12 cols=100></textarea></td></tr>");
  		puts("<tr><td align=center colspan=5 width=100%>");
		printf("<input type=submit value=%s></td></tr>", r->no?"����":"�ύ");
	}
	puts("</table></form>");
	if(r->no){
		sprintf(buf, REQUEST_LOG_DIR "/%d", r->no);
		fshowcon(stdout, buf, 3);
	}
}

int edit_new_request(int class)
{
	struct request r;

	if(!valid_request_owner(currentuser->userid))
		http_fatal("��Ǹ, ��ֻ�в鿴Ȩ��");
	memset(&r, 0, sizeof(r));
	r.class=class;
	strcpy(r.owner, currentuser->userid);
	strcpy(r.module, "web_bbs");
	r.priority=3;
	r.status=REQUEST_ACTIVE;
	r.reason=REASON_NEW;
	show_request(&r);
	return 0;
}

void pt(char *str, int st)
{
	char p[100], tmp[100];
	if(st<0){
		printf("<td>%s", str);
		return;
	}
	p[0]=0;
	if(filter_owner[0]){
		strcat(p, "&o=");
		strcat(p, filter_owner);
	}
	if(filter_reporter[0]){
		strcat(p, "&r=");
		strcat(p, filter_reporter);
	}
	if(filter_status!=3){
		sprintf(tmp, "&s=%d", filter_status);
		strcat(p, tmp);
	}
	if(st){
		sprintf(tmp, "&x=%d", st);
		strcat(p, tmp);
	}
	if(p[0])
		p[0]='?';
	printf("<td><a href=bug%s>%s</a>", p, str);
}

void show_request_list(struct request *r, int count)
{
	int i;
	char t[100];
	puts("<table><tr>");
	pt("������", 0);
	pt("���", 1);
	pt("���ȼ�", 2);
	pt("״̬", 3);
	pt("��ǰ������", 4);
	pt("����", -1);
	pt("�ύʱ��", 5);
	pt("�ϴδ���ʱ��",6);
	for(i=0;i<count;i++,r++){
		if(filter_request(r))
			continue;
		if(r->last_time)
			strcpy(t, request_date(r->last_time));
		else
			strcpy(t, "��δ����");
		printf("<tr><td><a href=bug?id=%d>%d</a><td>%s<td>%s<td>%s<td>%s"
		      "<td><a href=bug?id=%d>%s</a><td>%s<td>%s",
		      r->no, r->no, request_class_str[r->class-1],
		      request_priority_str[r->priority-1],
		      request_status_str[r->status-1],
		      r->owner, r->no, void1(nohtml(r->title)),
		      request_date(r->new_time),t);
	}
	puts("</table>");
}

int edit_request(int id)
{
	struct request r;
	char * content;
	if(!valid_request_owner(currentuser->userid))
		http_fatal("��Ǹ, ��ֻ�в鿴Ȩ��");
	memset(&r, 0, sizeof(r));
	r.no=id;
	r.priority=atoi((getparm("priority")));
	if(!r.priority||r.priority>=REQUEST_MAX_PRIORITY)
		http_fatal("��֧�ֵ����󼶱�");
	strsncpy(r.owner, getparm("owner"), IDLEN+1);
	if(!valid_request_owner(r.owner))
		http_fatal("������ύ����");
	strsncpy(r.module, getparm("module"), REQUEST_MODULE_LEN+1);
	if(!str_in_strs(r.module, request_module_str))
		http_fatal("�����ģ��");
	r.status=str_in_strs(getparm("status"), request_status_str);
	r.reason=str_in_strs(getparm("reason"), request_reason_str);
	if(r.status==0)
		http_fatal("�����״̬");
	if(r.reason==0)
		http_fatal("�����ԭ��");
	strsncpy(r.title, getparm("title"), REQUEST_TITLE_LEN+1);
	content=getparm("content");
	if(!r.title[0])
		http_fatal("�������д����");
	if(change_request(&r, content)<0)
		http_fatal("���ܸ���, ������������Ƿ����");
	else
		show_request(&r);
}

int show_edit_request(int id)
{
	struct request r;
	if(load_single_request(id, &r)<0){
		http_fatal("�����ڵ�bug");
		return 0;
	}
	show_request(&r);
	return 0;
}

#define DEF_FUNC_SORT_REQUEST_BY(id, x, y) \
static int sort_request_by_##id(struct request *a, struct request *b) \
{ \
	if(x->id > y->id) \
		return 1; \
	else if(x->id < y->id) \
		return -1; \
	return 0; \
}

DEF_FUNC_SORT_REQUEST_BY(no, b, a)
DEF_FUNC_SORT_REQUEST_BY(class, b, a)
DEF_FUNC_SORT_REQUEST_BY(priority, a, b)
DEF_FUNC_SORT_REQUEST_BY(status, a, b)
DEF_FUNC_SORT_REQUEST_BY(last_time, a, b)
DEF_FUNC_SORT_REQUEST_BY(new_time, a, b)

static int sort_request_by_owner(struct request *a, struct request *b)
{
	return strcmp(a->owner, b->owner);
}

typedef int sort_func(struct request *, struct request *);

static sort_func * sort_funcs[]={
	sort_request_by_no,
	sort_request_by_class,
	sort_request_by_priority,
	sort_request_by_status,
	sort_request_by_owner,
	sort_request_by_new_time,
	sort_request_by_last_time
};

#define SORT_FUNC_NUM (sizeof(sort_funcs)/sizeof(sort_func *))

int
sort_request(struct request *a, struct request *b)
{
	int i, ret;
	for(i=0; i<SORT_FUNC_NUM;i++){
		ret=(*(sort_funcs[(sort_type+i)%SORT_FUNC_NUM]))(a, b);
		if(ret)
			return ret;
	}
}

int
bbsbug_main()
{
	struct request *rs;
	int ret, count, id;
	html_header(1);
	//check_msg();
	if (!loginok || isguest)
		http_fatal("����δ��¼, ���ȵ�¼");
	if (!has_read_perm(currentuser, REQUEST_BOARD)&&!valid_request_owner(currentuser->userid))
		http_fatal("�Բ���, ����BBSHelp���汨��BUG");
	changemode(GMENU);
	puts("<body>");
	printf("<table><tr><td><a href=bbsbug?s=3>�����</a></td>"
		"<td><a href=bbsbug?s=4>�Ѿ��رյ�����</a></td>"
		"<td><a href=bbsbug?o=%s>��Ҫ�Ҵ��������</a></td><td>"
		"<a href=bbsbug?r=%s>�����������</a></td><td>"
		"<a href=bbsbug?edit_newtask=1>�ύ����!!!</a></td><td>"
		"<a href=bbsbug?edit_newbug=1>�ύBUG!!!</a></td>"
		"</tr></table><hr>",
		currentuser->userid, currentuser->userid);
	if(atoi(getparm("edit_newbug")))
			return edit_new_request(REQUEST_BUG);
	else if(atoi(getparm("edit_newtask")))
			return edit_new_request(REQUEST_TASK);
	else if(atoi(getparm("send_request"))){
			if(new_request()<0)
				http_fatal("�ύʧ��");
	} else if((id=atoi(getparm("id"))))
			return show_edit_request(id);
	else if((id=atoi(getparm("edit_request"))))
			return edit_request(id);
	rs=load_all_request(&count);
	if(!rs)
		return 0;
	filter_status=atoi(getparm("s"));
	if(!filter_status)
		filter_status=3;
	strsncpy(filter_owner, getparm("o"), IDLEN + 1);
	strsncpy(filter_reporter, getparm("r"), IDLEN +1);
	sort_type=atoi(getparm("x"));
	if(sort_type>0)
		qsort(rs, count, sizeof(*rs), sort_request);
	show_request_list(rs, count);
	free(rs);
	return 0;
}
