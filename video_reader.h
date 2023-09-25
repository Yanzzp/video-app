#ifndef VIDEO_APP_VIDEO_READER_H
#define VIDEO_APP_VIDEO_READER_H
#include <iostream>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
}

struct VideoReaderState{
    int width,height;
    int video_stream_index;

    AVFormatContext *av_format_ctx;
    AVCodecContext *av_codec_ctx;
    AVFrame *av_frame;
    AVPacket *av_packet;
    SwsContext *sws_scaler_ctx;

};

bool video_reader_open(VideoReaderState *state, const char *filename);

bool video_reader_read_frame(VideoReaderState *state, uint8_t* frame_buffer);

bool video_reader_close(VideoReaderState *state);
#endif