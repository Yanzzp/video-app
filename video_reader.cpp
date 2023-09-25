#include "video_reader.h"
bool video_reader_open(VideoReaderState *state, const char *filename) {
    auto& video_stream_index = state->video_stream_index;
    auto& av_format_ctx = state->av_format_ctx;
    auto& av_codec_ctx = state->av_codec_ctx;
    auto& av_frame = state->av_frame;
    auto& av_packet = state->av_packet;

    av_format_ctx = avformat_alloc_context();
    if (!av_format_ctx) {
        std::cout << "av_format_ctx failed" << std::endl;
        return false;
    }
    if (avformat_open_input(&av_format_ctx, filename, NULL, NULL) != 0) {
        std::cout << "avformat_open_input failed" << std::endl;
        return false;
    }

    // Find the first video stream inside the file
    video_stream_index = -1;
    AVCodecParameters *av_codec_params;
    const AVCodec *av_codec;
    for (int i = 0; i < av_format_ctx->nb_streams; ++i) {
        auto steam = av_format_ctx->streams[i];
        av_codec_params = steam->codecpar;
        av_codec = avcodec_find_decoder(av_codec_params->codec_id);

        if (!av_codec) {
            continue;
        }
        if (av_codec_params->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            break;
        }
        if (steam->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_stream_index = i;
            std::cout << "video_stream_index: " << video_stream_index << std::endl;
            break;
        }
    }
    if (video_stream_index == -1) {
        std::cout << "video_stream_index failed" << std::endl;
        return false;
    }
    // Set up a codec context for the decoder
    av_codec_ctx = avcodec_alloc_context3(av_codec);
    if (!av_codec_ctx) {
        std::cout << "avcodec_alloc_context3 failed" << std::endl;
        return false;
    }

    if (avcodec_parameters_to_context(av_codec_ctx, av_codec_params) < 0) {
        char error_msg[AV_ERROR_MAX_STRING_SIZE];
        av_strerror(errno, error_msg, AV_ERROR_MAX_STRING_SIZE);
        std::cout << "avcodec_parameters_to_context failed: " << error_msg << std::endl;
        return false;
    }
    if (avcodec_open2(av_codec_ctx, av_codec, NULL)) {
        std::cout << "avcodec_open2 failed" << std::endl;
        return false;
    }
    av_frame = av_frame_alloc();
    if (!av_frame) {
        std::cout << "av_frame_alloc failed" << std::endl;
        return false;
    }
    av_packet = av_packet_alloc();
    if (!av_packet) {
        std::cout << "av_packet_alloc failed" << std::endl;
        return false;
    }
    return true;
}



bool video_reader_read_frame(VideoReaderState *state, uint8_t* frame_buffer){
    auto& video_stream_index = state->video_stream_index;
    auto& av_format_ctx = state->av_format_ctx;
    auto& av_codec_ctx = state->av_codec_ctx;
    auto& av_frame = state->av_frame;
    auto& av_packet = state->av_packet;
    https://www.youtube.com/watch?v=lFncAJqDQeE
    21.52
    // Decode one frame
    int response;
    while (av_read_frame(av_format_ctx, av_packet) >= 0) {
        if (av_packet->stream_index != video_stream_index) {
            continue;
        }
        response = avcodec_send_packet(av_codec_ctx, av_packet);
        if (response < 0) {
            std::cout << "avcodec_send_packet failed"<<av_err2str(response) << std::endl;
            return false;
        }
        response = avcodec_receive_frame(av_codec_ctx, av_frame);
        if (response == AVERROR(EAGAIN) || response == AVERROR_EOF) {
            continue;
        } else if (response < 0) {
            std::cout << "avcodec_receive_frame failed" <<av_err2str(response)<< std::endl;
            return false;
        }
        av_packet_unref(av_packet);
        break;
    }
    uint8_t *data = new uint8_t[av_frame->width * av_frame->height * 4];
    SwsContext* sws_scaler_ctx = sws_getContext(av_frame->width,
                                                av_frame->height,
                                                av_codec_ctx->pix_fmt,
                                                av_frame->width,
                                                av_frame->height,
                                                AV_PIX_FMT_RGB0,
                                                SWS_BILINEAR,
                                                NULL,
                                                NULL,
                                                NULL);
    if (!sws_scaler_ctx) {
        std::cout << "sws_getContext failed" << std::endl;
        return false;
    }
    uint8_t *dest[4] = {data, NULL, NULL, NULL};
    int dest_linesize[4] = {av_frame->width * 4, 0, 0, 0};

    sws_scale(sws_scaler_ctx, av_frame->data, av_frame->linesize, 0, av_frame->height, dest, dest_linesize);
    sws_freeContext(sws_scaler_ctx);
    *width_out = av_frame->width;
    *height_out = av_frame->height;
    *data_out = data;
    avformat_close_input(&av_format_ctx);
    avformat_free_context(av_format_ctx);
    avcodec_free_context(&av_codec_ctx);
    av_frame_free(&av_frame);
    av_packet_free(&av_packet);
    return true;
}

bool video_reader_close(VideoReaderState *state){

}