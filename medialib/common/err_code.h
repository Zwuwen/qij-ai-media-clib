#ifndef __ERR_CODE_H__
#define __ERR_CODE_H__

#define    QJ_BOX_OP_CODE_SUCESS                        0//成功
#define    QJ_BOX_OP_CODE_UNKOWNERR            1//未知错误
#define    QJ_BOX_OP_CODE_INPUTPARAMERR        2//参数错误
#define    QJ_BOX_OP_CODE_NOSUPPORT            3//不支持
#define    QJ_BOX_OP_CODE_TIMEOUT              4//超时
#define    QJ_BOX_OP_CODE_ACCESSFAILURE        5//访问失败
#define    QJ_BOX_OP_CODE_CREATEFAILURE        6//创建错误
#define    QJ_BOX_OP_CODE_OPENFAILURE          7//打开失败(文件、设备)
#define    QJ_BOX_OP_CODE_DATAINVALID          8//数据无效
#define    QJ_BOX_OP_CODE_NOREADY              9//未就绪
#define    QJ_BOX_OP_CODE_CMDINVALID           10//命令错误
#define    QJ_BOX_OP_CODE_CONNECTFAILURE       11//链接失败
#define    QJ_BOX_OP_CODE_NETREQNOSUPPORT      12//请求不支持
#define    QJ_BOX_OP_CODE_NETBUSY              13////网络忙
#define    QJ_BOX_OP_CODE_DEVBUSY              14////设备忙
#define    QJ_BOX_OP_CODE_SRVNORUNNING                      15//服务未启动
#define    QJ_BOX_OP_CODE_PATHNOEXIST                            16//路径不存在
#define    QJ_BOX_OP_CODE_CHECKFAILURE                         17//校验失败
#define    QJ_BOX_OP_CODE_LOADCONFFAILURE                  18//加载配置失败
#define    QJ_BOX_OP_CODE_CLOSEDEVFAILURE                   20//关闭设备失败
#define    QJ_BOX_OP_CODE_NOINIT                                            21//未初始化
#define    QJ_BOX_OP_CODE_NOENOUGHSPACE                     22//没有足够空间
#define    QJ_BOX_OP_CODE_SAVECONFFAILURE                     23//保存配置失败
#define    QJ_BOX_OP_CODE_REGISTERFAILURE                       24//注册失败
#define    QJ_BOX_OP_CODE_VERSIONERR           25//版本错误
#define    QJ_BOX_OP_CODE_SVRINITFAILURE       26//系统初始化失败
#define    QJ_BOX_OP_CODE_DEVREFUSEACC         27//设备拒绝访问
#define    QJ_BOX_OP_CODE_ACCNOEXIST           28//访问不存在
#define    QJ_BOX_OP_CODE_DATAFORMATERROR      29//数据格式错误
#define    QJ_BOX_OP_CODE_LACKAUTHORITY        30//没有权限
#define    QJ_BOX_OP_CODE_EXCEPT               31//程序捕获异常
#define    QJ_BOX_OP_CODE_INAUTO               32//设备处于自动模式
#define    QJ_BOX_OP_CODE_INMANUAL             33//设备处于手动模式
#define    QJ_BOX_OP_CODE_DECOMPRESSIONFAILURE   34//解压失败
#define    QJ_BOX_OP_CODE_INLOGOUT             35//设备未认证
#define    QJ_BOX_OP_CODE_READONLY             36//只读
#define    QJ_BOX_OP_CODE_DEVICERETURNFAILED   37//设备返回执行失败
#define    QJ_BOX_OP_CODE_CONSQLFAILURE        50//链接数据库失败
#define    QJ_BOX_OP_CODE_HANDLESQLFAILURE     51//操作数据库失败
#define    QJ_BOX_OP_CODE_SQLCONNINVALID       52//无效链接
#define    QJ_BOX_OP_CODE_NETLOCALIPINVALID    60//本地IP无效
#define    QJ_BOX_OP_CODE_NETREMOTEIPINVALID   61//远程IP无效
#define    QJ_BOX_OP_CODE_SSLNOSUPPORT         62//SSL 不支持
#define    QJ_BOX_OP_CODE_FDINVALID                        63 //文件描述符无效
#define    QJ_BOX_OP_CODE_MALLOC_FAILURE                        64 //申请资源失败
#endif //