// General Template for CMD-line video generation

//#include <stdio.h>
//
//int main()
//{
//	// start ffmpeg telling it to expect raw rgba 720p-60hz frames
//	// -i - tells it to read frames from stdin
//	const char* cmd = "ffmpeg -r 60 -f rawvideo -pix_fmt rgba -s 1280x720 -i - "
//		"-threads 0 -preset fast -y -pix_fmt yuv420p -crf 21 -vf vflip output.mp4";
//	// open pipe to ffmpeg's stdin in binary write mode
//	FILE* ffmpeg = _popen(cmd, "wb");
//	/* Set up buffer */
//	int* buffer = new int[width*height];
//
//	glutSwapBuffers();
//	/* Read pixels from GPU into buffer */
//	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, buffer);
//	/* Write buffer into FFMPEG */
//	fwrite(buffer, sizeof(int)*width*height, 1, ffmpeg);
//
//	_pclose(ffmpeg);
//
//}
//
