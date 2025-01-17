#define NumPairsMemory	1
#define MAXNUMBYTES 60000
#define MEMDATOSC1 _T("MemoriaVideoDatoCliente1")
#define MEMREDC1 _T("MemoriaVideoRedCliente1")
#define MEMDATOSS1 _T("MemoriaVideoDatoServidor1")
#define MEMREDS1 _T("MemoriaVideoRedServidor1")


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
    UINT dataMustBeReceived;
	UINT dataReceived;

} NET_VIRTUAL_DEVICE_OUT;


typedef struct tag_VIRTUAL_DEVICE_IN {

	unsigned char dataMemory[MAXNUMBYTES];

} VIRTUAL_DEVICE_IN;

typedef struct tag_VIRTUAL_DEVICE_OUT {

	unsigned char dataMemory[MAXNUMBYTES];

} VIRTUAL_DEVICE_OUT;