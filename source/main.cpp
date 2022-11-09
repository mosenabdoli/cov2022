#include "yuv.h"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;

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

/* Parameters*/
const int seqIdx = 0;
const int search_size = 15;
const int block_size = 8;
const bool fast_search = false;
const int current_frame_idx = 1;
const int reference_frame_idx = 0;
const int seq_width = sequence_widths[seqIdx];
const int seq_height = sequence_heights[seqIdx];
const int seq_nb_frames = sequence_frames[seqIdx];

/* buffers */
int *current_frame = new int[seq_width * seq_height];
int *reference_frame = new int[seq_width * seq_height];
int *prediction_frame = new int[seq_width * seq_height];
int free();

/* Misc */
void make_output_dir();

int main()
{
   make_output_dir();
   string in_yuv_path  = yuv_dir + sequence_names[seqIdx] + ".yuv";
   string out_yuv_path = "outputs/" + sequence_names[seqIdx] + "_output.yuv";

   /* init */
   yuv sequence(in_yuv_path, out_yuv_path, seq_width, seq_height, seq_nb_frames);

   /* read input */
   sequence.read_yuv_frame(current_frame, current_frame_idx);
   sequence.read_yuv_frame(reference_frame, reference_frame_idx);
   
   /* perform motion estimation */
   sequence.motion_estimation_frame(search_size, block_size, reference_frame, current_frame, prediction_frame, fast_search);

   /* visualize the prediction */
   sequence.write_yuv(prediction_frame, true);

   free();
   return 0;
}

int free()
{
   delete[] current_frame;
   delete[] reference_frame;
   delete[] prediction_frame;
   return 0;
}

void make_output_dir()
{
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
	return;
}