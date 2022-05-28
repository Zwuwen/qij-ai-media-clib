#include "common.h"
#include "base_media.h"
#include "algorithm"

cbase_media::cbase_media()
{
}

cbase_media::~cbase_media()
{
   
}

UINT32 cbase_media::init_resource(vector<media_conf_t>& media_conf_list)
{
    if(media_conf_list.size() <= 0)
    {
        cmylog::mylog("WAR","media configure list size <=0\n");
        return QJ_BOX_OP_CODE_INPUTPARAMERR;
    }
    //资源列表初始化
    for(auto it = media_conf_list.begin() ; it != media_conf_list.end() ; ++it)
    {
        //流配置初始化
        cmylog::mylog("INFO","media configure id=%s\n",it->m_id.c_str());
        this->m_media_conf_list.push_back(*it);
        ffmpeg_pull_flow_param_t ffmpeg_pull_obj;
        ffmpeg_pull_obj.m_id = it->m_id;
        this->m_pull_flow_param_list.push_back(ffmpeg_pull_obj);
    }
    avformat_network_init();
    cmylog::mylog("INFO","base media configure complete\n");
    return QJ_BOX_OP_CODE_SUCESS;
}

 void cbase_media::reset_sample_resource(media_conf_t& media_conf)
 {
//    for(int i = 0 ; i < this->m_media_conf_list.size();++i)
//    {
//        if(m_media_conf_list[i].m_id != media_conf.m_id)
//            continue;
//        m_media_conf_list[i] = media_conf;
//
//        for(auto & it : this->m_pull_flow_param_list)
//        {
//            if(it.m_id != media_conf.m_id)
//                continue;
//            stop_pull_media_task(it.m_id);
//            start_pull_media_task(it.m_id);
//        }
//    }
//     for(int i = 0 ; i < this->m_media_conf_list.size();++i)
    for (auto &conf:m_media_conf_list)
     {
         if(conf.m_id != media_conf.m_id) continue;

         conf = media_conf;
         for(auto & it : this->m_pull_flow_param_list)
         {
             if(it.m_id != media_conf.m_id) continue;
             stop_pull_media_task(it.m_id);
             start_pull_media_task(it.m_id);
         }
     }
 }

void cbase_media::destory_resource()
{
    m_pull_flow_param_list.clear() ;
    m_media_conf_list.clear();

//    while(!this->m_pull_flow_param_list.empty())
//    {
//        this->m_pull_flow_param_list.pop_back();
//    }
////    vector<ffmpeg_pull_flow_param_t>().swap(this->m_pull_flow_param_list);
//    while(!this->m_media_conf_list.empty())
//    {
//        this->m_media_conf_list.pop_back();
//    }
//    vector<media_conf_t>().swap(this->m_media_conf_list);
    avformat_network_deinit();
}

