#include "precompile.h"
#include "RPCReader.h"

#define TEST_FILE	"HY_Domain01_00047.rpc"

template<typename T> void GetVectorLimits(const RPCFile::ChannelInfo* channel, int numComps, char* minValBuf, char* maxValBuf, char* avgValBuf, char* minMagBuf, char* maxMagBuf)
{
	const T* minVal = (const T*)channel->GetMinValue();
	const T* maxVal = (const T*)channel->GetMaxValue();
	const T* avgVal = (const T*)channel->GetAvgValue();

	switch(numComps)
	{
		case 2:
			sprintf(minValBuf, "%.3f %.3f", minVal[0], minVal[1]);
			sprintf(maxValBuf, "%.3f %.3f", maxVal[0], maxVal[1]);
			sprintf(avgValBuf, "%.3f %.3f", avgVal[0], avgVal[1]);
			break;

		case 3:
			sprintf(minValBuf, "%.3f %.3f %.3f", minVal[0], minVal[1], minVal[2]);
			sprintf(maxValBuf, "%.3f %.3f %.3f", maxVal[0], maxVal[1], maxVal[2]);
			sprintf(avgValBuf, "%.3f %.3f %.3f", avgVal[0], avgVal[1], avgVal[2]);
			break;

		case 4:
			sprintf(minValBuf, "%.3f %.3f %.3f %.3f", minVal[0], minVal[1], minVal[2], minVal[3]);
			sprintf(maxValBuf, "%.3f %.3f %.3f %.3f", maxVal[0], maxVal[1], maxVal[2], maxVal[3]);
			sprintf(avgValBuf, "%.3f %.3f %.3f %.3f", avgVal[0], avgVal[1], avgVal[2], avgVal[3]);
			break;

		default:
			assert(!"Illegal number of components in GetVectorLimits()");
			break;
	}

	sprintf(minMagBuf, "%.3f", *(const T*)channel->GetMinMagnitude());
	sprintf(maxMagBuf, "%.3f", *(const T*)channel->GetMaxMagnitude());
}

static void PrintChannelInfo(const RPCFile::ChannelInfo* channel, int idx)
{
	const char* typeName;
	char minValBuf[128] = { 0 }, maxValBuf[128] = { 0 }, avgValBuf[128] = { 0 }, minMagBuf[128] = { 0 }, maxMagBuf[128] = { 0 };

	switch(channel->GetDataType())
	{
		case RPCFile::ChannelDataType_Float3:
			typeName = "float3";
			GetVectorLimits<float>(channel, 3, minValBuf, maxValBuf, avgValBuf, minMagBuf, maxMagBuf);
			break;

		case RPCFile::ChannelDataType_Double3:
			typeName = "double3";
			GetVectorLimits<double>(channel, 3, minValBuf, maxValBuf, avgValBuf, minMagBuf, maxMagBuf);
			break;

		case RPCFile::ChannelDataType_Float2:
			typeName = "float2";
			GetVectorLimits<float>(channel, 2, minValBuf, maxValBuf, avgValBuf, minMagBuf, maxMagBuf);
			break;

		case RPCFile::ChannelDataType_UInt64:
			typeName = "uint64";
			sprintf(minValBuf, "%lu", *(const uint64_t*)channel->GetMinValue());
			sprintf(maxValBuf, "%lu", *(const uint64_t*)channel->GetMaxValue());
			sprintf(avgValBuf, "%lu", *(const uint64_t*)channel->GetAvgValue());
			break;

		case RPCFile::ChannelDataType_UInt16:
			typeName = "uint16";
			sprintf(minValBuf, "%u", *(const uint16_t*)channel->GetMinValue());
			sprintf(maxValBuf, "%u", *(const uint16_t*)channel->GetMaxValue());
			sprintf(avgValBuf, "%u", *(const uint16_t*)channel->GetAvgValue());
			break;

		case RPCFile::ChannelDataType_UInt8:
			typeName = "uint8";
			sprintf(minValBuf, "%u", *(const uint8_t*)channel->GetMinValue());
			sprintf(maxValBuf, "%u", *(const uint8_t*)channel->GetMaxValue());
			sprintf(avgValBuf, "%u", *(const uint8_t*)channel->GetAvgValue());
			break;

		case RPCFile::ChannelDataType_Int32:
			typeName = "int32";
			sprintf(minValBuf, "%d", *(const int32_t*)channel->GetMinValue());
			sprintf(maxValBuf, "%d", *(const int32_t*)channel->GetMaxValue());
			sprintf(avgValBuf, "%d", *(const int32_t*)channel->GetAvgValue());
			break;

		case RPCFile::ChannelDataType_Bool:	
			typeName = "bool";
			break;

		case RPCFile::ChannelDataType_Float:
			typeName = "float";
			sprintf(minValBuf, "%.3f", *(const float*)channel->GetMinValue());
			sprintf(maxValBuf, "%.3f", *(const float*)channel->GetMaxValue());
			sprintf(avgValBuf, "%.3f", *(const float*)channel->GetAvgValue());
			break;

		case RPCFile::ChannelDataType_Double:
			typeName = "double";
			sprintf(minValBuf, "%.3f", *(const double*)channel->GetMinValue());
			sprintf(maxValBuf, "%.3f", *(const double*)channel->GetMaxValue());
			sprintf(avgValBuf, "%.3f", *(const double*)channel->GetAvgValue());
			break;

		case RPCFile::ChannelDataType_Float4:
			typeName = "float4";
			GetVectorLimits<float>(channel, 4, minValBuf, maxValBuf, avgValBuf, minMagBuf, maxMagBuf);
			break;

		default:
			assert(!"Unknown data type in PrintChannelInfo()");
			typeName = "unknown";
			break;
	}

	printf("Channel %d: '%s'\n", idx, channel->GetName());
	printf("    type             : %s\n", typeName);
	printf("    compressed size  : %d\n", channel->GetCompressedSize());
	printf("    decompressed size: %d\n", channel->GetDecompressedSize());
	
	if(minValBuf[0])
	{
		printf("    min value        : %s\n", minValBuf);
		printf("    max value        : %s\n", maxValBuf);
		printf("    avg value        : %s\n", avgValBuf);
	}

	if(minMagBuf[0])
	{
		printf("    min magnitude    : %s\n", minMagBuf);
		printf("    max magnitude    : %s\n", maxMagBuf);
	}

	printf("\n");
}

