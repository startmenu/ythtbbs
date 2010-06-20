/* article.c */
#ifndef __ARTICLE_H
#define __ARTICLE_H

struct fileheader {
	int filetime;
	int edittime;
	int thread;
	unsigned int accessed;
	char title[60];
	char owner[14];		//如果是本站的, 就用id, 如果时email, 就取第一个词并
	//且加'.'. 如果是本站匿名, 则第一个为'\0', 后面跟id
	unsigned short viewtime;
	unsigned char sizebyte;
	unsigned char staravg50;	//staravg 最大为5, staravg50 = staravg * 50
	//i.e. staravg50 = totalstar * 50 / hasvoted
	unsigned char oldhasvoted;
	char deltime;		//记录回收站和纸篓里面的文章时什么时间删除的, now_t / (3600 * 24) % 100, 用于自动清除垃圾
	unsigned short hasvoted;
	char unused[30];
};



//fileheader - accessed values
#define FH_READ 0x1		//whether the file has been viewed if it is a mail
#define FH_HIDE 0x2		//whether the file has been set as hidden in backnumber
#define FH_MARKED 0x4
#define FH_DIGEST 0x8		//Has been put into digest
#define FH_NOREPLY 0x10
#define FH_ATTACHED 0x20	//Has attachments
#define FH_DEL 0x40		//Marked to be deleted
#define FH_SPEC 0x80		//Will be put to 0Announce, and this flag will be clear then
#define FH_INND 0x100		//write into innd/out.bntp
#define FH_ANNOUNCE 0x200	//have been put into 0Announce
#define FH_1984 0x400		//have been checked to see if there is any ...
#define FH_ISDIGEST 0x800	//whether it is a digest file, i.e., filename is start with G., but not M.
#define FH_REPLIED 0x1000	//this mail has been replied
#define FH_ALLREPLY 0x2000	//this article can be re by all...
#define FH_MATH 0x4000		//this article contains itex math functions.
#define FH_DANGEROUS 0x8000
#define FH_ISTOP 0x10000	//本文置顶
#define FH_SYSMAIL 0x20000      //全站信件，或者deliver信件等，不受mailsize上限影响
#define FH_BMUSE 0x40000	//版主专用
#define feditmark(x)  ((x)->edittime?((x)->filetime-(x)->edittime):0)

struct bknheader {
	int filetime;
	char boardname[20];
	char title[60];
	char unused[44];
};

struct boardtop {
        char title[60];
        int unum;
        int thread;
        char firstowner[14];
        time_t lasttime;
	char board[24];
};

char *fh2fname(struct fileheader *fh);
char *bknh2bknname(struct bknheader *bknh);
char *fh2owner(struct fileheader *fh);
char *fh2realauthor(struct fileheader *fh);
void fh_setowner(struct fileheader *fh, char *owner, int anony);
int fh2modifytime(struct fileheader *fh);
int change_dir(char *, struct fileheader *,
	       void *func(struct fileheader *, struct fileheader *, void *), int, int, int, void *);
int DIR_do_mark(struct fileheader *, struct fileheader *, void *);
int DIR_do_setbmuse(struct fileheader *, struct fileheader *, void *);
int DIR_do_digest(struct fileheader *, struct fileheader *, void *);
int DIR_do_underline(struct fileheader *, struct fileheader *, void *);
int DIR_do_allcanre(struct fileheader *, struct fileheader *, void *);
int DIR_do_attach(struct fileheader *, struct fileheader *, void *);
int DIR_clear_dangerous(struct fileheader *, struct fileheader *, void *);
int DIR_do_dangerous(struct fileheader *, struct fileheader *, void *);
int DIR_do_markdel(struct fileheader *, struct fileheader *, void *);
int DIR_do_edit(struct fileheader *, struct fileheader *, void *);
int DIR_do_changetitle(struct fileheader *, struct fileheader *, void *);
int DIR_do_evaluate(struct fileheader *, struct fileheader *, void *);
int DIR_do_spec(struct fileheader *, struct fileheader *, void *);
int DIR_do_import(struct fileheader *, struct fileheader *, void *);
int DIR_do_suremarkdel(struct fileheader *, struct fileheader *, void *);
int DIR_do_sureunmarkdel(struct fileheader *, struct fileheader *, void *);
int outgo_post(struct fileheader *, char *, char *, char *);
void cancelpost(char *, char *, struct fileheader *, int);
int cmp_title(char *title, struct fileheader *fh1);
int fh_find_thread(struct fileheader *fh, char *board);
int Search_Bin(struct fileheader *ptr, int key, int start, int end);	
int add_edit_mark(char *fname, char *userid, time_t now_t, char *fromhost);
int delete_dir_callback(char *dirname, int ent, int filetime, int (*callback)(struct fileheader *, void *), void *cb_arg);
int append_dir_callback(char *filename, struct fileheader *record, int thread, int (*callback)(struct fileheader *, void *), void *cb_arg);
#define delete_dir(x, y, z) delete_dir_callback(x, y, z, NULL, NULL);
#define append_dir(x, y, z) append_dir_callback(x, y, z, NULL, NULL);
#endif