UINT32 cbase_media::start_pull_media_task(std::string media_flow_id)
{
    if(this->m_media_conf_list.empty())
    {
        cmylog::mylog("WAR","media configure list size <=0\n");
        return QJ_BOX_OP_CODE_INPUTPARAMERR;
    }
    try{
        //启动拉流线程
        for(auto & it : this->m_media_conf_list)
        {
            if(it.m_id != media_flow_id)
                continue;
            for(auto & pull_it : this->m_pull_flow_param_list)
            {
                if(pull_it.m_id == it.m_id)
                {
                    pull_it.m_thread = new std::thread([this,&pull_it,&it]()mutable{
//                        pull_it.m_is_running = true;
                        cbase_media::pull_flow_thread((void*)this,pull_it,it);
                    });
                    break;
                }
            }
            break;
        }
    }
    catch(const std::exception& e)
    {
        this->stop_pull_media_task(media_flow_id);
        cmylog::mylog("WAR","create %d pull flow thread failure\n",media_flow_id.c_str());
        return QJ_BOX_OP_CODE_CREATEFAILURE;
    }
    return QJ_BOX_OP_CODE_SUCESS;
}
UINT32 cbase_media::start_all_pull_media_task()
{
    if(this->m_media_conf_list.size() <= 0)
    {
        cmylog::mylog("WAR","media configure list size <=0\n");
        return QJ_BOX_OP_CODE_INPUTPARAMERR;
    }
    try{
        //启动拉流解码线程
        for(auto it = this->m_media_conf_list.begin() ; it != this->m_media_conf_list.end(); ++it)
        {
            for(auto pull_it = this->m_pull_flow_param_list.begin() ; pull_it != this->m_pull_flow_param_list.end(); ++pull_it)
            {
                if(pull_it->m_id == it->m_id)
                {
                    cmylog::mylog("INFO","media handler thread begin,id=%s\n",pull_it->m_id.c_str());
                    pull_it->m_thread = new std::thread([this,pull_it,it]()mutable{
//                    pull_it->m_is_running = true;
                    cbase_media::pull_flow_thread((void*)this,*pull_it,*it);
                    });
                    break;
                }
                else
                {
                    continue;
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        this->stop_all_pull_media_task();
        cmylog::mylog("WAR","create all pull flow thread failure\n");
        return QJ_BOX_OP_CODE_CREATEFAILURE;
    }
    return QJ_BOX_OP_CODE_SUCESS;
}

void cbase_media::pull_flow_thread(void* pthis,ffmpeg_pull_flow_param_t& pull_flow_param,media_conf_t& conf )
{
    cmylog::mylog("INFO","decode type=%s\n",conf.m_decode_type.c_str());
    
    #ifdef VPUMEDIA
    if(conf.m_decode_type == VPU_DECODE_TYPE)
    {
        pull_flow_param.m_decode = new cvpu_decode();
    }
    else
    {
        pull_flow_param.m_decode = new ccpu_decode();
    }
    #else
    pull_flow_param.m_decode = new ccpu_decode();
    #endif
    assert(pull_flow_param.m_decode !=NULL);

    pull_flow_param.m_is_running = true;
    cbase_media::pull_flow_thread_for_decode(pthis,pull_flow_param, conf,pull_flow_param.m_decode);
}

void cbase_media::pull_flow_thread_for_decode(void* pthis,ffmpeg_pull_flow_param_t& pull_flow_param,media_conf_t& conf,cbase_decode* decode)
{
    cbase_media* in_this = (cbase_media*)pthis;
    ffmpeg_pull_flow_param_t* param = (ffmpeg_pull_flow_param_t*)&pull_flow_param;
    media_conf_t* pconf = (media_conf_t*)&conf;
Begin:
    cmylog::mylog("INFO","##############thread begin,id=%s###############\n",pconf->m_id.c_str());
    int read_pack_cnt = -1;
    int retvalue = -1;
    int video_index = -1;
    //curtime
    steady_clock::time_point start_time = steady_clock::now();
    cmylog::mylog("INFO","pull flow begin,id=%s\n",pconf->m_id.c_str());
    //net类型初始化参数
    if(pconf->m_type ==NET_FLOW_TYPE )
    {
        av_dict_set(&param->m_opts,"rtsp_transport",pconf->m_net_type.c_str(),0);
        av_dict_set(&param->m_opts,"stimeout","5000000",0);
        av_dict_set(&param->m_opts,"buffer_size",pconf->m_buffer_size.c_str(),0); 
        av_dict_set(&param->m_opts,"max_delay",pconf->m_max_delay.c_str(),0); 
        assert(param->m_opts != NULL);
    }
    
    //打开媒体源
    cmylog::mylog("INFO","avformat_open_input,id=%s\n",pconf->m_id.c_str());
    param->m_input_ctx = avformat_alloc_context();

    while (TRUE){
        retvalue = avformat_open_input(&param->m_input_ctx, (char*)pconf->m_url.c_str(),NULL, &param->m_opts);
        if(retvalue < 0)
        {
            cmylog::mylog("ERR","could not open meidia source,url=%s\n",pconf->m_url.c_str());
            std::cout<<"could not open meidia source"<<std::endl;
            /* retry always  */

            sleep(5);
            continue;
//            goto End;
        }
        break;
    }
    param->m_input_ctx->flags |= AVFMT_FLAG_NONBLOCK;
    cmylog::mylog("INFO","open meidia success,url=%s\n",pconf->m_id.c_str());
    //获取媒体信息
    retvalue = avformat_find_stream_info(param->m_input_ctx,NULL);
    cmylog::mylog("INFO","avformat find media stream info,result = %d,url=%s\n",retvalue,pconf->m_url.c_str());
     if(retvalue < 0)
    {
        cmylog::mylog("ERR","could not find stream infomation,url=%s\n",pconf->m_url.c_str());
        goto End;
    }
    //匹配视频流的index
    video_index = -1;
    std::cout<<"streams_num:"<<param->m_input_ctx->nb_streams<<std::endl;
//    std::cout<<"streams:"<<param->m_input_ctx->streams[0]->codecpar->codec_type<<std::endl;
    std::cout<<"streams:"<<param->m_input_ctx->streams[0]->codec->codec_type<<std::endl;
    for(int i = 0;i<param->m_input_ctx->nb_streams;++i)
    {
//        if(param->m_input_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        if(param->m_input_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            video_index = i;
            break;
        }
    }
//    while (true){
//        std::cout<<"wait..."<<std::endl;
//        std::cout<<"video_index:"<<video_index<<std::endl;
//        sleep(1);
//    }
    if(video_index == -1)
    {
        cmylog::mylog("ERR","find media stream failure,url=%s\n",pconf->m_url.c_str());
        goto End;
    }
    cmylog::mylog("INFO","find stream infomation success,url=%s\n",pconf->m_url.c_str());
    //解码器初始化消息
    cmylog::mylog("INFO","product video index,url=%s\n",pconf->m_url.c_str());

//    while (true){
//        std::cout<<"wait..."<<std::endl;
//        sleep(1);
//    }
    //拉流开始
    #if 0   //解决缓存增加问题，av_read_frame接口导致pkt不断申请内存而不释放内存，导致虚拟内存增加
    param->m_pkt = av_packet_alloc();
    if(param->m_pkt == NULL)
    {
        cmylog::mylog("ERR","alloc packet failure,url=%s\n",pconf->m_url.c_str());
        goto End;
    }
    #else
    av_init_packet(&param->m_pkt);
    #endif
    decode->init((void*)param);
    cmylog::mylog("INFO","decode initialization complete,url=%s\n",pconf->m_url.c_str());
    
    if(decode->ctrl(std::to_string(video_index)) != QJ_BOX_OP_CODE_SUCESS)
    {
        cmylog::mylog("ERR","decode ctrl failure,url=%s\n",pconf->m_url.c_str());
        goto End;
    }
    param->m_video_index = video_index;
    read_pack_cnt = -1;
    cmylog::mylog("INFO","decode data type=%d\n",pconf->m_decode_data_type);
    decode->set_cache_type(pconf->m_decode_data_type);
    while(param->m_is_running)
    {
         //timeout
        //cmylog::mylog("INFO","timeout count %d\n",duration_cast<seconds>(steady_clock::now() - start_time).count());
        if (duration_cast<seconds>(steady_clock::now() - start_time).count() > atoi(pconf->m_unlink_timeout.c_str())) {
            cmylog::mylog("WAR", "av_read_frame() timeout,url=%s\n", pconf->m_url.c_str());
#pragma mark - 断网时没有释放vpu相关资源 begin 会重新init
            in_this->clean_pull_resource(pconf->m_id);
	        start_time = steady_clock::now();
            goto Begin;
            //break;
        }
        //read h264
        if (read_pack_cnt = av_read_frame(param->m_input_ctx,&param->m_pkt)<0)
        {
            //cmylog::mylog("WAR","av_read_frame() read packet count < 0,url=%s\n",pconf->m_url.c_str());
            continue;
        }
        else
        {
	        start_time = steady_clock::now();
        }
        
         if(param->m_pkt.stream_index == video_index)
         {
            //printf("type=%d\n",pconf->m_decode_data_type) ;
            decode->data() ;
         }    
        av_packet_unref(&param->m_pkt);
	    av_init_packet(&param->m_pkt);
         //cmylog::mylog("INFO","thread id = %d,id=%s\n",std::this_thread::get_id(),pconf->m_id.c_str());
    }
End:
    /*回收资源*/
    //解码器资源回收
    decode->deinit();
    delete decode;
    decode = nullptr;
    //清除拉流资源
    in_this->clean_pull_resource(pconf->m_id);
}

void cbase_media::stop_pull_media_task(std::string media_flow_id)
{
    for(auto & pull_it : this->m_pull_flow_param_list)
    {
        if(pull_it.m_id != media_flow_id) continue;
        if(pull_it.m_id == media_flow_id)
        {
            if(pull_it.m_is_running){
                pull_it.m_is_running = false;
                if(pull_it.m_thread){
                    pull_it.m_thread->join();
                    delete pull_it.m_thread;
                    pull_it.m_thread = nullptr;
                }
            }
        }
        break;
    }

}
void cbase_media::stop_all_pull_media_task()
{
    for(auto pull_it = this->m_pull_flow_param_list.begin() ; pull_it != this->m_pull_flow_param_list.end(); ++pull_it)
    {
       if(pull_it->m_is_running){
           pull_it->m_is_running = false;
           pull_it->m_thread->join();
           delete pull_it->m_thread;
           pull_it->m_thread = nullptr;
       }
    }
}

void cbase_media::clean_pull_resource(std::string media_flow_id)
{
     for(auto pull_it = this->m_pull_flow_param_list.begin() ; pull_it != this->m_pull_flow_param_list.end(); ++pull_it)
     {
         if(pull_it->m_id == media_flow_id)
         {
             this->clean(*pull_it);
             break;
         }
     }
}
void cbase_media::clean_all_pull_resource()
{
     for(auto pull_it = this->m_pull_flow_param_list.begin() ; pull_it != this->m_pull_flow_param_list.end(); ++pull_it)
     {
        this->clean(*pull_it);
     }
}

void cbase_media::clean(ffmpeg_pull_flow_param_t& pull_param)
{
    ffmpeg_pull_flow_param_t* param = &pull_param;
    av_packet_unref(&param->m_pkt);
    cmylog::mylog("INFO","---release decode swscontext\n");
    if(param->m_img_conver_ctx != NULL)
    {
        sws_freeContext(param->m_img_conver_ctx);
        param->m_img_conver_ctx = NULL;
    }
    cmylog::mylog("INFO","---release decode sws frame\n");
    if(param->m_sw_frame != NULL)
    {
        av_frame_free(&param->m_sw_frame);
        param->m_sw_frame = NULL;
    }
    cmylog::mylog("INFO","---release decode frame\n");
    if(param->m_frame != NULL)
    {
        av_frame_free(&param->m_frame);
        param->m_frame = NULL;
    }
    cmylog::mylog("INFO","---release decode avcodecontext\n");
    if(param->m_avcodectxt != NULL)
    {
        avcodec_close(param->m_avcodectxt);
        param->m_avcodectxt = NULL;
    }
    cmylog::mylog("INFO","---release decode avformatcontext\n");
    if(param->m_input_ctx != NULL)
    {
        avformat_close_input(&param->m_input_ctx);
        param->m_input_ctx = NULL;
    }
    cmylog::mylog("INFO","---release decode avdict \n");
    if(param->m_opts != NULL)
    {
        av_dict_free(&param->m_opts);
        param->m_opts = NULL;
    }
}

UINT32 cbase_media::get_media_by_id(const char* media_id,int media_type,uint8_t * data,int size)
{
    std::string id = media_id;

    for(auto & it : m_pull_flow_param_list)
     {
         if(it.m_id == id)
         {
             //查看线程是否启动
            if(!it.m_is_running)
            {
                return 0;
            } 
#pragma mark -新增
             if(it.m_decode != nullptr)
            {
                if(!it.m_decode->is_run()){
                    return 0;
                }
                it.m_decode->set_cache_type(media_type);
                return it.m_decode->get((UINT8*)data, (UINT32)size);
            }
         }
     }
     return 0;
}
#pragma mark - 以下为新增
std::list<media_conf_t> &cbase_media::get_media_conf() {
    return m_media_conf_list;
}
/**
 * 添加资源
 * @param media_conf_list
 * @return
 */
int cbase_media::add_resource(vector<media_conf_t> &media_conf_list) {
    if (media_conf_list.empty()) {
        cmylog::mylog("WAR", "media configure list size <=0\n");
        return QJ_BOX_OP_CODE_INPUTPARAMERR;
    }
    //资源列表初始化
    for (auto &it: media_conf_list) {
        //流配置初始化
        cmylog::mylog("INFO", "media configure id=%s\n", it.m_id.c_str());
        this->m_media_conf_list.push_back(it);
        ffmpeg_pull_flow_param_t ffmpeg_pull_obj;
        ffmpeg_pull_obj.m_id = it.m_id;
        this->m_pull_flow_param_list.push_back(ffmpeg_pull_obj);
    }
    cmylog::mylog("INFO", "base media add complete\n");
    return QJ_BOX_OP_CODE_SUCESS;
}
/**
 * 删除资源
 * @param media_id
 * @return
 */
int cbase_media::remove_resource(const string& media_id) {
   string media_fow_id = media_id;
    /*①停止拉流解码线程，回收相应资源*/
    stop_pull_media_task(media_fow_id);

    /*②清除拉流/解码句柄*/
    m_pull_flow_param_list.erase(
            std::remove_if(
                    begin(m_pull_flow_param_list), end(m_pull_flow_param_list),
                    [&media_fow_id](const ffmpeg_pull_flow_param_t& m) {
                        return m.m_id == media_fow_id;
                    }
            ), m_pull_flow_param_list.end()
    );

    /*③清除资源句柄*/
    m_media_conf_list.erase(
            std::remove_if(
                    begin(m_media_conf_list), end(m_media_conf_list),
                    [&media_fow_id](const media_conf_t& m) {
                        return m.m_id == media_fow_id;
                    }
            ), m_media_conf_list.end()
    );

    return 0;
}
