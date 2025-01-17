#define NumPairsMemory	1
#define MAXNUMFLOAT 1024
#define MEMDATOSC1 _T("MemoriaDatoCliente1")
#define MEMREDC1 _T("MemoriaRedCliente1")
#define MEMDATOSS1 _T("MemoriaDatoServidor1")
#define MEMREDS1 _T("MemoriaRedServidor1")


typedef struct tag_NET_VIRTUAL_DEVICE_IN {

	bool signalWrite;	//Se leen los flancos
	UINT sizeMemoryOut;  //Cantidad de floats

} NET_VIRTUAL_DEVICE_IN;

typedef struct tag_NET_VIRTUAL_DEVICE_OUT {

	bool signalRead;	//Se leen los flancos
	UINT numberSentPackets;
	UINT numberRecvPackets;
	UINT numberLostPackets;
	UINT numberDelPackets;
	float rateSentPackets;
	float rateRecvPackets;
	float rateLostPackets;
	float rateDelPackets;
	float roundTripDelay;

} NET_VIRTUAL_DEVICE_OUT;


typedef struct tag_VIRTUAL_DEVICE_IN {

	float dataMemory[MAXNUMFLOAT];

} VIRTUAL_DEVICE_IN;

typedef struct tag_VIRTUAL_DEVICE_OUT {

	float dataMemory[MAXNUMFLOAT];

} VIRTUAL_DEVICE_OUT;