#include "inpaint.h"
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <math.h>

using namespace cv;

class inpaintParallel_8u_Body : public ParallelLoopBody
{
private:
	Mat* img;
	Mat* maskCur;
	Mat* maskNew;
	bool *inpainted;
	// offsets is a group of pixels that determines whether to paint.
	constexpr static int K = 8;
	constexpr static int offsets[K][2] = {
		{-1,-1},{-1,0},{-1,1},
		{0,-1},{0,1},
		{1,-1},{1,0},{1,1}
	};
	// subOffsets are additional pixels that are referenced to determine the color to paint.
	constexpr static int subK = 25 - 9;
	constexpr static int subOffsets[subK][2] = {
		{-2,-2},{-2,-1},{-2,0},{-2,1},{-2,2},
		{-1,-2},{-1,2},
		{0,-2},{0,2},
		{1,-2},{1,2},
		{2,-2},{2,-1},{2,0},{2,1},{2,2}
	};
public:

	inpaintParallel_8u_Body(Mat& _img, Mat& _maskCur, Mat& _maskNew, bool& _inpainted) :
		img(&_img), maskCur(&_maskCur), maskNew(&_maskNew), inpainted(&_inpainted)
	{
	}
	virtual void operator() (const Range& range) const
	{
		const int imgStep = (int)(img->step);
		const int maskCurStep = (int)(maskCur->step);
		const int maskNewStep = (int)(maskNew->step);

		int imgOffsets[K];
		int maskCurOffsets[K];
		int imgSubOffsets[subK];
		int maskCurSubOffsets[subK];
		for (int i = 0; i < K; ++i) {
			imgOffsets[i] = imgStep * offsets[i][0] + offsets[i][1] * 3;
			maskCurOffsets[i] = maskCurStep * offsets[i][0] + offsets[i][1];
		}
		for (int i = 0; i < subK; ++i) {
			imgSubOffsets[i] = imgStep * subOffsets[i][0] + subOffsets[i][1] * 3;
			maskCurSubOffsets[i] = maskCurStep * subOffsets[i][0] + subOffsets[i][1];
		}

		uchar* pImgRow = img->ptr(range.start) + 6;
		uchar* pMaskCurRow = maskCur->ptr(range.start) + 2;
		uchar* pMaskNewRow = maskNew->ptr(range.start) + 2;
		for (int i = range.start; i < range.end; ++i, pImgRow += imgStep, pMaskCurRow += maskCurStep, pMaskNewRow += maskNewStep)
		{
			uchar* pImg = pImgRow;
			uchar* pMaskCur = pMaskCurRow;
			uchar* pMaskNew = pMaskNewRow;
			for (int j = 2; j < img->cols-2; ++j, pImg += 3, ++pMaskCur, ++pMaskNew) {
				if (*pMaskCur > 0) {
					int n = 0;
					int sum_b = 0, sum_g = 0, sum_r = 0;
					for (int k = 0; k < K; ++k) {
						if (*(pMaskCur + maskCurOffsets[k]) == 0) {
							++n;
							sum_b += *(pImg + imgOffsets[k]);
							sum_g += *(pImg + imgOffsets[k] + 1);
							sum_r += *(pImg + imgOffsets[k] + 2);
						}
					}
					if (n > 0) {
						for (int k = 0; k < subK; ++k) {
							if (*(pMaskCur + maskCurSubOffsets[k]) == 0) {
								++n;
								sum_b += *(pImg + imgSubOffsets[k]);
								sum_g += *(pImg + imgSubOffsets[k] + 1);
								sum_r += *(pImg + imgSubOffsets[k] + 2);
							}
						}
						*pImg = saturate_cast<uchar>(sum_b / n);
						*(pImg + 1) = saturate_cast<uchar>(sum_g / n);
						*(pImg + 2) = saturate_cast<uchar>(sum_r / n);
						*pMaskNew = 0;
						*inpainted = true;
					}
				}
			}
		}
	}
};

