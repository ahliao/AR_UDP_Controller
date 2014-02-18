/**
 *  Processes a IplImage in OpenCV for 
 *  QR codes
 *  First converts to a Mat format for convience
 *  Written by: Alex Liao
*/

#ifndef QR_READER_H
#define QR_READER_H

// Include the libraries needed by OpenCV and file reading
#include <stdlib.h>
#include <stdio.h>
#include <highgui.h>
#include <cv.h>
#include <zbar.h>

// The capture dimensions
extern const int FRAME_WIDTH;
extern const int FRAME_HEIGHT;

// Where the camera origin is (quadcopter view)
extern const int MID_X;
extern const int MID_Y;

// Linear variables for the qr_length to inches (distance)
extern const double DISTANCE_M;
extern const int DISTANCE_B;

// PI for the trig calculations
extern const double MATH_PI;

typedef struct
{
	double distance;
	double angle;
	double x;
	double y;
} QR_Data;

void process_QR(IplImage* img, QR_Data * data, IplImage* outimg);

#endif
