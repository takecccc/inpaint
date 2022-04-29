#ifndef INPAINT_H_INCLUDED
#define INPAINT_H_INCLUDED

#include <opencv2/core.hpp>

int inpaintParallel(cv::InputArray src, cv::InputArray mask, cv::OutputArray dst);
// int inpaint(cv::InputArray src, cv::InputArray mask, cv::OutputArray dst);

#endif