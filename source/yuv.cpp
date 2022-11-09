#include "yuv.h"

yuv::yuv(string in_path, string out_path, int width, int height, int numFrames)
{
   in_address = in_path;
   out_address = out_path;
   m_width = width;
   m_height = height;
   m_numFrames = numFrames;
}

yuv::yuv()
{
}

yuv::~yuv()
{
}

int yuv::width()
{
   return m_width;
}

int yuv::height()
{
   return m_height;
}

void yuv::read_yuv_frame(int * raw_dst, int frame_idx) 
{
   std::ifstream fid_file;
   fid_file.open(in_address, std::ios::in | ios::binary);
   int* raw_src_tmp = raw_dst;
   if (fid_file.is_open()) {
      fid_file.seekg(0, fid_file.end);
      int length = static_cast<int>(fid_file.tellg());
      int length_in_frame = length / m_numFrames;
      fid_file.seekg(length_in_frame * frame_idx, fid_file.beg);
      std::vector<uint8_t> buff_v(m_width);
      uint8_t *buf = &(buff_v[0]);
      for (int j = 0; j < m_height; ++j) {
         fid_file.read(reinterpret_cast<char*>(buf), m_width);
         for (int i = 0; i < m_width; ++i)
            raw_src_tmp[i] = short(buf[i]);
         raw_src_tmp += m_width;
      }
      fid_file.close();
   }
   else
      std::cout << "unable to open the file";
}

void yuv::write_yuv(int * raw_dst, bool firstFrame) {
   std::ofstream fid_file;

   fid_file.open(out_address, !firstFrame ? std::ios::app : std::ios::out);
   int x = 0;
   if (fid_file.is_open()) {
      std::vector<uint8_t> buff_v(m_width);
      uint8_t *buf = &(buff_v[0]);

      for (int j = 0; j < m_height; ++j) {
         for (int i = 0; i < m_width; ++i) {
            buf[i] = uint8_t(raw_dst[x++]);
         }
         fid_file.write(reinterpret_cast<char*>(buf), m_width);
      }

      std::vector<uint8_t> buff_uv(m_width / 2);
      uint8_t *buf_uv = &(buff_uv[0]);

      for (int i = 0; i < m_width / 2; ++i) {
         buf_uv[i] = uint8_t(127);
      }
      for (int z = 0; z < m_height; ++z) {
         fid_file.write(reinterpret_cast<char*>(buf_uv), m_width / 2);
      }

      fid_file.close();
   }
   else {
      std::cout << "unable to open the file";
   }
}

void yuv::read_two_frames(int * raw_dst_1, int idx1, int * raw_dst_2, int idx2, int origStride, int* size_dst)
{
   read_yuv_frame(raw_dst_1, idx1);
   read_yuv_frame(raw_dst_2, idx2);
   return;
}

void yuv::read_select_write(int *frame_idx, int numFrames)
{
   int * raw_tmp; 
   raw_tmp = new int[m_width*m_height];

   bool isFirst = true;
   for (int frame = 0; frame < numFrames; frame++)
   {
      int sizes[2] = { m_width, m_height };
      read_yuv_frame(raw_tmp, frame_idx[frame]);
      write_yuv(raw_tmp, isFirst);
      isFirst = false;
   }

   delete[] raw_tmp;
   return;
}

void yuv::get_block(int block_pos_x, int block_pos_y, int block_size, int* frame, int* out_block)
{
   /* -- 
   Locate pixels of a square block at <X: block_pos_x, Y: block_pos_y>
   Store the pixels (row-by-row) in the output out_block array
   -- */
   for (int line = 0; line < block_size; line++)
      memcpy(out_block + line * block_size, frame + (block_pos_y + line) * m_width + block_pos_x, block_size * sizeof(out_block));
   return;
}

void yuv::set_block(int block_pos_x, int block_pos_y, int block_size, int* frame, int* in_block)
{
   /* --
   Locate pixels of a square block at <X: block_pos_x, Y: block_pos_y>
   Store the pixels (row-by-row) in the output out_block array
   -- */
   for (int line = 0; line < block_size; line++)
      memcpy(frame + (block_pos_y + line) * m_width + block_pos_x, in_block + line * block_size, block_size * sizeof(in_block));
   return;
}