int inpaintParallel(InputArray src, InputArray mask, OutputArray dst) {
	CV_Assert(src.channels() == 3);
	CV_Assert(mask.channels() == 1);
	CV_Assert(src.type() == CV_8UC3);
	CV_Assert(mask.type() == CV_8UC1);
	CV_Assert(src.size() == mask.size());
	CV_Assert(dst.empty());
	Mat srcMat = src.getMat();
	Mat maskMat = mask.getMat();
	Mat bufImg;
	Mat bufMask;
	Mat bufMaskCurrent;
	copyMakeBorder(srcMat, bufImg, 2, 2, 2, 2, BORDER_REPLICATE);
	copyMakeBorder(maskMat, bufMask, 2, 2, 2, 2, BORDER_CONSTANT, Scalar(255));
	bool inpainted = false;
	inpaintParallel_8u_Body body(bufImg, bufMaskCurrent, bufMask, inpainted);
	int count = 0;
	do {
		// std::cout << count++ << std::endl;
		bufMask.copyTo(bufMaskCurrent);
		inpainted = false;
		parallel_for_(Range(2, bufImg.rows-2), body);
	} while (inpainted);
	bufImg = bufImg(Rect(2, 2, srcMat.cols, srcMat.rows)).clone();
	dst.assign(bufImg);
	return 0;
}

// int inpaint(InputArray src, InputArray mask, OutputArray dst) {
// 	CV_Assert(src.channels() == 3);
// 	CV_Assert(mask.channels() == 1);
// 	CV_Assert(src.type() == CV_8UC3);
// 	CV_Assert(mask.type() == CV_8UC1);
// 	CV_Assert(src.size() == mask.size());
// 	CV_Assert(dst.empty());
// 	Mat srcMat = src.getMat();
// 	Mat maskMat = mask.getMat();
// 	Mat bufImg;
// 	Mat bufMask;
// 	Mat bufMaskCurrent;
// 	copyMakeBorder(srcMat, bufImg, 1, 1, 1, 1, BORDER_REPLICATE);
// 	copyMakeBorder(maskMat, bufMask, 1, 1, 1, 1, BORDER_CONSTANT, Scalar(255));
// 	Vec3b* pImg = nullptr;
// 	Vec3b* pImgU = nullptr;
// 	Vec3b* pImgD = nullptr;
// 	uchar* pMaskNew = nullptr;
// 	uchar* pMaskCur = nullptr;
// 	uchar* pMaskCurU = nullptr;
// 	uchar* pMaskCurD = nullptr;
// 	int h = bufImg.rows;
// 	int w = bufImg.cols;
// 	bool painted = false;
// 	int count = 0;
// 	do {
// 		std::cout << count++ << std::endl;
// 		painted = false;
// 		bufMask.copyTo(bufMaskCurrent);
// 		for (int y = 1; y < h-1; ++y) {
// 			pImg		= bufImg.ptr<Vec3b>(y);
// 			pMaskNew	= bufMask.ptr<uchar>(y);
// 			pMaskCur	= bufMaskCurrent.ptr<uchar>(y);
// 			pImgU		= bufImg.ptr<Vec3b>(y-1);
// 			pMaskCurU	= bufMaskCurrent.ptr<uchar>(y-1);
// 			pImgD		= bufImg.ptr<Vec3b>(y+1);
// 			pMaskCurD	= bufMaskCurrent.ptr<uchar>(y+1);
// 			for (int x = 1; x < w-1; ++x) {
// 				if (*pMaskCur > 0) {
// 					int n = 0;
// 					Vec3s sumColor = Vec3s(0, 0, 0);
// 					// up
// 					if (pMaskCurU[-1] == 0) { sumColor += pImgU[-1]; ++n; }
// 					if (pMaskCurU[0] == 0) { sumColor += pImgU[0]; ++n; }
// 					if (pMaskCurU[1] == 0) { sumColor += pImgU[1]; ++n; }
// 					// middle
// 					if (pMaskCur[-1] == 0) { sumColor += pImg[-1]; ++n; }
// 					if (pMaskCur[1] == 0) { sumColor += pImg[1]; ++n; }
// 					// down
// 					if (pMaskCurD[-1] == 0) { sumColor += pImgD[-1]; ++n; }
// 					if (pMaskCurD[0] == 0) { sumColor += pImgD[0]; ++n; }
// 					if (pMaskCurD[1] == 0) { sumColor += pImgD[1]; ++n; }
// 					if (n > 0) {
// 						*pImg = (sumColor / n);
// 						*pMaskNew = 0;
// 						painted = true;
// 					}
// 				}
// 				++pImg;
// 				++pMaskNew;
// 				++pMaskCur;
// 				++pImgU;
// 				++pMaskCurU;
// 				++pImgD;
// 				++pMaskCurD;
// 			}
// 		}
// 	} while (painted);
// 	bufImg = bufImg(Rect(1, 1, srcMat.cols, srcMat.rows)).clone();
// 	dst.assign(bufImg);
// 	return 0;
// }