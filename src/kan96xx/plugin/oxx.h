/* $OpenXM: OpenXM/src/kan96xx/plugin/oxx.h,v 1.6 2002/11/07 13:32:06 takayama Exp $ */
struct object KoxCreateClient(struct object ip,struct object portStream,
			      struct object portControl);
struct object KoxCloseClient(struct object client);
struct object KoxIsThereErrorClient(struct object ob);
struct object KoxReq(struct object client,struct object func,
		     struct object ob1);
struct object KoxGet(struct object client);
struct object KoxGetFromControl(struct object client);
struct object KoxMultiSelect(struct object clients,struct object t);
struct object KoxWatch(struct object client,struct object f);
extern int SerialOX;
struct object KoxGenPortFile(void);
void KoxRemovePortFile(void);
struct object KoxGetPort(struct object host);
struct object KoxGetPort1(struct object host);
struct object KoxCreateClient2(struct object peer,struct object ipmask,struct object pass);
struct object KoxGenPass(void);
int KgetCmoTagOfObject(struct object obj);

struct object KoxPushCMD(struct object client,struct object cmd);
struct object KoxPushCMO(struct object client,struct object ob);
struct object KoxCreateControl_RFC_101(struct object peer,struct object ipmask,struct object pass);
struct object KoxCreateEngine_RFC_101(struct object peer,struct object ipmask,struct object pass,struct object engineID);

void oxClientListUpdate(struct object ob);
void oxClientListRemove(struct object ob);
struct object KoxGetClientList();

