#define _CRT_SECURE_NO_WARNINGS

#include "stdio.h"
#include "stdlib.h"
#include "string.h"


int GetCfgItem(char *pFileName /*in*/, char *pKey /*in*/, char * pValue/*in out*/, int * pValueLen /*out*/);
int WriteCfgItem(char *pFileName /*in*/, char *pItemName /*in*/, char *pItemValue/*in*/, int itemValueLen /*in*/);
int SetCfgItem(char *pFileName /*in*/, char *pKey /*in*/, char * pValue/*in*/, int ValueLen /*in*/);
//ʵ������
//���ļ�
//�����еķ�ʽ ѭ�����ļ�
//����ÿһ�У���ƥ��key�ؼ��֣��ڽ���valueֵ����ȡ
//��ȡvalueֵ��Ҫ 1ȥ��ǰ��ո� 1��ָ�����Ӧ��

#define  LineMaxLen 2048
#define  KeyMaxLen 64

int GetCfgItem(char *pFileName /*in*/, char *pKey /*in*/, char * pValue/*in out*/, int * pValueLen /*out*/)
{
	int		rv = 0;
	FILE	*fp = NULL;
	char	lineBuf[LineMaxLen];
	char	*pTmp = NULL, *pBegin = NULL, *pEnd = NULL;

	if (pFileName==NULL || pKey==NULL || pValue==NULL || pValueLen==NULL) 
	{
		rv = -1;
		printf("GetCfgItem() err. param err \n");
		goto End;
	}

	fp = fopen(pFileName, "r");
	if (fp == NULL)
	{
		rv = -2;
		printf("fopen() err. \n");
		goto End;
	}
	while (!feof(fp))
	{
		//��ÿһ��
		memset(lineBuf, 0, sizeof(lineBuf));
		pTmp = fgets(lineBuf, LineMaxLen, fp);
		if (pTmp == NULL) 
		{
			break;
		}

		//����=, ��������
		pTmp = strchr(lineBuf, '=');
		if (pTmp == NULL)
		{
			continue;
		}
		//key�Ƿ��ڱ���
		pTmp = strstr(lineBuf, pKey);
		if (pTmp == NULL)
		{
			continue;
		}
		
		//������=�ұߣ�ȡvalue׼��
		pTmp = strchr(lineBuf, '=');
		if (pTmp == NULL)
		{
			continue;
		}
		pTmp = pTmp + 1;

		//��ȡvalue ���
		while (1) 
		{
			if (*pTmp == ' ')
			{
				pTmp ++ ;
			} 
			else
			{
				pBegin = pTmp;
				if (*pBegin == '\n')
				{
					//û������value
					printf("������:%s û������value \n", pKey);
					goto End;
				}
				break;
			}
		}
		
		//��ȡvalude������
		while (1) 
		{
			if ((*pTmp == ' ' || *pTmp == '\n'))
			{
				break;
			}
			else 
			{
				pTmp ++;
			}
		}
		pEnd = pTmp;

		//��ֵ
		*pValueLen = pEnd-pBegin;
		memcpy(pValue, pBegin, pEnd-pBegin);
		printf("\n%s = %s", pKey, pValue);
	}

End:
	if (fp != NULL)
	{
		fclose(fp); 
	}
	
	return rv;
}

//ʵ������
//ѭ����ÿһ�У����key�������Ƿ���� �������޸Ķ�Ӧvalueֵ
//�������ڣ����ļ�ĩβ ��� "key = value"
//�ѵ㣺����޸��ļ����е�ֵ

int SetCfgItem(char *pFileName /*in*/, char *pKey /*in*/, char * pValue/*in*/, int ValueLen /*in*/)
{
	int		rv = 0, iTag = 0, length = 0;
	FILE	*fp = NULL;
	char	lineBuf[LineMaxLen];
	char	filebuf[10240*8] = {0};
	char *pTmp = NULL;
	if (pFileName==NULL || pKey==NULL || pValue==NULL) 
	{
		rv = -1;
		printf("SetCfgItem() err. param err \n");
		goto End;
	}
	
	fp = fopen(pFileName, "r+");
	if (fp == NULL)
	{
		rv = -2;
		printf("fopen() err. \n");
		//goto End;
	}

	if (fp == NULL)
	{
		fp = fopen(pFileName, "w+t");
		if (fp == NULL)
		{
			rv = -3;
			printf("fopen() err. \n");
			goto End;
		}
	}
	
	fseek(fp, 0L, SEEK_END); 
	//��ȡ�ļ�����;
	length = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	
	if (length > 10240*8) 
	{
		rv = -3;
		printf("�ļ�����1024*8, nunsupport");
		goto End;
	}
	
	while (!feof(fp))
	{
		//��ÿһ��
		memset(lineBuf, 0, sizeof(lineBuf));
		pTmp = fgets(lineBuf, LineMaxLen, fp);
		if (pTmp == NULL) 
		{
			break;
		}
	
		//key�ؼ����Ƿ��ڱ���
		pTmp = strstr(lineBuf, pKey);
		if (pTmp == NULL)
		{
			strcat(filebuf, lineBuf);
			continue;
		}
		else
		{
			sprintf(lineBuf, "%s = %s\n", pKey, pValue);
			strcat(filebuf, lineBuf);
			//������key
			iTag = 1; 
		}
	}

	//�������� ׷��
	if (iTag == 0) 
	{
		fprintf(fp, "%s = %s\n", pKey, pValue);
	}
	else //������
	{
		if (fp != NULL) 
		{ 
			fclose(fp); 
			fp = NULL; //����Ұָ��
		}

		fp = fopen(pFileName, "w+t");

		if (fp == NULL)
		{
			rv = -4;
			printf("fopen() err. \n");
			goto End;
		}
		fputs(filebuf, fp);
		//fwrite(filebuf, sizeof(char), strlen(filebuf), fp);
	}
	
End:
	if (fp != NULL)
	{
		fclose(fp); 
	}
	return rv;
}
int WriteCfgItem(char *pFileName /*in*/, char *pItemName /*in*/, char *pItemValue /*in*/, int itemValueLen /*in*/)
{
	int		rv = 0;
	FILE	*fp = NULL;
	char	lineBuf[LineMaxLen] = {0};

	if (pFileName == NULL || pItemValue == NULL || pItemName == NULL )
	{
		rv = -1;
		printf("WriteCfgItem() err. param err \n");
		goto End;
	}
	fp = fopen(pFileName, "a+");
	if (fp == NULL)
	{
		rv = -2;
		printf("fopen() err. \n");
		goto End;
	}
	sprintf(lineBuf, "%s=%s\n", pItemName, pItemValue);
	fputs(lineBuf,fp);


End:
	if (fp != NULL)
	{
		fclose(fp);
	}
	return rv;

}