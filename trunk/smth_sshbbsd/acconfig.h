/*


acconfig.h - template used by autoheader to create config.h.in
config.h.in - used by autoconf to create config.h
config.h - created by autoconf; contains defines generated by autoconf

Copyright (c) 1995 Tatu Ylonen <ylo@cs.hut.fi>

*/

/*@TOP@*/
/* Define this to the canonical name of your host type (e.g., 
   "sparc-sun-sunos4.0.3"). */
#undef HOSTTYPE
/* Define if you have SYSV-style /dev/ptmx and /dev/pts/. */
#undef HAVE_DEV_PTMX
/* Define if you have /dev/pts and /dev/ptc devices (as in AIX). */
#undef HAVE_DEV_PTS_AND_PTC
/* Define if you have shadow passwords in /etc/security/passwd (AIX style). */
#undef HAVE_ETC_SECURITY_PASSWD
/* Define if you have shadow passwords in /etc/security/passwd.adjunct
   (SunOS style). */
#undef HAVE_ETC_SECURITY_PASSWD_ADJUNCT
/* Define if you have SIA, Security Integration Architecture (as in
   Tru64 UNIX). */
#undef HAVE_SIA
/* Define if you have shadow passwords in /etc/shadow (Solaris style). */
#undef HAVE_ETC_SHADOW
/* Define if you have system login defaults in /etc/default/login. */
#undef HAVE_ETC_DEFAULT_LOGIN
/* Define if utmp structure has host field. */
#undef HAVE_HOST_IN_UTMP
/* Define if utmp structure has addr field. */
#undef HAVE_ADDR_IN_UTMP
/* Define if utmp structure has id field. */
#undef HAVE_ID_IN_UTMP
/* Define if utmp structure has name field. */
#undef HAVE_NAME_IN_UTMP
/* Define if utmp structure has pid field. */
#undef HAVE_PID_IN_UTMP
/* Define if utmpx structure has syslen field. */
#undef HAVE_SYSLEN_IN_UTMPX
/* Define if /var/adm/lastlog or whatever it is called is a directory
   (e.g. SGI IRIX). */
#undef LASTLOG_IS_DIR
/* Define to use RSAREF. */
#undef RSAREF
/* Define this to be the path of the rsh program to support executing rsh. */
#undef RSH_PATH
/* Define this to be the path of the xauth program. */
#undef XAUTH_PATH
/* Default path for utmp.  Determined by configure. */
#undef SSH_UTMP
/* Default path for wtmp.  Determined by configure. */
#undef SSH_WTMP
/* Default path for lastlog.  Determined by configure. */
#undef SSH_LASTLOG
/* This is defined if we found a lastlog file.  The presence of lastlog.h
   alone is not a sufficient indicator (at least newer BSD systems have
   lastlog but no lastlog.h. */
#undef HAVE_LASTLOG
/* Define this if libutil.a contains BSD 4.4 compatible login(), logout(),
   and logwtmp() calls. */
#undef HAVE_LIBUTIL_LOGIN
/* Location of system mail spool directory. */
#undef MAIL_SPOOL_DIRECTORY
/* Name of user's mail spool file if stored in user's home directory. */
#undef MAIL_SPOOL_FILE
/* Define this to be the default user path if you don't like the default. 
   See the --with-path=<path> configure option. */
#undef DEFAULT_PATH
/* Define this if O_NONBLOCK does not work on your system (e.g., Ultrix). */
#undef O_NONBLOCK_BROKEN
/* Define this if sys/syslog.h needs to be included in addition to syslog.h.
   This is the case on some Ultrix versions. */
#undef NEED_SYS_SYSLOG_H
/* Define this to leave out that cipher. */
#undef WITH_IDEA
#undef WITH_DES
#undef WITH_ARCFOUR
#undef WITH_BLOWFISH
#undef WITH_NONE
/* Define this to include libwrap (tcp_wrappers) support. */
#undef LIBWRAP
#undef HAVE_LIBWRAP
/* This is defined to pw_encrypt on Linux when using John Faugh's shadow 
   password implementation. */
#undef crypt
/* This is defined on 386BSD to preted we are on FreeBSD. */
#undef __FreeBSD__
/* If defines, this overrides "tty" as the terminal group. */
#undef TTY_GROUP
/* Define this if you want to support Security Dynammics SecurID
   cards. */
#undef HAVE_SECURID
/* Define this if you want to support TIS Authentication scheme. */
#undef HAVE_TIS
/* Define this if you are using HPSUX.  HPUX uses non-standard shared
   memory communication for X, which seems to be enabled by the display name
   matching that of the local host.  This circumvents it by using the IP
   address instead of the host name in DISPLAY. */
#undef NONSTANDARD_IP_ADDRESS_X11_KLUDGE
/* This will enable using magic directory to find x11 sockets */
#undef HPSUX_NONSTANDARD_X11_KLUDGE
/* Define this is compiling on HPSUX 7.x.  This will avoid including
   arpa/inet.h, and will define struct linger in includes.h. */
