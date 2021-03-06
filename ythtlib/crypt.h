/* crypt.c */
#ifndef __CRYPT_H
#define __CRYPT_H
#define MD5LEN 16
void randomnumalpha(char *s, int n);
char *genpasswd(char pw_crypted[MD5LEN], const int salt, const char *pw);
char *genpasswd_des(const char *pw);
int gendiscuzpasswd(char* discuzpassmd5, const char *salt, const char *passbuf);
int checkpasswd(const char pw_crypted[MD5LEN], const int salt, const char *pw_try);
int checkpasswd_des(const char *pw_crypted, const char *pw_try);
int getsalt_md5();
#endif
