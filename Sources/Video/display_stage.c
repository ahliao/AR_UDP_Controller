/**
 * @file display_stage.c
 * @author nicolas.brulez@parrot.com
 * @date 2012/09/25
 *
 * This stage is a naive example of how to display video using GTK2 + Cairo
 * In a complete application, all GTK handling (gtk main thread + widgets/window creation)
 *  should NOT be handled by the video pipeline (see the Navigation linux example)
 *
 * The window will be resized according to the picture size, and should not be resized bu the user
 *  as we do not handle any gtk event except the expose-event
 *
 * This example is not intended to be a GTK/Cairo tutorial, it is only an example of how to display
 *  the AR.Drone live video feed. The GTK Thread is started here to improve the example readability
 *  (we have all the gtk-related code in one file)
 */

// Self header file
#include "display_stage.h"

// GTK/Cairo headers
#include <cairo.h>
#include <gtk/gtk.h>

#include "cv.h"
#include "highgui.h"
#include "qr_reader.h"

// Funcs pointer definition
const vp_api_stage_funcs_t display_stage_funcs = {
    NULL,
    (vp_api_stage_open_t) display_stage_open,
    (vp_api_stage_transform_t) display_stage_transform,
    (vp_api_stage_close_t) display_stage_close
};

// Extern so we can make the ardrone_tool_exit() function (ardrone_testing_tool.c)
// return TRUE when we close the video window
extern int exit_program;

// Boolean to avoid asking redraw of a not yet created / destroyed window
bool_t gtkRunning = FALSE;

IplImage *ipl_image_from_data(uint8_t* data, int reduced_image, int width, int height)
{
	IplImage *currframe;
	IplImage *dst;

	currframe = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 3);
	dst = cvCreateImage(cvSize(width,height), IPL_DEPTH_8U, 3);

	currframe->imageData = (char*)data;
	cvCvtColor(currframe, dst, CV_BGR2RGB);
	cvReleaseImage(&currframe);

	return dst;
}

// Picture size getter from input buffer size
// This function only works for RGB565 buffers (i.e. 2 bytes per pixel)
static void getPicSizeFromBufferSize (uint32_t bufSize, uint32_t *width, uint32_t *height)
{
    if (NULL == width || NULL == height)
    {
        return;
    }

    switch (bufSize)
    {
    case 50688: //QCIF > 176*144 *2bpp
        *width = 176;
        *height = 144;
        break;
    case 153600: //QVGA > 320*240 *2bpp
        *width = 320;
        *height = 240;
        break;
    case 460800: //360p > 640*360 *2bpp
        *width = 640;
        *height = 360;
        break;
    case 1843200: //720p > 1280*720 *2bpp
        *width = 1280;
        *height = 720;
        break;
    default:
        *width = 0;
        *height = 0;
        break;
    }
}

// Get actual frame size (without padding)
void getActualFrameSize (display_stage_cfg_t *cfg, uint32_t *width, uint32_t *height)
{
    if (NULL == cfg || NULL == width || NULL == height)
    {
        return;
    }

    *width = cfg->decoder_info->width;
    *height = cfg->decoder_info->height;
}


C_RESULT display_stage_open (display_stage_cfg_t *cfg)
{
    // Check that we use RGB565
    if (2 != cfg->bpp)
    {
        // If that's not the case, then don't display anything
        cfg->paramsOK = FALSE;
    }
    else
    {
        // Else, start GTK thread and window
        cfg->paramsOK = TRUE;
        cfg->frameBuffer = NULL;
        cfg->fbSize = 0;
        START_THREAD (gtk, cfg);
    }
    return C_OK;
}

C_RESULT display_stage_transform (display_stage_cfg_t *cfg, vp_api_io_data_t *in, vp_api_io_data_t *out)
{
    // Ask GTK to redraw the window
    uint32_t width = 0, height = 0;
    getPicSizeFromBufferSize (in->size, &width, &height);
	IplImage *frame = ipl_image_from_data((uint8_t*)in->buffers[0],1,640,360);

	//IplImage* test = cvCreateImage(cvGetSize(frame), frame->depth,frame->nChannels);
	//double a[9]={-1,20,1,-1,20,1,-1,20,1};
	//CvMat kernel= cvMat(3,3,CV_32FC1,a);
	//cvFilter2D(frame,test,&kernel,cvPoint(-1,-1));

	IplImage* outputimg = cvCreateImage(cvGetSize(frame), frame->depth,3);
	cvCvtColor(frame, outputimg, CV_RGB2BGR);
	QR_Data data;
	process_QR(outputimg, &data, frame);

	cvNamedWindow("video", CV_WINDOW_AUTOSIZE);
	cvShowImage("video", frame);
	if(cvWaitKey(1) == 27) exit_program = 0;
	cvReleaseImage(&frame);
	cvReleaseImage(&outputimg);

    return C_OK;
}

C_RESULT display_stage_close (display_stage_cfg_t *cfg)
{
    // Free all allocated memory
    if (NULL != cfg->frameBuffer)
    {
        vp_os_free (cfg->frameBuffer);
        cfg->frameBuffer = NULL;
    }

    return C_OK;
}
