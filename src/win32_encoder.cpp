#include "VideoEncoder.h"
#include "vpx/vpx_image.h"
#include <windows.h>

static int XOffset = 0;

static unsigned char *Memory;
static HDC dc;
static HDC memDC;
static HBITMAP destBMP;
static HDC hScreenDC;

unsigned char* readBMP(char* filename)
{
    int i;
    FILE* f = fopen(filename, "rb");
    unsigned char info[54];
    fread(info, sizeof(unsigned char), 54, f); // read the 54-byte header

    // extract image height and width from header
#if 0
    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int size = 3 * width * height;
    unsigned char* data = new unsigned char[size]; // allocate 3 bytes per pixel
    fread(data, sizeof(unsigned char), size, f); // read the rest of the data at once
    fclose(f);

    for(i = 0; i < size; i += 3)
    {
            unsigned char tmp = data[i];
            data[i] = data[i+2];
            data[i+2] = tmp;
    }
#endif

#if 1
    int destWidth = 400;
    int destHeight = 300;

	if (!Memory) {
		Memory = (unsigned char *)VirtualAlloc(0, destWidth * destHeight * 3, MEM_COMMIT, PAGE_READWRITE);
		//HDC memDC = CreateCompatibleDC(0);
		dc = GetDC(0);
		memDC = CreateCompatibleDC(dc);
		destBMP = CreateCompatibleBitmap(dc, destWidth, destHeight);
		hScreenDC = CreateDC("DISPLAY", NULL, NULL, NULL);
	}

	int width = GetDeviceCaps(hScreenDC, HORZRES);
	int height = GetDeviceCaps(hScreenDC, VERTRES);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreenDC, width, height);
	HDC hMemoryDC = CreateCompatibleDC(hScreenDC);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemoryDC, hBitmap);
	BitBlt(hMemoryDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);
	hBitmap = (HBITMAP)SelectObject(hMemoryDC, hOldBitmap);

	BITMAPINFO bitmapInfo;
	bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfo.bmiHeader.biBitCount = 24;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biSizeImage = width*height * 3;
	bitmapInfo.bmiHeader.biWidth = width;
	bitmapInfo.bmiHeader.biHeight = height;

	HGDIOBJ old = SelectObject(memDC,destBMP);
    int result = StretchDIBits(memDC,0,0,destWidth,destHeight,0,0,width,height,data,&bitmapInfo,DIB_RGB_COLORS,SRCCOPY);   
	//SetDIBits(memDC, destBMP, 0, destHeight, data, &bitmapInfo, DIB_RGB_COLORS);
	SelectObject(memDC, old);

	BITMAPINFO bitmapInfoDest;
	bitmapInfoDest.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoDest.bmiHeader.biBitCount = 24;
	bitmapInfoDest.bmiHeader.biCompression = BI_RGB;
	bitmapInfoDest.bmiHeader.biPlanes = 1;
	bitmapInfoDest.bmiHeader.biSizeImage = destWidth*destHeight * 3;
	bitmapInfoDest.bmiHeader.biWidth = destWidth;
	bitmapInfoDest.bmiHeader.biHeight = destHeight;

    int result2 = GetDIBits(memDC,destBMP,0,destHeight,Memory,&bitmapInfoDest,DIB_RGB_COLORS);
    
	//DeleteObject(destBMP);    
	//DeleteDC(0, memDC);

    return Memory;
#else
	return data;
#endif
}

void addFrame(VideoEncoder* encoder, int width, int height) {
    vpx_image_t* vpxImage = vpx_img_alloc(NULL, VPX_IMG_FMT_RGB24, width, height, 1);
	unsigned char *pixel = vpxImage->img_data;

    unsigned char *buffer = readBMP("testbig.bmp");

    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            #if 0
            *pixel++ = x + XOffset; //R
            *pixel++ = 0; //G
            *pixel++ = 255; //B
            #else
            *pixel++ = *buffer++;
            *pixel++ = *buffer++;
            *pixel++ = *buffer++;
            #endif
        }
    }
    encoder->writeFrame(vpxImage);  
    vpx_img_free(vpxImage);

    XOffset++;
}

int main(int ArgCount, char **Args) {
    int fps = 30;
    int bitrate = 5000;
    char* outfile = "test.webm";

    VideoEncoder* encoder = new VideoEncoder(outfile, fps, bitrate);    

    int width = 400;
    int height = 300;    
    int maxFrames = 900;

    for(int frameIndex = 0; frameIndex < maxFrames; frameIndex++) {
        addFrame(encoder, width, height);
    }
    encoder->finish();
} 