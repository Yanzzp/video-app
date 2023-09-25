#include <iostream>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <inttypes.h>
}


static bool load_frame1(const char *filename, int *width_out, int *height_out, unsigned char **data_out) {

}