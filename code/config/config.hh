#ifndef _CONFIG_H_
#define _CONFIG_H_

#include "../server/webserver.hh"

class Config{
public:
    Config();
    ~Config() = default;

    void parse_arg(int argc, char** argv);

    int port;
    int trigMode;
    int timeoutMS;
    int optLinger;
    int connPoolNum;
    int threadNum;
    bool openLog;
    int logLevel;
    int logQueSize;
private:

};

#endif