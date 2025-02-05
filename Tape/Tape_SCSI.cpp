#include "main.h"
static BOOL ScsiIoControl(HANDLE hFile, DWORD deviceNumber, PVOID cdb, UCHAR cdbLength, PVOID dataBuffer, USHORT bufferLength, BYTE dataIn, ULONG timeoutValue, PVOID senseBuffer)
{
    DWORD bytesReturned;
    BOOL result = FALSE;
    BYTE scsiBuffer[sizeof(SCSI_PASS_THROUGH_DIRECT) + SENSE_INFO_LEN];

    PSCSI_PASS_THROUGH_DIRECT scsiDirect = (PSCSI_PASS_THROUGH_DIRECT)scsiBuffer;
    memset(scsiDirect, 0, sizeof(scsiBuffer));

    scsiDirect->Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
    scsiDirect->CdbLength = cdbLength;
    scsiDirect->DataBuffer = dataBuffer;
    scsiDirect->SenseInfoLength = SENSE_INFO_LEN;
    scsiDirect->SenseInfoOffset = sizeof(SCSI_PASS_THROUGH_DIRECT);
    scsiDirect->DataTransferLength = bufferLength;
    scsiDirect->TimeOutValue = timeoutValue;
    scsiDirect->DataIn = dataIn;

    memcpy(scsiDirect->Cdb, cdb, cdbLength);

    result = DeviceIoControl(hFile, IOCTL_SCSI_PASS_THROUGH_DIRECT, scsiDirect, sizeof(scsiBuffer), scsiDirect, sizeof(scsiBuffer), &bytesReturned, NULL);

    if (senseBuffer)
        memcpy(senseBuffer, scsiBuffer + sizeof(SCSI_PASS_THROUGH_DIRECT), SENSE_INFO_LEN);

    return result;
}
BOOL _TapeSCSIIOCtlFull(HANDLE tapeDrive, PVOID cdb, UCHAR cdbLength, PVOID dataBuffer, USHORT bufferLength, BYTE dataIn, ULONG timeoutValue, PVOID senseBuffer)
{
	BOOL result = FALSE;
	DWORD bytesReturned;
	BYTE scsiBuffer[sizeof(SCSI_PASS_THROUGH_DIRECT) + SENSE_INFO_LEN];
	PSCSI_PASS_THROUGH_DIRECT scsiDirect = (PSCSI_PASS_THROUGH_DIRECT)scsiBuffer;
	memset(scsiDirect, 0, sizeof(scsiBuffer));
	scsiDirect->Length = sizeof(SCSI_PASS_THROUGH_DIRECT);
	scsiDirect->CdbLength = cdbLength;
	scsiDirect->DataBuffer = dataBuffer;
	scsiDirect->SenseInfoLength = SENSE_INFO_LEN;
	scsiDirect->SenseInfoOffset = sizeof(SCSI_PASS_THROUGH_DIRECT);
	scsiDirect->DataTransferLength = bufferLength;
	scsiDirect->TimeOutValue = timeoutValue;
	scsiDirect->DataIn = dataIn;
	memcpy(scsiDirect->Cdb, cdb, cdbLength);
	result = DeviceIoControl(tapeDrive, IOCTL_SCSI_PASS_THROUGH_DIRECT, scsiDirect, sizeof(scsiBuffer), scsiDirect, sizeof(scsiBuffer), &bytesReturned, NULL);
	if (senseBuffer)
		memcpy(senseBuffer, scsiBuffer + sizeof(SCSI_PASS_THROUGH_DIRECT), SENSE_INFO_LEN);
	return result;
}
BOOL TapeCheckMedia(HANDLE tapeDrive)
{
    BOOL result = FALSE;
    BYTE cdb[10];
    BYTE dataBuffer[64];
    BYTE senseBuffer[SENSE_INFO_LEN];
    memset(cdb, 0, sizeof(cdb));
    memset(dataBuffer, 0, sizeof(dataBuffer));
    memset(senseBuffer, 0, sizeof(senseBuffer));
    ((PCDB)(cdb))->READ_POSITION.Operation = SCSIOP_READ_POSITION;
    ((PCDB)(cdb))->READ_POSITION.Reserved1 = 0x03;
    result = ScsiIoControl(tapeDrive, 0, cdb, sizeof(cdb), dataBuffer, sizeof(dataBuffer), SCSI_IOCTL_DATA_IN, 300, senseBuffer);
    if (!result)
    {
		printf("Can't get tape description");
        return FALSE;
    }
    if (((senseBuffer[2] & 0x0F) == 0x02) && (senseBuffer[12] == 0x3A) && (senseBuffer[13] == 0x00))
    {
        printf("No tape loaded");
        return TRUE;
    }
    memset(cdb, 0, sizeof(cdb));
    memset(dataBuffer, 0, sizeof(dataBuffer));
    ((PCDB)(cdb))->MODE_SENSE10.OperationCode = SCSIOP_MODE_SENSE10;
    ((PCDB)(cdb))->MODE_SENSE10.PageCode = TC_MP_MEDIUM_CONFIGURATION;
    ((PCDB)(cdb))->MODE_SENSE10.Pc = TC_MP_PC_CURRENT;
    ((PCDB)(cdb))->MODE_SENSE10.AllocationLength[0] = sizeof(dataBuffer) >> 8;
    ((PCDB)(cdb))->MODE_SENSE10.AllocationLength[1] = sizeof(dataBuffer) & 0xFF;
    result = ScsiIoControl(tapeDrive, 0, cdb, sizeof(cdb), dataBuffer, sizeof(dataBuffer), SCSI_IOCTL_DATA_IN, 300, NULL);
    if (result)
    {
        USHORT mediaType = (USHORT)dataBuffer[8] + ((USHORT)(dataBuffer[18] & 0x01) << 8);
        if (!(mediaType & 0x100))
            mediaType |= ((USHORT)(dataBuffer[3] & 0x80) << 2);

        switch (mediaType)
        {
        case 0x005E:
            printf("LTO8 RW");
            break;
        case 0x015E:
            printf("LTO8 WORM");
            break;
        case 0x025E:
            printf("LTO8 RO");
            break;
        case 0x005D:
            printf("LTOM8 RW");
            break;
        case 0x015D:
            printf("LTOM8 WORM");
            break;
        case 0x025D:
            printf("LTOM8 RO");
            break;
        case 0x005C:
            printf("LTO7 RW");
            break;
        case 0x015C:
            printf("LTO7 WORM");
            break;
        case 0x025C:
            printf("LTO7 RO");
            break;
        case 0x005A:
            printf("LTO6 RW");
            break;
        case 0x015A:
            printf("LTO6 WORM");
            break;
        case 0x025A:
            printf("LTO6 RO");
            break;
        case 0x0058:
            printf("LTO5 RW");
            break;
        case 0x0158:
            printf("LTO5 WORM");
            break;
        case 0x0258:
            printf("LTO5 RO");
            break;
        case 0x0046:
            printf("LTO4 RW");
            break;
        case 0x0146:
            printf("LTO4 WORM");
            break;
        case 0x0246:
            printf("LTO4 RO");
            break;
        case 0x0044:
            printf("LTO3 RW");
            break;
        case 0x0144:
            printf("LTO3 WORM");
            break;
        case 0x0244:
            printf("LTO3 RO");
            break;
        default:
            printf("Unknown media type 0x%X", mediaType);
        }
    }
    return result;
}