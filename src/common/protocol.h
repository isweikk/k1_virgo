

#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__



typedef struct {
    int type;   //0= unvalid cmd, 1 = common command, 2= key-value
    int mode;
    int spin;
    int steer;
    int gearbox;
    int ptz;
    int color;
}StCmdParam;

int ParseString(const char *str, StCmdParam * prm);

#endif
