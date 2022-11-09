#include <stdio.h>
#include <iostream>
#include <stdint.h>
#include <array>
#include <vector>
#include <math.h>
#include <fstream>
#include <string>

using namespace std;

class yuv
{
public:
   yuv();
   yuv(string in_addr, string out_addr, int width, int height, int numFrames);
   ~yuv();

   void read_yuv_frame(int * raw_dst, int frame_idx);
   void write_yuv(int * raw_dst, bool firstFrame);
   void read_two_frames(int * raw_dst_1, int idx1, int * raw_dst_2, int idx2, int origStride, int* size_dst);
   void read_select_write(int *frame_idx, int numFrames);
   void get_block(int block_pos_x, int block_pos_y, int block_size, int* frame, int* out_block);
   void set_block(int block_pos_x, int block_pos_y, int block_size, int* frame, int* in_block);
   int ssd_block(int cur_block_pos_x, int cur_block_pos_y, int ref_block_pos_x, int ref_block_pos_y, int block_size, int* reference_frame, int* current_frame);
   void quantize(int* in_signal, int quantization_parameter, int* out_signal);
   void dequantize(int* in_signal, int quantization_parameter, int* out_signal);
   void motion_estimation_block(int *reference_frame, int *current_frame, int curr_block_x, int curr_block_y, int search_size, int block_size, int *out_motion_vectors, int &bestSsd, bool fastSearch = true);
   void motion_estimation_frame(int search_size, int block_size, int *reference_frame, int *current_frame, int *prediction_frame, bool fastSearch = true);
   void distortion_map(int* distorted, int *original, int* heat_map);
   int width();
   int height();

private:
   string in_address;
   string out_address;
   int m_width, m_height;
   int m_numFrames;
};


