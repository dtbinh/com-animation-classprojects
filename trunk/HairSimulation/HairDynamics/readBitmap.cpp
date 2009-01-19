#include	<iostream>
#include	"glaux.h"
#include	"glut.h"

using std::cerr;


// 練習 讀bmp
//BITMAPFILEHEADER bh;
//BITMAPINFO *info;
//要動態配置記憶體空間給 BitmapInfo? 不要
#define GL_BGR 0x80E0
GLubyte * ReadDIBitmap(BITMAPINFO &info)
{
	long bitSize, width;
	GLint viewport[4];
	GLubyte *bits;
	//GLubyte *rgb, temp;

	glGetIntegerv(GL_VIEWPORT, viewport);

	//info = (BITMAPINFO *)malloc( sizeof(BITMAPINFO) );
	//if( 0 == info ){
	//	cerr << "malloc failed\n";
	//	exit(1);
	//}

	width = viewport[2] * 3;
	width = (width + 3)& ~3;	// aligned to 4 byte
	bitSize = width * viewport[3];	// size of an aligned bitmap

	if( (bits = (GLubyte *) calloc(bitSize, 1)) == 0 ){
		cerr << "malloc failed\n";
		//free(info);
		exit(1);
	}
	glFinish();
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

	// GL_BGR 要 glew.h 才有定義
	glReadPixels(0, 0, viewport[2], viewport[3], GL_BGR, GL_UNSIGNED_BYTE, bits);

#ifdef DEBUG2
	FILE *fptr;
	if( (fptr = fopen("debug.txt", "w+")) == 0 ){ cerr<<"open file error"; exit(1); }
	i = 0;
	for( i = 0; i < bitSize; i += 4 )
		fprintf(fptr, "%d ", bits[i]);
	fprintf(fptr, "\n");
		
#endif

	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = viewport[2];
	info.bmiHeader.biHeight = viewport[3];
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 24;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biSizeImage = bitSize;
	info.bmiHeader.biXPelsPerMeter = 2952;	// 75 dpi
	info.bmiHeader.biYPelsPerMeter = 2952;
	info.bmiHeader.biClrUsed = 0;
	info.bmiHeader.biClrImportant = 0;

	return bits;
}

bool SaveDIBitmap( const char *fileName, BITMAPINFO *info, GLubyte *bits )
{
	FILE *fp;
	int size, infoSize, bitSize;
	BITMAPFILEHEADER header;

	if( (fp = fopen(fileName, "w+")) == 0 ){
		cerr << "open file error\n";
		return false;
	}
	if( info->bmiHeader.biSizeImage == 0 )
		bitSize = (info->bmiHeader.biWidth * info->bmiHeader.biBitCount + 7)/8 * abs(info->bmiHeader.biHeight );
	else
		bitSize = info->bmiHeader.biSizeImage;

	infoSize = sizeof(BITMAPINFO);
	size = sizeof( BITMAPFILEHEADER ) + infoSize + bitSize;
	
	header.bfType = 'MB';
	//printf("MB: %d\n", 'MB');
	header.bfSize = size;
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfOffBits = sizeof(BITMAPFILEHEADER) + infoSize;

	if( fwrite( &header, sizeof(BITMAPFILEHEADER), 1, fp ) < 1 ){
		cerr << "write header failed\n";
		fclose(fp);
		return false;
	}
	if( fwrite( info, infoSize, 1, fp) < 1 ) {
		cerr << "write info failed\n";
		fclose(fp);
		return false;
	}
	if( fwrite( bits, 1, bitSize, fp )< size_t( bitSize ) ){
		cerr << "write bits failed\n";
		fclose(fp);
		return false;
	}

	fclose(fp);

	return true;
}// close function