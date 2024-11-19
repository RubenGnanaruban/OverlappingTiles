/*
* Author: Vimalatharmaiyah Gnanaruban
* October 2022
*/

#include <string>
#include <iostream>
#include<opencv2/opencv.hpp>
//#include <fstream>
//#include <cstdlib>
#include <filesystem>
#include <opencv2/core/utility.hpp>
#include <vector>

namespace fs = std::filesystem;
using namespace std;
using namespace cv;

string getFileName(const string& s) {
    /*char sep = '/';
#ifdef _WIN32
    sep = '\\';
#endif*/
    char sep = '\\';
    size_t i = s.rfind(sep, s.length());
    if (i != string::npos)
    {
        string filename = s.substr(i + 1, s.length() - i);
        size_t lastindex = filename.find_last_of(".");
        string rawname = filename.substr(0, lastindex);
        return(rawname);
    }
    return("");
}

string insert_FileName(const string& s) {
    char sep = '\\';
    size_t i = s.rfind(sep, s.length());
    if (i != string::npos)
    {
        string filename = s.substr(i + 1, s.length() - i);
        size_t lastindex = filename.find_last_of(".");
        string rawname = filename.substr(0, lastindex);
        size_t i = rawname.rfind(' ', rawname.length());
        string newname = rawname.substr(0, i) + '_' + rawname.substr(i + 1, rawname.length() - 1);
        return(newname);
    }
    return("");
}

string makeSiblingFolder(string dir_raw, string new_folder_name) {
  size_t ii = dir_raw.rfind('\\', dir_raw.length());
  string dir_patch = dir_raw.substr(0, ii);
  dir_patch += "\\";
  dir_patch += new_folder_name;
  dir_patch += "_" + dir_raw.substr(ii + 1, dir_raw.length());
  fs::create_directories(dir_patch);
  return dir_patch;
}