static uint32_t ReadWholeBuffers(float*& posBuf, uint64_t*& idBuf, float*& velocityBuf)
{
	RPCFile reader;
	if(!reader.Open(TEST_FILE))
	{
		assert(!"Cannot open file");
		return 0;
	}

	uint32_t numParticles = reader.GetNumParticles();
	uint32_t numChannels = reader.GetNumChannels();

	printf("%d particles, %d channels.\n", numParticles, numChannels);

	const float* bboxMin = reader.GetBBoxMin();
	const float* bboxMax = reader.GetBBoxMax();
	printf("bbox: { %.3f %.3f %.3f } -> { %.3f %.3f %.3f }\n\n", bboxMin[0], bboxMin[1], bboxMin[2], bboxMax[0], bboxMax[1], bboxMax[2]);

	for(uint32_t i = 0; i < numChannels; ++i)
	{
		const RPCFile::ChannelInfo* channel = reader.GetChannelInfo(i);
		assert(channel->GetDecompressedSize() == numParticles * RPCFile::GetDataTypeSize(channel->GetDataType()));
		PrintChannelInfo(channel, i);
	}

	posBuf = (float*)reader.ReadWholeChannel("position");
	idBuf = (uint64_t*)reader.ReadWholeChannel("id");
	velocityBuf = (float*)reader.ReadWholeChannel("velocity");

	assert(posBuf && idBuf && velocityBuf);
	return numParticles;
}

static void ReadBuffersElementWise(float*& posBuf, uint64_t*& idBuf, float*& velocityBuf)
{
	RPCFile reader;
	if(!reader.Open(TEST_FILE))
	{
		assert(!"Cannot open file");
		return;
	}

	uint32_t numParticles = reader.GetNumParticles();

	RPCFile::ChannelReader* posReader = reader.OpenChannel("position", true);
	RPCFile::ChannelReader* idReader = reader.OpenChannel("id", true);
	RPCFile::ChannelReader* velReader = reader.OpenChannel("velocity", true);

	assert(posReader && idReader && velReader);

	int vecChanSize = numParticles * 3 * sizeof(float);
	int idChanSize = numParticles * sizeof(uint64_t);
	
	assert(posReader->GetDataSize() == vecChanSize);
	assert(posReader->GetElementSize() == 3 * sizeof(float));
	assert(idReader->GetDataSize() == idChanSize);
	assert(idReader->GetElementSize() == sizeof(uint64_t));
	assert(velReader->GetDataSize() == vecChanSize);
	assert(velReader->GetElementSize() == 3 * sizeof(float));
	
	posBuf = (float*)malloc(vecChanSize);
	idBuf = (uint64_t*)malloc(idChanSize);
	velocityBuf = (float*)malloc(vecChanSize);

	float* posPtr = posBuf;
	uint64_t* idPtr = idBuf;
	float* velPtr = velocityBuf;

	for(uint32_t partIdx = 0; partIdx < numParticles; ++partIdx)
	{
		posReader->ReadData(posPtr, 3*sizeof(float));
		idReader->ReadData(idPtr, sizeof(uint64_t));
		velReader->ReadData(velPtr, 3*sizeof(float));

		posPtr += 3;
		idPtr += 1;
		velPtr += 3;
	}

	delete posReader;
	delete idReader;
	delete velReader;
}

int main()
{
	float* posBuffers[2] = { 0 };
	uint64_t* idBuffers[2] = { 0 };
	float* velBuffers[2] = { 0 };

	uint32_t numParticles = ReadWholeBuffers(posBuffers[0], idBuffers[0], velBuffers[0]);
	ReadBuffersElementWise(posBuffers[1], idBuffers[1], velBuffers[1]);

	for(uint32_t partIdx = 0; partIdx < numParticles; ++partIdx)
	{
		for(int i = 0; i < 3; ++i)
		{
			assert(posBuffers[0][3*partIdx + i] == posBuffers[1][3*partIdx + i]);
			assert(velBuffers[0][3*partIdx + i] == velBuffers[1][3*partIdx + i]);
		}

		assert(idBuffers[0][partIdx] == idBuffers[1][partIdx]);
	}

	free(posBuffers[0]); free(posBuffers[1]);
	free(idBuffers[0]); free(idBuffers[1]);
	free(velBuffers[0]); free(velBuffers[1]);

	printf("Tests passed.\n");

	return 0;
}