#undef HPSUX7_KLUDGES
/* Define this if inet_network should be used instead of inet_addr.  This is
   the case on DGUX 5.4. */
#undef BROKEN_INET_ADDR
/* Define this if your system does not like sizeof(struct sockaddr_un) as the
   size argument in bind and connect calls for unix domain sockets. */
#undef USE_STRLEN_FOR_AF_UNIX
/* Define this to use pipes instead of socketpairs for communicating with the
   client program.  Socketpairs do not seem to work on all systems. */
#undef USE_PIPES
/* Directory containing ssh_config, ssh_known_hosts, sshd_pid, etc.  Normally
   /etc. */
#undef ETCDIR
/* Define this if speed_t is defined in stdtypes.h or otherwise gets included
   into ttymodes.c from system headers. */
#undef SPEED_T_IN_STDTYPES_H
/* Define this if compiling on Ultrix.  Defining this does not actually require
   shadow passwords to be present; this just includes support for them. */
#undef HAVE_ULTRIX_SHADOW_PASSWORDS
/* Define this if compiling with SOCKS (the firewall traversal library). */
#undef SOCKS
#undef SOCKS4
#undef SOCKS5
/* Define these if on SCO Unix. */
#undef HAVE_SCO_ETC_SHADOW
#undef SCO
/* Additionally define this if on SCO 3.2v5 Unix */
#undef SCO5
/* Define this if you have getpseudotty() (DYNIX/ptx 2.1) */
#undef HAVE_GETPSEUDOTTY
/* Define this if you have sco-style pty:s (ptyp0, ..., ptyp9, ptyp10...) */
#undef HAVE_DEV_PTYP10
/* Define this if you have setpgid() (replaces setpgrp) */
#undef HAVE_SETPGID
/* Define this for HP-UX 10.x shadow passwords */
#undef HAVE_HPUX_TCB_AUTH
/* Support for Secure RPC */
#undef SECURE_RPC
/* Support for Secure NFS */
#undef SECURE_NFS
/* Support for NIS+ */
#undef NIS_PLUS
/* Define this to disable all port forwardings in server (except X11) */
#undef SSHD_NO_PORT_FORWARDING
/* Define this to disable all port forwardings in client (except X11) */
#undef SSH_NO_PORT_FORWARDING
/* Define this to disable X11 forwarding in server */
#undef SSHD_NO_X11_FORWARDING
/* Define this to disable X11 forwarding in client */
#undef SSH_NO_X11_FORWARDING
/* The code in sshconnect.c is written for SOCKS4. If SOCKS5 should be used
   these needs redefining */
#undef Rconnect
#undef Rgetsockname
#undef Rgetpeername
#undef Rbind
#undef Raccept
#undef Rlisten
#undef Rselect
#undef Rrecvfrom
#undef Rsendto
#undef Rrecv
#undef Rsend
#undef Rread
#undef Rwrite
#undef Rrresvport
#undef Rshutdown
#undef Rlisten
#undef Rclose
#undef Rdup
#undef Rdup2
#undef Rfclose
#undef Rgethostbyname
/* Set this to allow group writeability of $HOME, .ssh and authorized_keys */
#undef ALLOW_GROUP_WRITEABILITY
/* Define if you want the login patches. */
#undef USELOGIN
#undef PATH_LOGIN
/* Define this if your gettimeofday doesn't have TZ parameter */
#undef HAVE_NO_TZ_IN_GETTIMEOFDAY
/* Define this if you want to compile in Kerberos support. */
#undef KERBEROS
/* Define this if you want to compile in Kerberos V5 support.
   KERBEROS must be compiled in as well. This can be done at configure
   time with the --with-kerberos5 argument*/
#undef KRB5
/* Define this if you want to pass the Kerberos TGT. */
#undef KERBEROS_TGT_PASSING
/* Define this if you dont have SIGINFO as signal but some other macro */
#undef HAVE_INCOMPATIBLE_SIGINFO
/* Define this if your spwd struct defined shadow.h have sp_expire field */
#undef HAVE_STRUCT_SPWD_EXPIRE
/* Define this if your spwd struct defined shadow.h have sp_inact field */
#undef HAVE_STRUCT_SPWD_INACT
/* Define this if you want to enable TCP_NODELAY option */
#undef ENABLE_TCP_NODELAY
/* File used to override /etc/nologin on a per-user basis.  The default is
   /etc/nologin.allow. */
#undef NOLOGIN_ALLOW
/* Where to find the X11 socket */
#undef X11_DIR
/* Define this if you want to enable SO_LINGER option */
#undef ENABLE_SO_LINGER
/* Define this if you want to enable scp statistics option */
#undef WITH_SCP_STATS
/* Define this to be the path to the passwd program */
#undef PASSWD_PATH
/* Define this if you have SGI proj.h */
#undef HAVE_SGI_PROJ_H
/* Define this if you want to have scp statistics on by default */
#undef SCP_STATISTICS_ENABLED
/* Define this if you want to have scp statistics on by default for each
   file */
#undef SCP_ALL_STATISTICS_ENABLED
