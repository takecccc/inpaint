#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <cxxopts.hpp>
#include <string>
#include <iostream>
#include <vector>
#include "inpaint.h"

int main(int argc, char **argv) {
	cxxopts::Options options("inpaint", "inpaint masked region");
	options.add_options()
		("src", "source image", cxxopts::value<std::string>(), "RGB or RGBA. Alpha channel will not processed.")
		("mask", "mask image", cxxopts::value<std::string>(), "Black pixel is inpainted. For 4-channel images, use channel 4.")
		("dst", "destination image", cxxopts::value<std::string>())
		;
	try {
		auto args = options.parse(argc, argv);

		if (args.count("help")) {
			std::cout << options.help() << std::endl;
			return 0;
		}

		// =====================================================
		// read src
		std::cout << "read src" << std::endl;
		cv::Mat src = cv::imread(args["src"].as<std::string>(), cv::IMREAD_UNCHANGED);
		if (src.empty()) {
			std::cerr << "src read failed." << std::endl;
			return -1;
		}
		CV_Assert(src.channels() == 3 || src.channels() == 4);
		cv::Mat bgr;
		cv::Mat alpha;
		if (src.channels() == 4) {
			cv::cvtColor(src, bgr, cv::COLOR_BGRA2BGR);
			std::vector<cv::Mat> channels(4);
			cv::split(src, channels);
			alpha = channels[3];
		}
		else {
			bgr = src;
		}

		// =====================================================
		// read mask
		std::cout << "read mask" << std::endl;
		cv::Mat mask = cv::imread(args["mask"].as<std::string>(), cv::IMREAD_UNCHANGED);
		if (mask.empty()) {
			std::cerr << "mask read failed." << std::endl;
			return -1;
		}
		CV_Assert(mask.channels() == 1 || mask.channels() == 3 || mask.channels() == 4);
		switch(mask.channels()){
			case 4:
				{
					std::vector<cv::Mat> channels(4);
					cv::split(mask, channels);
					mask = channels[3];
				}
				mask = (mask == cv::Scalar(0));
				break;
			case 3:
				cv::inRange(mask, cv::Scalar(0,0,0), cv::Scalar(0,0,0), mask);
				break;
			case 1:
				mask = (mask == cv::Scalar(0));
				break;
			default:
				std::cerr << "not supported mask channel count." << std::endl;
				break;
		}
		std::cout << mask.channels() << std::endl;

		// =====================================================
		// inpaint
		std::cout << "inpaint" << std::endl;
		cv::Mat dst;
		if (inpaintParallel(bgr, mask, dst)) {
			std::cerr << "inpaint failed." << std::endl;
			return -1;
		}
		if (dst.empty()) {
			std::cerr << "dst is empty." << std::endl;
			return -1;
		}

		// =====================================================
		// write dst
		if (src.channels() == 4) {
			std::vector<cv::Mat> channels;
			cv::split(dst, channels);
			channels.push_back(alpha);
			cv::merge(channels, dst);
		}
		std::cout << "write dst" << std::endl;
		if (!cv::imwrite(args["dst"].as<std::string>(), dst)) {
			std::cerr << "dst write failed." << std::endl;
			return -1;
		}
	}
	catch (cxxopts::OptionException ex) {
		std::cerr << ex.what() << std::endl;
		std::cerr << options.help() << std::endl;
		return -1;
	}
	return 0;
}