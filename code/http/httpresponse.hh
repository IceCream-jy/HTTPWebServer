#ifndef _HTTP_RESPONSE_H_
#define _HTTP_RESPONSE_H_

#include <unordered_map>
#include <fcntl.h>       // open
#include <unistd.h>      // close
#include <sys/stat.h>    // stat
#include <sys/mman.h>    // mmap, munmap

#include "../buffer/buffer.hh"
#include "../log/log.hh"

class HttpResponse {
public:
    HttpResponse();
    ~HttpResponse();

    void Init(const std::string& srcDir, std::string& path, 
              bool isKeepAlive = false, int code = -1);
    void MakeResponse(buffer& buff);
    void UnmapFile();
    char* File();
    size_t FileLen() const;
    void ErrorContent(buffer& buff, std::string message);
    int Code() const { return code_; }

private:
    void AddStateLine_(buffer &buff);
    void AddHeader_(buffer &buff);
    void AddContent_(buffer &buff);

    void ErrorHtml_();
    std::string GetFileType_();

    // HTTP 状态码（200 404）
    int code_;
    bool isKeepAlive_;

    std::string path_;      // 相对路径，实际路径为 srcDir_ + path_
    std::string srcDir_;
    // 内存映射的地址
    char* mmFile_; 
    // 储存资源文件的 stat
    struct stat mmFileStat_;    

    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;
    // 状态行最后一个字段
    static const std::unordered_map<int, std::string> CODE_STATUS;
    // 错误状态码的对应 html 文件的路径
    static const std::unordered_map<int, std::string> CODE_PATH;
};


#endif