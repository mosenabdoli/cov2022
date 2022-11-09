#include "yuv.h"
#include <filesystem>
#include <iostream>

const string yuv_dir = "E:/Docs/Teaching/ESIR-2022/TP-2021/00-Sequences/";
const string sequence_names[5] = {
           /* seqIdx = 0 */       "BlowingBubbles_416x240p_50Hz_iyuv",
           /* seqIdx = 1 */       "BQSquare_416x240p_60Hz_iyuv",
           /* seqIdx = 2 */       "Johnny_1280x720p_60Hz_i420",
           /* seqIdx = 3 */       "KristenAndSara_1280x720p_60Hz_i420",
           /* seqIdx = 4 */       "Kimono_1920x1080_20Hz_120_raw",
                                 };
const int sequence_widths [5] = { 416, 416, 1280, 1280, 1920};
const int sequence_heights[5] = { 240, 240, 720,  720,  1080 };
const int sequence_frames [5] = { 501, 601, 540,  540,  240 };

namespace fs = std::filesystem;

int main()
{
   const int seqIdx = 0;
   string in_yuv_address  = yuv_dir + sequence_names[seqIdx] + ".yuv";

   string out_yuv_address = "outputs/" + sequence_names[seqIdx] + "_output.yuv";
   try {
	   if (!fs::exists("outputs"))
	   {
		   if (fs::create_directory("outputs"))
			   std::cout << "Created a directory\n";
		   else
			   std::cerr << "Failed to create a directory\n";
	   }
   }
   catch (const std::exception& e) {
	   std::cerr << e.what() << '\n';
   }
   yuv sequence(in_yuv_address, out_yuv_address, sequence_widths[seqIdx], sequence_heights[seqIdx], sequence_frames[seqIdx]);

   /* test motion_estimation_block */
   const int search_size = 15;
   const int block_size = 8;
   const bool fast_search = false;
   int current_frame_idx = 1;  int *current_frame = new int[sequence.width() * sequence.height()];
   sequence.read_yuv_frame(current_frame, current_frame_idx);
   int reference_frame_idx = 0;
   int *reference_frame = new int[sequence.width() * sequence.height()];
   sequence.read_yuv_frame(reference_frame, reference_frame_idx);
   int *prediction_frame = new int[sequence.width() * sequence.height()];
   sequence.motion_estimation_frame(search_size, block_size, reference_frame, current_frame, prediction_frame, fast_search);
   
   // visualize the prediction
   sequence.write_yuv(prediction_frame, true);
   delete[] current_frame;
   delete[] reference_frame;
   delete[] prediction_frame;
   return 0;
}