int yuv::ssd_block(int cur_block_pos_x, int cur_block_pos_y, int ref_block_pos_x, int ref_block_pos_y, int block_size, int* reference_frame, int* current_frame)
{
   /* -- 
   Compute Sum of Square Error (SSD) between two blocks of size block_size in current_frame and reference_frame
   The two blocks obviously have the same size, but they might be located in different coordinates
   -- */
   int *curr_block = new int[block_size * block_size]; 
   int *ref_block = new int[block_size * block_size];
   get_block(cur_block_pos_x, cur_block_pos_y, block_size, current_frame, curr_block);
   get_block(ref_block_pos_x, ref_block_pos_y, block_size, reference_frame, ref_block);
   int ssd = 0;
   for (int pixel = 0; pixel < block_size * block_size; pixel++)
      ssd += static_cast<int>(pow((curr_block[pixel] - ref_block[pixel]), 2));
   delete[] curr_block;
   delete[] ref_block;
   return ssd;
}

void yuv::quantize(int* in_signal, int quantization_parameter, int* out_signal)
{
   /* -- 
   Quantize pixel values with the step size defined by the quantization_parameter
   Output will have a different domain than the input (ie divided by the step size)
   -- */
   return;
}

void yuv::dequantize(int* in_signal, int quantization_parameter, int* out_signal)
{
   /* -- 
   Dequantize pixel values wrt the quantization_parameter
   Output will bring back the pixel values to their initial domain
   -- */
   return;
}

void yuv::motion_estimation_block(int *reference_frame, int *current_frame, int curr_block_x, int curr_block_y, int search_size, int block_size, int *out_motion_vectors, int &bestSsd, bool fastSearch)
{
   /* --
   Perform ME for a square block of size block_size positioned at (curr_block_x, curr_block_y)
   And return the best MV (and bestSsd)
   -- */

   int currSsd;
   bestSsd = INT_MAX;
   const int speed_factor = 2;
   const int mask = (1 << speed_factor) - 1;
   /* -- Motion search -- */
   for (int y = curr_block_y - search_size; y < curr_block_y + search_size; y++)
      for (int x = curr_block_x - search_size; x < curr_block_x + search_size; x++)
      {
         /* -- check if outside frame -- */
         if (y >= 0 && x >= 0 && (y + block_size - 1) < m_height && (x + block_size - 1) < m_width)
         {
            /* -- In fast mode, do not check points that 
                  are not multiple of (1<<speed_factor) -- */
            if (fastSearch && ((y & mask) || x & mask))
               continue;
            currSsd = ssd_block(curr_block_x, curr_block_y, x, y, block_size, reference_frame, current_frame);
            if (currSsd < bestSsd)
            {
               bestSsd = currSsd;
               out_motion_vectors[0] = x - curr_block_x;
               out_motion_vectors[1] = y - curr_block_y;
            }
         }
      }
   return;
}


void yuv::motion_estimation_frame(int search_size, int block_size, int *reference_frame, int *current_frame, int *prediction_frame, bool fastSearch)
{
   /* --
   Perform ME for all non-overlapping square blocks within current frame.
   And return the prediction frame
   -- */

   if (m_width % block_size || m_height % block_size)
      printf("WARNING: frame size is not a multiple of block size\n");

   const int num_block_hor = m_width / block_size;
   const int num_block_ver = m_height / block_size;
   printf("progress:");
   for (int y = 0; y < num_block_ver / 10; y++)
      printf(".");
   printf("\n         ");
   int *block = new int[block_size * block_size];
   int *pred_block = new int[block_size * block_size];
   for (int y = 0; y < num_block_ver; y++)
   {
      if (y % 10 == 0) printf(".");
      for (int x = 0; x < num_block_hor; x++)
      {
         int mv[2];
         int bestSsd;
         motion_estimation_block(reference_frame, current_frame, x * block_size, y * block_size, search_size, block_size, mv, bestSsd, fastSearch);
         get_block(x * block_size + mv[0], y * block_size + mv[1], block_size, reference_frame, pred_block);
         set_block(x * block_size, y * block_size, block_size, prediction_frame, pred_block);
      }
   }
   printf("\n");
   delete[] block;
   delete[] pred_block;
   return;
}


void yuv::distortion_map(int* distorted, int *original, int* heat_map)
{
   return;
}