int main()
{
    int imgheight, imgwidth, M, N;
    int N_x = 2;
    int N_y = 2;
    int overlap_original_sides = 0;// 32;
    int overlap_original_top_bottom = 0;// 24;
    int n_X_tiles = 10; // number of horizontal tiles
    int n_Y_tiles = 30; // number of vertical tiles
;
    //string dirname = "D:\\PaigeAI subtype 254\\FIBI Breast\\Breast HVS-21-123 section 2 Serial H&E FIBI EDOF scan 1\\Breast HVS-21-123 section 2 Serial H&E FIBI EDOF scan 1";
    string dirname = "C:\\Users\\Histo\\Documents\\ProstateTissue_for_CycleGAN\\187\\187";
    //string dirname = "C:\\Users\\Histo\\Documents\\Breast Tissue Scans for CycleGAN Project\\NonMalignant Breast Samples\\NonMalignant Breast Sample 3\\Non-Malignant Breast Sample 3 Raw FIBI Images";
    /*string dir_OLT = dirname + "\\overlapping_tiles";
    fs::create_directories(dir_OLT);*/
    string dir_OLT = makeSiblingFolder(dirname, "overlapping_tiles");

    vector<cv::String> fn;
    glob(dirname, fn, false);

    size_t count = fn.size(); //number of jpg files in original tiles
    Mat img = imread(fn[0]);
    imgheight = img.rows;
    imgwidth = img.cols;
    M = imgheight / N_y;
    N = imgwidth / N_x;

    int i_y = 0, i_x;
    Mat overlapping_tile;
    //Case 1: vertically aligned with the original tiles, horizontally in between tiles
    while (i_y < n_Y_tiles) {
        i_x = 0;
        while (i_x < n_X_tiles - 1) {
            int i_first_image = n_X_tiles * i_y + i_x;
            Mat img_l = imread(fn[i_first_image]);
            Mat tile_l = img_l(Range(0, imgheight), Range(N - overlap_original_sides, imgwidth - overlap_original_sides));
            Mat img_r = imread(fn[i_first_image + 1]);
            Mat tile_r = img_r(Range(0, imgheight), Range(overlap_original_sides, N + overlap_original_sides));
            cv::hconcat(tile_l, tile_r, overlapping_tile);
            imwrite(dir_OLT + "\\" + getFileName(fn[i_first_image]) + '_' + ".jpg", overlapping_tile);
            ++i_x; 
        }
        ++i_y;
    }
    //Case 2: vertically in between the original tiles, horizontally aligned
    i_y = 0;
    while (i_y < n_Y_tiles - 1) {
        i_x = 0;
        while (i_x < n_X_tiles) {
            int i_first_image = n_X_tiles * i_y + i_x;
            Mat img_top = imread(fn[i_first_image]);
            Mat tile_top = img_top(Range(M - overlap_original_top_bottom, imgheight - overlap_original_top_bottom), Range(0, imgwidth));
            Mat img_b = imread(fn[i_first_image + n_X_tiles]);
            Mat tile_b = img_b(Range(overlap_original_top_bottom, M + overlap_original_top_bottom), Range(0, imgwidth));
            cv::vconcat(tile_top, tile_b, overlapping_tile);
            imwrite(dir_OLT + "\\" + insert_FileName(fn[i_first_image]) + ".jpg", overlapping_tile);
            ++i_x;
        }
        /*while (i_x < 2 * n_X_tiles - 1) {
            if (i_x % 2) {//Case 3: vertically and horizontally in between tiles
                int i_first_image = n_X_tiles * i_y + (i_x - 1) / 2;
                Mat img_top_l = imread(fn[i_first_image]);
                Mat tile_top_l = img_top_l(Range(M - overlap_original_top_bottom, imgheight - overlap_original_top_bottom), Range(N - overlap_original_sides, imgwidth - overlap_original_sides));
                Mat img_b_l = imread(fn[i_first_image + n_X_tiles]);
                Mat tile_b_l = img_b_l(Range(overlap_original_top_bottom, M + overlap_original_top_bottom), Range(N - overlap_original_sides, imgwidth - overlap_original_sides));
                Mat ol_tile_l, ol_tile_r;
                cv::vconcat(tile_top_l, tile_b_l, ol_tile_l);
                Mat img_top_r = imread(fn[i_first_image + 1]);
                Mat tile_top_r = img_top_r(Range(M - overlap_original_top_bottom, imgheight - overlap_original_top_bottom), Range(overlap_original_sides, N + overlap_original_sides));
                Mat img_b_r = imread(fn[i_first_image + n_X_tiles + 1]);
                Mat tile_b_r = img_b_r(Range(overlap_original_top_bottom, M + overlap_original_top_bottom), Range(overlap_original_sides, N + overlap_original_sides));
                cv::vconcat(tile_top_r, tile_b_r, ol_tile_r);
                cv::hconcat(ol_tile_l, ol_tile_r, overlapping_tile);
                imwrite(dir_OLT + "\\" + insert_FileName(fn[i_first_image]) + '_' + ".jpg", overlapping_tile);
            }
            else {//Case 2: vertically in between, horizontally aligned
                int i_first_image = n_X_tiles * i_y + i_x / 2;
                Mat img_top = imread(fn[i_first_image]);
                Mat tile_top = img_top(Range(M - overlap_original_top_bottom, imgheight - overlap_original_top_bottom), Range(0, imgwidth));
                Mat img_b = imread(fn[i_first_image + n_X_tiles]);
                Mat tile_b = img_b(Range(overlap_original_top_bottom, M + overlap_original_top_bottom), Range(0, imgwidth));
                cv::vconcat(tile_top, tile_b, overlapping_tile);
                imwrite(dir_OLT + "\\" + insert_FileName(fn[i_first_image]) + ".jpg", overlapping_tile);
            }
            ++i_x;
        }*/
            /*
                imwrite(dir_OLT + getFileName(fn[i_y]) + "_Y" + b + "_X" + a + ".jpg", tiles);
                auto xs = fn[i_y];
            }*/

        ++i_y;
    }
}

