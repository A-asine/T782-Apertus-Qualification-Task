/* Copyright (C) 2019 Devendra Shaktawat
 *
 * Anyone can use, distribute and modify this code under the
 * terms of the MIT license.
 */

#include "AVIStruct.h"

void AviCreator::MakeRiffHeader(int& offset, uint8_t* aviFile)
{
   List riff;
   riff.FourCC   = MakeFourCC("RIFF");
   riff.FileSize = headerMap["avi "];
   riff.FileType = MakeFourCC("AVI ");
   
   std::memcpy(&aviFile[offset], (const void *)&riff, sizeof(List));   
   offset += sizeof(List);
}

void AviCreator::MakeHdrlHeader(int& offset, uint8_t* aviFile)
{
   List hdrl;

   hdrl.FourCC   = MakeFourCC("LIST");
   hdrl.FileSize = headerMap["hdrl"];
   hdrl.FileType = MakeFourCC("hdrl");
   
   std::memcpy(&aviFile[offset], (const void *)&hdrl, sizeof(List));
   offset += sizeof(List);
}

void AviCreator::MakeAvihHeader(int& offset, uint8_t* aviFile)
{
   AVIMainHeader avih;
   
   avih.fcc                  = MakeFourCC("avih");
   avih.cb                   = headerMap["avih"] - 8;
   avih.dwMicroSecPerFrame   = 100;
   avih.dwMaxBytesPerSec     = 1000;
   avih.dwPaddingGranularity = 0;
   avih.dwFlags              = 1;
   avih.dwTotalFrames        = frameCount;  
   avih.dwInitialFrames      = 0;
   avih.dwStreams            = 1;
   avih.dwSuggestedBufferSize= 100000;
   avih.dwWidth              = width;
   avih.dwHeight             = height;
   avih.dwReserved[0]        = 0;
   avih.dwReserved[1]        = 0;
   avih.dwReserved[2]        = 0;
   avih.dwReserved[3]        = 0;
    
   std::memcpy(&aviFile[offset], (const void *)&avih, sizeof(AVIMainHeader));
   offset += sizeof(AVIMainHeader); 
}

void AviCreator::MakeStrlHeader(int& offset, uint8_t* aviFile)
{
   List strl;
   
   strl.FourCC   = MakeFourCC("LIST");
   strl.FileSize = headerMap["strl"];
   strl.FileType = MakeFourCC("strl");
   
   std::memcpy(&aviFile[offset], (const void *)&strl, sizeof(List));
   offset += sizeof(List);
}

void AviCreator::MakeStrhHeader(int& offset, uint8_t* aviFile)
{
   AVIStreamHeader strh;
  
   strh.FourCCType          = MakeFourCC("strh");
   strh.cb                  = headerMap["strh"] - 8;
   strh.fccType             = MakeFourCC("vids");
   strh.fccHandler          = 0;
   strh.Flags               = 0;
   strh.Priority            = 0;
   strh.Language            = 0;
   strh.InitialFrames       = 0;
   strh.Scale               = 1;
   strh.Rate                = 30;
   strh.Start               = 0;
   strh.Length              = strh.Rate / strh.Scale;
   strh.SuggestedBufferSize = 10000000;
   strh.Quality             = 0;
   strh.SampleSize          = 120;

   strh.rcFrame.left        = 0;
   strh.rcFrame.right       = width;
   strh.rcFrame.top         = 0;
   strh.rcFrame.bottom      = height;
   
   std::memcpy(&aviFile[offset], (const void *)&strh, sizeof(AVIStreamHeader));
   offset += sizeof(AVIStreamHeader);
} 

void AviCreator::MakeStrfHeader(int& offset, uint8_t* aviFile)
{ 
   Chunk strf;  
   
   strf.Ckid   = MakeFourCC("strf");
   strf.CkSize = headerMap["strf"];
    
   std::memcpy(&aviFile[offset], (const void *)&strf, sizeof(Chunk));
   offset += sizeof(Chunk);
}

void AviCreator::MakeBitHeader(int& offset, uint8_t* aviFile)
{
   BitmapInfoHeader vidf;
  
   vidf.Size          = sizeof(BitmapInfoHeader);
   vidf.Width         = width;
   vidf.Height        = height;
   vidf.Planes        = 1;
   vidf.BitCount      = 24;
   vidf.Compression   = 0;
   vidf.SizeImage     = 0;
   vidf.XPelsPerMeter = 2835;
   vidf.YPelsPerMeter = 2835;
   vidf.ClrUsed       = 0;
   vidf.ClrImportant  = 0;
 
   std::memcpy(&aviFile[offset], (const void *)&vidf, sizeof(BitmapInfoHeader));
   offset += sizeof(BitmapInfoHeader);
}

void AviCreator::MakeMoviHeader(int& offset, uint8_t* aviFile)
{
   List movi;

   movi.FourCC   = MakeFourCC("LIST");
   movi.FileSize = headerMap["movi"];
   movi.FileType = MakeFourCC("movi");
   
   std::memcpy(&aviFile[offset], (const void *)&movi, sizeof(List));
   offset += sizeof(List);
}

void MakeAVIFile(uint8_t* frame, int _width, int _height, int _frameCount)
{     
   AviCreator aviObject(_height, _width, _frameCount);
     
   int offset = 0;
   uint8_t *aviFile = new uint8_t[220];
   
   aviObject.FindSize("avi ");
   aviObject.AddNode(aviObject , "avi ", offset, aviFile);   

   std::ofstream avi;
   avi.open("sample.avi", std::ios::binary);

   if (!avi)
   {
     std::cout << "AVI file not opening" << std::endl;
     return;
   }  

   avi.write(reinterpret_cast<char *>(aviFile), offset);

   Chunk vidf1;
   vidf1.Ckid   = MakeFourCC("00db");
   vidf1.CkSize = _width * _height * 3;

   for (int count = 0; count < _frameCount; count++)
   {
      avi.write(reinterpret_cast<char *>(&vidf1), sizeof(vidf1));        //  writing video data (chunk)
      avi.write(reinterpret_cast<char *>(frame), _width * _height * 3); //  actual frame
   }

   avi.close();

   delete[] aviFile;
   delete[] frame;

   std::cout << "The avi file is ready" << std::endl;    
     
}

