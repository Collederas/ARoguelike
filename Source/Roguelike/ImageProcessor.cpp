#include "ImageProcessor.h"
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb_image.h"


FVector UImageProcessor::GetPixelColor(FVector2D PixelCoord)
{
	return FVector::OneVector;
}

void UImageProcessor::LoadImage(TArray<FVector>& OutImage, int&Width, int& Height, FString ImagePath)
{
	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	if (!FileManager.FileExists(*ImagePath))
	{
		UE_LOG(LogTemp, Warning, TEXT("File not found!"));
		return;
	}
	const FString FileNameOnDisk = FileManager.GetFilenameOnDisk(*ImagePath);
	UE_LOG(LogTemp, Log, TEXT("Loading: %s"), *FileNameOnDisk);

	int n;
	
	unsigned char* data = stbi_load(TCHAR_TO_ANSI(*ImagePath), &Width, &Height, &n, 3);
	
	if (data != nullptr && Width > 0 && Height > 0)
	{
	    if (n !=3)
	    {
	        UE_LOG(LogTemp, Warning, TEXT("Only RGB pictures are supported (not RGBA)."))
	    }
		
		for  (int32 Count = 0; Count < Width * Height * 3; Count = Count + 3)
		{
			FVector PixelData(data[Count], data[Count+1], data[Count+2]);
			OutImage.Add(PixelData);
		}
		
        
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Some error occurred."));
	}

	stbi_image_free(data);
}
