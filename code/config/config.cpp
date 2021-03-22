#include "config.hh"

Config::Config(){
    port = 9999;
    trigMode = 3;
    timeoutMS = 60000;
    optLinger = false;
    connPoolNum = 12;
    threadNum = 6;
    openLog = false;
    logLevel = 1;
    logQueSize = 1024;
};

void Config::parse_arg(int argc, char** argv){
    int opt;
    const char* option = "p:m:s:oc:t:le:q";
    /*
    p : port
    m : trigMode
    s : timeoutMS
    o : optLinger(无参， 指定选项则为开启)
    c : connPoolNum
    t: threadPoolNum
    l : openLog(无参，指定选项为开启)
    e : logLevel
    q : logQueSize
    */
    while((opt = getopt(argc, argv, option)) != -1){
        switch(opt){
            case 'p' : {
                port = atoi(optarg);
                break;
            }
            case 'm' : {
                trigMode = atoi(optarg);
                break;
            }
            case 's' : {
                timeoutMS = atoi(optarg);
                break;
            }
            case 'o' : {
                optLinger = true;
                break;
            }
            case 'c' : {
                connPoolNum = atoi(optarg);
                break;
            }
            case 't' : {
                threadNum = atoi(optarg);
                break;
            }
            case 'l' : {
                openLog = true;
                break;
            }
            case 'e' : {
                logLevel = atoi(optarg);
                break;
            }
            case 'q' : {
                logQueSize = atoi(optarg);
                break;
            }
            default : break;
        }
    }
    
}