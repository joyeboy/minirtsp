#ifndef __AUTHENTICATION_H__
#define __AUTHENTICATION_H__

#ifdef __cplusplus
extern "C" {
#endif

#define HTTP_AUTH_BASIC		(0)
#define HTTP_AUTH_DIGEST	(1)

typedef struct _authentication
{
	int type;
	char user[64];
	char pwd[64];
	
}Authentication_t;

extern int AUTH_init(struct _authentication *auth,int type,const char *user,const char *pwd);
extern int AUTH_destroy(struct _authentication *auth);
extern int AUTH_validate(struct _authentication *auth);

#ifdef __cplusplus
}
#endif
#endif
