/*
Author: Katrina Santillan
Date: 14 May 2018
Robotics and Automation: Object Measurement Assignment

1. Read the Reference Image as a gray scale image
2. Get rid of the noise in the image
3. Threshold the image
4. Detect edges using Canny
5. Find the contours
6. Draw the minimum bounding rectangle around the contours of the object
7. Identify objects (rectangle or circle)
8. Measure the circle's dimension using the rectangle as a reference
*/



#include "stdafx.h"
#include <opencv2/opencv.hpp>

int main()
{
	cv::Mat image = cv::imread("../Reference Image.png", cv::IMREAD_GRAYSCALE);
	cv::Mat working_image;
	cv::Size image_size;

	if (image.empty())
	{
		std::cout << "Could not find or open image" << std::endl;
	}

	cv::Mat blur, threshold, edges;
	cv::GaussianBlur(
		image,
		blur,
		cv::Size(5, 5),
		0
	);

	cv::threshold(
		blur,
		threshold,
		0,
		255,
		cv::THRESH_OTSU | cv::THRESH_BINARY
	);

	//Detect edges using Canny
	int threshold_ratio = 2;			//set threshold ratio, recommended 2:1 or 3:1
	double low_threshold = 10;			//set low threshold value
	double high_threshold = low_threshold * threshold_ratio;
	cv::Canny(threshold, edges, low_threshold, high_threshold);


	//Find contours
	std::vector<std::vector<cv::Point>> contour;
	std::vector<cv::Vec4i> hierarchy;
	cv::findContours(
		edges,						//input
		contour,					//detected contours, stored as a vector of points
		hierarchy,					//image topology
		cv::RetrievalModes::RETR_CCOMP,							//retrieves all of the contours and organizes them into a two-level hierarchy
		cv::ContourApproximationModes::CHAIN_APPROX_SIMPLE		//compresses segments and leaves only their end points, eg rectangle contour encoded with 4 points
	);

	std::vector<std::vector<cv::Point> > contours_poly(contour.size());
	std::vector<cv::Rect> bound_rect(contour.size());

	//Draw bounding rectangles
	cv::Rect circle, reference;
	for (int i = 0; i < contour.size(); i++)
	{
		approxPolyDP(cv::Mat(contour[i]), contours_poly[i], 3, true);
		bound_rect[i] = cv::boundingRect(cv::Mat(contours_poly[i]));

		std::cout << "Contour " << i + 1 << std::endl;
		std::cout << "Point(x,y) = " << bound_rect[i].size() << std::endl;

		double shape = cv::matchShapes(contour[0], contour[i], 1, 0.0);

		//Determine shape of contour
		if (bound_rect[i].width != bound_rect[i].height)
		{
			reference = bound_rect[i];			//if sides are not equal, shape is rectangle, use as scale
		}
		else
		{
			circle = bound_rect[i];				//if sides are equal, then shape is circle
		}
		
	}

	//Reference shape
	int pixels_per_mm, known_width;
	known_width = 210;										//assumed to be the width of A4
	pixels_per_mm = reference.width / known_width;

	//Circle
	double diameter = circle.width / pixels_per_mm;
	
	std::cout << "\n\nCircle\n\nDiameter = " << diameter << " mm" << std::endl;
	std::cout << "Circumeference = " << 3.14*diameter << " mm" << std::endl;
	std::cout << "Area = " << (diameter / 2)*(diameter / 2)*3.14 << " mm" << std::endl;


	//Draw contours
	cv::RNG rng(12345);
	cv::Mat draw_contours = cv::Mat::zeros(edges.size(), CV_8UC3);
	for (int i = 0; i < contour.size(); i++)
	{
		//cv::Point org = (bound_rect[i].height, bound_rect[i].width);
		cv::Scalar colour = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		cv::drawContours(draw_contours, contour, i, colour, 3);
		cv::rectangle(draw_contours, bound_rect[i], colour, 3);
	};

	cv::Size working_image_size = { 593, 962 };

	cv::resize(image, working_image, working_image_size);
	cv::namedWindow("Reference Image", CV_WINDOW_AUTOSIZE);
	cv::imshow("Reference Image", working_image);

	
	cv::resize(draw_contours, working_image, working_image_size);
	cv::namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	cv::imshow("Contours", working_image);

	cv::waitKey(0);
	cv::destroyAllWindows();

    return 0;
}

