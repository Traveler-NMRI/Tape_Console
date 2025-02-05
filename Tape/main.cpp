#include "main.h"
void ShowError(DWORD ErrorCode){
	printf("����ֵ:");
	switch(ErrorCode){
	case ERROR_BEGINNING_OF_MEDIA:
		printf("�������еȿ�ʼ���ʧ��֮ǰ��������");
		break;
	case ERROR_BUS_RESET:
		printf("�������ϼ�⵽��������");
		break;
	case ERROR_DEVICE_NOT_PARTITIONED:
		printf("���شŴ�ʱ�Ҳ���������Ϣ");
		break;
	case ERROR_DEVICE_REQUIRES_CLEANING:
		printf("�Ŵ��������ܹ���������Ҫ��࣬��������ȷʵ��Ҫ���");
		break;
	case ERROR_END_OF_MEDIA:
		printf("�����ڼ䵽��Ŵ��������");
		break;
	case ERROR_FILEMARK_DETECTED:
		printf("�����ڼ��ѵ����ļ����");
		break;
	case ERROR_INVALID_BLOCK_LENGTH:
		printf("���������´Ŵ��ϵĿ��С����ȷ");
		break;
	case ERROR_MEDIA_CHANGED:
		printf("�������еĴŴ��ѱ��滻��ɾ��");
		break;
	case ERROR_NO_DATA_DETECTED:
		printf("�����ڼ��ѵ������ݽ������");
		break;
	case ERROR_NO_MEDIA_IN_DRIVE:
		printf("��������û��ý��");
		break;
	case ERROR_NOT_SUPPORTED:
		printf("�Ŵ���������֧������ĺ���");
		break;
	case ERROR_PARTITION_FAILURE:
		printf("�޷��ԴŴ����з���");
		break;
	case ERROR_SETMARK_DETECTED:
		printf("�����ڼ��Ѵﵽһ�����ñ��");
		break;
	case ERROR_UNABLE_TO_LOCK_MEDIA:
		printf("����������������ʧ��");
		break;
	case ERROR_UNABLE_TO_UNLOAD_MEDIA:
		printf("����ж�شŴ�ʧ��");
		break;
	case ERROR_WRITE_PROTECT:
		printf("ý����д�뱣��");
		break;
	case 0:
		printf("����");
		break;
	default:
		printf("δ֪����:%d",ErrorCode);
		break;
	}
	printf("\n");
	return;
}
void YesNo(int b){
	if(b==TRUE)
		printf("��\n");
	else
		printf("��\n");
	return;
}
int main(){
	bool isReadonly=true;
	char TapePath[260],File1[260];
	TAPE_GET_MEDIA_PARAMETERS Temp_TG;
	TAPE_SET_MEDIA_PARAMETERS Temp_TS;
	TAPE_GET_DRIVE_PARAMETERS Temp_DG;
//	TAPE_SET_DRIVE_PARAMETERS Temp_DS;
	clock_t start,end;
	unsigned int Temp_INT,time_s;
	char RWPower,command[32];
	unsigned char buf[65536];
//	LPSTR Tape_Desc[48];
	LARGE_INTEGER FileSize,Templ;
	unsigned __int64 Temp_64,size_mb;
	HANDLE hTape,hFile1;
	DWORD BlockLow,BlockHigh,Temp_DWORD;
	printf("�Ŵ�ֻ��·��(��:\\\\.\\TAPE0):");
	gets_s(TapePath);
	printf("��ֻ����ʽ��?(Y/N):");
	RWPower=_getch();
	if(RWPower=='Y' || RWPower=='y')
		hTape=CreateFileA(TapePath,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
	else{
		hTape=CreateFileA(TapePath,GENERIC_READ | GENERIC_WRITE,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
		isReadonly=false;
	}
	if(hTape==INVALID_HANDLE_VALUE){
		printf("\n�򲻿��Ŵ�ֻ��! ������:%d\n",GetLastError());
		system("pause");
		return -1;
	}
	printf("\n�Ŵ�ֻ��򿪳ɹ�!\n");
	while(1){
		printf(">");
		gets_s(command);
		if(!strcmp(command,"status")){
			ShowError(GetTapeStatus(hTape));
		}else if(!strcmp(command,"getposition")){
			ShowError(GetTapePosition(hTape,TAPE_ABSOLUTE_POSITION,&Temp_DWORD,&BlockLow,&BlockHigh));
			Templ.LowPart=BlockLow;
			Templ.HighPart=BlockHigh;
			printf("��ͷλ��:0x%08X%08X %llu ��λ:0x%08X %u ��λ:0x%08X %u\n",BlockHigh,BlockLow,Templ.QuadPart,BlockLow,BlockLow,BlockHigh,BlockHigh);
		}else if(!strcmp(command,"setposition")){
			printf("�Ϳ��ַ:");
			scanf_s("%u",&BlockLow);
			printf("�߿��ַ:");
			scanf_s("%u",&BlockHigh);
			ShowError(SetTapePosition(hTape,TAPE_ABSOLUTE_BLOCK,0,BlockLow,BlockHigh,FALSE));
		}else if(!strcmp(command,"gettapeinfo")){
			printf("�Ŵ�����:");
			TapeCheckMedia(hTape);
			printf("\n");
			memset(&Temp_TG,0,sizeof(Temp_TG));
			ShowError(GetTapeParameters(hTape,GET_TAPE_MEDIA_INFORMATION,&Temp_DWORD,&Temp_TG));
			printf("�Ŵ��ֽ�����:%llu\n",Temp_TG.Capacity.QuadPart);
			printf("ʣ��Ŵ��ֽ���:%llu\n",Temp_TG.Remaining.QuadPart);
			printf("ÿ����ֽ���:%u\n",Temp_TG.BlockSize);
			printf("�Ŵ��ķ�����:%u\n",Temp_TG.PartitionCount);
			if(Temp_TG.WriteProtected==TRUE)
				printf("�Ŵ��Ƿ�д����:��\n");
			else
				printf("�Ŵ��Ƿ�д����:��\n");
		}else if(!strcmp(command,"getdriveinfo")){
			memset(&Temp_DG,0,sizeof(Temp_TG));
			ShowError(GetTapeParameters(hTape,GET_TAPE_DRIVE_INFORMATION,&Temp_DWORD,&Temp_DG));
			printf("�Ƿ�֧��Ӳ���������:");
			YesNo(Temp_DG.ECC);
			printf("�Ƿ�����Ӳ������ѹ��:");
			YesNo(Temp_DG.Compression);
			printf("�Ƿ������������:");
			YesNo(Temp_DG.DataPadding);
			printf("�Ƿ�����setmark����:");
			YesNo(Temp_DG.ReportSetmarks);
			printf("�豸��Ĭ�Ͽ��С:%u\n",Temp_DG.DefaultBlockSize);
			printf("�豸�������С:%u\n",Temp_DG.MaximumBlockSize);
			printf("�豸����С���С:%u\n",Temp_DG.MinimumBlockSize);
			printf("�����豸�ϴ�������������:%u\n",Temp_DG.MaximumPartitionCount);
			printf("�豸���ܱ�־�ĵ���λ(���ý���):0x%08X\n�豸���ܱ�־�ĸ���λ(���ý���):0x%08X\n",Temp_DG.FeaturesLow,Temp_DG.FeaturesHigh);
			printf("�Ŵ�����������Ŵ��������֮����ֽ���:%u\n",Temp_DG.EOTWarningZoneSize);
		}else if(!strcmp(command,"settapeblocksize")){
			printf("���С:");
			scanf_s("%u",&Temp_DWORD);
			memset(&Temp_TS,0,sizeof(Temp_TS));
			Temp_TS.BlockSize=Temp_DWORD;
			ShowError(SetTapeParameters(hTape,SET_TAPE_MEDIA_INFORMATION,&Temp_TS));
		}else if(!strcmp(command,"writetape")){
			if(isReadonly)
				printf("��ǰģʽ��֧�ִ˹���\n");
			else{
				printf("ת�����Ŵ����ļ�·��(���ļ�,��:C:\\Test.img):");
				gets_s(File1);
				hFile1=CreateFileA(File1,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
				if(hFile1==INVALID_HANDLE_VALUE){
					printf("�򲻿��ļ� ������:%d\n",GetLastError());
				}else{
					memset(&Temp_DG,0,sizeof(Temp_TG));
					ShowError(GetTapeParameters(hTape,GET_TAPE_MEDIA_INFORMATION,&Temp_DWORD,&Temp_TG));
					if(Temp_TG.BlockSize!=65536){
						printf("���С����65536��������������! ʹ��settapeblocksize�������С\n");
					}else{
						GetFileSizeEx(hFile1,&FileSize);
						start=clock();
						for(Temp_64=1;Temp_64<=(unsigned __int64)((FileSize.QuadPart/65536)+1);Temp_64++){
							memset(buf,0,sizeof(buf));
							ReadFile(hFile1,&buf,sizeof(buf),&Temp_DWORD,NULL);
							Temp_INT=GetLastError();
							if(Temp_INT==0){
								WriteFile(hTape,&buf,sizeof(buf),&Temp_DWORD,NULL);
								Temp_INT=GetLastError();
								if(Temp_INT!=0){
									printf("��д��Ŵ�ʱ�������� ������:%d\n",Temp_INT);
									break;
								}
							}else{
								printf("�ڶ�ȡ�ļ�ʱ�������� ������:%d\n",Temp_INT);
								break;
							}
						}
						end=clock();
						time_s=((end-start)/1000);
						if(time_s==0)
							time_s++;
						size_mb=FileSize.QuadPart/1048576;
						printf("��ʱ%d�� �ٶ�:%d MB/s\n",time_s,size_mb/time_s);
					}
					CloseHandle(hFile1);
				}
			}
		}else if(!strcmp(command,"readtape")){
			printf("ת���Ŵ��ļ���������(���ļ�,��:C:\\Test.img,�������ļ�,�򸲸�)?");
			gets_s(File1);
			printf("��������(��):");
			scanf_s("%u",&BlockLow);
			printf("��������(��):");
			scanf_s("%u",&BlockHigh);
			Templ.LowPart=BlockLow;
			Templ.HighPart=BlockHigh;
			hFile1=CreateFileA(File1,GENERIC_READ | GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
			if(hFile1==INVALID_HANDLE_VALUE){
				printf("�򲻿��ļ� ������:%d\n",GetLastError());
			}else{
				memset(&Temp_DG,0,sizeof(Temp_TG));
				ShowError(GetTapeParameters(hTape,GET_TAPE_MEDIA_INFORMATION,&Temp_DWORD,&Temp_TG));
				if(Temp_TG.BlockSize!=65536){
					printf("���С����65536��������������! ʹ��settapeblocksize�������С\n");
				}else{
					start=clock();
					for(Temp_64=1;Temp_64<=(unsigned __int64)Templ.QuadPart;Temp_64++){
						memset(buf,0,sizeof(buf));
						ReadFile(hTape,&buf,sizeof(buf),&Temp_DWORD,NULL);
						Temp_INT=GetLastError();
						if(Temp_INT==0){
							WriteFile(hFile1,&buf,sizeof(buf),&Temp_DWORD,NULL);
							Temp_INT=GetLastError();
							if(Temp_INT!=0){
								printf("�ڶ�ȡ�Ŵ�ʱ�������� ������:%d\n",Temp_INT);
								break;
							}
						}else{
							printf("��д���ļ�ʱ�������� ������:%d\n",Temp_INT);
							break;
						}
					}
					end=clock();
					time_s=(end-start)/1000;
					if(time_s==0)
						time_s++;
					size_mb=FileSize.QuadPart/1048576;
					printf("��ʱ%d�� �ٶ�:%d MB/s\n",time_s,size_mb/time_s);
				}
				CloseHandle(hFile1);
			}
		}else if(!strcmp(command,"load")){
			ShowError(PrepareTape(hTape,TAPE_LOAD,FALSE));
		}else if(!strcmp(command,"unload")){
			ShowError(PrepareTape(hTape,TAPE_UNLOAD,FALSE));
		}else if(!strcmp(command,"lock")){
			ShowError(PrepareTape(hTape,TAPE_LOCK,FALSE));
		}else if(!strcmp(command,"unlock")){
			ShowError(PrepareTape(hTape,TAPE_UNLOCK,FALSE));
		}else if(!strcmp(command,"tension")){
			ShowError(PrepareTape(hTape,TAPE_TENSION,FALSE));
		}else if(!strcmp(command,"writelongfilemark")){
			printf("�Ŵ������:");
			scanf_s("%u",&Temp_DWORD);
			ShowError(WriteTapemark(hTape,TAPE_LONG_FILEMARKS,Temp_DWORD,FALSE));
		}else if(!strcmp(command,"writeshortfilemark")){
			printf("�Ŵ������:");
			scanf_s("%u",&Temp_DWORD);
			ShowError(WriteTapemark(hTape,TAPE_SHORT_FILEMARKS,Temp_DWORD,FALSE));
		}else if(!strcmp(command,"writesetmark")){
			printf("�Ŵ������:");
			scanf_s("%u",&Temp_DWORD);
			ShowError(WriteTapemark(hTape,TAPE_SETMARKS,Temp_DWORD,FALSE));
		}else if(!strcmp(command,"writefilemark")){
			printf("�Ŵ������:");
			scanf_s("%u",&Temp_DWORD);
			ShowError(WriteTapemark(hTape,TAPE_FILEMARKS,Temp_DWORD,FALSE));
		}else if(!strcmp(command,"rewind")){
			ShowError(SetTapePosition(hTape,TAPE_REWIND,0,0,0,FALSE));
		}else if(!strcmp(command,"gotofilemarks")){
			printf("�ļ������(��λ):");
			scanf_s("%u",&BlockLow);
			printf("�ļ������(��λ):");
			scanf_s("%u",&BlockHigh);
			ShowError(SetTapePosition(hTape,TAPE_SPACE_FILEMARKS,0,BlockLow,BlockHigh,FALSE));
		}else if(!strcmp(command,"gotosetmarks")){
			printf("���ñ����(��λ):");
			scanf_s("%u",&BlockLow);
			printf("���ñ����(��λ):");
			scanf_s("%u",&BlockHigh);
			ShowError(SetTapePosition(hTape,TAPE_SPACE_SETMARKS,0,BlockLow,BlockHigh,FALSE));
		}else if(!strcmp(command,"createpartition")){
			printf("����������(getdriveinfo�����ṩ�Ŵ�����֧�ֵ���������,�ֱ�����һ��������,���һ�������Ĵ�С�ǴŴ���ʣ�ಿ��):");
			scanf_s("%u",&BlockLow);
			printf("�����Ĵ�С(MB):");
			scanf_s("%u",&BlockHigh);
			ShowError(CreateTapePartition(hTape,TAPE_INITIATOR_PARTITIONS,BlockLow,BlockHigh));
		}else if(!strcmp(command,"erasetape")){
			printf("���Ҫ������д�����ݽ�����ʾ��,��������ݽ��ᶪʧ(Y/N)?");
			unsigned char charcode;
			charcode=_getch();
			if(charcode=='Y' || charcode=='y'){
				printf("\n���Ժ�...\n");
				ShowError(EraseTape(hTape,TAPE_ERASE_SHORT,FALSE));
			}else{
				printf("\n���û�ȡ��");
			}
		}else if(!strcmp(command,"loadunthread")){
			unsigned char buffer[6]={0x1B,0x00,0x00,0x00,0x09,0x00},buffer2[5],buffer3[128];
			memset(buffer2,0,sizeof(buffer2));
			if(_TapeSCSIIOCtlFull(hTape,buffer,sizeof(buffer),buffer2,0,2,300,buffer3))
				printf("˳��!\n");
			else
				printf("ʧ��!\n");
		}else if(!strcmp(command,"sendSCSIcommand")){
			unsigned char HEX_ASCII[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'},HEX_SCSIDATA[128],HEX_SCSIDATA_2[128],reback_data[1024];
			char HEX_SCSIDATA_ASCII[256],HEX_SCSIDATA_2_ASCII[256];
			unsigned int lens=0;
			memset(HEX_SCSIDATA_ASCII,0,sizeof(HEX_SCSIDATA_ASCII));
			memset(HEX_SCSIDATA_2_ASCII,0,sizeof(HEX_SCSIDATA_2_ASCII));
			printf("ʮ����������(��д):");
			scanf_s("%s",&HEX_SCSIDATA_ASCII);
			printf("ʮ�����ƶ�������(��д):");
			scanf_s("%s",&HEX_SCSIDATA_2_ASCII);
			for(int i=0;i<sizeof(HEX_SCSIDATA_ASCII);i++){
					for(int j=0;j<16;j++){
						if(HEX_SCSIDATA_ASCII[i]==HEX_ASCII[j]){
							if(i%2==0){ //H
								HEX_SCSIDATA[lens]=j<<4;
							}else{		//L
								lens++;
								HEX_SCSIDATA[lens-1]=HEX_SCSIDATA[lens-1]+j;
							}
							//HEX_SCSIDATA[i]=j;
						}
					}
			}
			for(int i=0;i<sizeof(HEX_SCSIDATA_2_ASCII);i++){
					for(int j=0;j<16;j++){
						if(HEX_SCSIDATA_2_ASCII[i]==HEX_ASCII[j]){
							if(i%2==0){ //H
								HEX_SCSIDATA_2[lens]=j<<4;
							}else{		//L
								lens++;
								HEX_SCSIDATA_2[lens-1]=HEX_SCSIDATA_2[lens-1]+j;
							}
							//HEX_SCSIDATA[i]=j;
						}
					}
			}
			for(int prtlens=1;prtlens<=strlen(HEX_SCSIDATA_ASCII)/32;prtlens++){
				printf("0x%04X |",(prtlens-1)*16);
				for(int m=0;m<=15;m++)
					printf("%02X ",HEX_SCSIDATA[((prtlens-1)*16)+m]);
				printf("\n");
			}
			if(_TapeSCSIIOCtlFull(hTape,HEX_SCSIDATA,strlen(HEX_SCSIDATA_ASCII)/2,HEX_SCSIDATA_2,strlen(HEX_SCSIDATA_2_ASCII)/2,2,300,reback_data)){
				printf("�ɹ�!\n����:\n");
				for(unsigned int prtlens=1;prtlens<=sizeof(reback_data)/16;prtlens++){
					printf("0x%04X |",(prtlens-1)*16);
					for(int m=0;m<=15;m++)
						printf("%02X ",reback_data[((prtlens-1)*16)+m]);
					printf("\n");
				}
			}else{
				printf("ʧ��!\n����:\n");
				for(unsigned int prtlens=1;prtlens<=sizeof(reback_data)/16;prtlens++){
					printf("0x%04X |",(prtlens+1)*16);
					for(int m=0;m<=15;m++)
						printf("%02X ",reback_data[((prtlens-1)*16)+m]);
					printf("\n");
				}
			}
		}else if(!strcmp(command,"readhexdata")){
			printf("���ڼ�����(��):");
			scanf_s("%d",&BlockLow);
			printf("���ڼ�����(��):");
			scanf_s("%d",&BlockHigh);
			ShowError(SetTapePosition(hTape,1,0,BlockLow,BlockHigh,FALSE));
			memset(buf,0,sizeof(buf));
			ReadFile(hTape,&buf,sizeof(buf),&Temp_DWORD,NULL);
			Temp_INT=GetLastError();
			if(Temp_INT!=0){
				printf("��ȡ�Ŵ�ʱ��������:%d\n",Temp_INT);
			}else{
				for(int prtlens=1;prtlens<=65536/16;prtlens++){
					printf("0x%04X |",(prtlens-1)*16);
					for(int m=0;m<=15;m++)
						printf("%02X ",buf[((prtlens-1)*16)+m]);
					printf("\n");
				}
			}
			ShowError(GetTapePosition(hTape,0,&Temp_DWORD,&BlockLow,&BlockHigh));
			printf("��ǰ��ͷλ��:0x%08X%08X\n",BlockHigh,BlockLow);
		}else if(!strcmp(command,"readdata")){
			memset(buf,0,sizeof(buf));
			ReadFile(hTape,&buf,sizeof(buf),&Temp_DWORD,NULL);
			Temp_INT=GetLastError();
			if(Temp_INT!=0){
				printf("��ȡ�Ŵ�ʱ��������:%d\n",Temp_INT);
			}else{
				for(int prtlens=1;prtlens<=65536/16;prtlens++){
					printf("0x%04X |",(prtlens-1)*16);
					for(int m=0;m<=15;m++)
						printf("%02X ", buf[((prtlens-1)*16)+m]);
					printf("\n");
				}
			}
			ShowError(GetTapePosition(hTape,0,&Temp_DWORD,&BlockLow,&BlockHigh));
			printf("��ǰ��ͷλ��:0x%08X%08X\n",BlockHigh,BlockLow);
		}
		else{
			printf("δ֪������:%s\n",command);
		}
	}
	return 0;
}